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
 * \brief DS3231 RTC set and get date/time example for Arduino
 * \details
 *    Source:         https://github.com/Erriez/ErriezDS3231
 *    Documentation:  https://erriez.github.io/ErriezDS3231
 */

#include <Wire.h>
#include <ErriezDS3231.h>

// Create RTC object
ErriezDS3231 rtc;

#define DATE_STRING_SHORT           3

// Month names in flash
const char monthNames_P[] PROGMEM = "JanFebMarAprMayJunJulAugSepOctNovDec";

// Day of the week names in flash
const char dayNames_P[] PROGMEM= "SunMonTueWedThuFriSat";


void setup()
{
    // Initialize serial port
    delay(500);
    Serial.begin(115200);
    while (!Serial) {
        ;
    }
    Serial.println(F("\nErriez DS3231 read example"));

    // Initialize I2C
    Wire.begin();
    Wire.setClock(100000);

    // Initialize RTC
    while (!rtc.begin()) {
        Serial.println(F("RTC not found"));
        delay(3000);
    }

    // Set date/time: 12:34:56 31 December 2020 Sunday
    if (!rtc.setDateTime(12, 34, 56,  31, 12, 2020, 0)) {
        Serial.println(F("Set date/time failed"));
        return;
    }

    // Set square wave out pin
    // SquareWaveDisable, SquareWave1Hz, SquareWave4096Hz, SquareWave8192Hz, SquareWave32768Hz
    rtc.setSquareWave(SquareWaveDisable);
}

void loop()
{
    char name[DATE_STRING_SHORT + 1];
    uint8_t hour;
    uint8_t min;
    uint8_t sec;
    uint8_t mday;
    uint8_t mon;
    uint16_t year;
    uint8_t wday;

    // Read date/time
    if (!rtc.getDateTime(&hour, &min, &sec, &mday, &mon, &year, &wday)) {
        Serial.println(F("Read date/time failed"));
        return;
    }

    // Print day week
    strncpy_P(name, &(dayNames_P[wday * DATE_STRING_SHORT]), DATE_STRING_SHORT);
    name[DATE_STRING_SHORT] = '\0';
    Serial.print(name);
    Serial.print(F(" "));

    // Print month
    strncpy_P(name, &(monthNames_P[(mon - 1) * DATE_STRING_SHORT]), DATE_STRING_SHORT);
    name[DATE_STRING_SHORT] = '\0';
    Serial.print(name);
    Serial.print(F(" "));

    // Print day month
    Serial.print(mday);
    Serial.print(F(" "));

    // Print time
    Serial.print(hour);
    Serial.print(F(":"));
    if (min < 10) {
        Serial.print(F("0"));
    }
    Serial.print(min);
    Serial.print(F(":"));
    if (sec < 10) {
        Serial.print(F("0"));
    }
    Serial.print(sec);
    Serial.print(F(" "));

    // Print year
    Serial.println(year);

    // Wait a second
    delay(1000);
}