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
 * \file ErriezDS3231.cpp
 * \brief DS3231 high precision RTC library for Arduino
 * \details
 *      Source:         https://github.com/Erriez/ErriezDS3231
 *      Documentation:  https://erriez.github.io/ErriezDS3231
 */

#if (defined(__AVR__))
#include <avr/pgmspace.h>
#else
#include <pgmspace.h>
#endif

#include <Wire.h>

#include "ErriezDS3231.h"

// -------------------------------------------------------------------------------------------------
/*!
 * \brief Initialize and detect DS3231 RTC.
 * \details
 *      Call this function from setup().
 * \retval Success
 *      RTC detected.
 * \retval false
 *      RTC not detected.
 * \retval true
 *      Invalid status register or RTC not detected.
 */
bool DS3231::begin()
{
    // Check zero bits in status register
    if (readStatusRegister() & 0x70) {
        return true;
    }

    return false;
}

// -------------------------------------------------------------------------------------------------
/*!
 * \brief Enable or disable oscillator when running on V-BAT.
 * \param enable
 *      true:  Enable RTC clock when running on V-BAT.\n
 *      false: Stop RTC clock when running on V-BAT. Oscillator Stop Flag (OSF) bit will be set in
 *      status register which can be read on next power-on.
 */
void DS3231::oscillatorEnable(bool enable)
{
    uint8_t controlReg;

    // Read status register
    controlReg = readControlRegister();

    // Set or clear EOSC bit in control register
    if (enable) {
        // Clear to enable
        controlReg &= ~(1 << DS3231_CTRL_EOSC);
    } else {
        // Set to disable
        controlReg |= (1 << DS3231_CTRL_EOSC);
    }

    // Write control register
    writeControlRegister(controlReg);
}

/*!
 * \brief Read RTC OSF (Oscillator Stop Flag) from status register.
 * \details
 *      The application is responsible for checking the Oscillator Stop Flag (OSF) before reading
 *      date/time date. This function may be used to judge the validity of the date/time registers.
 * \retval true
 *      RTC oscillator was stopped: The date/time data is invalid. The application should
 *      synchronize and program a new date/time.
 * \retval false
 *      RTC oscillator is running.
 */
bool DS3231::isOscillatorStopped()
{
    // Check OSF bit in status register
    if (readStatusRegister() & (1 << DS3231_STAT_OSF)) {
        // RTC oscillator was stopped
        return true;
    } else {
        // RTC oscillator is running
        return false;
    }
}

/*!
 * \brief Clear Oscillator Stop Flag (OSF) in status register
 * \details
 *
 */
void DS3231::clearOscillatorStopFlag()
{
    uint8_t statusReg;

    // Clear OSF bit in status register
    statusReg = readStatusRegister() & ~(1 << DS3231_STAT_OSF);

    // Write status register
    writeStatusRegister(statusReg);
}

/*!
 * \brief Write date and time to RTC.
 * \details
 *      Write all RTC registers at once to prevent a time/date register change in the middle of the
 *      register write operation. This function enables the oscillator and clear the Oscillator Stop
 *      Flag (OSF) in the status register.
 * \param dateTime
 *      Date time structure. Providing invalid date/time data may result in unpredictable behavior.
 */
void DS3231::setDateTime(DS3231_DateTime *dateTime)
{
    uint8_t buffer[7];

    // Encode date time from decimal to BCD
    buffer[0] = (decToBcd((uint8_t)(dateTime->second & 0x7F)));
    buffer[1] = (decToBcd((uint8_t)(dateTime->minute & 0x7F)));
    buffer[2] = (decToBcd((uint8_t)(dateTime->hour & 0x3F)));
    buffer[3] = (decToBcd((uint8_t)(dateTime->dayWeek & 0x07)));
    buffer[4] = (decToBcd((uint8_t)(dateTime->dayMonth & 0x3F)));
    buffer[5] = (decToBcd((uint8_t)(dateTime->month & 0x1F)));
    buffer[6] = (decToBcd((uint8_t)((dateTime->year - 2000) & 0xFF)));

    // Write BCD encoded buffer to RTC registers
    writeBuffer(0x00, buffer, sizeof(buffer));

    // Enable oscillator
    oscillatorEnable(true);

    // Clear oscillator halt flag
    clearOscillatorStopFlag();
}

