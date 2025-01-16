

#include <DCCEXLoco.h>
#include "DccExController.h"
#include "ExternalSingletons.h"
#include "GlobalValues.h"
#include "OledDisplay.h"
#include "Throttles.h"
#include "DCCEXProtocol.h"
#include "FunctionStates.h"
#include "KeypadManager.h"
#include "WitServer.h"

using namespace DCCExController;

#define maxFoundWitServers 5     // must be 5 for the moment
#define maxFoundSsids 60     // must be a multiple of 5
#define maxRoster 60     // must be a multiple of 10
#define maxTurnoutList 60     // must be a multiple of 10
#define maxRouteList 60     // must be a multiple of 10

extern void displayUpdateFromWit(int multiThrottleIndex);

// Setup method
void DccExController::setup() {
}


void DccExController::loop() {
}

void DccExController::selectRoster(int selection){
  debug_print("selectRoster() "); debug_println(std::to_string(selection).c_str());

  if ((selection>=0) && (selection < globalValues->rosterSize)) {
    int address = globalValues->rosterAddress[selection];
    debug_print("add Loco: "); debug_print(std::to_string(address).c_str());
    Loco* loco1 = dccExProtocol->findLocoInRoster(address);
    throttles->throttles[throttles->currentThrottleIndex]->addLoco(loco1,FacingForward);

    debug_print(" name: "); debug_println(loco1->getName());

    functionStates->resetFunctionStates(throttles->currentThrottleIndex);
    throttles->loadFunctionLabels(throttles->currentThrottleIndex);

    oledDisplay->writeOledSpeed();
    globalValues->keypadUseType = KEYPAD_USE_OPERATION;
  }
}

void DccExController::selectTurnoutList(int selection, bool action) {
      debug_print("selectTurnoutList() "); debug_println(std::to_string(selection).c_str());

  if ((selection>=0) && (selection < globalValues->turnoutListSize)) {
    int id = globalValues->turnoutListSysName[selection];
    debug_print("Turnout Selected: "); debug_println(std::to_string(id).c_str());
    if (action) {
      dccExProtocol->throwTurnout(id);
    } else {
      dccExProtocol->closeTurnout(id);
    }
    oledDisplay->writeOledSpeed();
    globalValues->keypadUseType = KEYPAD_USE_OPERATION;
  }
}

void DccExController::selectRouteList(int selection) {
      debug_print("selectRouteList() "); debug_println(std::to_string(selection).c_str());

  if ((selection>=0) && (selection < globalValues->routeListSize)) {
    int id = globalValues->routeListSysName[selection];
    debug_print("Route Selected: "); debug_println(std::to_string(id).c_str());
    dccExProtocol->startRoute(id);
    oledDisplay->writeOledSpeed();
    globalValues->keypadUseType = KEYPAD_USE_OPERATION;
  }
}

void DccExController::selectFunctionList(int selection) {
      debug_print("selectFunctionList() "); debug_println(std::to_string(selection).c_str());

  if ((selection>=0) && (selection < MAX_STATE_FUNCTS)) {
    std::string function = globalValues->functionLabels[throttles->currentThrottleIndex][selection];
    debug_print("Function Selected: "); debug_println(function.c_str());
    keypadManager->doFunction(throttles->currentThrottleIndex, selection, true);
    oledDisplay->writeOledSpeed();
    globalValues->keypadUseType = KEYPAD_USE_OPERATION;
  }
}

void DccExController::selectEditConsistList(int selection) {
      debug_print("selectEditConsistList() "); debug_println(std::to_string(selection).c_str());

  if (throttles->throttles[throttles->currentThrottleIndex]->getLocoCount() > 1 ) {
    int address = throttles->throttles[throttles->currentThrottleIndex]->getLocoAtPosition(selection)->getLoco()->getAddress();
    throttles->toggleLocoFacing(throttles->currentThrottleIndex, address);

    oledDisplay->writeOledSpeed();
    globalValues->keypadUseType = KEYPAD_USE_OPERATION;
  }
}

void DccExController::_loadRoster() {
      debug_print("_loadRoster()");
  globalValues->rosterSize = dccExProtocol->getRosterCount();
  debug_println(std::to_string(globalValues->rosterSize).c_str());
  if (globalValues->rosterSize > 0) {
    int index = 0;
    for (Loco* rl=dccExProtocol->roster->getFirst(); rl; rl=rl->getNext()) {
      if (index < maxRoster) {
        globalValues->rosterIndex[index] = index; 
        globalValues->rosterName[index] = rl->getName(); 
        globalValues->rosterAddress[index] = rl->getAddress();
        index++;
      }
    }
  }
  debug_print("_loadRoster() end");
}

