#include "KeypadManager.h"
#include <pico/stdlib.h>

#include "GlobalValues.h"
#include "OledDisplay.h"
#include "Throttle.h"
#include "static.h"
#include "WitServer.h"
#include "WiFiManager.h"
#include "FunctionStates.h"
#include "DccExController.h"

#include <DCCEXLoco.h>

extern bool hashShowsFunctionsInsteadOfKeyDefs;
extern bool oledDirectCommandsAreBeingDisplayed;

using namespace DCCExController;

void KeypadManager::update() {

  auto key = keypad.getKey();
  if (key) {
    GlobalValues &globalValues = GlobalValues::getInstance();
    OledDisplay &oledDisplay = OledDisplay::getInstance();
    Throttles &throttles = Throttles::getInstance();
    WitServer &witServer = WitServer::getInstance();
    WiFiManager &wifiManager = WiFiManager::getInstance();
    DccExController &dccexController = DccExController::getInstance();

    debug_print("Key Pressed: ");
    debug_println(std::to_string(key).c_str());

    // Add specific key press handling logic here
    // For example, you can map keys to specific actions
    switch (globalValues.keypadUseType) {
    case KEYPAD_USE_OPERATION:
      debug_print("key operation... ");
      debug_println(std::to_string(key).c_str());
      switch (key) {
      case '*': // menu command
        oledDisplay.menuCommand = "";
        if (oledDisplay.menuCommandStarted) { // then cancel the menu
          resetMenu(globalValues, oledDisplay);
          oledDisplay.writeOledSpeed();
        } else {
          oledDisplay.menuCommandStarted = true;
          debug_println("Command started");
          oledDisplay.writeOledMenu("");
        }
        break;

      case '#': // end of command
        debug_print("end of command... ");
        debug_println(std::to_string(key).c_str());
        debug_print("  :  ");
        debug_println(oledDisplay.menuCommand.c_str());
        if ((oledDisplay.menuCommandStarted) && (oledDisplay.menuCommand.length() >= 1)) {
          doMenu();
        } else {
          if (!hashShowsFunctionsInsteadOfKeyDefs) {
            if (!oledDirectCommandsAreBeingDisplayed) {
              oledDisplay.writeOledDirectCommands();
            } else {
              oledDirectCommandsAreBeingDisplayed = false;
              oledDisplay.writeOledSpeed();
            }
          } else {
            oledDisplay.writeOledFunctionList("");
          }
        }
        break;

      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
        debug_print("number... ");
        debug_println(std::to_string(key).c_str());
        debug_print("  cmd: '");
        debug_print(oledDisplay.menuCommand.c_str());
        debug_println("'");

        if (oledDisplay.menuCommandStarted) { // append to the string

          if ((menuCharsRequired[key - 48] == 0) && (oledDisplay.menuCommand.length() == 0)) { // menu type is effectively a direct commands from this point
            oledDisplay.menuCommand += key;
            doMenu();
          } else {
            if ((menuCharsRequired[ std::stoi(oledDisplay.menuCommand.substr(0, 1))] == 1) && (oledDisplay.menuCommand.length() == 1)) { // menu type needs only one char
              oledDisplay.menuCommand += key;
              doMenu();

            } else { // menu type allows/requires more than one char
              oledDisplay.menuCommand += key;
              oledDisplay.writeOledMenu(oledDisplay.menuCommand);
            }
          }
        } else {
          doDirectCommand(key, true);
        }
        break;

      default: // A, B, C, D
        doDirectCommand(key, true);
        break;
      }
      break;

    case KEYPAD_USE_ENTER_WITHROTTLE_SERVER:
      debug_print("key: Enter wit... ");
      debug_println(std::to_string(key).c_str());

      switch (key) {
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
        witServer.witEntryAddChar(key);
        break;
      case '*': // backspace
        witServer.witEntryDeleteChar(key);
        break;
      case '#': // end of command
        if (globalValues.witServerIpAndPortEntered.length() == 17) {
          globalValues.dccexConnectionState = CONNECTION_STATE_ENTERED;
        }
        break;
      default: // do nothing
        break;
      }

      break;

    case KEYPAD_USE_ENTER_SSID_PASSWORD:
      debug_print("key: Enter password... ");
      debug_println(std::to_string(key).c_str());

      switch (key) {
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
        wifiManager.ssidPasswordAddChar(key);
        break;
      case '*': // backspace
        wifiManager.ssidPasswordDeleteChar(key);
        break;
      case '#': // end of command
        globalValues.selectedSsidPassword = globalValues.ssidPasswordEntered;
        globalValues.encoderUseType = ENCODER_USE_OPERATION;
        globalValues.keypadUseType = KEYPAD_USE_OPERATION;
        globalValues.ssidConnectionState = CONNECTION_STATE_SELECTED;
        break;
      default: // do nothing
        break;
      }

      break;

    case KEYPAD_USE_SELECT_WITHROTTLE_SERVER:
      debug_print("key: Select wit... ");
      debug_println(std::to_string(key).c_str());

      switch (key) {
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
        witServer.selectWitServer(key - '0');
        break;
      case '#': // show ip address entry screen
        globalValues.dccexConnectionState = CONNECTION_STATE_ENTRY_REQUIRED;
        witServer.buildWitEntry();
        witServer.enterWitServer();
        break;
      default: // do nothing
        break;
      }
      break;

    case KEYPAD_USE_SELECT_SSID:
      debug_print("key ssid... ");
      debug_println(std::to_string(key).c_str());

      switch (key) {
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
        wifiManager.selectSsid(key - '0'); 
        break;
      case '#': // show found SSIds
        globalValues.ssidConnectionState = CONNECTION_STATE_DISCONNECTED;
        globalValues.keypadUseType = KEYPAD_USE_SELECT_SSID_FROM_FOUND;
        globalValues.ssidSelectionSource = SSID_CONNECTION_SOURCE_BROWSE;
        // browseSsids();
        break;
      default: // do nothing
        break;
      }
      break;

    case KEYPAD_USE_SELECT_SSID_FROM_FOUND:
      debug_print("key ssid from found... ");
      debug_println(std::to_string(key).c_str());

      switch (key) {
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
        wifiManager.selectSsidFromFound(key - '0' + (globalValues.page * 5)); 
        break;
      case '#': // next page
        if (globalValues.foundSsidsCount > 5) {
          if ((globalValues.page + 1) * 5 < globalValues.foundSsidsCount) {
            globalValues.page++;
          } else {
            globalValues.page = 0;
          }
          oledDisplay.writeOledFoundSSids("");
        }
        break;
      case '9': // show in code list of SSIDs
        globalValues.ssidConnectionState = CONNECTION_STATE_DISCONNECTED;
        globalValues.keypadUseType = KEYPAD_USE_SELECT_SSID;
        globalValues.ssidSelectionSource = SSID_CONNECTION_SOURCE_LIST;
        break;
      default: // do nothing
        break;
      }
      break;

    case KEYPAD_USE_SELECT_ROSTER:
      debug_print("key Roster... ");
      debug_println(std::to_string(key).c_str());

      switch (key) {
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
        dccexController.selectRoster((key - '0') + (globalValues.page * 5)); 
        break;
      case '#': // next page
        if (globalValues.rosterSize > 5) {
          if ((globalValues.page + 1) * 5 < globalValues.rosterSize) {
            globalValues.page++;
          } else {
            globalValues.page = 0;
          }
          oledDisplay.writeOledRoster("");
        }
        break;
      case '*': // cancel
        resetMenu(globalValues, oledDisplay);
        oledDisplay.writeOledSpeed();
        break;
      default: // do nothing
        break;
      }
      break;

    case KEYPAD_USE_SELECT_TURNOUTS_THROW:
    case KEYPAD_USE_SELECT_TURNOUTS_CLOSE:
      debug_print("key turnouts... ");
      debug_println(std::to_string(key).c_str());

      switch (key) {
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
        dccexController.selectTurnoutList((key - '0') + (globalValues.page * 10), (globalValues.keypadUseType == KEYPAD_USE_SELECT_TURNOUTS_THROW) ? true : false); 
        break;
      case '#': // next page
        if (globalValues.turnoutListSize > 10) {
          if ((globalValues.page + 1) * 10 < globalValues.turnoutListSize) {
            globalValues.page++;
          } else {
            globalValues.page = 0;
          }
          oledDisplay.writeOledTurnoutList("", (globalValues.keypadUseType == KEYPAD_USE_SELECT_TURNOUTS_THROW) ? true : false);
        }
        break;
      case '*': // cancel
        resetMenu(globalValues, oledDisplay);
        oledDisplay.writeOledSpeed();
        break;
      default: // do nothing
        break;
      }
      break;

    case KEYPAD_USE_SELECT_ROUTES:
      debug_print("key routes... ");
      debug_println(std::to_string(key).c_str());

      switch (key) {
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
        dccexController.selectRouteList((key - '0') + (globalValues.page * 10));
        break;
      case '#': // next page
        if (globalValues.routeListSize > 10) {
          if ((globalValues.page + 1) * 10 < globalValues.routeListSize) {
            globalValues.page++;
          } else {
            globalValues.page = 0;
          }
          oledDisplay.writeOledRouteList("");
        }
        break;
      case '*': // cancel
        resetMenu(globalValues, oledDisplay);
        oledDisplay.writeOledSpeed();
        break;
      default: // do nothing
        break;
      }
      break;

    case KEYPAD_USE_SELECT_FUNCTION:
      debug_print("key function... ");
      debug_println(std::to_string(key).c_str());

      switch (key) {
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
        dccexController.selectFunctionList((key - '0') + (globalValues.functionPage * 10));
        break;
      case '#': // next page
        if ((globalValues.functionPage + 1) * 10 < MAX_STATE_FUNCTS) {
          globalValues.functionPage++;
          oledDisplay.writeOledFunctionList("");
        } else {
          globalValues.functionPage = 0;
          globalValues.keypadUseType = KEYPAD_USE_OPERATION;
          oledDisplay.writeOledDirectCommands();
        }
        break;
      case '*': // cancel
        resetMenu(globalValues, oledDisplay);
        oledDisplay.writeOledSpeed();
        break;
      default: // do nothing
        break;
      }
      break;

    case KEYPAD_USE_EDIT_CONSIST:
      debug_print("key Edit Consist... ");
      debug_println(std::to_string(key).c_str());

      switch (key) {
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
        if ((key - '0') <= throttles.throttles[throttles.currentThrottleIndex]->getLocoCount()) {
          dccexController.selectEditConsistList(key - '0');
        }
        break;
      case '*': // cancel
        resetMenu(globalValues, oledDisplay);
        oledDisplay.writeOledSpeed();
        break;
      default: // do nothing
        break;
      }
      break;

    default: // do nothing
      break;
    }
  }
}


