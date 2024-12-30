#include "DccExController.h"
#include "WiFiManager.h"
#include "OledDisplay.h"
#include "RotaryEncoder.h"
#include "BatteryTest.h"
#include "KeypadManager.h"
#include "AdditionalButtons.h"

DccExController dccExController;
WiFiManager wifiManager;
OledDisplay oledDisplay;
RotaryEncoder rotaryEncoder;
BatteryTest batteryTest;
KeypadManager keypadManager;
AdditionalButtons additionalButtons;

void setup() {
    Serial.begin(115200);
    dccExController.initialize();
    wifiManager.initialize();
    oledDisplay.initialize();
    rotaryEncoder.initialize();
    batteryTest.initialize();
    keypadManager.initialize();
    additionalButtons.initialize();
}

void loop() {
    wifiManager.loop();
    dccExController.loop();
    oledDisplay.loop();
    rotaryEncoder.loop();
    batteryTest.loop();
    keypadManager.loop();
    additionalButtons.loop();
}