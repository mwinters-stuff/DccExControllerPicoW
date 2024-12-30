#ifndef _BATTERYTEST_H
#define _BATTERYTEST_H


class BatteryTest {
public:
    BatteryTest(int testPin);
    void begin();
    bool isBatteryLow();
    void loop();
    int getBatteryLevel();
    void displayBatteryStatus();

private:
    int batteryTestPin;
    int lastBatteryTestValue;
};

#endif // BATTERYTEST_H