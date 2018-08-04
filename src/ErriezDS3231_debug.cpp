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
 * \file ErriezDS3231_debug.cpp
 * \brief DS3231 high precision RTC debug library for Arduino
 * \details
 *      Source:         https://github.com/Erriez/ErriezDS3231
 *      Documentation:  https://erriez.github.io/ErriezDS3231
 */

#include "ErriezDS3231_debug.h"

/*!
 * \brief Register names as string in flash.
 */
const char reg_0x00[] PROGMEM = "Seconds";
const char reg_0x01[] PROGMEM = "Minutes";
const char reg_0x02[] PROGMEM = "Hours";
const char reg_0x03[] PROGMEM = "Day";
const char reg_0x04[] PROGMEM = "Date";
const char reg_0x05[] PROGMEM = "Month/Century";
const char reg_0x06[] PROGMEM = "Year";
const char reg_0x07[] PROGMEM = "Alarm 1 Seconds";
const char reg_0x08[] PROGMEM = "Alarm 1 Minutes";
const char reg_0x09[] PROGMEM = "Alarm 1 Hours";
const char reg_0x0A[] PROGMEM = "Alarm 1 Day/Date";
const char reg_0x0B[] PROGMEM = "Alarm 2 Minutes";
const char reg_0x0C[] PROGMEM = "Alarm 2 Hours";
const char reg_0x0D[] PROGMEM = "Alarm 2 Day/Date";
const char reg_0x0E[] PROGMEM = "Control";
const char reg_0x0F[] PROGMEM = "Control/Status";
const char reg_0x10[] PROGMEM = "Aging Offset";
const char reg_0x11[] PROGMEM = "MSB of Temp";
const char reg_0x12[] PROGMEM = "LSB of Temp";

/*!
 * \brief Array with all register names in flash.
 */
const char* const registerNames[] PROGMEM = {
        reg_0x00, reg_0x01, reg_0x02, reg_0x03,
        reg_0x04, reg_0x05, reg_0x06, reg_0x07,
        reg_0x08, reg_0x09, reg_0x0A, reg_0x0B,
        reg_0x0C, reg_0x0D, reg_0x0E, reg_0x0F,
        reg_0x10, reg_0x11, reg_0x12
};

/*!
 * \brief Dump all registers to serial port.
 * \param ser
 *      Serial port.
 * \param printBitfields
 *      true: Print register bitfields.
 */
void DS3231Debug::dumpRegisters(Stream *ser, bool printBitfields)
{
    ser->println(F("DS3231 registers:"));
    for (uint8_t reg = 0; reg < DS3231_NUM_REGS; reg++) {
        printRegister(ser, reg, printBitfields);
    }
}

/*!
 * \brief Print register and value.
 * \param ser
 *      Serial port.
 * \param reg
 *      Register number.
 * \param printBitfields
 *      true: Print register bitfields.
 */
void DS3231Debug::printRegister(Stream *ser, uint8_t reg, bool printBitfields)
{
    uint8_t regVal;
    char buf[20];

    regVal = readRegister(reg);
    snprintf(buf, sizeof(buf), " 0x%02x: 0x%02x", reg, regVal);
    ser->print(buf);

    ser->print(F("  "));
    strncpy_P(buf, (char *)pgm_read_dword(&(registerNames[reg])), sizeof(buf));
    ser->println(buf);

    if (printBitfields) {
        printRegisterBitfields(ser, reg, regVal);
    }
}

/*!
 * \brief Print register bitfields.
 * \param ser
 *      Serial port.
 * \param reg
 *      Register number.
 * \param regVal
 *      Register value.
 */
