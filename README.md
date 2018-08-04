# DS3231 high precision I2C RTC library for Arduino

[![Build Status](https://travis-ci.org/Erriez/ErriezDS3231.svg?branch=master)](https://travis-ci.org/Erriez/ErriezDS3231)

This is an advanced DS3231 high precision I2C RTC library for Arduino.

![DS3231](https://raw.githubusercontent.com/Erriez/ErriezDS3231/master/extras/DS3231.png)

## Library features

* Read time
* Set time
* Read date and time
* Set date and time
* Read Unix Epoch UTC 32-bit timestamp
* Read temperature (0.25 degree resolution)
* Alarm 1 (second/minute/hour/day/date match) 
* Alarm 2 (minute/hour/day/date match)
* Polling and Alarm ```INT/SQW``` interrupt pin
* Control ```32kHz``` out signal (enable/disable)
* Control ```SQW``` signal (disable/1/1024/4096/8192Hz)
* Configure aging offset
* Serial terminal interface
* Full RTC register access
* Easy debug functionality
* Basic and advanced examples
* Set date/time over serial with Python script
* Low RAM footprint:
  * ```sizeof(DS3231)```: 1 Byte RTC object.
  * ```sizeof(DS3231_DateTime)```: 8 Bytes date/time object.
* Full Doxygen documentation

## Hardware

Any Arduino hardware with a TWI interface and ```Wire.h``` support.

![DS3231 - Arduino UNO](https://raw.githubusercontent.com/Erriez/ErriezDS3231/master/extras/DS3231_Arduino_UNO.png)

## Pins

| DS3231 | Arduino UNO / Nano / Micro / Pro Micro | Mega2560  | Leonardo  | ESP8266 / WeMos D1 & R2 / Node MCU |
| ------ | -------------------------------------- | --------- | --------- | ---------------------------------- |
| VCC    | 5V                                     | 5V        | 5V        | 3V3                                |
| GND    | GND                                    | GND       | GND       | GND                                |
| SDA    | A4                                     | D20       | D2        | D2 (GPIO4)                         |
| CLK    | A5                                     | D21       | D3        | D1 (GPIO5)                         |
| SQW    | D2 (INT0)                              | D2 (INT4) | D7 (INT6) | D3 (GPIO0)                         |


## Examples

Arduino IDE | Examples | Erriez DS3231 RTC:

* [AgingOffset](https://github.com/Erriez/ErriezDS3231/blob/master/examples/AgingOffset/AgingOffset.ino) Aging offset programming.
* [AlarmInterrupt](https://github.com/Erriez/ErriezDS3231/blob/master/examples/AlarmInterrupt/AlarmInterrupt.ino) Alarm with interrupts.
* [AlarmPolling](https://github.com/Erriez/ErriezDS3231/blob/master/examples/AlarmPolling/AlarmPolling.ino) Alarm polled.
* [GettingStarted](https://github.com/Erriez/ErriezDS3231/blob/master/examples/GettingStarted/GettingStarted.ino) Getting started example which contains most date/time/temperature features.
* [Minimum](https://github.com/Erriez/ErriezDS3231/blob/master/examples/Minimum/Minimum.ino) Minimum example to read time.
* [PrintDiagnostics](https://github.com/Erriez/ErriezDS3231/blob/master/examples/PrintDiagnostics/PrintDiagnostics.ino) Print diagnostics and registers.
* [ReadTimeInterrupt](https://github.com/Erriez/ErriezDS3231/blob/master/examples/ReadTimeInterrupt/ReadTimeInterrupt.ino)  Read time with 1Hz SQW interrupt. (Highly recommended)
* [ReadTimePolled](https://github.com/Erriez/ErriezDS3231/blob/master/examples/ReadTimePolled/ReadTimePolled.ino) Read time polled.
* [SetDateTime](https://github.com/Erriez/ErriezDS3231/blob/master/examples/SetDateTime/SetDateTime.ino) Set date time. (Must be started first)
* [Temperature](https://github.com/Erriez/ErriezDS3231/blob/master/examples/Temperature/Temperature.ino) Temperature.
* [Terminal](https://github.com/Erriez/ErriezDS3231/blob/master/examples/Terminal/Terminal.ino) Advanced terminal interface with [set date/time Python](https://github.com/Erriez/ErriezDS3231/blob/master/examples/Terminal/Terminal.py) script.


## Documentation

- [Doxygen online HTML](https://erriez.github.io/ErriezDS3231) 
- [Doxygen PDF](https://github.com/Erriez/ErriezDS3231/raw/gh-pages/latex/ErriezDS3231.pdf)
- [DS3231 datasheet](https://github.com/Erriez/ErriezDS3231/blob/master/extras/DS3231.pdf)


## Usage

**Initialization**

```c++
#include <Wire.h>
#include <ErriezDS3231.h>

// Create DS3231 RTC object
static DS3231 rtc;


void setup()
{
    // Initialize TWI with a 100kHz (default) or 400kHz clock
    Wire.begin();
    Wire.setClock(400000);
    
    // Initialize RTC
    while (rtc.begin()) {
        // Error: Could not detect DS3231 RTC, retry after some time
        delay(3000);
    }
}
```

**Check oscillator status at startup**

```c++
// Check oscillator status
if (rtc.isOscillatorStopped()) {
    // Error: DS3231 RTC oscillator stopped. Date/time cannot be trusted. 
    // Set new date/time before reading date/time.
    while (1) {
        ;
    }
}
```

**Set time**

```c++
// Write time to RTC
if (rtc.setTime(12, 0, 0)) {
    // Error: Write time failed
}
```

**Get time**

```c++
uint8_t hour;
uint8_t minute;
uint8_t second;

// Read time from RTC
if (rtc.getTime(&hour, &minute, &second)) {
    // Error: Read time failed
}
```

**Set date time**

```c++
// Create and initialize date time object
static DS3231_DateTime dt = {
    .second = 0,
    .minute = 36,
    .hour = 21,
    .dayWeek = 7, // 1 = Monday
    .dayMonth = 29,
    .month = 7,
    .year = 2018
};

// Set new RTC date/time
rtc.setDateTime(&dt);
```

**Get date time**

```c++
DS3231_DateTime dt;

// Read RTC date and time from RTC
if (rtc.getDateTime(&dt)) {
    // Error: Read date time failed
}
```

**Get Epoch Unix UTC time**

```c++
uint32_t epoch;

// Read date/time from RTC
if (rtc.getDateTime(&dt)) {
    // Error: Read date/time failed
    return;
}

// Convert date/time to 32-bit epoch time
epoch = rtc.getEpochTime(&dt));
```

**Get temperature**

```c++
int8_t temperature;
uint8_t fraction;

// Force temperature conversion
// Without this call, it takes 64 seconds before the temperature is updated.
rtc.startTemperatureConversion();

// Read temperature
rtc.getTemperature(&temperature, &fraction);

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
rtc.setAlarm1(Alarm1EverySecond, 0, 0, 0, 0);

// Generate alarm 1 every minute and second match
rtc.setAlarm1(Alarm1EverySecond, 0, 0, 45, 30);

// Generate alarm 1 every day, hour, minute and second match
rtc.setAlarm1(Alarm1MatchDay, 
              1,  // Alarm day match (1 = Monday)
              12, // Alarm hour match
              45, // Alarm minute match
              30  // Alarm second match
);
```

**Program Alarm 2**

```c++
// Generate alarm 2 every minute
rtc.setAlarm2(Alarm2EveryMinute, 0, 0, 0);

// Generate alarm 2 every hour, minute match
rtc.setAlarm2(Alarm2MatchHours, 0, 23, 59);

// Generate alarm 2 every date, hour, minute match
rtc.setAlarm2(Alarm2MatchDate, 28, 7, 0);
```

**Alarm polling**

Note: The ```INT``` pin changes to low when an Alarm 1 or Alarm 2 match occurs and and the interrupt is enabled. The pin remains low until both alarm flags are cleared by the application.

```c++
// Poll alarm 1 flag
if (rtc.getAlarmFlag(Alarm1)) {
    // Handle Alarm 1
    
    // Clear alarm 1 flag
	rtc.clearAlarmFlag(Alarm1);
}

// Poll alarm 2 flag
if (rtc.getAlarmFlag(Alarm2)) {
    // Handle Alarm 2
    
    // Clear alarm 2 flag
	rtc.clearAlarmFlag(Alarm2);
}
```

**Alarm interrupt**

Note: Enabling interrupt will disable the ```SQW``` output signal.

```c++
// Uno, Nano, Mini, other 328-based: pin D2 (INT0) or D3 (INT1)
#define INT_PIN     2

// Alarm interrupt flag must be volatile
static volatile bool alarmInterrupt = false;


static void alarmHandler()
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
    rtc.alarmInterruptEnable(Alarm1, true);
    rtc.alarmInterruptEnable(Alarm2, true);
}

void loop()
{
    // Check global alarm interrupt flag
    if (alarmInterrupt) {
        if (rtc.getAlarmFlag(Alarm1)) {
            // Handle alarm 1
            
            // Clear alarm 1 interrupt
            rtc.clearAlarmFlag(Alarm1);
        }
        
        if (rtc.getAlarmFlag(Alarm2)) {
            // Handle alarm 2
            
            // Clear alarm 2 interrupt
            rtc.clearAlarmFlag(Alarm2);
        }
    }
}
```

**32kHz clock out**

Enable or disable ```32kHz``` output pin.

```c++
rtc.outputClockPinEnable(true);		// Enable 
rtc.outputClockPinEnable(false);	// Disable
```

**Square Wave Out (SQW)**

Note: Enabling ```SQW``` pin will disable the alarm ```INT``` signal.

```c++
rtc.setSquareWave(SquareWaveDisable);	// Disable
rtc.setSquareWave(SquareWave1Hz);		// 1Hz
rtc.setSquareWave(SquareWave1024Hz);	// 1024Hz
rtc.setSquareWave(SquareWave4096Hz);	// 4096Hz
rtc.setSquareWave(SquareWave8192Hz);	// 8192Hz
```


## Library dependencies

* ```Wire.h```
* ```Terminal.ino``` requires ```ErriezSerialTerminal``` library.


## Library installation

Please refer to the [Wiki](https://github.com/Erriez/ErriezArduinoLibrariesAndSketches/wiki) page.


## Other Arduino Libraries and Sketches from Erriez

* [Erriez Libraries and Sketches](https://github.com/Erriez/ErriezArduinoLibrariesAndSketches)
