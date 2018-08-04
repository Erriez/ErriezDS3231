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
 * \brief DS3231 high accurate RTC getting started example for Arduino
 * \details
 *    Connect the nINT/SQW pin to an Arduino interrupt pin.
 *    Source: https://github.com/Erriez/ErriezDS3231
 */

#include <Wire.h>

#include <ErriezDS3231.h>

// Create DS3231 RTC object
static DS3231 rtc;

// Create date time object
static DS3231_DateTime dt;

// Define days of the week in flash
const char day_1[] PROGMEM = "Monday";
const char day_2[] PROGMEM = "Tuesday";
const char day_3[] PROGMEM = "Wednesday";
const char day_4[] PROGMEM = "Thursday";
const char day_5[] PROGMEM = "Friday";
const char day_6[] PROGMEM = "Saturday";
const char day_7[] PROGMEM = "Sunday";

const char* const day_week_table[] PROGMEM = {
        day_1, day_2, day_3, day_4, day_5, day_6, day_7
};

// Define months in flash
const char month_1[] PROGMEM = "January";
const char month_2[] PROGMEM = "February";
const char month_3[] PROGMEM = "March";
const char month_4[] PROGMEM = "April";
const char month_5[] PROGMEM = "May";
const char month_6[] PROGMEM = "June";
const char month_7[] PROGMEM = "July";
const char month_8[] PROGMEM = "August";
const char month_9[] PROGMEM = "September";
const char month_10[] PROGMEM = "October";
const char month_11[] PROGMEM = "November";
const char month_12[] PROGMEM = "December";

const char* const month_table[] PROGMEM = {
        month_1, month_2, month_3, month_4, month_5, month_6,
        month_7, month_8, month_9, month_10, month_11, month_12
};

// Function prototypes
static void printDateTime();
static void printTemperature();


void setup()
{
    // Initialize serial port
    Serial.begin(115200);
    while (!Serial) {
        ;
    }
    Serial.println(F("DS3231 RTC getting started example\n"));

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

    // Disable 32kHz clock output pin
    rtc.outputClockPinEnable(false);

    // Disable square wave out
    rtc.setSquareWave(SquareWaveDisable);

    Serial.println(F("RTC epoch/date/time/temperature:"));
}

void loop()
{
    // Read RTC date and time from RTC
    if (rtc.getDateTime(&dt)) {
        Serial.println(F("Error: Read date time failed"));
        return;
    }

    // Print date time on serial port
    printDateTime();

    // Print temperature every minute
    if (dt.second == 0) {
        printTemperature();
    }

    // Just an example to print the date and time every second. Recommended code is to use the 1Hz
    // square wave out pin with an interrupt instead of polling.
    delay(1000);
}

static void printDateTime()
{
    char buf[32];

    // Print Unix Epoch time
    snprintf(buf, sizeof(buf), "%lu", rtc.getEpochTime(&dt));
    Serial.print(buf);
    Serial.print(F("  "));

    // Print day of the week, day month, month and year
    strncpy_P(buf, (char *)pgm_read_dword(&(day_week_table[dt.dayWeek - 1])), sizeof(buf));
    Serial.print(buf);
    Serial.print(F(" "));
    Serial.print(dt.dayMonth);
    Serial.print(F(" "));
    strncpy_P(buf, (char *)pgm_read_dword(&(month_table[dt.month - 1])), sizeof(buf));
    Serial.print(buf);
    Serial.print(F(" "));
    Serial.print(dt.year);
    Serial.print(F("  "));

    // Print time
    snprintf(buf, sizeof(buf), "%d:%02d:%02d", dt.hour, dt.minute, dt.second);
    Serial.println(buf);
}

static void printTemperature()
{
    int8_t temperature;
    uint8_t fraction;

#if 0
    // Force temperature conversion only when reading temperature faster than 64 seconds
    if (rtc.startTemperatureConversion() != Success) {
        Serial.println(F("Temperature conversion busy"));
        return;
    }
#endif

    // Read temperature
    rtc.getTemperature(&temperature, &fraction);
    Serial.print(F("Temperature: "));
    Serial.print(temperature);
    Serial.print(F("."));
    Serial.print(fraction);
    Serial.println(F("C"));
}
