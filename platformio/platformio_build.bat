rem Install Python27 platformio
rem C:\Python27\Scripts\pip.exe install -U platformio

rem Set Python path
set PATH=%PATH%;C:\Python27
set PATH=%PATH%;C:\Python27\Scripts

rem Update library
rem git fetch
rem git pull

rem Build example(s)
platformio ci --lib=".." --board uno --board micro --board d1_mini --board nanoatmega328 --board pro16MHzatmega328 --board pro8MHzatmega328 --board leonardo --board megaatmega2560 --board due --board d1_mini --board nodemcuv2 --board lolin_d32 ../examples/AgingOffset/AgingOffset.ino
platformio ci --lib=".." --board uno --board micro --board d1_mini --board nanoatmega328 --board pro16MHzatmega328 --board pro8MHzatmega328 --board leonardo --board megaatmega2560 --board due --board d1_mini --board nodemcuv2 --board lolin_d32 ../examples/AlarmInterrupt/AlarmInterrupt.ino
platformio ci --lib=".." --board uno --board micro --board d1_mini --board nanoatmega328 --board pro16MHzatmega328 --board pro8MHzatmega328 --board leonardo --board megaatmega2560 --board due --board d1_mini --board nodemcuv2 --board lolin_d32 ../examples/AlarmPolling/AlarmPolling.ino
platformio ci --lib=".." --board uno --board micro --board d1_mini --board nanoatmega328 --board pro16MHzatmega328 --board pro8MHzatmega328 --board leonardo --board megaatmega2560 --board due --board d1_mini --board nodemcuv2 --board lolin_d32 ../examples/GettingStarted/GettingStarted.ino
platformio ci --lib=".." --board uno --board micro --board d1_mini --board nanoatmega328 --board pro16MHzatmega328 --board pro8MHzatmega328 --board leonardo --board megaatmega2560 --board due --board d1_mini --board nodemcuv2 --board lolin_d32 ../examples/Minimum/Minimum.ino
platformio ci --lib=".." --board uno --board micro --board d1_mini --board nanoatmega328 --board pro16MHzatmega328 --board pro8MHzatmega328 --board leonardo --board megaatmega2560 --board due --board d1_mini --board nodemcuv2 --board lolin_d32 ../examples/PrintDiagnostics/PrintDiagnostics.ino
platformio ci --lib=".." --board uno --board micro --board d1_mini --board nanoatmega328 --board pro16MHzatmega328 --board pro8MHzatmega328 --board leonardo --board megaatmega2560 --board due --board d1_mini --board nodemcuv2 --board lolin_d32 ../examples/ReadTimeInterrupt/ReadTimeInterrupt.ino
platformio ci --lib=".." --board uno --board micro --board d1_mini --board nanoatmega328 --board pro16MHzatmega328 --board pro8MHzatmega328 --board leonardo --board megaatmega2560 --board due --board d1_mini --board nodemcuv2 --board lolin_d32 ../examples/ReadTimePolled/ReadTimePolled.ino
platformio ci --lib=".." --board uno --board micro --board d1_mini --board nanoatmega328 --board pro16MHzatmega328 --board pro8MHzatmega328 --board leonardo --board megaatmega2560 --board due --board d1_mini --board nodemcuv2 --board lolin_d32 ../examples/SetDateTime/SetDateTime.ino
platformio ci --lib=".." --board uno --board micro --board d1_mini --board nanoatmega328 --board pro16MHzatmega328 --board pro8MHzatmega328 --board leonardo --board megaatmega2560 --board due --board d1_mini --board nodemcuv2 --board lolin_d32 ../examples/Temperature/Temperature.ino
platformio ci --lib=".." --lib="../../ErriezSerialTerminal" --board uno --board micro --board d1_mini --board nanoatmega328 --board pro16MHzatmega328 --board pro8MHzatmega328 --board leonardo --board megaatmega2560 --board due --board d1_mini --board nodemcuv2 --board lolin_d32 ../examples/Terminal/Terminal.ino

@pause