/*!
 * \brief Read date and time from RTC.
 * \details
 *      Read all RTC registers at once to prevent a time/date register change in the middle of the
 *      register read operation.
 * \param dateTime
 *      Date and time structure.
 * \retval false
 *      Success
 * \retval true
 *      An invalid date/time format was read from the RTC.
 */
bool DS3231::getDateTime(DS3231_DateTime *dateTime)
{
    uint8_t buf[7];

    // Read clock date and time registers
    readBuffer(0x00, &buf, sizeof(buf));

    // Convert BCD buffer to Decimal
    dateTime->second = bcdToDec(buf[0]);
    dateTime->minute = bcdToDec(buf[1]);
    dateTime->hour = bcdToDec(buf[2] & 0x3f);
    dateTime->dayWeek = bcdToDec(buf[3]);
    dateTime->dayMonth = bcdToDec(buf[4]);
    dateTime->month = bcdToDec(buf[5] & 0x1f);
    dateTime->year = 2000 + bcdToDec(buf[6]);

    // Check buffer for valid data
    if ((dateTime->second > 59) ||
        (dateTime->minute > 59) ||
        (dateTime->hour > 23) ||
        (dateTime->dayMonth < 1) || (dateTime->dayMonth > 31) ||
        (dateTime->month < 1) || (dateTime->month > 12) ||
        (dateTime->dayWeek < 1) || (dateTime->dayWeek > 7) ||
        (dateTime->year > 2099))
    {
        // Invalid date/time read from RTC: Clear date time
        memset(dateTime, 0x00, sizeof(DS3231_DateTime));
        return true;
    }

    return false;
}

/*!
 * \brief Write time to RTC.
 * \details
 *      Read all date/time register from RTC, update time registers and write all date/time
 *      registers to the RTC with one write operation.
 * \param hour Hours 0..23.
 * \param minute Minutes 0..59.
 * \param second Seconds 0..59.
 */
void DS3231::setTime(uint8_t hour, uint8_t minute, uint8_t second)
{
    DS3231_DateTime dt;

    // Read date and time from RTC
    if (getDateTime(&dt) == false) {
        // Update time
        dt.hour = hour;
        dt.minute = minute;
        dt.second = second;

        // Write updated date and time to RTC
        setDateTime(&dt);
    }
}

/*!
 * \brief Read time from RTC.
 * \details
 *      Read hour, minute and second registers from RTC.
 * \param hour Hours 0..23.
 * \param minute Minutes 0..59.
 * \param second Seconds 0..59.
 * \retval false
 *      Success
 * \retval true
 *      Invalid second, minute or hour read from RTC. The time is set to zero.
 */
bool DS3231::getTime(uint8_t *hour, uint8_t *minute, uint8_t *second)
{
    uint8_t buf[3];

    // Read clock time registers
    readBuffer(0x00, &buf, sizeof(buf));

    // Convert BCD buffer to Decimal
    *second = bcdToDec(buf[0]);
    *minute = bcdToDec(buf[1]);
    *hour = bcdToDec(buf[2] & 0x3f);

    // Check buffer for valid data
    if ((*second > 59) || (*minute > 59) || (*hour > 23)) {
        // Invalid time
        *second = 0x00;
        *minute = 0x00;
        *hour = 0x00;

        return true;
    }

    return false;
}

/*!
 * \brief Define number of days in a month once in flash.
 */
const uint8_t daysMonth[12] PROGMEM = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

/*!
 * \brief Get Unix Epoch 32-bit timestamp in current timezone
 * \details
 *      The DS3231 RTC year range is valid between years 2000...2100.
 *      The time is in UTC.
 * \retval epoch
 *      32-bit unsigned Unix Epoch time
 */
