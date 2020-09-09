/*
 * MIT License
 *
 * Copyright (c) 2020 Erriez
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/*!
 * \brief DS3231 high accurate RTC alarm polling example for Arduino
 * \details
  *   Source:         https://github.com/Erriez/ErriezDS3231
 *    Documentation:  https://erriez.github.io/ErriezDS3231
 *
 *    The example generates the following alarms without an external nINT/SQW interrupt pin:
 *      Alarm 1 every 30 seconds (by programming alarm 1 once)
 *      Alarm 2 every 2 minutes (by reprogramming alarm 2 on every alarm 2 interrupt)
 */

#include <Wire.h>

#include <ErriezDS3231.h>

// Create DS3231 RTC object
ErriezDS3231 ds3231;

// Create date time object (automatically cleared at startup)
struct tm dt;


void setAlarm1()
{
    Serial.println(F("Set Alarm 1 at every 30 seconds"));

    // Program alarm 1
    // Alarm1EverySecond, Alarm1MatchSeconds, Alarm1MatchMinutes, Alarm1MatchHours, Alarm1MatchDay,
    // Alarm1MatchDate
    ds3231.setAlarm1(Alarm1MatchSeconds, 0, 0, 0, 30);

    // Disable alarm 1 interrupt
    ds3231.alarmInterruptEnable(Alarm1, false);
}

void setAlarm2()
{
    uint8_t hour;
    uint8_t minute;
    uint8_t second;

    // Get time from RTC
    ds3231.getTime(&hour, &minute, &second);

    // Increment minute interval
    minute += 2;
    minute %= 60;

    Serial.println(F("Set Alarm 2 at every hour at 2 minutes"));

    // Program alarm 2
    // Alarm2EveryMinute, Alarm2MatchMinutes, Alarm2MatchHours, Alarm2MatchDay or Alarm2MatchDate
    ds3231.setAlarm2(Alarm2MatchMinutes, 0, 0, minute);

    // Disable alarm 2 interrupt
    ds3231.alarmInterruptEnable(Alarm2, false);
}

void printDateTimeShort()
{
    char buf[10];

    // Get date time from RTC
    if (!ds3231.read(&dt)) {
        Serial.println(F("RTC read failed"));
        return;
    }

    // Print day of the week
    Serial.print(dt.tm_wday);
    Serial.print(F("  "));

    // Print day month, month and year
    Serial.print(dt.tm_mday);
    Serial.print(F("-"));
    Serial.print(dt.tm_mon + 1);
    Serial.print(F("-"));
    Serial.print(dt.tm_year + 1900);
    Serial.print(F("  "));

    // Print time
    snprintf(buf, sizeof(buf), "%d:%02d:%02d", dt.tm_hour, dt.tm_min, dt.tm_sec);
    Serial.println(buf);
}

void setup()
{
    // Initialize serial port
    delay(500);
    Serial.begin(115200);
    while (!Serial) {
        ;
    }
    Serial.println(F("\nErriez DS3231 RTC alarm polling example\n"));

    // Initialize TWI
    Wire.begin();
    Wire.setClock(400000);

    // Initialize RTC
    while (!ds3231.begin()) {
        Serial.println(F("RTC not found"));
        delay(3000);
    }

    // Enable oscillator
    ds3231.clockEnable(true);

    // Set alarms
    setAlarm1();
    setAlarm2();

    // Clear alarm interrupt flags
    ds3231.clearAlarmFlag(Alarm1);
    ds3231.clearAlarmFlag(Alarm2);

    // Print current date and time
    Serial.print(F("Current date and time: "));
    printDateTimeShort();

    Serial.println(F("Waiting for a alarms (polling)..."));
}

void loop()
{
    // Get Alarm 1 flag
    if (ds3231.getAlarmFlag(Alarm1)) {
        Serial.print(F("Alarm 1 interrupt: "));

        // Print date time
        printDateTimeShort();

        // Clear alarm 1 flag
        ds3231.clearAlarmFlag(Alarm1);
    }

    // Get Alarm 2 flag
    if (ds3231.getAlarmFlag(Alarm2)) {
        Serial.print(F("Alarm 2 interrupt: "));

        // Print date time
        printDateTimeShort();

        // Reprogram new alarm 2
        setAlarm2();

        // Clear alarm 2 interrupt
        ds3231.clearAlarmFlag(Alarm2);
    }

    // Wait some time
    delay(1000);
}