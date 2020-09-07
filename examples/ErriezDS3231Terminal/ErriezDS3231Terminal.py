#
# MIT License
#
# Copyright (c) 2020 Erriez
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#
# Source:         https://github.com/Erriez/ErriezDS3231
# Documentation:  https://erriez.github.io/ErriezDS3231
#

import datetime
import serial
import sys

SERIAL_PORT = '/dev/ttyACM0'
BAUDRATE = 115200


def read_line(line):
    line = line.decode('ascii')
    line = line.strip()
    return line


def set_date(ser):
    # Get system date/time
    now = datetime.datetime.now()

    date_str = 'set date {} {}-{}-{}\n'.format(
        now.weekday() + 1, now.day, now.month, now.year)

    # Write set date command
    ser.write(str.encode(date_str))


def set_time(ser):
    # Wait for second change
    last = datetime.datetime.now()
    while 1:
        now = datetime.datetime.now()
        if last.second != now.second:
            break
    now += datetime.timedelta(0, 1)

    # Write set time command
    ser.write(str.encode('set time {}:{}:{}'.format(now.hour, now.minute, now.second)))

    # Wait for second change
    newline = str.encode('\n')
    last = datetime.datetime.now()
    while 1:
        now = datetime.datetime.now()
        if last.second != now.second:
            break

    # Write newline
    ser.write(newline)


def main():
    print('Erriez Arduino DS3213 RTC set date time via terminal example')

    ser = serial.Serial()
    ser.baudrate = int(BAUDRATE)
    ser.bytesize = 8
    ser.stopbits = 1
    ser.parity = serial.PARITY_NONE
    ser.timeout = 0.01
    ser.port = SERIAL_PORT
    try:
        ser.open()
    except serial.SerialException:
        print('Error: Cannot open serial port {}'.format(SERIAL_PORT))
        sys.exit(1)

    while 1:
        # Read line from serial port
        line = read_line(ser.readline())
        if line:
            # Print every line
            print(line)

            # Wait for terminal startup string
            if line.find('Erriez DS3231 RTC terminal example') == 0:
                # Set date
                set_date(ser)

                # Set time
                set_time(ser)

                # Enable continues prints
                ser.write(str.encode('print\n'))


if __name__ == '__main__':
    main()