uint32_t DS3231::getEpochTime(DS3231_DateTime *dateTime)
{
    uint16_t year;
    uint16_t days;
    uint8_t i;

    // Convert date time to epoch
    // Subtract year 2000
    year = dateTime->year - 2000;

    // Calculate total number of days including leap days
    days = ((365 * year) + (year + 3) / 4);

    // Add number of days each month in current year
    for (i = 1; i < dateTime->month; i++) {
        days += pgm_read_byte(daysMonth + i - 1);
    }

    // Check month and leap year
    if ((dateTime->month > 2) && ((year % 4) == 0)) {
        days++;
    }

    // Add number of days in current month
    days += (uint16_t)(dateTime->dayMonth - 1);

    // Calculate epoch, starting at offset year 2000
    return SECONDS_FROM_1970_TO_2000 +
           (((((days * 24UL) + dateTime->hour) * 60) + dateTime->minute) * 60) + dateTime->second;
}

//--------------------------------------------------------------------------------------------------
/*!
 * \brief Set Alarm 1.
 * \details
 *      Alarm 1 contains several alarm modes which can be configured with the alarmType parameter.
 *      Unused matches can be set to zero. The alarm interrupt must be enabled after setting the
 *      alarm, followed by clearing the alarm interrupt flag.
 * \param alarmType
 *      Alarm 1 types:\n
 *          Alarm1EverySecond\n
 *          Alarm1MatchSeconds\n
 *          Alarm1MatchMinutes\n
 *          Alarm1MatchHours\n
 *          Alarm1MatchDay\n
 *          Alarm1MatchDate\n
 * \param dayDate
 *      Alarm match day of the week or day of the month. This depends on alarmType.
 * \param hours
 *      Alarm match hours.
 * \param minutes
 *      Alarm match minutes.
 * \param seconds
 *      Alarm match seconds.
 */
void DS3231::setAlarm1(Alarm1Type alarmType,
                       uint8_t dayDate, uint8_t hours, uint8_t minutes, uint8_t seconds)
{
    uint8_t buffer[4];

    // Store alarm 1 registers in buffer
    buffer[0] = decToBcd(seconds);
    buffer[1] = decToBcd(minutes);
    buffer[2] = decToBcd(hours);
    buffer[3] = decToBcd(dayDate);

    // Set alarm 1 bits
    if (alarmType & 0x01) { buffer[0] |= (1 << DS3231_A1M1); }
    if (alarmType & 0x02) { buffer[1] |= (1 << DS3231_A1M2); }
    if (alarmType & 0x04) { buffer[2] |= (1 << DS3231_A1M3); }
    if (alarmType & 0x08) { buffer[3] |= (1 << DS3231_A1M4); }
    if (alarmType & 0x10) { buffer[3] |= (1 << DS3231_DYDT); }

    // Write alarm 1 registers
    writeBuffer(DS3231_REG_ALARM1_SEC, buffer, sizeof(buffer));

    // Clear alarm 1 flag
    clearAlarmFlag(Alarm1);
}

/*!
 * \brief Set Alarm 2.
 * \details
 *      Alarm 2 contains different alarm modes which can be configured with the alarmType
 *      parameter. Unused matches can be set to zero. The alarm interrupt must be enabled after
 *      setting the alarm, followed by clearing the alarm interrupt flag.
 * \param alarmType
 *      Alarm 2 types:\n
 *          Alarm2EveryMinute\n
 *          Alarm2MatchMinutes\n
 *          Alarm2MatchHours\n
 *          Alarm2MatchDay\n
 *          Alarm2MatchDate\n
 * \param dayDate
 *      Alarm match day of the week or day of the month. This depends on alarmType.
 * \param hours
 *      Alarm match hours.
 * \param minutes
 *      Alarm match minutes.
 */