void DS3231Debug::printRegisterBitfields(Stream *ser, uint8_t reg, uint8_t regVal)
{
    switch (reg) {
        case DS3231_REG_SECONDS:
            ser->print(F("  [6:0]: "));
            ser->println(bcdToDec(regVal & 0x7F));
            break;
        case DS3231_REG_MINUTES:
            ser->print(F("  [6:0]: "));
            ser->println(bcdToDec(regVal & 0x7F));
            break;
        case DS3231_REG_HOURS:
            ser->print(F("  [6]: "));
            ser->println(regVal & (1 << DS3231_HOUR_12H_24H) ? F("12H") : F("24H"));

            ser->print(F("  [5]: "));
            ser->println(regVal & (1 << DS3231_HOUR_AM_PM) ? F("AM") : F("PM"));

            ser->print(F("  [4:0]: "));
            ser->println(bcdToDec(regVal & 0x1F));
            break;
        case DS3231_REG_DAY_WEEK:
            ser->print(F("  [2:0]: "));
            ser->println(bcdToDec(regVal & 0x07));
            break;
        case DS3231_REG_DAY_MONTH:
            ser->print(F("  [5:0]: "));
            ser->println(bcdToDec(regVal & 0x3F));
            break;
        case DS3231_REG_MONTH:
            ser->print(F("  [7]: Century: "));
            ser->println(regVal & (1 << DS3231_MONTH_CENTURY) ? F("1") : F("0"));

            ser->print(F("  [4:0]: "));
            ser->println(bcdToDec(regVal & 0x1F));
            break;
        case DS3231_REG_YEAR:
            ser->print(F("  [7:0]: "));
            ser->println(bcdToDec(regVal));
            break;
        case DS3231_REG_ALARM1_SEC:
            ser->print(F("  [7]: A1M1: "));
            ser->println(regVal & (1 << DS3231_A1M1) ? F("1") : F("0"));

            ser->print(F("  [6:0]: "));
            ser->println(bcdToDec(regVal & 0x7F));
            break;
        case DS3231_REG_ALARM1_MIN:
            ser->print(F("  [7]: A1M2: "));
            ser->println(regVal & (1 << DS3231_A1M2) ? F("1") : F("0"));

            ser->print(F("  [6:0]: "));
            ser->println(bcdToDec(regVal & 0x7F));
            break;
        case DS3231_REG_ALARM1_HOUR:
            ser->print(F("  [7]: A1M3: "));
            ser->println(regVal & (1 << DS3231_A1M3) ? F("1") : F("0"));

            ser->print(F("  [6]: "));
            ser->println(regVal & (1 << DS3231_HOUR_12H_24H) ? F("12H") : F("24H"));

            ser->print(F("  [5]: "));
            ser->println(regVal & (1 << DS3231_HOUR_AM_PM) ? F("AM") : F("PM"));

            ser->print(F("  [4:0]: "));
            ser->println(bcdToDec(regVal & 0x1F));
            break;
        case DS3231_REG_ALARM1_DD:
            ser->print(F("  [7]: A1M4: "));
            ser->println(regVal & (1 << DS3231_A1M4) ? F("1") : F("0"));

            ser->print(F("  [6]: "));
            ser->println(regVal & (1 << DS3231_DYDT) ? F("Day") : F("Date"));

            ser->print(F("  [5:0]: "));
            ser->println(bcdToDec(regVal & 0x3F));
            break;
        case DS3231_REG_ALARM2_MIN:
            ser->print(F("  [7]: A2M2: "));
            ser->println(regVal & (1 << DS3231_A2M2) ? F("1") : F("0"));

            ser->print(F("  [6:0]: "));
            ser->println(bcdToDec(regVal & 0x7F));
            break;
        case DS3231_REG_ALARM2_HOUR:
            ser->print(F("  [7]: A2M3: "));
            ser->println(regVal & (1 << DS3231_A2M3) ? F("1") : F("0"));

            ser->print(F("  [6]: "));
            ser->println(regVal & (1 << DS3231_HOUR_12H_24H) ? F("12H") : F("24H"));

            ser->print(F("  [5]: "));
            ser->println(regVal & (1 << DS3231_HOUR_AM_PM) ? F("AM") : F("PM"));

            ser->print(F("  [4:0]: "));
            ser->println(bcdToDec(regVal & 0x1F));
            break;
        case DS3231_REG_ALARM2_DD:
            ser->print(F("  [7]: A2M4: "));
            ser->println(regVal & (1 << DS3231_A2M4) ? F("1") : F("0"));

            ser->print(F("  [6]: "));
            ser->println(regVal & (1 << DS3231_DYDT) ? F("Day") : F("Date"));

            ser->print(F("  [5:0]: "));
            ser->println(bcdToDec(regVal & 0x3F));
            break;
        case DS3231_REG_CONTROL:
            ser->print(F("  [7]: EOSC: "));
            ser->println(regVal & (1 << DS3231_CTRL_EOSC) ? F("1") : F("0"));
            ser->print(F("  [6]: BBSQW: "));
            ser->println(regVal & (1 << DS3231_CTRL_BBSQW) ? F("1") : F("0"));
            ser->print(F("  [5]: CONV: "));
            ser->println(regVal & (1 << DS3231_CTRL_CONV) ? F("1") : F("0"));
            ser->print(F("  [4]: RS2: "));
            ser->println(regVal & (1 << DS3231_CTRL_RS2) ? F("1") : F("0"));
            ser->print(F("  [3]: RS1: "));
            ser->println(regVal & (1 << DS3231_CTRL_RS1) ? F("1") : F("0"));
            ser->print(F("  [2]: INTCN: "));
            ser->println(regVal & (1 << DS3231_CTRL_INTCN) ? F("1") : F("0"));
            ser->print(F("  [1]: A12E: "));
            ser->println(regVal & (1 << DS3231_CTRL_A2IE) ? F("1") : F("0"));
            ser->print(F("  [0]: A1IE: "));
            ser->println(regVal & (1 << DS3231_CTRL_A1IE) ? F("1") : F("0"));
            break;
        case DS3231_REG_STATUS:
            ser->print(F("  [7]: OSF: "));
            ser->println(regVal & (1 << DS3231_STAT_OSF) ? F("1") : F("0"));
            ser->print(F("  [3]: EN32kHz: "));
            ser->println(regVal & (1 << DS3231_STAT_EN32KHZ) ? F("1") : F("0"));
            ser->print(F("  [2]: BSY: "));
            ser->println(regVal & (1 << DS3231_STAT_BSY) ? F("1") : F("0"));
            ser->print(F("  [1]: A2F: "));
            ser->println(regVal & (1 << DS3231_STAT_A2F) ? F("1") : F("0"));
            ser->print(F("  [0]: A1F: "));
            ser->println(regVal & (1 << DS3231_STAT_A1F) ? F("1") : F("0"));
            break;
        case DS3231_REG_AGING_OFFSET:
            int8_t agingOffset;

            if (regVal & 0x80) {
                // Calculate two's complement for negative aging register value
                agingOffset = regVal | ~((1 << 8) - 1);
            } else {
                agingOffset = regVal;
            }
            ser->print(F("  [7:0]: "));
            ser->println(agingOffset);
            break;
        case DS3231_REG_TEMP_MSB:
            ser->print(F("  [7]: SIGN: "));
            ser->println(regVal & (1 << 7) ? F("1") : F("0"));

            ser->print(F("  [6:0]: 0x"));
            ser->println(regVal, HEX);
            break;
        case DS3231_REG_TEMP_LSB:
            ser->print(F("  [7:6]: 0x"));
            ser->println(regVal & 0xC0, HEX);
            break;
        default:
            ser->println(F("Unknown register"));
    }
}

