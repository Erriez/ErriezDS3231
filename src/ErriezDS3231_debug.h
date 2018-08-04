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
 * \file ErriezDS3231_debug.h
 * \brief DS3231 high precision RTC debug library for Arduino
 * \details
 *      Source:         https://github.com/Erriez/ErriezDS3231
 *      Documentation:  https://erriez.github.io/ErriezDS3231
 */

#ifndef ERRIEZ_DS3231_DEBUG_H_
#define ERRIEZ_DS3231_DEBUG_H_

#include <Arduino.h>

#include "ErriezDS3231.h"

/*!
 * \brief DS3231 RTC debug class.
 * \details
 *      The output will be redirected to a user defined serial port. This class can be used by
 *      advanced developers to print internal RTC registers and diagnostics. Keep in mind that all
 *      strings used in this class are flash consuming.
 *
 */
class DS3231Debug : public DS3231
{
public:
    // Debug functionality
    virtual void dumpRegisters(Stream *ser, bool printBitfields=true);
    virtual void printRegister(Stream *ser, uint8_t reg, bool printBitfields=true);
    virtual void printRegisterBitfields(Stream *ser, uint8_t reg, uint8_t regVal);
    virtual void printDiagnostics(Stream *ser);
};

#endif // ERRIEZ_DS3231_DEBUG_H_
