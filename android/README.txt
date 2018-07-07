To compile:

./gradlew lua:assembleDebug
./gradlew SDL2:assembleDebug
./gradlew installDebug


Debug:

adb logcat |grep "^./tal"

