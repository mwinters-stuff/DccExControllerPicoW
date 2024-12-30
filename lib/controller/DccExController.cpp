#include "DccExController.h"
#include "WiFiManager.h"
#include "OledDisplay.h"
#include "RotaryEncoder.h"
#include "BatteryTest.h"
#include "KeypadManager.h"
#include "AdditionalButtons.h"
// #include "DCCEXProtocolDelegate.h"

// Constructor
// DccExController::DccExController() {
//     // Initialize components
//     wifiManager = new WiFiManager();
//     oledDisplay = new OledDisplay();
//     rotaryEncoder = new RotaryEncoder();
//     batteryTest = new BatteryTest();
//     keypadManager = new KeypadManager();
//     additionalButtons = new AdditionalButtons();
// }

// // Destructor
// DccExController::~DccExController() {
//     delete wifiManager;
//     delete oledDisplay;
//     delete rotaryEncoder;
//     delete batteryTest;
//     delete keypadManager;
//     delete additionalButtons;
// }

// Setup method
void DccExController::setup() {
//     wifiManager->setup();
//     oledDisplay->setup();
//     rotaryEncoder->setup();
//     batteryTest->setup();
//     keypadManager->setup();
//     additionalButtons->setup();
}

// // Main loop
void DccExController::loop() {
    // wifiManager->loop();
    // oledDisplay->loop();
    // rotaryEncoder->loop();
    // batteryTest->loop();
    // keypadManager->loop();
    // additionalButtons->loop();
}

void DccExController::selectRoster(int index){

}

void DccExController::selectTurnoutList(int index, bool state) {
      debug_print("selectTurnoutList() "); debug_println(selection);

  if ((selection>=0) && (selection < turnoutListSize)) {
    int id = turnoutListSysName[selection].toInt();
    debug_print("Turnout Selected: "); debug_println(id);
    if (action) {
      dccexProtocol.throwTurnout(id);
    } else {
      dccexProtocol.closeTurnout(id);
    }
    writeOledSpeed();
    keypadUseType = KEYPAD_USE_OPERATION;
  }
}

void DccExController::selectRouteList(int index) {
      debug_print("selectRouteList() "); debug_println(selection);

  if ((selection>=0) && (selection < routeListSize)) {
    int id = routeListSysName[selection].toInt();
    debug_print("Route Selected: "); debug_println(id);
    dccexProtocol.startRoute(id);
    writeOledSpeed();
    keypadUseType = KEYPAD_USE_OPERATION;
  }
}

void DccExController::selectFunctionList(int index) {
      debug_print("selectFunctionList() "); debug_println(selection);

  if ((selection>=0) && (selection < MAX_STATE_FUNCTS)) {
    String function = functionLabels[currentThrottleIndex][selection];
    debug_print("Function Selected: "); debug_println(function);
    doFunction(currentThrottleIndex, selection, true);
    writeOledSpeed();
    keypadUseType = KEYPAD_USE_OPERATION;
  }
}

void DccExController::selectEditConsistList(int index) {
      debug_print("selectEditConsistList() "); debug_println(selection);

  if (throttles[currentThrottleIndex]->getLocoCount() > 1 ) {
    int address = throttles[currentThrottleIndex]->getLocoAtPosition(selection)->getLoco()->getAddress();
    toggleLocoFacing(currentThrottleIndex, address);

    writeOledSpeed();
    keypadUseType = KEYPAD_USE_OPERATION;
  }
}

void DccExController::_loadRoster() {
      debug_print("_loadRoster()");
  rosterSize = dccexProtocol.getRosterCount();
  debug_println(rosterSize);
  if (rosterSize > 0) {
    int index = 0;
    for (Loco* rl=dccexProtocol.roster->getFirst(); rl; rl=rl->getNext()) {
      if (index < maxRoster) {
        rosterIndex[index] = index; 
        rosterName[index] = rl->getName(); 
        rosterAddress[index] = rl->getAddress();
        index++;
      }
    }
  }
  debug_print("_loadRoster() end");
}

void DccExController::_loadTurnoutList() {
   debug_println("_loadTurnoutList()");
  turnoutListSize = dccexProtocol.getTurnoutCount();
  debug_println(turnoutListSize);
  if (turnoutListSize > 0) {
    int index = 0;
    for (Turnout* tl=dccexProtocol.turnouts->getFirst(); tl; tl=tl->getNext()) {
      if (index < maxTurnoutList) {
        turnoutListIndex[index] = index; 
        turnoutListSysName[index] = tl->getId(); 
        turnoutListUserName[index] = tl->getName();
        turnoutListState[index] = tl->getThrown();
        index++;
      }
    }
  }
  debug_println("_loadTurnoutList() end");
}

void DccExController::_loadRouteList() {
      debug_println("_loadRouteList()");
  routeListSize = dccexProtocol.getRouteCount();
  debug_println(routeListSize);
  if (routeListSize > 0) {
    int index = 0;
    for (Route* rl=dccexProtocol.routes->getFirst(); rl; rl=rl->getNext()) {
      if (index < maxRouteList) {
        debug_print("_loadRouteList() : "); debug_print(rl->getId());
        routeListIndex[index] = index; 
        routeListSysName[index] = rl->getId(); 
        routeListUserName[index] = rl->getName();
        index++;
      }
    }
  }
  debug_println("_loadRouteList() end");
}


void DccExController::debugLocoSpeed(String txt, Loco* loco) {
    debugLocoSpeed(txt, loco->getAddress(), loco->getSource(), loco->getSpeed(), loco->getDirection()); 
}

void DccExController::debugLocoSpeed(String txt, int locoId, LocoSource source, int speed, Direction dir) {
    debug_print(txt);
    debug_print(" loco: "); debug_print(locoId); 
    debug_print(" source: "); debug_print( (source == 0) ? "Roster" : "Entered"); 
    debug_print(" speed: "); debug_print(speed); 
    debug_print(" dir: "); debug_print( (dir == Forward) ? "Forward" : "Reverse" ); 
    debug_println("");
}

void DccExController::deepSleepStart() {
    deepSleepStart(SLEEP_REASON_COMMAND);
}

void DccExController::deepSleepStart(int shutdownReason) {
    clearOledArray(); 
    setAppnameForOled();
    int delayPeriod = 2000;
    if (shutdownReason == SLEEP_REASON_INACTIVITY) {
        oledText[2] = MSG_AUTO_SLEEP;
        delayPeriod = 10000;
    } else if (shutdownReason == SLEEP_REASON_BATTERY) {
        oledText[2] = MSG_BATTERY_SLEEP;
        delayPeriod = 10000;
    }
    oledText[3] = MSG_START_SLEEP;
    writeOledArray(false, false, true, true);
    delay(delayPeriod);

    u8g2.setPowerSave(1);
    esp_deep_sleep_start();
}

char DccExController::getMultiThrottleChar(int index) {
    // Implementation for getting a multi-throttle character
   return '0' + multiThrottleIndex;
}

int DccExController::getMultiThrottleIndex(char throttleChar) {
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