void DS3231::setAlarm2(Alarm2Type alarmType, uint8_t dayDate, uint8_t hours, uint8_t minutes)
{
    uint8_t buffer[3];

    // Store alarm 2 registers in buffer
    buffer[0] = decToBcd(minutes);
    buffer[1] = decToBcd(hours);
    buffer[2] = decToBcd(dayDate);

    // Set alarm 2 bits
    if (alarmType & 0x02) { buffer[0] |= (1 << DS3231_A1M2); }
    if (alarmType & 0x04) { buffer[1] |= (1 << DS3231_A1M3); }
    if (alarmType & 0x08) { buffer[2] |= (1 << DS3231_A1M4); }
    if (alarmType & 0x10) { buffer[2] |= (1 << DS3231_DYDT); }

    // Write alarm 2 registers
    writeBuffer(DS3231_REG_ALARM2_MIN, buffer, sizeof(buffer));

    // Clear alarm 2 flag
    clearAlarmFlag(Alarm2);
}

/*!
 * \brief Enable or disable Alarm 1 or 2 interrupt.
 * \details
 *      Enabling the alarm interrupt will disable the Square Wave output on the INT/SQW pin. The
 *      INT pin remains high until an alarm match occurs.
 * \param alarmId
 *      Alarm1 or Alarm2 enum.
 * \param enable
 *      true: Enable alarm interrupt.\n
 *      false: Disable alarm interrupt.
 */
void DS3231::alarmInterruptEnable(AlarmId alarmId, bool enable)
{
    uint8_t controlReg;

    // Clear alarm flag
    clearAlarmFlag(alarmId);

    // Read control register
    controlReg = readControlRegister();

    // Disable square wave out and enable INT
    controlReg |= (1 << DS3231_CTRL_INTCN);

    // Set or clear alarm interrupt enable bit
    if (enable) {
        controlReg |= (1 << (alarmId - 1));
    } else {
        controlReg &= ~(1 << (alarmId - 1));
    }

    // Write control register
    writeControlRegister(controlReg);
}

/*!
 * \brief Get Alarm 1 or 2 flag.
 * \details
 *      Call this function to retrieve the alarm status flag. This function can be used in
 *      polling as well as with interrupts enabled.
 *
 *      The INT pin changes to low when an Alarm 1 or Alarm 2 match occurs and the interrupt is
 *      enabled. The pin remains low until both alarm flags are cleared by the application.
 * \param alarmId
 *      Alarm1 or Alarm2 enum.
 * \retval true
 *      Alarm interrupt flag set.
 * \retval false
 *      Alarm interrupt flag cleared.
 */
bool DS3231::getAlarmFlag(AlarmId alarmId)
{
    // Mask alarm flags
    if (readStatusRegister() & (1 << (alarmId - 1))) {
        return true;
    } else {
        return false;
    }
}

/*!
 * \brief Clear alarm flag.
 * \details
 *      This function should be called when the alarm flag has been handled in polling and
 *      interrupt mode. The INT pin changes to high when both alarm flags are cleared and alarm
 *      interrupts are enabled.
 * \param alarmId
 *      Alarm1 or Alarm2 enum.
 * \retval Success
 * \retval Failure
 *      Incorrect alarm ID.
 */
void DS3231::clearAlarmFlag(AlarmId alarmId)
{
    uint8_t statusReg;

    // Read status register
    statusReg = readStatusRegister();

    // Clear alarm interrupt flag
    statusReg &= ~(1 << (alarmId - 1));

    // Write status register
    writeStatusRegister(statusReg);
}

// -------------------------------------------------------------------------------------------------
/*!
 * \brief Configure SQW (Square Wave) output pin.
 * \details
 *      This will disable or initialize the SQW clock pin. The alarm interrupt INT pin will be
 *      disabled.
 * \param squareWave
 *      SquareWave configuration:\n
 *          Disable:    SquareWaveDisable\n
 *          1Hz:        SquareWave1Hz\n
 *          1024Hz:     SquareWave1024Hz\n
 *          4096Hz:     SquareWave4096Hz\n
 *          8192Hz:     SquareWave8192Hz
 * \retval Success
 * \retval Failure
 *      Incorrect squareWave.
 */
