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
 * \file ErriezDS3231.h
 * \brief DS3231 high precision RTC library for Arduino
 * \details
 *      Source:         https://github.com/Erriez/ErriezDS3231
 *      Documentation:  https://erriez.github.io/ErriezDS3231
 */

#ifndef ERRIEZ_DS3231_H_
#define ERRIEZ_DS3231_H_

#include <stdint.h>
#include <time.h>
#include <wire.h>

//! DS3231 registers
#define DS3231_REG_SECONDS      0x00    //!< Seconds register
#define DS3231_REG_MINUTES      0x01    //!< Minutes register
#define DS3231_REG_HOURS        0x02    //!< Hours register
#define DS3231_REG_DAY_WEEK     0x03    //!< Day of the week register
#define DS3231_REG_DAY_MONTH    0x04    //!< Day of the month register
#define DS3231_REG_MONTH        0x05    //!< Month register
#define DS3231_REG_YEAR         0x06    //!< Year register

#define DS3231_REG_ALARM1_SEC   0x07    //!< Alarm 1 seconds register
#define DS3231_REG_ALARM1_MIN   0x08    //!< Alarm 1 minutes register
#define DS3231_REG_ALARM1_HOUR  0x09    //!< Alarm 1 hour register
#define DS3231_REG_ALARM1_DD    0x0A    //!< Alarm 1 day/date register
#define DS3231_REG_ALARM2_MIN   0x0B    //!< Alarm 2 seconds register
#define DS3231_REG_ALARM2_HOUR  0x0C    //!< Alarm 2 hour register
#define DS3231_REG_ALARM2_DD    0x0D    //!< Alarm 2 day/date register

#define DS3231_REG_CONTROL      0x0E    //!< Control register
#define DS3231_REG_STATUS       0x0F    //!< Status register
#define DS3231_REG_AGING_OFFSET 0x10    //!< Aging offset register
#define DS3231_REG_TEMP_MSB     0x11    //!< Temperature MSB register
#define DS3231_REG_TEMP_LSB     0x12    //!< Temperature LSB register

//! DS3231 number of registers
#define DS3231_NUM_REGS         19      //!< 19 RTC register: 0x00..0x12

//! DS3231 register bit defines
#define DS3231_HOUR_12H_24H     6       //!< 12 or 24 hour mode
#define DS3231_HOUR_AM_PM       5       //!< AM/PM

#define DS3231_MONTH_CENTURY    7       //!< Century

#define DS3231_CTRL_EOSC        7       //!< Enable oscillator
#define DS3231_CTRL_BBSQW       6       //!< Battery-Backed Square-Wave Enable
#define DS3231_CTRL_CONV        5       //!< Start temperature conversion
#define DS3231_CTRL_RS2         4       //!< Square wave rate-select 2
#define DS3231_CTRL_RS1         3       //!< Square wave rate-select 1
#define DS3231_CTRL_INTCN       2       //!< Interrupt control
#define DS3231_CTRL_A2IE        1       //!< Alarm 2 interrupt enable
#define DS3231_CTRL_A1IE        0       //!< Alarm 1 interrupt enable

#define DS3231_STAT_OSF         7       //!< Oscillator Stop Flag
#define DS3231_STAT_EN32KHZ     3       //!< Enable 32kHz clock output
#define DS3231_STAT_BSY         2       //!< Temperature conversion busy flag
#define DS3231_STAT_A2F         1       //!< Alarm 2 status flag
#define DS3231_STAT_A1F         0       //!< Alarm 1 status flag

#define DS3231_A1M1             7       //!< Alarm 1 bit 7 seconds register
#define DS3231_A1M2             7       //!< Alarm 1 bit 7 minutes register
#define DS3231_A1M3             7       //!< Alarm 1 bit 7 hours register
#define DS3231_A1M4             7       //!< Alarm 1 bit 7 day/date register
#define DS3231_A2M2             7       //!< Alarm 2 bit 7 minutes register
#define DS3231_A2M3             7       //!< Alarm 2 bit 7 hours register
#define DS3231_A2M4             7       //!< Alarm 2 bit 7 day/date register
#define DS3231_DYDT             6       //!< Alarm 2 bit 6

//! DS3231 I2C 7-bit address
#define DS3231_ADDR             (0xD0 >> 1)

//! Number of seconds between year 1970 and 2000
#define SECONDS_FROM_1970_TO_2000 946684800

/*!
 * \brief Alarm ID
 */
typedef enum {
    Alarm1 = 1,                 //!< Alarm ID 1
    Alarm2 = 2                  //!< Alarm ID 2
} AlarmId;