void KeypadManager::resetMenu(GlobalValues &globalValues, OledDisplay &oledDisplay) {
  debug_println("resetMenu(globalValues, oledDisplay)");
  globalValues.page = 0;
  oledDisplay.menuCommand = "";
  oledDisplay.menuCommandStarted = false;
  if ( (globalValues.keypadUseType != KEYPAD_USE_SELECT_SSID) 
    && (globalValues.keypadUseType != KEYPAD_USE_SELECT_WITHROTTLE_SERVER) ) {
    globalValues.keypadUseType = KEYPAD_USE_OPERATION; 
  }
}


void KeypadManager::doMenu() {
  int address;
  std::string function = "";
  bool result = false;
  // int index;

    GlobalValues &globalValues = GlobalValues::getInstance();
    OledDisplay &oledDisplay = OledDisplay::getInstance();
    Throttles &throttles = Throttles::getInstance();
    WitServer &witServer = WitServer::getInstance();
    WiFiManager &wifiManager = WiFiManager::getInstance();
    FunctionStates &functionStates = FunctionStates::getInstance();
    DccExController &dccexController = DccExController::getInstance();

  debug_print("Menu: "); debug_println(oledDisplay.menuCommand.c_str());
  
  switch (oledDisplay.menuCommand[0]) {
    case MENU_ITEM_ADD_LOCO: { // select loco
        if (oledDisplay.menuCommand.length()>1) {
          address = std::stoi(oledDisplay.menuCommand.substr(1, oledDisplay.menuCommand.length()));
          debug_print("add Loco: "); debug_println(std::to_string(address).c_str());
          
          // Loco* loco1 = new Loco(address, LocoSource::LocoSourceEntry);
          // throttles.throttles[throttles.currentThrottleIndex]->addLoco(loco1,FacingForward);
          // debug_println(throttles.throttles[throttles.currentThrottleIndex]->getLocoCount());
          // resetFunctionStates(throttles.currentThrottleIndex);
          // loadDefaultFunctionLabels(throttles.currentThrottleIndex);

          Loco* loco1 = nullptr;
          loco1 = dccexProtocol->findLocoInRoster(address);
          if ( ( loco1 == nullptr )
          || ((!searchRosterOnEntryOfDccAddress) && ( loco1 != nullptr ))) {
            loco1 = new Loco(address, LocoSource::LocoSourceEntry);
            throttles.throttles[throttles.currentThrottleIndex]->addLoco(loco1,FacingForward);
            debug_print("Add Loco: LocoNotInRoster ");
            debug_println(std::to_string(throttles.throttles[throttles.currentThrottleIndex]->getLocoCount()).c_str());
            dccexProtocol->requestLocoUpdate(throttles.throttles[throttles.currentThrottleIndex]->getFirst()->getLoco()->getAddress());
            functionStates.resetFunctionStates(throttles.currentThrottleIndex);
            functionStates.loadDefaultFunctionLabels(throttles.currentThrottleIndex);  
          } else {
            throttles.throttles[throttles.currentThrottleIndex]->addLoco(loco1,FacingForward);
            debug_print("Add loco: LocoInRoster ");
            debug_println(std::to_string(throttles.throttles[throttles.currentThrottleIndex]->getLocoCount()).c_str());
            dccexProtocol->requestLocoUpdate(throttles.throttles[throttles.currentThrottleIndex]->getFirst()->getLoco()->getAddress());
            functionStates.resetFunctionStates(throttles.currentThrottleIndex);
            functionStates.loadDefaultFunctionLabels(throttles.currentThrottleIndex);  
          }
          oledDisplay.writeOledSpeed();
        } else {
          globalValues.page = 0;
          oledDisplay.writeOledRoster("");
        }
        break;
      }
    case MENU_ITEM_DROP_LOCO: { // de-select loco
        address = std::stoi(oledDisplay.menuCommand.substr(1,oledDisplay.menuCommand.length()));
        if (address!=0) { // a loco is specified
          throttles.releaseOneLoco(throttles.currentThrottleIndex, address);
        } else { //not loco specified so release all
          throttles.releaseAllLocos(throttles.currentThrottleIndex);
        }
        oledDisplay.writeOledSpeed();
        break;
      }
    case MENU_ITEM_TOGGLE_DIRECTION: { // change direction
        throttles.toggleDirection(throttles.currentThrottleIndex);
        break;
      }
     case MENU_ITEM_SPEED_STEP_MULTIPLIER: { // toggle speed step additional Multiplier
        throttles.toggleAdditionalMultiplier();
        break;
      }
   case MENU_ITEM_THROW_POINT: {  // throw point
        if (oledDisplay.menuCommand.length()>1) {
          int id =std::stoi(oledDisplay.menuCommand.substr(1,oledDisplay.menuCommand.length()));
          debug_print("throw point: "); debug_println(std::to_string(id).c_str());
          dccexProtocol->throwTurnout(id);
          oledDisplay.writeOledSpeed();
        } else {
          globalValues.page = 0;
          oledDisplay.writeOledTurnoutList("", true);
        }
        break;
      }
    case MENU_ITEM_CLOSE_POINT: {  // close point
        if (oledDisplay.menuCommand.length()>1) {
          int id =std::stoi(oledDisplay.menuCommand.substr(1,oledDisplay.menuCommand.length()));
          debug_print("close point: "); debug_println(std::to_string(id).c_str());
          dccexProtocol->closeTurnout(id);
          oledDisplay.writeOledSpeed();
        } else {
          globalValues.page = 0;
          oledDisplay.writeOledTurnoutList("", false);
        }
        break;
      }
    case MENU_ITEM_ROUTE: {  // route
        if (oledDisplay.menuCommand.length()>1) {
          int id =std::stoi(oledDisplay.menuCommand.substr(1,oledDisplay.menuCommand.length()));
          // if (!route.equals("")) { // a loco is specified
            debug_print("route: "); debug_println(std::to_string(id).c_str());
            dccexProtocol->startRoute(id);
          // }
          oledDisplay.writeOledSpeed();
        } else {
          globalValues.page = 0;
          oledDisplay.writeOledRouteList("");
        }
        break;
      }
    case MENU_ITEM_TRACK_POWER: {
        dccexController.powerToggle();
        break;
      }
    case MENU_ITEM_EXTRAS: { // Extra menu - e.g. disconnect/reconnect/sleep
        char subCommand =oledDisplay.menuCommand.at(1);
        if (oledDisplay.menuCommand.length() > 1) {
          switch (subCommand) {
            case EXTRA_MENU_CHAR_FUNCTION_KEY_TOGGLE: { // toggle showing Def Keys vs Function labels
                hashShowsFunctionsInsteadOfKeyDefs = !hashShowsFunctionsInsteadOfKeyDefs;
                oledDisplay.writeOledSpeed();
                break;
              } 
            case EXTRA_MENU_CHAR_EDIT_CONSIST: { // edit consist - loco facings
                oledDisplay.writeOledEditConsist();
                break;
              } 
            case EXTRA_MENU_CHAR_HEARTBEAT_TOGGLE: { // disable/enable the heartbeat Check
                dccexController.toggleHeartbeatCheck();
                oledDisplay.writeOledSpeed();
                break;
              }
            case EXTRA_MENU_CHAR_INCREASE_MAX_THROTTLES: { //increase number of Throttles
                dccexController.changeNumberOfThrottles(true);
                break;
              }
            case EXTRA_MENU_CHAR_DECREASE_MAX_THROTTLES: { // decrease numbe rof throttles
                dccexController.changeNumberOfThrottles(false);
                break;
              }
            case EXTRA_MENU_CHAR_DISCONNECT: { // disconnect   
                if (globalValues.dccexConnectionState == CONNECTION_STATE_CONNECTED) {
                  globalValues.dccexConnectionState = CONNECTION_STATE_DISCONNECTED;
                  witServer.disconnectWitServer();
                } else {
                  witServer.connectWitServer();
                }
                break;
              }
            case EXTRA_MENU_CHAR_OFF_SLEEP:
            case EXTRA_MENU_CHAR_OFF_SLEEP_HIDDEN: { // sleep/off
                // deepSleepStart();
                break;
              }
          }
        } else {
          oledDisplay.writeOledSpeed();
        }
        break;
      }
    case MENU_ITEM_FUNCTION: { // function button
        if (oledDisplay.menuCommand.length()>1) {
          function =oledDisplay.menuCommand.substr(1,oledDisplay.menuCommand.length());
          int functionNumber = std::stoi(function);
          if (function != "") { // a function is specified
            doFunction(throttles.currentThrottleIndex, functionNumber, true);  // always act like latching i.e. pressed
          }
          oledDisplay.writeOledSpeed();
        } else {
          globalValues.functionPage = 0;
          oledDisplay.writeOledFunctionList("");
        }
        break;
      }
  }
 oledDisplay.menuCommandStarted = result; 
}

void KeypadManager::doDirectCommand(char key, bool isPressed) {

  // debug_print("doDirectCommand(): key: "); debug_print(key);  debug_print(" pressed: "); debug_println(pressed); 
  // int buttonAction = 0 ;
  // if (key<=57) {
  //   buttonAction = buttonActions[(key - '0')];
  // } else {
  //   buttonAction = buttonActions[(key - 55)]; // A, B, C, D
  // }
  // if (buttonAction!=FUNCTION_NULL) {
  //   if ( (buttonAction>=FUNCTION_0) && (buttonAction<=FUNCTION_31) ) {
  //     doDirectCommandFunction(throttles.currentThrottleIndex, buttonAction, pressed);
  //   } else {       
  //     if (pressed) { // only process these on the key press, not the release
  //         doDirectAction(buttonAction);
  //     }
  //   }
  // }
  // debug_println("doDirectCommand(): end"); 
}

