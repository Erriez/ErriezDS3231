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
 * \brief DS3231 high accurate RTC aging offset example for Arduino
 * \details
 *      Source:         https://github.com/Erriez/ErriezDS3231
 *      Documentation:  https://erriez.github.io/ErriezDS3231
 */

#include <Wire.h>

#include <ErriezDS3231.h>

// Set 8-bit signed aging offset value -127...127 for clock speed compensation 0.1ppm per LSB
#define AGING_OFFSET_VALUE      10

// Create DS3231 RTC object
static DS3231 rtc;

// Function prototypes
static void setAgingOffset();
static void printAgingOffset();
static void printTime();


void setup()
{
    // Initialize serial port
    Serial.begin(115200);
    while (!Serial) {
        ;
    }
    Serial.println(F("DS3231 RTC aging offset example\n"));

    // Initialize TWI
    Wire.begin();
    Wire.setClock(400000);

    // Initialize RTC
    while (rtc.begin()) {
        Serial.println(F("Error: DS3231 not detected"));
        delay(3000);
    }

    // Set aging offset
    setAgingOffset();

    // Print aging offset register
    printAgingOffset();

    // Print current date and time only when clock is running
    if (rtc.isOscillatorStopped()) {
        Serial.println(F("Error: DS3231 RTC oscillator stopped. Program new date/time."));
        while (1) {
            ;
        }
    }

    Serial.println(F("RTC date and time:"));
}

void loop()
{
    printTime();
    delay(1000);
}

static void setAgingOffset()
{
    Serial.print(F("Set aging offset: "));
    Serial.println(AGING_OFFSET_VALUE, DEC);

    // Set aging offset
    rtc.setAgingOffset(AGING_OFFSET_VALUE);
}

static void printAgingOffset()
{
    int8_t agingOffset;

    // Get aging offset
    agingOffset = rtc.getAgingOffset();

    Serial.print(F("Aging offset: "));
    Serial.println(agingOffset, DEC);
}

static void printTime()
{
    DS3231_DateTime dt;

    // Get date time from RTC
    if (rtc.getDateTime(&dt)) {
        Serial.println(F("Error: Read date time failed"));
        return;
    }

    Serial.print(dt.hour);
    Serial.print(F(":"));
    Serial.print(dt.minute);
    Serial.print(F(":"));
    Serial.println(dt.second);
}