void DS3231::setSquareWave(SquareWave squareWave)
{
    uint8_t controlReg;

    // Read control register
    controlReg = readRegister(DS3231_REG_CONTROL);
    controlReg &= ~(DS3231_CTRL_BBSQW | DS3231_CTRL_INTCN | DS3231_CTRL_RS2 | DS3231_CTRL_RS1);
    controlReg |= squareWave;

    // Write control register
    writeBuffer(DS3231_REG_CONTROL, &controlReg, sizeof(controlReg));
}

/*!
 * \brief Enable or disable 32kHz output clock pin.
 * \param enable
 *      true: Enable 32kHz output clock pin.\n
 *      false: Disable 32kHz output clock pin.
 */
void DS3231::outputClockPinEnable(bool enable)
{
    uint8_t statusReg;

    // Read status register
    statusReg = readStatusRegister();

    // Set or clear EN32kHz flag in status register
    if (enable) {
        statusReg |= (1 << DS3231_STAT_EN32KHZ);
    } else {
        statusReg &= ~(1 << DS3231_STAT_EN32KHZ);
    }

    // Write status register
    writeStatusRegister(statusReg);
}

/*!
 * \brief Set aging offset register
 * \details
 *      The aging offset register capacitance value is added or subtracted from the capacitance
 *      value that the device calculates for each temperature compensation.
 * \param val
 *      Aging offset value -127..127, 0.1ppm per LSB (Factory default value: 0).\n
 *      Negative values increases the RTC oscillator frequency.
 */
void DS3231::setAgingOffset(int8_t val)
{
    uint8_t regVal;

    // Convert 8-bit signed value to register value
    if (val < 0) {
        // Calculate two's complement for negative value
        regVal = ~(-val) + 1;
    } else {
        // Positive register value
        regVal = (uint8_t)val;
    }

    // Write aging offset register
    writeRegister(DS3231_REG_AGING_OFFSET, regVal);

    // A temperature conversion is required to apply the aging offset change
    startTemperatureConversion();
}

/*!
 * \brief Get aging offset register
 * \details
 *      The aging offset register capacitance value is added or subtracted from the capacitance
 *      value that the device calculates for each temperature compensation.
 * \return val
 *      Aging offset value.
 */
int8_t DS3231::getAgingOffset()
{
    uint8_t regVal;

    // Read aging register
    regVal = readRegister(DS3231_REG_AGING_OFFSET);

    // Convert to 8-bit signed value
    if (regVal & 0x80) {
        // Calculate two's complement for negative aging register value
        return regVal | ~((1 << 8) - 1);
    } else {
        // Positive aging register value
        return regVal;
    }
}

/*!
 * \brief Start temperature conversion
 * \details
 *      Starting a conversion is only needed when the application requires temperature reads within
 *      64 seconds, or changing the aging offset register.
 */
void DS3231::startTemperatureConversion()
{
    uint8_t controlReg;

    // Check if temperature busy flag is set
    if (readStatusRegister() & (1 << DS3231_STAT_BSY)) {
        return;
    }

    // Start temperature conversion
    controlReg = readControlRegister() | (1 << DS3231_CTRL_CONV);

    // Write control register
    writeControlRegister(controlReg);
}

/*!
 * \brief Read temperature
 * \param temperature
 *      8-bit signed temperature in degree Celsius.
 * \param fraction
 *      Temperature fraction in steps of 0.25 degree Celsius. The returned value is a decimal value
 *      to prevent floating point usage. The application should divided the fraction by 100.
 */
void DS3231::getTemperature(int8_t *temperature, uint8_t *fraction)
{
    uint8_t temp[2];

    // Read temperature MSB and LSB registers
    readBuffer(DS3231_REG_TEMP_MSB, &temp, sizeof(temp));

    // Set temperature argument
    *temperature = temp[0];

    // Calculate two's complement when negative
    if (*temperature & 0x80) {
        *temperature |= ~((1 << 8) - 1);
    }

    // Shift fraction bits 6 and 7 with 0.25 degree Celsius resolution
    *fraction = (temp[1] >> 6) * 25;
}

