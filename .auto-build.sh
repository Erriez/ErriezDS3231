#!/bin/sh

################################################################################
# Title         : .auto-build.sh
# Date created  : 2 August 2018
__AUTHOR__="Erriez"
#
# This script will start PlatformIO build.
#
################################################################################

################################################################################
##### Setup this script and get the current gh-pages branch.               #####
echo 'Setting up the script...'

# Exit with nonzero exit code if anything fails
set -e

# Build sources
platformio ci --lib="." --project-conf=platformio/platformio.ini examples/AgingOffset/AgingOffset.ino
platformio ci --lib="." --project-conf=platformio/platformio.ini examples/AlarmInterrupt/AlarmInterrupt.ino
platformio ci --lib="." --project-conf=platformio/platformio.ini examples/AlarmPolling/AlarmPolling.ino
platformio ci --lib="." --project-conf=platformio/platformio.ini examples/GettingStarted/GettingStarted.ino
platformio ci --lib="." --project-conf=platformio/platformio.ini examples/Minimum/Minimum.ino
platformio ci --lib="." --project-conf=platformio/platformio.ini examples/PrintDiagnostics/PrintDiagnostics.ino
platformio ci --lib="." --project-conf=platformio/platformio.ini examples/ReadTimeInterrupt/ReadTimeInterrupt.ino
platformio ci --lib="." --project-conf=platformio/platformio.ini examples/ReadTimePolled/ReadTimePolled.ino
platformio ci --lib="." --project-conf=platformio/platformio.ini examples/SetDateTime/SetDateTime.ino
platformio ci --lib="." --project-conf=platformio/platformio.ini examples/Temperature/Temperature.ino
platformio ci --lib="." --project-conf=platformio/platformio.ini examples/Terminal/Terminal.ino
