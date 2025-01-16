#include "SingletonHolder.h"
#include <memory>

#include "AdditionalButtons.h"
#include "BatteryTest.h"
#include "DccExController.h"
#include "FunctionStates.h"
#include "GlobalValues.h"
#include "KeypadManager.h"
#include "OledDisplay.h"
#include "RotaryEncoder.h"
#include "Throttles.h"
#include "WiFiManager.h"
#include "WitServer.h"
#include "DCCEXProtocol.h"

#include "ExternalSingletons.h"

void SingletonHolder::setup() {
  additionalButtons = std::make_unique<AdditionalButtons>();
  dccExController = std::make_unique<DccExController>();
  functionStates = std::make_unique<FunctionStates>();
  globalValues = std::make_unique<GlobalValues>();
  keypadManager = std::make_unique<KeypadManager>();
  oledDisplay = std::make_unique<OledDisplay>();
  throttles = std::make_unique<Throttles>();
  wifiManager = std::make_unique<WiFiManager>();
  witServer = std::make_unique<WitServer>();

  additionalButtons->setup();
  // batteryTest->setup();
  dccExController->setup();
  functionStates->setup();
  globalValues->setup();
  keypadManager->setup();
  oledDisplay->setup();
  // rotaryEncoder->setup();
  throttles->setup();
  wifiManager->setup();
  witServer->setup();
}

void SingletonHolder::loop() {
  additionalButtons->loop();
  dccExController->loop();
  functionStates->loop();
  globalValues->loop();
  keypadManager->loop();
  oledDisplay->loop();
  throttles->loop();
  wifiManager->loop();
  witServer->loop();
}
