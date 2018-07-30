import datetime
import serial
import sys

SERIAL_PORT = 'COM3'
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
    print('Arduino DS3213 RTC set date time via terminal example')

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
            if line.find('DS3231 RTC terminal example') == 0:
                # Set date
                set_date(ser)

                # Set time
                set_time(ser)

                # Enable continues prints
                ser.write(str.encode('print\n'))


if __name__ == '__main__':
    main()
