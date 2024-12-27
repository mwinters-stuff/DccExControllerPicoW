// BatteryTest.h
#ifndef BATTERYTEST_H
#define BATTERYTEST_H

#include <Arduino.h>

class BatteryTest {
public:
    BatteryTest(int testPin);
    void begin();
    void checkBatteryLevel();
    int getBatteryLevel();
    void displayBatteryStatus();

private:
    int batteryTestPin;
    int lastBatteryTestValue;
};

#endif // BATTERYTEST_H