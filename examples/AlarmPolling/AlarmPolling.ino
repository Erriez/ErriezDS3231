/*
 * MIT License
 *
 * Copyright (c) 2018 Erriez
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
 *    Required library: https://github.com/Erriez/ErriezDS3231
 *
 *    The example generates the following alarms without an external nINT/SQW interrupt pin:
 *      Alarm 1 every 30 seconds (by programming alarm 1 once)
 *      Alarm 2 every 2 minutes (by reprogramming alarm 2 on every alarm 2 interrupt)
 */

#include <Wire.h>

#include <ErriezDS3231.h>

// Create DS3231 RTC object
static DS3231 rtc;

// Create date time object (automatically cleared at startup)
static DS3231_DateTime dt;

// Function prototypes
static void setAlarm1();
static void setAlarm2();
static void printDateTimeShort();


void setup()
{
    // Initialize serial port
    Serial.begin(115200);
    while (!Serial) {
        ;
    }
    Serial.println(F("DS3231 RTC alarm polling example\n"));

    // Initialize TWI
    Wire.begin();
    Wire.setClock(400000);

    // Initialize RTC
    while (rtc.begin()) {
        Serial.println(F("Error: Could not detect DS3231 RTC"));
        delay(3000);
    }

    // Check oscillator status
    if (rtc.isOscillatorStopped()) {
        Serial.println(F("Error: DS3231 RTC oscillator stopped. Program new date/time."));
        while (1) {
            ;
        }
    }

    // Set alarms
    setAlarm1();
    setAlarm2();

    // Clear alarm interrupt flags
    rtc.clearAlarmFlag(Alarm1);
    rtc.clearAlarmFlag(Alarm2);

    // Print current date and time
    Serial.print(F("Current date and time: "));
    printDateTimeShort();

    Serial.println(F("Waiting for a alarms (polling)..."));
}

void loop()
{
    // Get Alarm 1 flag
    if (rtc.getAlarmFlag(Alarm1)) {
        Serial.print(F("Alarm 1 interrupt: "));

        // Print date time
        printDateTimeShort();

        // Clear alarm 1 flag
        rtc.clearAlarmFlag(Alarm1);
    }

    // Get Alarm 2 flag
    if (rtc.getAlarmFlag(Alarm2)) {
        Serial.print(F("Alarm 2 interrupt: "));

        // Print date time
        printDateTimeShort();

        // Reprogram new alarm 2
        setAlarm2();

        // Clear alarm 2 interrupt
        rtc.clearAlarmFlag(Alarm2);
    }

    // Wait some time
    delay(1000);
}

static void setAlarm1()
{
    Serial.println(F("Set Alarm 1 at every 30 seconds"));

    // Program alarm 1
    rtc.setAlarm1(Alarm1MatchSeconds, 0, 0, 0, 30);

    // Disable alarm 1 interrupt
    rtc.alarmInterruptEnable(Alarm1, false);
}

static void setAlarm2()
{
    uint8_t hour;
    uint8_t minute;
    uint8_t second;

    // Get time from RTC
    rtc.getTime(&hour, &minute, &second);

    // Increment minute interval
    minute += 2;
    minute %= 60;

    Serial.println(F("Set Alarm 2 at every hour at 2 minutes"));

    // Program alarm 2
    rtc.setAlarm2(Alarm2MatchMinutes, 0, 0, minute);

    // Disable alarm 2 interrupt
    rtc.alarmInterruptEnable(Alarm2, false);
}

static void printDateTimeShort()
{
    char buf[10];

    // Get date time from RTC
    if (rtc.getDateTime(&dt)) {
        Serial.println(F("Error: Read date time failed"));
        return;
    }

    // Print day of the week
    Serial.print(dt.dayWeek);
    Serial.print(F(" "));

    // Print day month, month and year
    Serial.print(dt.dayMonth);
    Serial.print(F("-"));
    Serial.print(dt.month);
    Serial.print(F("-"));
    Serial.print(dt.year);
    Serial.print(F("  "));

    // Print time
    snprintf(buf, sizeof(buf), "%d:%02d:%02d", dt.hour, dt.minute, dt.second);
    Serial.println(buf);
}
