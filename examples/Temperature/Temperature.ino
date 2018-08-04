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
 * \brief DS3231 high accurate RTC temperature example for Arduino
 * \details
 *      Source:         https://github.com/Erriez/ErriezDS3231
 *      Documentation:  https://erriez.github.io/ErriezDS3231
 */

#include <Wire.h>

#include <ErriezDS3231.h>

// Create DS3231 RTC object
static DS3231 rtc;

// Function prototypes
static void printTemperature();


void setup()
{
    // Initialize serial port
    Serial.begin(115200);
    while (!Serial) {
        ;
    }
    Serial.println(F("DS3231 RTC temperature example\n"));

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

    Serial.println(F("Temperature:"));
}

void loop()
{
    // Print temperature
    printTemperature();

    // Wait some time
    delay(5000);
}

static void printTemperature()
{
    int8_t temperature;
    uint8_t fraction;

    // Force temperature conversion
    // Without this call, it takes 64 seconds before the temperature is updated.
    rtc.startTemperatureConversion();

    // Read temperature
    rtc.getTemperature(&temperature, &fraction);
    Serial.print(temperature);
    Serial.print(F("."));
    Serial.print(fraction);
    Serial.println(F("C"));
}
