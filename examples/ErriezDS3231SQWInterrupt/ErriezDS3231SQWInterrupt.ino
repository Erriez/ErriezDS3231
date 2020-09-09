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
 * \brief DS3231 high accurate RTC 1Hz square wave interrupt example for Arduino
 * \details
 *    Source:         https://github.com/Erriez/ErriezDS3231
 *    Documentation:  https://erriez.github.io/ErriezDS3231
 *
 *    Connect the nINT/SQW pin to an Arduino interrupt pin
 *
 *    The LED blinks at the 1Hz Square Wave pin falling edge.
 */

#include <Wire.h>

#include <ErriezDS3231.h>

// Uno, Nano, Mini, other 328-based: pin D2 (INT0) or D3 (INT1)
// DUE: Any digital pin
// Leonardo: pin D7 (INT4)
// ESP8266 / NodeMCU / WeMos D1&R2: pin D3 (GPIO0)
#if defined(__AVR_ATmega328P__) || defined(ARDUINO_SAM_DUE)
#define INT_PIN     2
#elif defined(ARDUINO_AVR_LEONARDO)
#define INT_PIN     7
#else
#define INT_PIN     0 // GPIO0 pin for ESP8266 / ESP32 targets
#endif

// LED pin
#define LED_PIN     LED_BUILTIN

// Create DS3231 RTC object
ErriezDS3231 rtc;

// SQW interrupt flag must be volatile
volatile bool sqwInterrupt = false;


#if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)
ICACHE_RAM_ATTR
#endif
void sqwHandler()
{
    // Set global interrupt flag
    sqwInterrupt = true;
}

void setup()
{
    // Initialize serial port
    delay(500);
    Serial.begin(115200);
    while (!Serial) {
        ;
    }
    Serial.println(F("\nErriez DS3231 SQW blink example\n"));

    // Initialize TWI
    Wire.begin();
    Wire.setClock(400000);

    // Attach to INT0 interrupt falling edge
    pinMode(INT_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(INT_PIN), sqwHandler, FALLING);

    // Initialize RTC
    while (!rtc.begin()) {
        Serial.println(F("RTC not found"));
        delay(3000);
    }

    // Enable RTC clock
    if (!rtc.isRunning()) {
        rtc.clockEnable();
    }

    // Set 1Hz square wave out:
    //      SquareWaveDisable
    //      SquareWave1Hz
    //      SquareWave1024Hz
    //      SquareWave4096Hz
    //      SquareWave8192Hz
    rtc.setSquareWave(SquareWave1Hz);

    // Initialize LED
    pinMode(LED_PIN, OUTPUT);
}

void loop()
{
    // Wait for 1Hz square wave clock interrupt
    if (sqwInterrupt) {
        // Toggle LED
        Serial.println(F("Toggle LED"));
        digitalWrite(LED_PIN, !digitalRead(LED_PIN));

        // Clear interrupt flag when interrupt has been handled
        sqwInterrupt = false;
    }

    // Todo: Set CPU in idle
}