/*!
 * \brief Print diagnostics.
 * \param ser
 *      Serial port.
 */
void DS3231Debug::printDiagnostics(Stream *ser)
{
    bool oscillatorStopFlag;
    bool alarm1Flag;
    bool alarm2Flag;
    int8_t agingOffset;
    uint8_t status;
    uint8_t control;

    ser->println(F("DS3231 diagnostics:"));

    oscillatorStopFlag = isOscillatorStopped();
    agingOffset = getAgingOffset();
    alarm1Flag = getAlarmFlag(Alarm1);
    alarm2Flag = getAlarmFlag(Alarm2);
    status = readStatusRegister();
    control = readControlRegister();

    // ---------------------------------------------------------
    ser->print(F(" I2C 7-bit address: 0x"));
    ser->println(DS3231_ADDR, HEX);

    // ---------------------------------------------------------
    ser->print(F(" Oscillator enabled: "));
    ser->println((control & (1 << DS3231_CTRL_EOSC)) ? F("Stopped") : F("Started"));

    ser->print(F(" Oscillator stop flag: "));
    ser->println(oscillatorStopFlag ? F("True") : F("False"));

    // ---------------------------------------------------------
    ser->print(F(" 32kHz clock output: "));
    ser->println(status & (1 << DS3231_STAT_EN32KHZ) ? F("Enable") : F("Disable"));

    ser->print(F(" INT/SQW mode: "));
    ser->println((control & DS3231_CTRL_INTCN) ? F("Alarm interrupt") : F("SQW"));

    ser->print(F(" SQW: "));
    if ((control & DS3231_CTRL_BBSQW) || (control & DS3231_CTRL_INTCN)) {
        ser->println(F("Disabled"));
    } else {
        switch (control & 0x18) {
            case 0x00:
                ser->println(F("1Hz"));
                break;
            case 0x08:
                ser->println(F("1024Hz"));
                break;
            case 0x10:
                ser->println(F("4096Hz"));
                break;
            default:
                ser->println(F("8192Hz"));
        }
    }

    // ---------------------------------------------------------
    ser->print(F(" Alarm 1 interrupt: "));
    ser->println((control & DS3231_CTRL_A1IE) ? F("Enable") : F("Disable"));

    ser->print(F(" Alarm 2 interrupt: "));
    ser->println((control & DS3231_CTRL_A1IE) ? F("Enable") : F("Disable"));

    ser->print(F(" Alarm 1 flag: "));
    ser->println(alarm1Flag);

    ser->print(F(" Alarm 2 flag: "));
    ser->println(alarm2Flag);

    ser->print(F(" Temperature conversion busy: "));
    ser->println(status & (1 << DS3231_STAT_BSY));

    // ---------------------------------------------------------
    ser->print(F(" Aging offset: "));
    ser->println(agingOffset, DEC);
}
