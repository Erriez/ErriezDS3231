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
 * \file ErriezDS3231.cpp
 * \brief DS3231 high precision RTC library for Arduino
 * \details
 *      Source:         https://github.com/Erriez/ErriezDS3231
 *      Documentation:  https://erriez.github.io/ErriezDS3231
 */


#if defined(__AVR__) || defined(ARDUINO_ARCH_SAM)
#include <avr/pgmspace.h>
#elif defined(ESP8266) || defined(ESP32) || defined(ARDUINO_ARCH_RP2040)
#include "pgmspace.h"
#else
#define pgm_read_byte(addr)                                                    \
  (*(const unsigned char *)(addr)) ///< PROGMEM workaround for non-AVR
#endif

#if !defined(__ARM_ARCH) && !defined(ENERGIA) && !defined(ESP8266) &&          \
    !defined(ESP32) && !defined(__arc__)
#include <util/delay.h>
#endif





#include <Wire.h>

#include "ErriezDS3231.h"

/*!
 * \brief Initialize and detect DS3231 RTC.
 * @param theWire interface Pointer.
 * \details
 *      Call this function from setup().
 * \retval true
 *      RTC detected.
 * \retval false
 *      RTC not detected.
 */
bool ErriezDS3231::begin(TwoWire *theWire)
{
    _wire = theWire;
    // Check zero bits in status register
    if (readRegister(DS3231_REG_STATUS) & 0x70) {
        return false;
    }

    // DS3231 detected
    return true;
}

/*!
 * \brief Initialize and detect DS3231 RTC.
 *
 * \details
 *      Uses default TwoWire interface.
 *      Call this function from setup().
 * \retval true
 *      RTC detected.
 * \retval false
 *      RTC not detected.
 */
bool ErriezDS3231::begin()
{
    _wire = &Wire;
    // Check zero bits in status register
    if (readRegister(DS3231_REG_STATUS) & 0x70) {
        return false;
    }

    // DS3231 detected
    return true;
}

/*!
 * \brief Enable or disable oscillator when running on V-BAT.
 * \param enable
 *      true:  Enable RTC clock when running on V-BAT.\n
 *      false: Stop RTC clock when running on V-BAT. Oscillator Stop Flag (OSF) bit will be set in
 *      status register which can be read on next power-on.
 * \retval true
 *      Success.
 * \retval false
 *      Oscillator enable failed.
 */
bool ErriezDS3231::clockEnable(bool enable)
{
    uint8_t reg;

    // Read status register
    reg = readRegister(DS3231_REG_CONTROL);

    // Set or clear EOSC bit in control register
    if (enable) {
        reg &= ~(1 << DS3231_CTRL_EOSC);
    } else {
        reg |= (1 << DS3231_CTRL_EOSC);
    }

    // Write control register
    if (!writeRegister(DS3231_REG_CONTROL, reg)) {
        return false;
    }

    // Clear OSF bit in status register
    reg = readRegister(DS3231_REG_STATUS);
    reg &= ~(1 << DS3231_STAT_OSF);
    return writeRegister(DS3231_REG_STATUS, reg);
}

/*!
 * \brief Read RTC OSF (Oscillator Stop Flag) from status register.
 * \details
 *      The application is responsible for checking the Oscillator Stop Flag (OSF) before reading
 *      date/time date. This function may be used to judge the validity of the date/time registers.
 * \retval true
 *      RTC clock is running.
 * \retval false
 *      RTC oscillator was stopped: The date/time data is invalid. The application should
 *      synchronize and program a new date/time.
 */
bool ErriezDS3231::isRunning()
{
    // Check OSF bit in status register
    if (readRegister(DS3231_REG_STATUS) & (1 << DS3231_STAT_OSF)) {
        // RTC clock stopped
        return false;
    } else {
        // RTC clock is running
        return true;
    }
}

/*!
 * \brief Read Unix UTC epoch time_t
 * \return
 *      Unix epoch time_t seconds since 1970.
 */
time_t ErriezDS3231::getEpoch()
{
    struct tm dt;
    time_t t;

    // Read time structure
    if (!read(&dt)) {
        // RTC read failed
        return 0;
    }

    // Convert date/time struct tm to time_t
    t = mktime(&dt);

    // An offset is needed for AVR target
#ifdef ARDUINO_ARCH_AVR
    t += UNIX_OFFSET;
#endif

    // Return Unix epoch UTC
    return t;
}

