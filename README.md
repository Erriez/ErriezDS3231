# DS3231 high precision I2C RTC library for Arduino

[![Build Status](https://travis-ci.org/Erriez/ErriezDS3231.svg?branch=master)](https://travis-ci.org/Erriez/ErriezDS3231)

This is a DS3231 high precision I2C RTC library for Arduino.

![DS3231](https://raw.githubusercontent.com/Erriez/ErriezDS3231/master/extras/DS3231.png)

## Library features

* Set / get time
* Set date / time with  `struct tm`
* Set / get Unix Epoch UTC 32-bit timestamp `time_t`
* Day of the week calculation
* Read temperature (0.25 degree resolution)
* Alarm 1 (second/minute/hour/day/date match) 
* Alarm 2 (minute/hour/day/date match)
* Polling and Alarm ```INT/SQW``` interrupt pin
* Control ```32kHz``` out signal (enable/disable)
* Control ```SQW``` signal (disable/1/1024/4096/8192Hz)
* Configure aging offset
* Serial terminal interface
* Full RTC register access
* Set date/time over serial with Python script

## Hardware

Any Arduino hardware with a TWI interface and ```Wire.h``` support.

![DS3231 - Arduino UNO](https://raw.githubusercontent.com/Erriez/ErriezDS3231/master/extras/DS3231_Arduino_UNO.png)

### ESP32 notes

**ESP32 problem:** The Arduino IDE | Board manager installs an old version ```1.0.0``` from `https://dl.espressif.com/dl/package_esp32_index.json` which contains a broken I2C repeated start. Generating a repeated start with ```Wire.endTransmission(false);``` results in reading zero's from any I2C device and is not a problem of this library.

**Solution:** Use the Git master branch (or a newer release when available) to solve this problem as described on:
https://github.com/espressif/arduino-esp32/blob/master/docs/arduino-ide/windows.md.

## Pins

| Pins board - DS3231            | VCC  | GND  |    SDA     |    SCL     |    SQW     |
| ------------------------------ | :--: | :--: | :--------: | :--------: | :--------: |
| Arduino UNO (ATMega328 boards) |  5V  | GND  |     A4     |     A5     | D2 (INT0)  |
| Arduino Mega2560               |  5V  | GND  |    D20     |    D21     | D2 (INT4)  |
| Arduino Leonardo               |  5V  | GND  |     D2     |     D3     | D7 (INT6)  |
| Arduino DUE (ATSAM3X8E)        | 3V3  | GND  |     20     |     21     |     2      |
| ESP8266                        | 3V3  | GND  | GPIO4 (D2) | GPIO5 (D1) | GPIO0 (D3) |
| ESP32                          | 3V3  | GND  |   GPIO21   |   GPIO22   |   GPIO0    |

Note: Tested ESP8266 / ESP32 boards:

* **ESP8266 boards**: ESP12E / WeMos D1 & R2 / Node MCU v2 / v3
* **ESP32 boards:** WeMos LOLIN32 / LOLIN D32

Other unlisted MCU's may work, but are not tested.

## Examples

Arduino IDE | Examples | Erriez DS3231 RTC:

* [AgingOffset](https://github.com/Erriez/ErriezDS3231/blob/master/examples/ErriezDS3231AgingOffset/ErriezDS3231AgingOffset.ino) Aging offset programming.
* [AlarmInterrupt](https://github.com/Erriez/ErriezDS3231/blob/master/examples/ErriezDS3231AlarmInterrupt/ErriezDS3231AlarmInterrupt.ino) Alarm with interrupts.
* [AlarmPolling](https://github.com/Erriez/ErriezDS3231/blob/master/examples/ErriezDS3231AlarmPolling/ErriezDS3231AlarmPolling.ino) Alarm polled.
* [DateStrings](https://github.com/Erriez/ErriezDS3231/blob/master/examples/ErriezDS3231DateStrings/ErriezDS3231DateStrings.ino) Date strings in flash example.
* [Read](https://github.com/Erriez/ErriezDS3231/blob/master/examples/ErriezDS3231Read/ErriezDS3231Read.ino) Simple RTC read example.
* [ReadTimeInterrupt](https://github.com/Erriez/ErriezDS3231/blob/master/examples/ErriezDS3231ReadTimeInterrupt/ErriezDS3231ReadTimeInterrupt.ino)  Read time with 1Hz SQW interrupt. (Highly recommended)
* [SetDateTime](https://github.com/Erriez/ErriezDS3231/blob/master/examples/ErriezDS3231SetDateTime/ErriezDS3231SetDateTime.ino) Set date time. (Must be started first)
* [Temperature](https://github.com/Erriez/ErriezDS3231/blob/master/examples/ErriezDS3231Temperature/ErriezDS3231Temperature.ino) Temperature.
* [Terminal](https://github.com/Erriez/ErriezDS3231/blob/master/examples/ErriezDS3231Terminal/ErriezDS3231Terminal.ino) Advanced terminal interface with [set date/time Python](https://github.com/Erriez/ErriezDS3231/blob/master/examples/ErriezDS3231Terminal/ErriezDS3231Terminal.py) script.
* [Test](https://github.com/Erriez/ErriezDS3231/blob/master/examples/ErriezDS3231Test/ErriezDS3231Test.ino) Regression test.


## Documentation

- [Doxygen online HTML](https://erriez.github.io/ErriezDS3231) 
- [Doxygen PDF](https://github.com/Erriez/ErriezDS3231/blob/master/extras/ErriezDS3231.pdf)
- [DS3231 datasheet](https://github.com/Erriez/ErriezDS3231/blob/master/extras/DS3231.pdf)


## Usage

**Initialization**

```c++
#include <Wire.h>
#include <ErriezDS3231.h>

// Create DS3231 RTC object
DS3231 rtc;

void setup()
{
    // Initialize TWI with a 100kHz (default) or 400kHz clock
    Wire.begin();
    Wire.setClock(400000);
    
    // Initialize RTC
    while (!ds3231.begin()) {
        // Error: Could not detect DS3231 RTC, retry after some time
        delay(3000);
    }
}
```

**Check oscillator status at startup**

```c++
// Check oscillator status
if (ds3231.isOscillatorStopped()) {
    // Error: DS3231 RTC oscillator stopped. Date/time cannot be trusted. 
    // Set new date/time before reading date/time.
    
    // Start oscillator with date/time "Sun Jan 1 2000 0:00:00"
    ds3231.oscillatorEnable(true);
}
```

**Set time**

```c++
// Write time to RTC
if (!ds3231.setTime(12, 0, 0)) {
    // Error: Write time failed
}
```

**Get time**

```c++
uint8_t hour;
uint8_t minute;
uint8_t second;

// Read time from RTC
if (!ds3231.getTime(&hour, &minute, &second)) {
    // Error: Read time failed
}
```

**Set date time**

```c++
// Create and initialize date time object
struct tm dt;

dt.tm_sec = 0,      // Seconds 0..59
dt.tm_min = 36,     // Minutes 0..59
dt.tm_hour = 21,    // Hours 0..23
dt.tm_wday = 6;     // Day of the week: 0 = Sunday .. 6=Saturday
dt.tm_mday = 29;    // Day of the month 1..31
dt.tm_mon = 7;      // Month 0..11
dt.tm_year = 2020-1900; // Year starting at 1900

// Write date/time to RTC
if (!ds3231.write(&dt)) {
    // Error: Write date/time failed
}
```

**Get date time**

```c++
struct tm dt;

// Read date/time from RTC
if (!ds3231.read(&dt)) {
    // Error: Read date/time failed
}
```

**Get Epoch Unix UTC time**

```c++
// Get Unix epoch UTC time
time_t t = ds3231.getEpoch();
```

**Get temperature**

```c++
int8_t temperature = 0;
uint8_t fraction = 0;

// Force temperature conversion
// Without this call, it takes 64 seconds before the temperature is updated.
if (!ds3231.startTemperatureConversion()) {
    // Error: Start temperature conversion failed 
}

// Read temperature
if (!ds3231.getTemperature(&temperature, &fraction)) {
    // Error: Get temperature failed
}

// Print temperature. The output below is for example: 28.25C
Serial.print(temperature);
Serial.print(F("."));
Serial.print(fraction);
Serial.println(F("C"));
```

**Program Alarm 1**

Note: Alarm 1 and Alarm 2 have different behavior. Please refer to the documentation which ```Alarm1Type``` and ```Alarm2Type``` are supported. Some examples:

```c++
// Generate alarm 1 every second
ds3231.setAlarm1(Alarm1EverySecond, 0, 0, 0, 0);

// Generate alarm 1 every minute and second match
ds3231.setAlarm1(Alarm1EverySecond, 0, 0, 45, 30);

// Generate alarm 1 every day, hour, minute and second match
ds3231.setAlarm1(Alarm1MatchDay, 
              1,  // Alarm day match (1 = Monday)
              12, // Alarm hour match
              45, // Alarm minute match
              30  // Alarm second match
);
```

**Program Alarm 2**

```c++
// Generate alarm 2 every minute
ds3231.setAlarm2(Alarm2EveryMinute, 0, 0, 0);

// Generate alarm 2 every hour, minute match
ds3231.setAlarm2(Alarm2MatchHours, 0, 23, 59);

// Generate alarm 2 every date, hour, minute match
ds3231.setAlarm2(Alarm2MatchDate, 28, 7, 0);
```

**Alarm polling**

Note: The ```INT``` pin changes to low when an Alarm 1 or Alarm 2 match occurs and and the interrupt is enabled. The pin remains low until both alarm flags are cleared by the application.

```c++
// Poll alarm 1 flag
if (ds3231.getAlarmFlag(Alarm1)) {
    // Handle Alarm 1
    
    // Clear alarm 1 flag
	ds3231.clearAlarmFlag(Alarm1);
}

// Poll alarm 2 flag
if (ds3231.getAlarmFlag(Alarm2)) {
    // Handle Alarm 2
    
    // Clear alarm 2 flag
	ds3231.clearAlarmFlag(Alarm2);
}
```

**Alarm interrupt**

Note: Enabling interrupt will disable the ```SQW``` output signal.

```c++
// Uno, Nano, Mini, other 328-based: pin D2 (INT0) or D3 (INT1)
#define INT_PIN     2

// Alarm interrupt flag must be volatile
volatile bool alarmInterrupt = false;


#if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)
ICACHE_RAM_ATTR
#endif
void alarmHandler()
{
    // Set global interrupt flag
    alarmInterrupt = true;
}

void setup()
{
    ...

    // Attach to INT0 interrupt falling edge
    pinMode(INT_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(INT_PIN), alarmHandler, FALLING);
    
    // Enable Alarm 1 and 2 interrupts
    ds3231.alarmInterruptEnable(Alarm1, true);
    ds3231.alarmInterruptEnable(Alarm2, true);
}

void loop()
{
    // Check global alarm interrupt flag
    if (alarmInterrupt) {
        if (ds3231.getAlarmFlag(Alarm1)) {
            // Handle alarm 1
            
            // Clear alarm 1 interrupt
            ds3231.clearAlarmFlag(Alarm1);
        }
        
        if (ds3231.getAlarmFlag(Alarm2)) {
            // Handle alarm 2
            
            // Clear alarm 2 interrupt
            ds3231.clearAlarmFlag(Alarm2);
        }
    }
}
```

**32kHz clock out**

Enable or disable ```32kHz``` output pin.

```c++
ds3231.outputClockPinEnable(true);  // Enable 
ds3231.outputClockPinEnable(false);	// Disable
```

**Square Wave Out (SQW)**

Note: Enabling ```SQW``` pin will disable the alarm ```INT``` signal.

```c++
ds3231.setSquareWave(SquareWaveDisable);	// Disable
ds3231.setSquareWave(SquareWave1Hz);		// 1Hz
ds3231.setSquareWave(SquareWave1024Hz);	// 1024Hz
ds3231.setSquareWave(SquareWave4096Hz);	// 4096Hz
ds3231.setSquareWave(SquareWave8192Hz);	// 8192Hz
```


## API changes v1.0.1 to v2.0.0

The API has been changed to make RTC libraries compatible with libc `time.h`. This makes it easier
to calculate with date/time and port the application to different platforms. See changes below:

| v1.0.1                           | v2.0.0                                                       |
| -------------------------------- | ------------------------------------------------------------ |
| `DS3231_DateTime`                | `struct tm`                                                  |
| Function returns `true`: failure | Function returns `false`: failure                            |
|                                  | `clearOscillatorStopFlag()` merged into `oscillatorEnable()` |
| `setDateTime()`                  | `bool write(struct tm *dt)`                                  |
| `getDateTime()`                  | `bool read(struct tm *dt)`                                   |
| `getEpochTime()`                 | `time_t getEpoch()`                                          |
|                                  | `bool setEpoch(time_t t)`                                    |
|                                  | `bool setDateTime(uint8_t hour, uint8_t min, uint8_t sec,                uint8_t mday, uint8_t mon, uint16_t year, uint8_t wday)` |
|                                  | `ErriezDS3231Debug` class removed to reduce flash size       |


## Library dependencies

* ```Wire.h```
* ```Terminal.ino``` requires ```ErriezSerialTerminal``` library.


## Library installation

Please refer to the [Wiki](https://github.com/Erriez/ErriezArduinoLibrariesAndSketches/wiki) page.


## Other Arduino Libraries and Sketches from Erriez

* [Erriez Libraries and Sketches](https://github.com/Erriez/ErriezArduinoLibrariesAndSketches)
