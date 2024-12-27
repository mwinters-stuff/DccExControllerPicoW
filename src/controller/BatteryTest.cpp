#include "BatteryTest.h"
#include <Pangodream_18650_CL.h>

Pangodream_18650_CL batteryTest(BATTERY_TEST_PIN);

void BatteryTest::begin() {
    batteryTest.begin();
}

int BatteryTest::getBatteryLevel() {
    return batteryTest.getBatteryLevel();
}

bool BatteryTest::isBatteryLow() {
    return batteryTest.getBatteryLevel() < USE_BATTERY_SLEEP_AT_PERCENT;
}

void BatteryTest::displayBatteryStatus() {
    int level = getBatteryLevel();
    if (isBatteryLow()) {
        // Code to display low battery warning on OLED
    } else {
        // Code to display battery level on OLED
    }
}

void BatteryTest::loop() {
    static unsigned long lastCheckTime = 0;
    if (millis() - lastCheckTime > 10000) { // Check every 10 seconds
        displayBatteryStatus();
        lastCheckTime = millis();
    }
}