/*!
 * \brief Write Unix epoch UTC time to RTC
 * \param t
 *      time_t time
 * \retval true
 *      Success.
 * \retval false
 *      Set epoch failed.
 */
bool ErriezDS3231::setEpoch(time_t t)
{
    struct tm *dt;

    // Subtract UNIX offset for AVR targets
#ifdef ARDUINO_ARCH_AVR
    t -= UNIX_OFFSET;
#endif

    // Convert time_t to date/time struct tm
    dt = gmtime(&t);

    // Write date/time to RTC
    return write(dt);
}

/*!
 * \brief Read date and time from RTC.
 * \details
 *      Read all RTC registers at once to prevent a time/date register change in the middle of the
 *      register read operation.
 * \param dt
 *      Date and time struct tm.
 * \retval true
 *      Success
 * \retval false
 *      Read failed.
 */
bool ErriezDS3231::read(struct tm *dt)
{
    uint8_t buffer[7];

    // Read clock date and time registers
    if (!readBuffer(0x00, buffer, sizeof(buffer))) {
        memset(dt, 0, sizeof(struct tm));
        return false;
    }

    // Clear dt
    memset(dt, 0, sizeof(struct tm));

    // Convert BCD buffer to Decimal
    dt->tm_sec = bcdToDec(buffer[0] & 0x7F);
    dt->tm_min = bcdToDec(buffer[1] & 0x7F);
    dt->tm_hour = bcdToDec(buffer[2] & 0x3F);
    dt->tm_wday = bcdToDec(buffer[3] & 0x07);
    dt->tm_mday = bcdToDec(buffer[4] & 0x3F);
    dt->tm_mon = bcdToDec(buffer[5] & 0x1F);
    dt->tm_year = bcdToDec(buffer[6]) + 100; // 2000-1900

    // Month: 0..11
    if (dt->tm_mon) {
        dt->tm_mon--;
    }

    // Day of the week: 0=Sunday
    if (dt->tm_wday) {
        dt->tm_wday--;
    }

    // Check buffer for valid data
    if ((dt->tm_sec > 59) || (dt->tm_min > 59) || (dt->tm_hour > 23) ||
        (dt->tm_mday < 1) || (dt->tm_mday > 31) || (dt->tm_mon > 11) || (dt->tm_year > 199) ||
        (dt->tm_wday > 6))
    {
        return false;
    }

    return true;
}

/*!
 * \brief Write date and time to RTC.
 * \details
 *      Write all RTC registers at once to prevent a time/date register change in the middle of the
 *      register write operation. This function enables the oscillator and clear the Oscillator Stop
 *      Flag (OSF) in the status register.
 * \param dt
 *      Date/time struct tm. Providing invalid date/time data may result in unpredictable behavior.
 * \retval true
 *      Success.
 * \retval false
 *      Write failed.
 */
bool ErriezDS3231::write(const struct tm *dt)
{
    uint8_t buffer[7];

    // Enable oscillator
    if (!clockEnable(true)) {
        return false;
    }

    // Encode date time from decimal to BCD
    buffer[0] = decToBcd(dt->tm_sec) & 0x7F;
    buffer[1] = decToBcd(dt->tm_min) & 0x7F;
    buffer[2] = decToBcd(dt->tm_hour) & 0x3F;
    buffer[3] = decToBcd(dt->tm_wday + 1) & 0x07;
    buffer[4] = decToBcd(dt->tm_mday) & 0x3F;
    buffer[5] = decToBcd(dt->tm_mon + 1) & 0x1F;
    buffer[6] = decToBcd(dt->tm_year % 100);

    // Write BCD encoded buffer to RTC registers
    return writeBuffer(0x00, buffer, sizeof(buffer));
}

/*!
 * \brief Write time to RTC.
 * \details
 *      Write hour, minute and second registers to RTC.
 * \param hour
 *      Hours 0..23.
 * \param min
 *      Minutes 0..59.
 * \param sec
 *      Seconds 0..59.
 * \retval true
 *      Success.
 * \retval false
 *      Set time failed.
 */
