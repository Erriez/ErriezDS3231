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
 *    The example generates a 1Hz interrupt at every square wave falling edge.
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

// Create DS3231 RTC object
ErriezDS3231 rtc;

// Clock interrupt flag must be volatile
volatile bool clockInterrupt = false;

// Time variables
uint8_t hour = 0;
uint8_t minute = 0;
uint8_t second = 0;


#if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)
ICACHE_RAM_ATTR
#endif
void clockHandler()
{
    // Set global interrupt flag
    clockInterrupt = true;
}

void incrementTime()
{
    if ((hour == 0) && (minute == 0) && (second == 0)) {
        // Get date time from RTC
        if (rtc.getTime(&hour, &minute, &second)) {
            Serial.println(F("Get time failed"));
            return;
        }
    } else {
        // Increment time every second
        if (second >= 59) {
            second = 0;
            if (minute >= 59) {
                minute = 0;
                if (hour >= 23) {
                    hour = 0;
                } else {
                    hour++;
                }
            } else {
                minute++;
            }
        } else {
            second++;
        }
    }
}

void printTime()
{
    // Print time
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

void setup()
{
    // Initialize serial port
    delay(500);
    Serial.begin(115200);
    while (!Serial) {
        ;
    }
    Serial.println(F("\nErriez DS3231 time interrupt example\n"));

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
        Serial.println(F("Clock reset"));
        rtc.clockEnable();
    }

    // Attach to INT0 interrupt falling edge
    pinMode(INT_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(INT_PIN), clockHandler, FALLING);

    // Enable 1Hz square wave out is required for this example
    rtc.setSquareWave(SquareWave1Hz);

    // Disable 32kHz output pin which is not needed for this example
    rtc.outputClockPinEnable(false);

    Serial.println(F("Waiting for 1Hz time interrupt signal..."));
}

void loop()
{
    // Wait for 1Hz square wave clock interrupt
    if (clockInterrupt) {
        // Increment time by software
        incrementTime();

        // Print time
        printTime();

        // Clear interrupt flag when interrupt has been handled
        clockInterrupt = false;
    }

    // Set CPU in idle
}