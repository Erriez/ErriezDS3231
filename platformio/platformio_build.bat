rem Install Python27 platformio
rem C:\Python27\Scripts\pip.exe install -U platformio

rem Set Python path
set PATH=%PATH%;C:\Python27
set PATH=%PATH%;C:\Python27\Scripts

rem Update library
rem git fetch
rem git pull

rem Build example(s)
platformio ci --lib=".." --lib="..\..\ErriezSerialTerminal" --board uno --board micro --board d1_mini --board nanoatmega328 --board pro16MHzatmega328 --board pro8MHzatmega328 --board leonardo --board megaatmega2560 --board d1_mini --board nodemcuv2 ../examples/AgingOffset/AgingOffset.ino
platformio ci --lib=".." --lib="..\..\ErriezSerialTerminal" --board uno --board micro --board d1_mini --board nanoatmega328 --board pro16MHzatmega328 --board pro8MHzatmega328 --board leonardo --board megaatmega2560 --board d1_mini --board nodemcuv2 ../examples/AlarmInterrupt/AlarmInterrupt.ino
platformio ci --lib=".." --lib="..\..\ErriezSerialTerminal" --board uno --board micro --board d1_mini --board nanoatmega328 --board pro16MHzatmega328 --board pro8MHzatmega328 --board leonardo --board megaatmega2560 --board d1_mini --board nodemcuv2 ../examples/AlarmPolling/AlarmPolling.ino
platformio ci --lib=".." --lib="..\..\ErriezSerialTerminal" --board uno --board micro --board d1_mini --board nanoatmega328 --board pro16MHzatmega328 --board pro8MHzatmega328 --board leonardo --board megaatmega2560 --board d1_mini --board nodemcuv2 ../examples/GettingStarted/GettingStarted.ino
platformio ci --lib=".." --lib="..\..\ErriezSerialTerminal" --board uno --board micro --board d1_mini --board nanoatmega328 --board pro16MHzatmega328 --board pro8MHzatmega328 --board leonardo --board megaatmega2560 --board d1_mini --board nodemcuv2 ../examples/Minimum/Minimum.ino
platformio ci --lib=".." --lib="..\..\ErriezSerialTerminal" --board uno --board micro --board d1_mini --board nanoatmega328 --board pro16MHzatmega328 --board pro8MHzatmega328 --board leonardo --board megaatmega2560 --board d1_mini --board nodemcuv2 ../examples/PrintDiagnostics/PrintDiagnostics.ino
platformio ci --lib=".." --lib="..\..\ErriezSerialTerminal" --board uno --board micro --board d1_mini --board nanoatmega328 --board pro16MHzatmega328 --board pro8MHzatmega328 --board leonardo --board megaatmega2560 --board d1_mini --board nodemcuv2 ../examples/ReadTimeInterrupt/ReadTimeInterrupt.ino
platformio ci --lib=".." --lib="..\..\ErriezSerialTerminal" --board uno --board micro --board d1_mini --board nanoatmega328 --board pro16MHzatmega328 --board pro8MHzatmega328 --board leonardo --board megaatmega2560 --board d1_mini --board nodemcuv2 ../examples/ReadTimePolled/ReadTimePolled.ino
platformio ci --lib=".." --lib="..\..\ErriezSerialTerminal" --board uno --board micro --board d1_mini --board nanoatmega328 --board pro16MHzatmega328 --board pro8MHzatmega328 --board leonardo --board megaatmega2560 --board d1_mini --board nodemcuv2 ../examples/SetDateTime/SetDateTime.ino
platformio ci --lib=".." --lib="..\..\ErriezSerialTerminal" --board uno --board micro --board d1_mini --board nanoatmega328 --board pro16MHzatmega328 --board pro8MHzatmega328 --board leonardo --board megaatmega2560 --board d1_mini --board nodemcuv2 ../examples/Temperature/Temperature.ino
platformio ci --lib=".." --lib="..\..\ErriezSerialTerminal" --board uno --board micro --board d1_mini --board nanoatmega328 --board pro16MHzatmega328 --board pro8MHzatmega328 --board leonardo --board megaatmega2560 --board d1_mini --board nodemcuv2 ../examples/Terminal/Terminal.ino

@pause