bool ErriezDS3231::setTime(uint8_t hour, uint8_t min, uint8_t sec)
{
    struct tm dt;

    read(&dt);
    dt.tm_hour = hour;
    dt.tm_min = min;
    dt.tm_sec = sec;
    return write(&dt);
}

/*!
 * \brief Read time from RTC.
 * \details
 *      Read hour, minute and second registers from RTC.
 * \param hour
 *      Hours 0..23.
 * \param min
 *      Minutes 0..59.
 * \param sec
 *      Seconds 0..59.
 * \retval true
 *      Success.
 * \retval false
 *      Invalid second, minute or hour read from RTC. The time is set to zero.
 */
bool ErriezDS3231::getTime(uint8_t *hour, uint8_t *min, uint8_t *sec)
{
    uint8_t buffer[3];

    // Read RTC time registers
    if (!readBuffer(0x00, buffer, sizeof(buffer))) {
        return false;
    }

    // Convert BCD buffer to Decimal
    *sec = bcdToDec(buffer[0] & 0x7F);
    *min = bcdToDec(buffer[1] & 0x7F);
    *hour = bcdToDec(buffer[2] & 0x3F);

    // Check buffer for valid data
    if ((*sec > 59) || (*min > 59) || (*hour > 23)) {
        // Invalid time
        *sec = 0x00;
        *min = 0x00;
        *hour = 0x00;

        return false;
    }

    return true;
}

/*!
 * \brief Set date time
 * \param hour
 *      Hours 0..23
 * \param min
 *      Minutes 0..59
 * \param sec
 *      Seconds 0..59
 * \param mday
 *      Day of the month 1..31
 * \param mon
 *      Month 1..12 (1=January)
 * \param year
 *      Year 2000..2099
 * \param wday
 *      Day of the week 0..6 (0=Sunday, .. 6=Saturday)
 * \retval true
 *      Success.
 * \retval false
 *      Set date/time failed.
 */
bool ErriezDS3231::setDateTime(uint8_t hour, uint8_t min, uint8_t sec,
                               uint8_t mday, uint8_t mon, uint16_t year,
                               uint8_t wday)
{
    struct tm dt;

    // Prepare struct tm
    dt.tm_hour = hour;
    dt.tm_min = min;
    dt.tm_sec = sec;
    dt.tm_mday = mday;
    dt.tm_mon = mon - 1;
    dt.tm_year = year - 1900;
    dt.tm_wday = wday;

    // Write date/time to RTC
    return write(&dt);
}

/*!
 * \brief Get date time
 * \param hour
 *      Hours 0..23
 * \param min
 *      Minutes 0..59
 * \param sec
 *      Seconds 0..59
 * \param mday
 *      Day of the month 1..31
 * \param mon
 *      Month 1..12 (1=January)
 * \param year
 *      Year 2000..2099
 * \param wday
 *      Day of the week 0..6 (0=Sunday, .. 6=Saturday)
 * \retval true
 *      Success.
 * \retval false
 *      Get date/time failed.
 */
bool ErriezDS3231::getDateTime(uint8_t *hour, uint8_t *min, uint8_t *sec,
                               uint8_t *mday, uint8_t *mon, uint16_t *year,
                               uint8_t *wday)
{
    struct tm dt;

    // Read date/time from RTC
    if (!read(&dt)) {
        return false;
    }

    // Set return values
    *hour = dt.tm_hour;
    *min = dt.tm_min;
    *sec = dt.tm_sec;
    *mday = dt.tm_mday;
    *mon = dt.tm_mon + 1;
    *year = dt.tm_year + 1900;
    *wday = dt.tm_wday;

    return true;
}

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
 * \retval true
 *      Success.
 * \retval false
 *      Set alarm 1 failed.
 */
