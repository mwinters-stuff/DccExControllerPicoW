#ifndef _EXTERNALSINGLETONS_H
#define _EXTERNALSINGLETONS_H

#include <memory>

class AdditionalButtons;
class DccExController;
class FunctionStates;
class GlobalValues;
class KeypadManager;
class OledDisplay;
class Throttles;
class WiFiManager;
class WitServer;
namespace DCCExController{
  class DCCEXProtocol; 
};


extern std::unique_ptr<AdditionalButtons> additionalButtons;
extern std::unique_ptr<DccExController> dccExController;
extern std::unique_ptr<FunctionStates> functionStates;
extern std::unique_ptr<GlobalValues> globalValues;
extern std::unique_ptr<KeypadManager> keypadManager;
extern std::unique_ptr<OledDisplay> oledDisplay;
extern std::unique_ptr<Throttles> throttles;
extern std::unique_ptr<WiFiManager> wifiManager;
extern std::unique_ptr<WitServer> witServer;
extern std::unique_ptr<DCCExController::DCCEXProtocol> dccExProtocol;


#endif