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
 * \brief DS3231 high accurate RTC read time polled example for Arduino
 * \details
 *      Source:         https://github.com/Erriez/ErriezDS3231
 *      Documentation:  https://erriez.github.io/ErriezDS3231
 */

#include <Wire.h>

#include <ErriezDS3231.h>

// Create DS3231 RTC object
static DS3231 rtc;

// Function prototypes
static void printTime();


void setup()
{
    // Initialize serial port
    Serial.begin(115200);
    while (!Serial) {
        ;
    }
    Serial.println(F("DS3213 time polled example"));

    // Initialize TWI
    Wire.begin();
    Wire.setClock(400000);

    // Initialize RTC
    while (rtc.begin()) {
        Serial.print(F("."));
        delay(3000);
    }

    // Check oscillator status
    if (rtc.isOscillatorStopped()) {
        Serial.println(F("OSF"));
        while (1) {
            ;
        }
    }
}

void loop()
{
    // Just an example to print the date and time every second. Recommended code is to use the 1Hz
    // square wave out pin with an interrupt.
    printTime();
    delay(1000);
}

static void printTime()
{
    uint8_t hour, minute, second;

    // Read RTC date and time from RTC
    if (rtc.getTime(&hour, &minute, &second)) {
        return;
    }

    // Print time
    Serial.print(hour);
    Serial.print(F(":"));
    Serial.print(minute);
    if (minute < 10) {
        Serial.print(F("0"));
    }
    Serial.print(F(":"));
    if (second < 10) {
        Serial.print(F("0"));
    }
    Serial.println(second);
}
