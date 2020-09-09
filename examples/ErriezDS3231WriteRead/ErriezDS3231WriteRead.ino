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


void setup()
{
    struct tm dt;
    
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
    while (!rtc.begin()) {
        Serial.println(F("RTC not found"));
        delay(3000);
    }

    // Set square wave out pin
    // SquareWaveDisable, SquareWave1Hz, SquareWave4096Hz, SquareWave8192Hz, SquareWave32768Hz
    rtc.setSquareWave(SquareWaveDisable);

    // Set date and time
    dt.tm_hour = 12;
    dt.tm_min = 34;
    dt.tm_sec = 56;
    dt.tm_mday = 29;
    dt.tm_mon = 2-1; // Month: 0..11
    dt.tm_year = 2020-1900; // Year since 1900

    // Calculate day of the week: 0=Sunday .. 6=Saturday
    time_t t = mktime(&dt);
    dt.tm_wday = localtime(&t)->tm_wday;

    // Write date/time
    while (!rtc.write(&dt)) {
        Serial.println(F("RTC write failed"));
        delay(3000);
    }
}

void loop()
{
    struct tm dt;
    
    // Get date/time
    if (!rtc.read(&dt)) {
        Serial.println(F("RTC read failed"));
    } else {
        // Print formatted date/time: "Sat Feb 29 12:34:56 2020"
        Serial.println(asctime(&dt));
    }

    // Wait some time
    delay(1000);
}