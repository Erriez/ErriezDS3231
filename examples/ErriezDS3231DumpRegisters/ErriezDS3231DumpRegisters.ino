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
 * \brief DS3231 RTC dump registers example for Arduino
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
    // Initialize serial port
    delay(500);
    Serial.begin(115200);
    while (!Serial) {
        ;
    }
    Serial.println(F("\nErriez DS3231 dump register example\n"));

    // Initialize TWI
    Wire.begin();
    Wire.setClock(400000);

    // Initialize RTC
    while (!rtc.begin()) {
        Serial.println(F("RTC not found"));
        delay(3000);
    }

    // Enable RTC clock
    if (!rtc.isRunning()) {
        rtc.clockEnable();
    }
}

void loop()
{
    uint8_t buf[DS3231_NUM_REGS];

    // Read all registers
    if (!rtc.readBuffer(0, buf, sizeof(buf))) {
        Serial.println(F("Read buffers failed"));
        return;
    }

    // Print all registers
    Serial.println(F("Registers: "));
    for (uint8_t i = 0; i < sizeof(buf); i++) {
        Serial.print(F("  "));
        Serial.print(i);
        Serial.print(F(": 0x"));
        if (buf[i] < 0x10) {
            Serial.print(F("0"));
        }
        Serial.println(buf[i], HEX);
    }

    delay(1000);
}