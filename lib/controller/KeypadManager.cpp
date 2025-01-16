#include "KeypadManager.h"
#include <pico/stdlib.h>
#include <DCCEXLoco.h>

#include "ExternalSingletons.h"
#include "GlobalValues.h"
#include "OledDisplay.h"  
#include "Throttles.h"
#include "WiFiManager.h"
#include "DCCEXProtocol.h"
#include "WitServer.h"
#include "FunctionStates.h"

extern bool hashShowsFunctionsInsteadOfKeyDefs;
extern bool oledDirectCommandsAreBeingDisplayed;

using namespace DCCExController;

void KeypadManager::setup() {

}

void KeypadManager::loop() {

  auto key = keypad.getKey();
  if (key) {

    debug_print("Key Pressed: ");
    debug_println(std::to_string(key).c_str());

    // Add specific key press handling logic here
    // For example, you can map keys to specific actions
    switch (globalValues->keypadUseType) {
    case KEYPAD_USE_OPERATION:
      debug_print("key operation... ");
      debug_println(std::to_string(key).c_str());
      switch (key) {
      case '*': // menu command
        oledDisplay->menuCommand = "";
        if (oledDisplay->menuCommandStarted) { // then cancel the menu
          resetMenu();
          oledDisplay->writeOledSpeed();
        } else {
          oledDisplay->menuCommandStarted = true;
          debug_println("Command started");
          oledDisplay->writeOledMenu("");
        }
        break;

      case '#': // end of command
        debug_print("end of command... ");
        debug_println(std::to_string(key).c_str());
        debug_print("  :  ");
        debug_println(oledDisplay->menuCommand.c_str());
        if ((oledDisplay->menuCommandStarted) && (oledDisplay->menuCommand.length() >= 1)) {
          doMenu();
        } else {
          if (!hashShowsFunctionsInsteadOfKeyDefs) {
            if (!oledDirectCommandsAreBeingDisplayed) {
              oledDisplay->writeOledDirectCommands();
            } else {
              oledDirectCommandsAreBeingDisplayed = false;
              oledDisplay->writeOledSpeed();
            }
          } else {
            oledDisplay->writeOledFunctionList("");
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
        debug_print(oledDisplay->menuCommand.c_str());
        debug_println("'");

        if (oledDisplay->menuCommandStarted) { // append to the string

          if ((menuCharsRequired[key - 48] == 0) && (oledDisplay->menuCommand.length() == 0)) { // menu type is effectively a direct commands from this point
            oledDisplay->menuCommand += key;
            doMenu();
          } else {
            if ((menuCharsRequired[ std::stoi(oledDisplay->menuCommand.substr(0, 1))] == 1) && (oledDisplay->menuCommand.length() == 1)) { // menu type needs only one char
              oledDisplay->menuCommand += key;
              doMenu();

            } else { // menu type allows/requires more than one char
              oledDisplay->menuCommand += key;
              oledDisplay->writeOledMenu(oledDisplay->menuCommand);
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
        witServer->witEntryAddChar(key);
        break;
      case '*': // backspace
        witServer->witEntryDeleteChar(key);
        break;
      case '#': // end of command
        if (globalValues->witServerIpAndPortEntered.length() == 17) {
          globalValues->dccexConnectionState = CONNECTION_STATE_ENTERED;
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
        wifiManager->ssidPasswordAddChar(key);
        break;
      case '*': // backspace
        wifiManager->ssidPasswordDeleteChar(key);
        break;
      case '#': // end of command
        globalValues->selectedSsidPassword = globalValues->ssidPasswordEntered;
        globalValues->encoderUseType = ENCODER_USE_OPERATION;
        globalValues->keypadUseType = KEYPAD_USE_OPERATION;
        globalValues->ssidConnectionState = CONNECTION_STATE_SELECTED;
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
        witServer->selectWitServer(key - '0');
        break;
      case '#': // show ip address entry screen
        globalValues->dccexConnectionState = CONNECTION_STATE_ENTRY_REQUIRED;
        witServer->buildWitEntry();
        witServer->enterWitServer();
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
        wifiManager->selectSsid(key - '0'); 
        break;
      case '#': // show found SSIds
        globalValues->ssidConnectionState = CONNECTION_STATE_DISCONNECTED;
        globalValues->keypadUseType = KEYPAD_USE_SELECT_SSID_FROM_FOUND;
        globalValues->ssidSelectionSource = SSID_CONNECTION_SOURCE_BROWSE;
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
        wifiManager->selectSsidFromFound(key - '0' + (globalValues->page * 5)); 
        break;
      case '#': // next page
        if (globalValues->foundSsidsCount > 5) {
          if ((globalValues->page + 1) * 5 < globalValues->foundSsidsCount) {
            globalValues->page++;
          } else {
            globalValues->page = 0;
          }
          oledDisplay->writeOledFoundSSids("");
        }
        break;
      case '9': // show in code list of SSIDs
        globalValues->ssidConnectionState = CONNECTION_STATE_DISCONNECTED;
        globalValues->keypadUseType = KEYPAD_USE_SELECT_SSID;
        globalValues->ssidSelectionSource = SSID_CONNECTION_SOURCE_LIST;
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
        dccExController->selectRoster((key - '0') + (globalValues->page * 5)); 
        break;
      case '#': // next page
        if (globalValues->rosterSize > 5) {
          if ((globalValues->page + 1) * 5 < globalValues->rosterSize) {
            globalValues->page++;
          } else {
            globalValues->page = 0;
          }
          oledDisplay->writeOledRoster("");
        }
        break;
      case '*': // cancel
        resetMenu();
        oledDisplay->writeOledSpeed();
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
        dccExController->selectTurnoutList((key - '0') + (globalValues->page * 10), (globalValues->keypadUseType == KEYPAD_USE_SELECT_TURNOUTS_THROW) ? true : false); 
        break;
      case '#': // next page
        if (globalValues->turnoutListSize > 10) {
          if ((globalValues->page + 1) * 10 < globalValues->turnoutListSize) {
            globalValues->page++;
          } else {
            globalValues->page = 0;
          }
          oledDisplay->writeOledTurnoutList("", (globalValues->keypadUseType == KEYPAD_USE_SELECT_TURNOUTS_THROW) ? true : false);
        }
        break;
      case '*': // cancel
        resetMenu();
        oledDisplay->writeOledSpeed();
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
        dccExController->selectRouteList((key - '0') + (globalValues->page * 10));
        break;
      case '#': // next page
        if (globalValues->routeListSize > 10) {
          if ((globalValues->page + 1) * 10 < globalValues->routeListSize) {
            globalValues->page++;
          } else {
            globalValues->page = 0;
          }
          oledDisplay->writeOledRouteList("");
        }
        break;
      case '*': // cancel
        resetMenu();
        oledDisplay->writeOledSpeed();
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
        dccExController->selectFunctionList((key - '0') + (globalValues->functionPage * 10));
        break;
      case '#': // next page
        if ((globalValues->functionPage + 1) * 10 < MAX_STATE_FUNCTS) {
          globalValues->functionPage++;
          oledDisplay->writeOledFunctionList("");
        } else {
          globalValues->functionPage = 0;
          globalValues->keypadUseType = KEYPAD_USE_OPERATION;
          oledDisplay->writeOledDirectCommands();
        }
        break;
      case '*': // cancel
        resetMenu();
        oledDisplay->writeOledSpeed();
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
        if ((key - '0') <= throttles->throttles[throttles->currentThrottleIndex]->getLocoCount()) {
          dccExController->selectEditConsistList(key - '0');
        }
        break;
      case '*': // cancel
        resetMenu();
        oledDisplay->writeOledSpeed();
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


void KeypadManager::resetMenu() {
  debug_println("resetMenu");
  globalValues->page = 0;
  oledDisplay->menuCommand = "";
  oledDisplay->menuCommandStarted = false;
  if ( (globalValues->keypadUseType != KEYPAD_USE_SELECT_SSID) 
    && (globalValues->keypadUseType != KEYPAD_USE_SELECT_WITHROTTLE_SERVER) ) {
    globalValues->keypadUseType = KEYPAD_USE_OPERATION; 
  }
}


void KeypadManager::doMenu() {
  int address;
  std::string function = "";
  bool result = false;
  // int index;

  debug_print("Menu: "); debug_println(oledDisplay->menuCommand.c_str());
  
  switch (oledDisplay->menuCommand[0]) {
    case MENU_ITEM_ADD_LOCO: { // select loco
        if (oledDisplay->menuCommand.length()>1) {
          address = std::stoi(oledDisplay->menuCommand.substr(1, oledDisplay->menuCommand.length()));
          debug_print("add Loco: "); debug_println(std::to_string(address).c_str());
          
          // Loco* loco1 = new Loco(address, LocoSource::LocoSourceEntry);
          // throttles->throttles[throttles->currentThrottleIndex]->addLoco(loco1,FacingForward);
          // debug_println(throttles->throttles[throttles->currentThrottleIndex]->getLocoCount());
          // resetFunctionStates(throttles->currentThrottleIndex);
          // loadDefaultFunctionLabels(throttles->currentThrottleIndex);

          Loco* loco1 = nullptr;
          loco1 = dccExProtocol->findLocoInRoster(address);
          if ( ( loco1 == nullptr )
          || ((!searchRosterOnEntryOfDccAddress) && ( loco1 != nullptr ))) {
            loco1 = new Loco(address, LocoSource::LocoSourceEntry);
            throttles->throttles[throttles->currentThrottleIndex]->addLoco(loco1,FacingForward);
            debug_print("Add Loco: LocoNotInRoster ");
            debug_println(std::to_string(throttles->throttles[throttles->currentThrottleIndex]->getLocoCount()).c_str());
            dccExProtocol->requestLocoUpdate(throttles->throttles[throttles->currentThrottleIndex]->getFirst()->getLoco()->getAddress());
            functionStates->resetFunctionStates(throttles->currentThrottleIndex);
            functionStates->loadDefaultFunctionLabels(throttles->currentThrottleIndex);  
          } else {
            throttles->throttles[throttles->currentThrottleIndex]->addLoco(loco1,FacingForward);
            debug_print("Add loco: LocoInRoster ");
            debug_println(std::to_string(throttles->throttles[throttles->currentThrottleIndex]->getLocoCount()).c_str());
            dccExProtocol->requestLocoUpdate(throttles->throttles[throttles->currentThrottleIndex]->getFirst()->getLoco()->getAddress());
            functionStates->resetFunctionStates(throttles->currentThrottleIndex);
            functionStates->loadDefaultFunctionLabels(throttles->currentThrottleIndex);  
          }
          oledDisplay->writeOledSpeed();
        } else {
          globalValues->page = 0;
          oledDisplay->writeOledRoster("");
        }
        break;
      }
    case MENU_ITEM_DROP_LOCO: { // de-select loco
        address = std::stoi(oledDisplay->menuCommand.substr(1,oledDisplay->menuCommand.length()));
        if (address!=0) { // a loco is specified
          throttles->releaseOneLoco(throttles->currentThrottleIndex, address);
        } else { //not loco specified so release all
          throttles->releaseAllLocos(throttles->currentThrottleIndex);
        }
        oledDisplay->writeOledSpeed();
        break;
      }
    case MENU_ITEM_TOGGLE_DIRECTION: { // change direction
        throttles->toggleDirection(throttles->currentThrottleIndex);
        break;
      }
     case MENU_ITEM_SPEED_STEP_MULTIPLIER: { // toggle speed step additional Multiplier
        throttles->toggleAdditionalMultiplier();
        break;
      }
   case MENU_ITEM_THROW_POINT: {  // throw point
        if (oledDisplay->menuCommand.length()>1) {
          int id =std::stoi(oledDisplay->menuCommand.substr(1,oledDisplay->menuCommand.length()));
          debug_print("throw point: "); debug_println(std::to_string(id).c_str());
          dccExProtocol->throwTurnout(id);
          oledDisplay->writeOledSpeed();
        } else {
          globalValues->page = 0;
          oledDisplay->writeOledTurnoutList("", true);
        }
        break;
      }
    case MENU_ITEM_CLOSE_POINT: {  // close point
        if (oledDisplay->menuCommand.length()>1) {
          int id =std::stoi(oledDisplay->menuCommand.substr(1,oledDisplay->menuCommand.length()));
          debug_print("close point: "); debug_println(std::to_string(id).c_str());
          dccExProtocol->closeTurnout(id);
          oledDisplay->writeOledSpeed();
        } else {
          globalValues->page = 0;
          oledDisplay->writeOledTurnoutList("", false);
        }
        break;
      }
    case MENU_ITEM_ROUTE: {  // route
        if (oledDisplay->menuCommand.length()>1) {
          int id =std::stoi(oledDisplay->menuCommand.substr(1,oledDisplay->menuCommand.length()));
          // if (!route.equals("")) { // a loco is specified
            debug_print("route: "); debug_println(std::to_string(id).c_str());
            dccExProtocol->startRoute(id);
          // }
          oledDisplay->writeOledSpeed();
        } else {
          globalValues->page = 0;
          oledDisplay->writeOledRouteList("");
        }
        break;
      }
    case MENU_ITEM_TRACK_POWER: {
        dccExController->powerToggle();
        break;
      }
    case MENU_ITEM_EXTRAS: { // Extra menu - e.g. disconnect/reconnect/sleep
        char subCommand =oledDisplay->menuCommand.at(1);
        if (oledDisplay->menuCommand.length() > 1) {
          switch (subCommand) {
            case EXTRA_MENU_CHAR_FUNCTION_KEY_TOGGLE: { // toggle showing Def Keys vs Function labels
                hashShowsFunctionsInsteadOfKeyDefs = !hashShowsFunctionsInsteadOfKeyDefs;
                oledDisplay->writeOledSpeed();
                break;
              } 
            case EXTRA_MENU_CHAR_EDIT_CONSIST: { // edit consist - loco facings
                oledDisplay->writeOledEditConsist();
                break;
              } 
            case EXTRA_MENU_CHAR_HEARTBEAT_TOGGLE: { // disable/enable the heartbeat Check
                dccExController->toggleHeartbeatCheck();
                oledDisplay->writeOledSpeed();
                break;
              }
            case EXTRA_MENU_CHAR_INCREASE_MAX_THROTTLES: { //increase number of Throttles
                dccExController->changeNumberOfThrottles(true);
                break;
              }
            case EXTRA_MENU_CHAR_DECREASE_MAX_THROTTLES: { // decrease numbe rof throttles
                dccExController->changeNumberOfThrottles(false);
                break;
              }
            case EXTRA_MENU_CHAR_DISCONNECT: { // disconnect   
                if (globalValues->dccexConnectionState == CONNECTION_STATE_CONNECTED) {
                  globalValues->dccexConnectionState = CONNECTION_STATE_DISCONNECTED;
                  witServer->disconnectWitServer();
                } else {
                  witServer->connectWitServer();
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
          oledDisplay->writeOledSpeed();
        }
        break;
      }
    case MENU_ITEM_FUNCTION: { // function button
        if (oledDisplay->menuCommand.length()>1) {
          function =oledDisplay->menuCommand.substr(1,oledDisplay->menuCommand.length());
          int functionNumber = std::stoi(function);
          if (function != "") { // a function is specified
            doFunction(throttles->currentThrottleIndex, functionNumber, true);  // always act like latching i.e. pressed
          }
          oledDisplay->writeOledSpeed();
        } else {
          globalValues->functionPage = 0;
          oledDisplay->writeOledFunctionList("");
        }
        break;
      }
  }
 oledDisplay->menuCommandStarted = result; 
}

void KeypadManager::doDirectCommand(char key, bool pressed) {
  debug_print("doDirectCommand(): key: "); debug_print(std::string(1, key).c_str());  debug_print(" pressed: "); debug_println(pressed ? "True" : "False"); 

  int buttonAction = 0 ;
  if (key<=57) {
    buttonAction = globalValues->buttonActions[(key - '0')];
  } else {
    buttonAction = globalValues->buttonActions[(key - 55)]; // A, B, C, D
  }
  if (buttonAction!=FUNCTION_NULL) {
    if ( (buttonAction>=FUNCTION_0) && (buttonAction<=FUNCTION_31) ) {
      doDirectCommandFunction(throttles->currentThrottleIndex, buttonAction, pressed);
    } else {       
      if (pressed) { // only process these on the key press, not the release
          doDirectAction(buttonAction);
      }
    }
  }
  debug_println("doDirectCommand(): end"); 
}

void  KeypadManager::doDirectFunction(int multiThrottleIndex, int functionNumber, bool pressed) {
  debug_print("doDirectFunction(): "); debug_print(std::to_string(multiThrottleIndex).c_str());  debug_print(" , "); debug_println(std::to_string(functionNumber).c_str());
  
  if (throttles->throttles[throttles->currentThrottleIndex]->getLocoCount() > 0) {
    debug_print("direct fn: "); debug_println(std::to_string(functionNumber).c_str()); debug_println( pressed ? " Pressed" : " Released");
    doFunctionWhichLocosInConsist(multiThrottleIndex, functionNumber, pressed);
    oledDisplay->writeOledSpeed(); 
  }
  // debug_print("doDirectFunction(): end"); 
}

void  KeypadManager::doFunction(int multiThrottleIndex, int functionNumber, bool pressed) {   // currently ignoring the pressed value
  debug_print("doFunction(): multiThrottleIndex "); debug_println(std::to_string(multiThrottleIndex).c_str());
  
  if (throttles->throttles[throttles->currentThrottleIndex]->getLocoCount() > 0) {
    doFunctionWhichLocosInConsist(multiThrottleIndex, functionNumber, true);
  

    if (!globalValues->functionStates[multiThrottleIndex][functionNumber]) {
      debug_print("fn: "); debug_print(std::to_string(functionNumber).c_str()); debug_println(" Pressed");
      // functionStates[functionNumber] = true;
    } else {
      sleep_ms(20);
      doFunctionWhichLocosInConsist(multiThrottleIndex, functionNumber, false);
      debug_print("fn: "); debug_print(std::to_string(functionNumber).c_str()); debug_println(" Released");
      // functionStates[functionNumber] = false;
    }
    oledDisplay->writeOledSpeed(); 
  }
  // debug_println("doFunction(): ");
}

// Work out which locos in a consist should get the function
//
void  KeypadManager::doFunctionWhichLocosInConsist(int multiThrottleIndex, int functionNumber, bool pressed) {
  debug_print("doFunctionWhichLocosInConsist() multiThrottleIndex "); debug_print(std::to_string(multiThrottleIndex).c_str());
  debug_print(" fn: "); debug_println(std::to_string(functionNumber).c_str());

  if (globalValues->functionFollow[multiThrottleIndex][functionNumber]==CONSIST_LEAD_LOCO) {
    throttles->throttles[multiThrottleIndex]->setFunction(throttles->throttles[multiThrottleIndex]->getFirst()->getLoco(), functionNumber,pressed);
  } else {  // at the momemnt the only other option in CONSIST_ALL_LOCOS
    for (int i=0; i<throttles->throttles[multiThrottleIndex]->getLocoCount(); i++) {
      throttles->throttles[multiThrottleIndex]->setFunction(throttles->throttles[multiThrottleIndex]->getLocoAtPosition(i)->getLoco(), functionNumber,pressed);
    }
  }
  debug_println("doFunctionWhichLocosInConsist(): end fn: ");
}

void KeypadManager::doDirectCommandFunction(int currentThrottleIndex, int buttonAction, bool pressed) {
  debug_print("doDirectCommandFunction(): "); debug_println(std::to_string(buttonAction).c_str());  debug_print(" pressed: "); debug_println(pressed ? "True" : "False"); 

  if (globalValues->functionMomentary[currentThrottleIndex][buttonAction])  { // is momentary
    debug_print("doDirectCommandFunction(): function is Momentary"); 
    doDirectFunction(currentThrottleIndex, buttonAction, pressed);
    return;
  }
  debug_print("doDirectCommandFunction(): function is Latching"); 
  if (pressed){ // onKeyDown only
    if (globalValues->functionStates[currentThrottleIndex][buttonAction] ) { // if function true set false
      doDirectFunction(currentThrottleIndex, buttonAction, false);
    } else { // if function false set true
      doDirectFunction(currentThrottleIndex, buttonAction, true);
    } 
  }
}

void KeypadManager::doDirectAction(int buttonAction) {
  debug_print("doDirectAction(): ");  debug_println(std::to_string(buttonAction).c_str());
    
  switch (buttonAction) {
      case DIRECTION_FORWARD: {
        throttles->changeDirection(throttles->currentThrottleIndex, Forward);
        break; 
      }
      case DIRECTION_REVERSE: {
        throttles->changeDirection(throttles->currentThrottleIndex, Reverse);
        break; 
      }
      case DIRECTION_TOGGLE: {
        throttles->toggleDirection(throttles->currentThrottleIndex);
        break; 
      }
      case SPEED_STOP: {
        throttles->speedSet(throttles->currentThrottleIndex, 0);
        break; 
      }
      case SPEED_UP: {
        throttles->speedUp(throttles->currentThrottleIndex, throttles->currentSpeedStep[throttles->currentThrottleIndex]);
        break; 
      }
      case SPEED_DOWN: {
        throttles->speedDown(throttles->currentThrottleIndex, throttles->currentSpeedStep[throttles->currentThrottleIndex]);
        break; 
      }
      case SPEED_UP_FAST: {
        throttles->speedUp(throttles->currentThrottleIndex, throttles->currentSpeedStep[throttles->currentThrottleIndex]*speedStepMultiplier);
        break; 
      }
      case SPEED_DOWN_FAST: {
        throttles->speedUp(throttles->currentThrottleIndex, throttles->currentSpeedStep[throttles->currentThrottleIndex]*speedStepMultiplier);
        break; 
      }
      case SPEED_MULTIPLIER: {
        throttles->toggleAdditionalMultiplier();
        break; 
      }
      case E_STOP: {
        throttles->speedEstop();
        break; 
      }
      case E_STOP_CURRENT_LOCO: {
        throttles->speedEstopCurrentLoco();
        break; 
      }
      case POWER_ON: {
        throttles->powerOnOff(PowerOn);
        break; 
      }
      case POWER_OFF: {
        throttles->powerOnOff(PowerOff);
        break; 
      }
      case POWER_TOGGLE: {
        throttles->powerToggle();
        break; 
      }
      case NEXT_THROTTLE: {
        throttles->nextThrottle();
        break; 
      }
      case SPEED_STOP_THEN_TOGGLE_DIRECTION: {
        throttles->stopThenToggleDirection();
        break; 
      }
      case MAX_THROTTLE_INCREASE: {
        throttles->changeNumberOfThrottles(true);
        break; 
      }
      case MAX_THROTTLE_DECREASE: {
        throttles->changeNumberOfThrottles(false);
        break; 
      }

      // case CUSTOM_1: {
      //   debug_println("doDirectAction(): CUSTOM_1");
      //   dccExProtocol->sendCommand(customCommand1);
      //   break; 
      // }
      // case CUSTOM_2: {
      //   debug_println("doDirectAction(): CUSTOM_2");
      //   dccExProtocol.sendCommand(customCommand2);
      //   break; 
      // }
      // case CUSTOM_3: {
      //   debug_println("doDirectAction(): CUSTOM_3");
      //   dccExProtocol.sendCommand(customCommand3);
      //   break; 
      // }
      // case CUSTOM_4: {
      //   debug_println("doDirectAction(): CUSTOM_4");
      //   dccExProtocol.sendCommand(customCommand4);
      //   break; 
      // }
      // case CUSTOM_5: {
      //   dccExProtocol.sendCommand(customCommand5);
      //   break; 
      // }
      // case CUSTOM_6: {
      //   dccExProtocol.sendCommand(customCommand6);
      //   break; 
      // }
      // case CUSTOM_7: {
      //   dccExProtocol.sendCommand(customCommand7);
      //   break; 
      // }
    }
  // debug_println("doDirectAction(): end");
}