//--------------------------------------------------------------------------------------------------

/*!
 * \brief BCD to decimal conversion.
 * \param bcd
 *      BCD encoded value.
 * \return
 *      Decimal value.
 */
uint8_t DS3231::bcdToDec(uint8_t bcd)
{
    return (uint8_t)(10 * ((bcd & 0xF0) >> 4) + (bcd & 0x0F));
}

/*!
 * \brief Decimal to BCD conversion.
 * \param dec
 *      Decimal value.
 * \return
 *      BCD encoded value.
 */
uint8_t DS3231::decToBcd(uint8_t dec)
{
    return (uint8_t)(((dec / 10) << 4) | (dec % 10));
}

//--------------------------------------------------------------------------------------------------
/*!
 * \brief Read control register
 * \return 8-bit unsigned register value
 */
uint8_t DS3231::readControlRegister()
{
    return readRegister(DS3231_REG_CONTROL);
}

/*!
 * \brief Write control register
 * \param value 8-bit unsigned register value
 */
void DS3231::writeControlRegister(uint8_t value)
{
    return writeRegister(DS3231_REG_CONTROL, value);
}

/*!
 * \brief Read status register
 * \return 8-bit unsigned register value
 */
uint8_t DS3231::readStatusRegister()
{
    return readRegister(DS3231_REG_STATUS);
}

/*!
 * \brief Write status register
 * \param value 8-bit unsigned register value
 */
void DS3231::writeStatusRegister(uint8_t value)
{
    return writeRegister(DS3231_REG_STATUS, value);
}

/*!
 * \brief Read register.
 * \details
 *      Please refer to the RTC datasheet.
 * \param reg
 *      RTC register number 0x00..0x12.
 * \returns value
 *      8-bit unsigned register value.
 */
uint8_t DS3231::readRegister(uint8_t reg)
{
    uint8_t value;

    // Read buffer with one 8-bit unsigned value
    readBuffer(reg, &value, 1);

    return value;
}

/*!
 * \brief Write to RTC register.
 * \details
 *      Please refer to the RTC datasheet.
 * \param reg
 *      RTC register number 0x00..0x12.
 * \param value
 *      8-bit unsigned register value.
 */
void DS3231::writeRegister(uint8_t reg, uint8_t value)
{
    // Write buffer with one 8-bit unsigned value
    writeBuffer(reg, &value, 1);
}

/*!
 * \brief Write buffer to RTC.
 * \details
 *      Please refer to the RTC datasheet.
 * \param reg
 *      RTC register number 0x00..0x12.
 * \param buffer
 *      Buffer.
 * \param len
 *      Buffer length. Writing is only allowed within valid RTC registers.
 */
void DS3231::writeBuffer(uint8_t reg, void *buffer, uint8_t len)
{
    // Start I2C transfer by writing the I2C address, register number and optional buffer
    Wire.beginTransmission(DS3231_ADDR);
    Wire.write(reg);
    for (uint8_t i = 0; i < len; i++) {
        Wire.write(((uint8_t *)buffer)[i]);
    }
    Wire.endTransmission(1);
}

/*!
 * \brief Read buffer from RTC.
 * \param reg
 *      RTC register number 0x00..0x12.
 * \param buffer
 *      Buffer.
 * \param len
 *      Buffer length. Reading is only allowed within valid RTC registers.
 */
void DS3231::readBuffer(uint8_t reg, void *buffer, uint8_t len)
{
    // Start I2C transfer by writing the I2C address and register number
    Wire.beginTransmission(DS3231_ADDR);
    Wire.write(reg);
    // Generate a repeated start, followed by a read buffer
    Wire.endTransmission(0);
    Wire.requestFrom((uint8_t)DS3231_ADDR, len);
    for (uint8_t i = 0; i < len; i++) {
        ((uint8_t *)buffer)[i] = (uint8_t)Wire.read();
    }
}
