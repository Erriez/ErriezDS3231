#!/bin/sh

# Install Python platformio 
# pip install -U platformio

# Update library
# git fetch
# git pull

# Build example(s)
platformio lib install https://github.com/Erriez/ErriezDS3231.git
platformio lib install https://github.com/Erriez/ErriezSerialTerminal.git
platformio lib update
platformio ci --lib=".piolibdeps/Erriez DS3231 RTC" --lib=".piolibdeps/Erriez Serial Terminal" --board uno --board micro --board d1_mini --board nanoatmega328 --board pro16MHzatmega328 --board pro8MHzatmega328 --board leonardo --board megaatmega2560 --board d1_mini --board nodemcuv2 ../examples/AgingOffset/AgingOffset.ino
platformio ci --lib=".piolibdeps/Erriez DS3231 RTC" --lib=".piolibdeps/Erriez Serial Terminal" --board uno --board micro --board d1_mini --board nanoatmega328 --board pro16MHzatmega328 --board pro8MHzatmega328 --board leonardo --board megaatmega2560 --board d1_mini --board nodemcuv2 ../examples/AlarmInterrupt/AlarmInterrupt.ino
platformio ci --lib=".piolibdeps/Erriez DS3231 RTC" --lib=".piolibdeps/Erriez Serial Terminal" --board uno --board micro --board d1_mini --board nanoatmega328 --board pro16MHzatmega328 --board pro8MHzatmega328 --board leonardo --board megaatmega2560 --board d1_mini --board nodemcuv2 ../examples/AlarmPolling/AlarmPolling.ino
platformio ci --lib=".piolibdeps/Erriez DS3231 RTC" --lib=".piolibdeps/Erriez Serial Terminal" --board uno --board micro --board d1_mini --board nanoatmega328 --board pro16MHzatmega328 --board pro8MHzatmega328 --board leonardo --board megaatmega2560 --board d1_mini --board nodemcuv2 ../examples/GettingStarted/GettingStarted.ino
platformio ci --lib=".piolibdeps/Erriez DS3231 RTC" --lib=".piolibdeps/Erriez Serial Terminal" --board uno --board micro --board d1_mini --board nanoatmega328 --board pro16MHzatmega328 --board pro8MHzatmega328 --board leonardo --board megaatmega2560 --board d1_mini --board nodemcuv2 ../examples/Minimum/Minimum.ino
platformio ci --lib=".piolibdeps/Erriez DS3231 RTC" --lib=".piolibdeps/Erriez Serial Terminal" --board uno --board micro --board d1_mini --board nanoatmega328 --board pro16MHzatmega328 --board pro8MHzatmega328 --board leonardo --board megaatmega2560 --board d1_mini --board nodemcuv2 ../examples/PrintDiagnostics/PrintDiagnostics.ino
platformio ci --lib=".piolibdeps/Erriez DS3231 RTC" --lib=".piolibdeps/Erriez Serial Terminal" --board uno --board micro --board d1_mini --board nanoatmega328 --board pro16MHzatmega328 --board pro8MHzatmega328 --board leonardo --board megaatmega2560 --board d1_mini --board nodemcuv2 ../examples/ReadTimeInterrupt/ReadTimeInterrupt.ino
platformio ci --lib=".piolibdeps/Erriez DS3231 RTC" --lib=".piolibdeps/Erriez Serial Terminal" --board uno --board micro --board d1_mini --board nanoatmega328 --board pro16MHzatmega328 --board pro8MHzatmega328 --board leonardo --board megaatmega2560 --board d1_mini --board nodemcuv2 ../examples/ReadTimePolled/ReadTimePolled.ino
platformio ci --lib=".piolibdeps/Erriez DS3231 RTC" --lib=".piolibdeps/Erriez Serial Terminal" --board uno --board micro --board d1_mini --board nanoatmega328 --board pro16MHzatmega328 --board pro8MHzatmega328 --board leonardo --board megaatmega2560 --board d1_mini --board nodemcuv2 ../examples/SetDateTime/SetDateTime.ino
platformio ci --lib=".piolibdeps/Erriez DS3231 RTC" --lib=".piolibdeps/Erriez Serial Terminal" --board uno --board micro --board d1_mini --board nanoatmega328 --board pro16MHzatmega328 --board pro8MHzatmega328 --board leonardo --board megaatmega2560 --board d1_mini --board nodemcuv2 ../examples/Temperature/Temperature.ino
platformio ci --lib=".piolibdeps/Erriez DS3231 RTC" --lib=".piolibdeps/Erriez Serial Terminal" --board uno --board micro --board d1_mini --board nanoatmega328 --board pro16MHzatmega328 --board pro8MHzatmega328 --board leonardo --board megaatmega2560 --board d1_mini --board nodemcuv2 ../examples/Terminal/Terminal.ino