bool ErriezDS3231::setAlarm1(Alarm1Type alarmType,
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
    if (!writeBuffer(DS3231_REG_ALARM1_SEC, buffer, sizeof(buffer))) {
        return false;
    }

    // Clear alarm 1 flag
    return clearAlarmFlag(Alarm1);
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
  * \retval true
 *      Success.
 * \retval false
 *      Set alarm 2 failed.
 */
bool ErriezDS3231::setAlarm2(Alarm2Type alarmType, uint8_t dayDate, uint8_t hours, uint8_t minutes)
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
    if (!writeBuffer(DS3231_REG_ALARM2_MIN, buffer, sizeof(buffer))) {
        return false;
    }

    // Clear alarm 2 flag
    return clearAlarmFlag(Alarm2);
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
  * \retval true
 *      Success
 * \retval false
 *      Alarm interrupt enable failed.
 */
bool ErriezDS3231::alarmInterruptEnable(AlarmId alarmId, bool enable)
{
    uint8_t controlReg;

    // Clear alarm flag
    clearAlarmFlag(alarmId);

    // Read control register
    controlReg = readRegister(DS3231_REG_CONTROL);

    // Disable square wave out and enable INT
    controlReg |= (1 << DS3231_CTRL_INTCN);

    // Set or clear alarm interrupt enable bit
    if (enable) {
        controlReg |= (1 << (alarmId - 1));
    } else {
        controlReg &= ~(1 << (alarmId - 1));
    }

    // Write control register
    return writeRegister(DS3231_REG_CONTROL, controlReg);
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
bool ErriezDS3231::getAlarmFlag(AlarmId alarmId)
{
    // Mask alarm flags
    if (readRegister(DS3231_REG_STATUS) & (1 << (alarmId - 1))) {
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
 * \retval true
 *      Success
 * \retval false
 *      Incorrect alarm ID.
 */
bool ErriezDS3231::clearAlarmFlag(AlarmId alarmId)
{
    uint8_t statusReg;

    // Read status register
    statusReg = readRegister(DS3231_REG_STATUS);

    // Clear alarm interrupt flag
    statusReg &= ~(1 << (alarmId - 1));

    // Write status register
    return writeRegister(DS3231_REG_STATUS, statusReg);
}

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
 * \retval true
 *      Success
 * \retval false
 *      Set squareWave failed.
 */
bool ErriezDS3231::setSquareWave(SquareWave squareWave)
{
    uint8_t controlReg;

    // Read control register
    controlReg = readRegister(DS3231_REG_CONTROL);
    controlReg &= ~((1 << DS3231_CTRL_BBSQW) |
                    (1 << DS3231_CTRL_INTCN) |
                    (1 << DS3231_CTRL_RS2) |
                    (1 << DS3231_CTRL_RS1));
    controlReg |= squareWave;

    // Write control register
    return writeRegister(DS3231_REG_CONTROL, controlReg);
}

/*!
 * \brief Enable or disable 32kHz output clock pin.
 * \param enable
 *      true: Enable 32kHz output clock pin.\n
 *      false: Disable 32kHz output clock pin.
  * \retval true
 *      Success
 * \retval false
 *      Set output clock pin failed.
 */
bool ErriezDS3231::outputClockPinEnable(bool enable)
{
    uint8_t statusReg;

    // Read status register
    statusReg = readRegister(DS3231_REG_STATUS);

    // Set or clear EN32kHz flag in status register
    if (enable) {
        statusReg |= (1 << DS3231_STAT_EN32KHZ);
    } else {
        statusReg &= ~(1 << DS3231_STAT_EN32KHZ);
    }

    // Write status register
    return writeRegister(DS3231_REG_STATUS, statusReg);
}

/*!
 * \brief Set aging offset register
 * \details
 *      The aging offset register capacitance value is added or subtracted from the capacitance
 *      value that the device calculates for each temperature compensation.
 * \param val
 *      Aging offset value -127..127, 0.1ppm per LSB (Factory default value: 0).\n
 *      Negative values increases the RTC oscillator frequency.
 * \retval true
 *      Success
 * \retval false
 *      Set aging offset failed.
 */
bool ErriezDS3231::setAgingOffset(int8_t val)
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
    if (!writeRegister(DS3231_REG_AGING_OFFSET, regVal)) {
        return false;
    }

    // A temperature conversion is required to apply the aging offset change
    return startTemperatureConversion();
}

/*!
 * \brief Get aging offset register
 * \details
 *      The aging offset register capacitance value is added or subtracted from the capacitance
 *      value that the device calculates for each temperature compensation.
 * \return val
 *      Aging offset value.
 */
int8_t ErriezDS3231::getAgingOffset()
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
 * \retval true
 *      Success
 * \retval false
 *      Start temperature conversion failed.
 */
bool ErriezDS3231::startTemperatureConversion()
{
    uint8_t controlReg;

    // Check if temperature busy flag is set
    if (readRegister(DS3231_REG_STATUS) & (1 << DS3231_STAT_BSY)) {
        return false;
    }

    // Start temperature conversion
    controlReg = readRegister(DS3231_REG_CONTROL) | (1 << DS3231_CTRL_CONV);

    // Write control register
    return writeRegister(DS3231_REG_CONTROL, controlReg);
}

/*!
 * \brief Read temperature
 * \param temperature
 *      8-bit signed temperature in degree Celsius.
 * \param fraction
 *      Temperature fraction in steps of 0.25 degree Celsius. The returned value is a decimal value
 *      to prevent floating point usage. The application should divided the fraction by 100.
 * \retval true
 *      Success
 * \retval false
 *      Set get temperature failed.
 */
bool ErriezDS3231::getTemperature(int8_t *temperature, uint8_t *fraction)
{
    uint8_t temp[2];

    // Read temperature MSB and LSB registers
    if (!readBuffer(DS3231_REG_TEMP_MSB, &temp, sizeof(temp))) {
        return false;
    }

    // Set temperature argument
    *temperature = temp[0];

    // Calculate two's complement when negative
    if (*temperature & 0x80) {
        *temperature |= ~((1 << 8) - 1);
    }

    // Shift fraction bits 6 and 7 with 0.25 degree Celsius resolution
    *fraction = (temp[1] >> 6) * 25;

    return true;
}

/*!
 * \brief BCD to decimal conversion.
 * \param bcd
 *      BCD encoded value.
 * \return
 *      Decimal value.
 */
uint8_t ErriezDS3231::bcdToDec(uint8_t bcd)
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
uint8_t ErriezDS3231::decToBcd(uint8_t dec)
{
    return (uint8_t)(((dec / 10) << 4) | (dec % 10));
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
uint8_t ErriezDS3231::readRegister(uint8_t reg)
{
    uint8_t value = 0;

    // Read buffer with one 8-bit unsigned value
    readBuffer(reg, &value, 1);

    return value;
}

/*!
 * \brief Write register.
 * \details
 *      Please refer to the RTC datasheet.
 * \param reg
 *      RTC register number 0x00..0x12.
 * \param value
 *      8-bit unsigned register value.
 * \retval true
 *      Success
 * \retval false
 *      Write register failed
 */
bool ErriezDS3231::writeRegister(uint8_t reg, uint8_t value)
{
    // Write buffer with one 8-bit unsigned value
    return writeBuffer(reg, &value, 1);
}

/*!
 * \brief Write buffer to RTC.
 * \details
 *      Please refer to the RTC datasheet.
 * \param reg
 *      RTC register number 0x00..0x12.
 * \param buffer
 *      Buffer.
 * \param writeLen
 *      Buffer length. Writing is only allowed within valid RTC registers.
 * \retval true
 *      Success
 * \retval false
 *      I2C write failed.
 */
bool ErriezDS3231::writeBuffer(uint8_t reg, void *buffer, uint8_t writeLen)
{
    // Start I2C transfer by writing the I2C address, register number and optional buffer
    _wire->beginTransmission(DS3231_ADDR);
    _wire->write(reg);
    for (uint8_t i = 0; i < writeLen; i++) {
        _wire->write(((uint8_t *)buffer)[i]);
    }
    if (_wire->endTransmission(true) != 0) {
        return false;
    }

    return true;
}

/*!
 * \brief Read buffer from RTC.
 * \param reg
 *      RTC register number 0x00..0x12.
 * \param buffer
 *      Buffer.
 * \param readLen
 *      Buffer length. Reading is only allowed within valid RTC registers.
 * \retval true
 *      Success
 * \retval false
 *      I2C read failed.
 */
bool ErriezDS3231::readBuffer(uint8_t reg, void *buffer, uint8_t readLen)
{
    // Start I2C transfer by writing the I2C address and register number
    _wire->beginTransmission(DS3231_ADDR);
    _wire->write(reg);
    // Generate a repeated start, followed by a read buffer
    if (_wire->endTransmission(false) != 0) {
        return false;
    }
    _wire->requestFrom((uint8_t)DS3231_ADDR, readLen);
    for (uint8_t i = 0; i < readLen; i++) {
        ((uint8_t *)buffer)[i] = (uint8_t)_wire->read();
    }

    return true;
}