/*!
 * \brief Alarm 1 types enum
 */
typedef enum
{
    Alarm1EverySecond = 0x0F,   //!< Alarm once per second
    Alarm1MatchSeconds = 0x0E,  //!< Alarm when seconds match
    Alarm1MatchMinutes = 0x0C,  //!< Alarm when minutes and seconds match
    Alarm1MatchHours = 0x08,    //!< Alarm when hours, minutes, and seconds match
    Alarm1MatchDay = 0x10,      //!< Alarm when date, hours, minutes, and seconds match
    Alarm1MatchDate = 0x00,     //!< Alarm when day, hours, minutes, and seconds match
} Alarm1Type;

/*!
 * \brief Alarm 2 types enum
 */
typedef enum
{
    Alarm2EveryMinute = 0x0E,   //!< Alarm once per minute (00 seconds of every minute)
    Alarm2MatchMinutes = 0x0C,  //!< Alarm when minutes match
    Alarm2MatchHours = 0x08,    //!< Alarm when hours and minutes match
    Alarm2MatchDay = 0x10,      //!< Alarm when date, hours, and minutes match
    Alarm2MatchDate = 0x00,     //!< Alarm when day, hours, and minutes match
} Alarm2Type;

/*!
 * \brief Squarewave enum
 */
typedef enum {
    SquareWaveDisable = (1 << DS3231_CTRL_INTCN),                           //!< SQW disable
    SquareWave1Hz = ((0 << DS3231_CTRL_RS2) | (0 << DS3231_CTRL_RS1)),      //!< SQW 1Hz
    SquareWave1024Hz = ((0 << DS3231_CTRL_RS2) | (1 << DS3231_CTRL_RS1)),   //!< SQW 1024Hz
    SquareWave4096Hz = ((1 << DS3231_CTRL_RS2) | (0 << DS3231_CTRL_RS1)),   //!< SQW 4096Hz
    SquareWave8192Hz = ((1 << DS3231_CTRL_RS2) | (1 << DS3231_CTRL_RS1)),   //!< SQW 8192Hz
} SquareWave;


/*!
 * \brief DS3231 RTC class
 */
class ErriezDS3231
{
public:
    // Initialize
    bool begin(TwoWire *theWire);
    bool begin();

    // Oscillator functions
    bool isRunning();
    bool clockEnable(bool enable=true);

    // Set/get date/time
    time_t getEpoch();
    bool setEpoch(time_t t);
    bool read(struct tm *dt);
    bool write(const struct tm *dt);
    bool setTime(uint8_t hour, uint8_t min, uint8_t sec);
    bool getTime(uint8_t *hour, uint8_t *min, uint8_t *sec);
    bool setDateTime(uint8_t hour, uint8_t min, uint8_t sec,
                     uint8_t mday, uint8_t mon, uint16_t year,
                     uint8_t wday);
    bool getDateTime(uint8_t *hour, uint8_t *min, uint8_t *sec,
                     uint8_t *mday, uint8_t *mon, uint16_t *year,
                     uint8_t *wday);

    // Alarm functions
    bool setAlarm1(Alarm1Type alarmType,
                          uint8_t dayDate, uint8_t hours, uint8_t minutes, uint8_t seconds);
    bool setAlarm2(Alarm2Type alarmType, uint8_t dayDate, uint8_t hours, uint8_t minutes);
    bool alarmInterruptEnable(AlarmId alarmId, bool enable);
    bool getAlarmFlag(AlarmId alarmId);
    bool clearAlarmFlag(AlarmId alarmId);

    // Output signal control
    bool setSquareWave(SquareWave squareWave);
    bool outputClockPinEnable(bool enable);

    // Aging offset compensation
    bool setAgingOffset(int8_t val);
    int8_t getAgingOffset();

    // Temperature functions
    bool startTemperatureConversion();
    bool getTemperature(int8_t *temperature, uint8_t *fraction);

    // BCD conversions
    uint8_t bcdToDec(uint8_t bcd);
    uint8_t decToBcd(uint8_t dec);

    // Read/write register
    uint8_t readRegister(uint8_t reg);
    bool writeRegister(uint8_t reg, uint8_t value);

    // Read/write buffer
    bool readBuffer(uint8_t reg, void *buffer, uint8_t len);
    bool writeBuffer(uint8_t reg, void *buffer, uint8_t len);

private:
    TwoWire *_wire;                     /**< Wire object */

};

#endif // ERRIEZ_DS3231_H_
