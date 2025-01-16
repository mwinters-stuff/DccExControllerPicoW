
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

std::unique_ptr<AdditionalButtons> additionalButtons;
std::unique_ptr<DccExController> dccExController;
std::unique_ptr<FunctionStates> functionStates;
std::unique_ptr<GlobalValues> globalValues;
std::unique_ptr<KeypadManager> keypadManager;
std::unique_ptr<OledDisplay> oledDisplay;
std::unique_ptr<Throttles> throttles;
std::unique_ptr<WiFiManager> wifiManager;
std::unique_ptr<WitServer> witServer;
std::unique_ptr<DCCExController::DCCEXProtocol> dccExProtocol;