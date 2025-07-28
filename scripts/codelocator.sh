#!/bin/zsh
./gradlew :codelocator:installDebug
pkg=$(sed '/^pkg=/!d;s/.*=//' config.properties)
adb shell am force-stop $pkg
#adb shell am start-activity