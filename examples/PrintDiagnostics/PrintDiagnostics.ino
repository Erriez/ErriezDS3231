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
 * \brief DS3231 high precision RTC print diagnostics example for Arduino
 * \details
 *      Source:         https://github.com/Erriez/ErriezDS3231
 *      Documentation:  https://erriez.github.io/ErriezDS3231
 */

#include <Wire.h>

#include <ErriezDS3231_debug.h>

// Create debug DS3231 RTC object
DS3231Debug rtc;


void setup()
{
    // Initialize serial port
    Serial.begin(115200);
    while (!Serial) {
        ;
    }

    Serial.println(F("DS3231 diagnostics example\n"));

    // Initialize TWI
    Wire.begin();
    Wire.setClock(400000);

    // Initialize RTC
    rtc.begin();
    rtc.dumpRegisters(&Serial, false);
    rtc.dumpRegisters(&Serial, true);
    rtc.printDiagnostics(&Serial);
}

void loop()
{
    DS3231_DateTime dt;
    int8_t temperature;
    uint8_t fraction;

    rtc.getDateTime(&dt);

    Serial.print(dt.hour);
    Serial.print(F(":"));
    Serial.print(dt.minute);
    Serial.print(F(":"));
    Serial.print(dt.second);
    Serial.print(F(" "));

    rtc.getTemperature(&temperature, &fraction);
    Serial.print(temperature);
    Serial.print(F("."));
    Serial.print(fraction);
    Serial.println(F("C"));

    delay(1000);
}
