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
 * \brief DS3231 RTC set build date/time example for Arduino
 * \details
 *    Source:         https://github.com/Erriez/ErriezDS3231
 *    Documentation:  https://erriez.github.io/ErriezDS3231
 */

#include <Wire.h>
#include <ErriezDS3231.h>

// Create DS3231 RTC object
ErriezDS3231 rtc;

// Global date/time object
struct tm dt;


void rtcInit()
{
    // Initialize RTC
    while (!rtc.begin()) {
        Serial.println(F("RTC not found"));
        delay(3000);
    }

    // Enable RTC clock
    rtc.clockEnable(true);

    // Set square wave out pin
    // SquareWaveDisable, SquareWave1Hz, SquareWave4096Hz, SquareWave8192Hz, SquareWave32768Hz
    rtc.setSquareWave(SquareWaveDisable);
}

bool getBuildTime(const char *str)
{
    int hour;
    int minute;
    int second;

    // Convert build time macro to time
    if (sscanf(str, "%d:%d:%d", &hour, &minute, &second) != 3) {
        return false;
    }

    dt.tm_hour = hour;
    dt.tm_min = minute;
    dt.tm_sec = second;

    return true;
}

bool getBuildDate(const char *str)
{
    const char *monthName[] = {
        "Jan", "Feb", "Mar", "Apr", "May", "Jun",
        "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
    };
    uint8_t monthIndex;
    char month[12];
    int dayMonth;
    int year;
    time_t t;

    // Convert build time macro to date
    if (sscanf(str, "%s %d %d", month, &dayMonth, &year) != 3) {
        return false;
    }

    // Convert month string to month number
    for (monthIndex = 0; monthIndex < 12; monthIndex++) {
        if (strcmp(month, monthName[monthIndex]) == 0) {
            break;
        }
    }
    if (monthIndex >= 12) {
        // Not found
        return false;
    }

    // Set date
    dt.tm_mday = dayMonth;
    dt.tm_mon = monthIndex;
    dt.tm_year = year - 1900;

    // Calculate day of the week
    t = mktime(&dt);
    dt.tm_wday = localtime(&t)->tm_wday;

    return true;
}

bool rtcSetDateTime()
{
    // Convert compile date/time to date/time string
    if (!getBuildDate(__DATE__) || !getBuildTime(__TIME__)) {
        Serial.print(F("Build date/time error"));
        return false;
    }

    // Print build date/time
    Serial.print(F("Build date time: "));
    Serial.println(asctime(&dt));

    // Set new date time
    Serial.print(F("Set RTC date time..."));
    if (!rtc.write(&dt)) {
        Serial.println(F("FAILED"));
    } else {
        Serial.println(F("OK"));
    }

    return true;
}

void setup()
{
    // Initialize serial port
    delay(500);
    Serial.begin(115200);
    while (!Serial) {
        ;
    }
    Serial.println(F("\nErriez DS3231 RTC Set Time example\n"));

    // Initialize TWI
    Wire.begin();
    Wire.setClock(100000);

    // Initialize RTC
    rtcInit();

    // Set date/time
    if (!rtcSetDateTime()) {
        // Could not parse build date/time to program RTC
        while (1) {
            delay(1000);
        }
    }
}

void loop()
{
    // Get date/time
    if (!rtc.read(&dt)) {
        Serial.println(F("RTC read failed"));
    } else {
        Serial.println(asctime(&dt));
    }

    // Wait some time
    delay(1000);
}