void DccExController::_loadTurnoutList() {
   debug_println("_loadTurnoutList()");
  globalValues->turnoutListSize = dccExProtocol->getTurnoutCount();
  debug_println(std::to_string(globalValues->turnoutListSize).c_str());
  if (globalValues->turnoutListSize > 0) {
    int index = 0;
    for (Turnout* tl=dccExProtocol->turnouts->getFirst(); tl; tl=tl->getNext()) {
      if (index < maxTurnoutList) {
        globalValues->turnoutListIndex[index] = index; 
        globalValues->turnoutListSysName[index] = tl->getId(); 
        globalValues->turnoutListUserName[index] = tl->getName();
        globalValues->turnoutListState[index] = tl->getThrown();
        index++;
      }
    }
  }
  debug_println("_loadTurnoutList() end");
}

void DccExController::_loadRouteList() {
      debug_println("_loadRouteList()");
  globalValues->routeListSize = dccExProtocol->getRouteCount();
  debug_println(std::to_string(globalValues->routeListSize).c_str());
  if (globalValues->routeListSize > 0) {
    int index = 0;
    for (Route* rl=dccExProtocol->routes->getFirst(); rl; rl=rl->getNext()) {
      if (index < maxRouteList) {
        debug_print("_loadRouteList() : "); debug_print(std::to_string(rl->getId()).c_str());
        globalValues->routeListIndex[index] = index; 
        globalValues->routeListSysName[index] = rl->getId(); 
        globalValues->routeListUserName[index] = rl->getName();
        index++;
      }
    }
  }
  debug_println("_loadRouteList() end");
}

void DccExController::_processLocoUpdate(Loco* loco) {
  throttles->debugLocoSpeed("_processLocoUpdate() start:", loco);
  bool found = false;
  for (int i=0; i<throttles->maxThrottles; i++) {
    if (throttles->throttles[i]->getLocoCount()>0) {
      Loco* firstLoco = throttles->throttles[i]->getFirst()->getLoco();
      if (loco == firstLoco) {
        found = true;

        // check for bounce. (intermediate speed sent back from the server, but is not up to date with the throttle)
        if ( (throttles->lastSpeedThrottleIndex != i) || ((pimoroni::millis()-throttles->lastSpeedSentTime)>2000) ) {
          throttles->currentSpeed[i] = loco->getSpeed();
          throttles->currentDirection[i] = loco->getDirection();
          throttles->debugLocoSpeed("_processLocoUpdate() Processing Received Speed: (" + std::to_string(throttles->lastSpeedThrottleIndex) + ") ", loco);
          displayUpdateFromWit(i);
        } else {
          // debug_print("Received Speed: skipping response: ("); debug_print(lastSpeedThrottleIndex); debug_print(") speed: "); debug_println(loco->getSpeed());
          throttles->debugLocoSpeed("_processLocoUpdate() Skipping Received Speed! ", loco);
        }
        for (int j=0; j<MAX_FUNCTIONS; j++) {
          int state = (loco->isFunctionOn(j)) ?  1 : 0;
          if (globalValues->functionStates[i][j] != loco->isFunctionOn(j)) {
            debug_println("Changed");
            globalValues->functionStates[i][j] = state;
            displayUpdateFromWit(i);
          }
        }
      }
    }
  }
  if (!found) {
    debug_println("_processLocoUpdate() loco not found");  
  }
  debug_println("_processLocoUpdate() end");
}

// void DccExController::deepSleepStart() {
//     // deepSleepStart(SLEEP_REASON_COMMAND);
// }

// void DccExController::deepSleepStart(int shutdownReason) {
//     // clearOledArray(); 
//     // setAppnameForOled();
//     // int delayPeriod = 2000;
//     // if (shutdownReason == SLEEP_REASON_INACTIVITY) {
//     //     oledText[2] = MSG_AUTO_SLEEP;
//     //     delayPeriod = 10000;
//     // } else if (shutdownReason == SLEEP_REASON_BATTERY) {
//     //     oledText[2] = MSG_BATTERY_SLEEP;
//     //     delayPeriod = 10000;
//     // }
//     // oledText[3] = MSG_START_SLEEP;
//     // writeOledArray(false, false, true, true);
//     // delay(delayPeriod);

//     // u8g2.setPowerSave(1);
//     // esp_deep_sleep_start();
// }

char DccExController::getMultiThrottleChar(int multiThrottleIndex) {
    // Implementation for getting a multi-throttle character
   return '0' + multiThrottleIndex;
}

int DccExController::getMultiThrottleIndex(char multiThrottle) {
    int mThrottle = multiThrottle - '0';
    if ((mThrottle >= 0) && (mThrottle<=5)) {
        return mThrottle;
    } else {
        return 0;
    }
}

void DccExController::powerToggle() {
    // Implementation for toggling power
}

void DccExController::powerOn() {
    // Implementation for turning power on
}

void DccExController::powerOff() {
    // Implementation for turning power off
} 
void DccExController::toggleHeartbeatCheck(){

}
void DccExController::changeNumberOfThrottles(bool increase){
  
}