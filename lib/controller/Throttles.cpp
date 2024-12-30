#include <pico/stdlib.h>

#include "Throttles.h"
#include "Throttle.h"
#include "FunctionStates.h"
#include "GlobalValues.h"
#include "OledDisplay.h"
#include "static.h"

// #define debug_print printf
// #define debug_println(x) printf("%s\n", x)


std::string Throttles::getDisplayLocoString(int multiThrottleIndex, int index) {
  auto loco = throttles[multiThrottleIndex]->getLocoAtPosition(index)->getLoco();
  if (loco == nullptr) return "";
  std::string locoNumber = std::to_string(loco->getAddress());
  if (index!=0) { // not the lead loco
    if (throttles[currentThrottleIndex]->getLocoAtPosition(index)->getFacing() == DCCExController::FacingReversed) {
      locoNumber = locoNumber + DIRECTION_REVERSE_INDICATOR;
    }
  }
  return locoNumber;
}

void Throttles::releaseAllLocos(int multiThrottleIndex) {
  FunctionStates &functionStates = FunctionStates::getInstance();
  if (throttles[currentThrottleIndex]->getLocoCount() > 0) {
    throttles[currentThrottleIndex]->removeAllLocos();
    functionStates.resetFunctionLabels(multiThrottleIndex);
  }
}

void Throttles::releaseOneLoco(int multiThrottleIndex, int address) {
  debug_print("releaseOneLoco(): "); debug_print(std::to_string(multiThrottleIndex).c_str()); debug_print(": "); debug_println(std::to_string(address).c_str());
  throttles[currentThrottleIndex]->removeLoco(address);
  FunctionStates::getInstance().resetFunctionLabels(multiThrottleIndex);
  debug_println("releaseOneLoco(): end"); 
}

void Throttles::speedEstop() {
  for (int i=0; i<maxThrottles; i++) {
    // dccexProtocol.emergencyStop(getMultiThrottleChar(i));
//*************
    throttles[i]->setSpeed(0);
//*************
    currentSpeed[i] = 0;
  }
  debug_println("Speed EStop"); 
  // writeOledSpeed();
}

void Throttles::speedEstopCurrentLoco() {
  // dccexProtocol.emergencyStop(currentThrottleIndexChar);
//*************
    throttles[currentThrottleIndex]->setSpeed(-1);
//*************
  currentSpeed[currentThrottleIndex] = 0;
  debug_println("Speed EStop Curent Loco"); 
  // writeOledSpeed();
}

void Throttles::speedDown(int multiThrottleIndex, int amt) {
  if (throttles[currentThrottleIndex]->getLocoCount() > 0) {
    int newSpeed = currentSpeed[multiThrottleIndex] - amt;
    debug_print("Speed Down: "); debug_println(std::to_string(amt).c_str());
    speedSet(multiThrottleIndex, newSpeed);
  }
}

void Throttles::speedUp(int multiThrottleIndex, int amt) {
  if (throttles[currentThrottleIndex]->getLocoCount() > 0) {
    int newSpeed = currentSpeed[multiThrottleIndex] + amt;
    debug_print("Speed Up: "); debug_println(std::to_string(amt).c_str());
    speedSet(multiThrottleIndex, newSpeed);
  }
}

  uint32_t millis() {
      return to_ms_since_boot(get_absolute_time());
    }

void Throttles::speedSet(int multiThrottleIndex, int amt) {
  if (throttles[currentThrottleIndex]->getLocoCount() > 0) {
    int newSpeed = amt;
    if (newSpeed >126) { newSpeed = 126; }
    if (newSpeed <0) { newSpeed = 0; }
    throttles[multiThrottleIndex]->setSpeed(newSpeed);
    currentSpeed[multiThrottleIndex] = newSpeed;
    debug_print("Speed Set: "); debug_println(std::to_string(newSpeed).c_str());
    debugLocoSpeed("setspeed() first loco in consist: ", throttles[currentThrottleIndex]->getFirst()->getLoco());

    // used to avoid bounce
    lastSpeedSentTime = pimoroni::millis();
    lastSpeedSent = newSpeed;
    // lastDirectionSent = -1;
    lastSpeedThrottleIndex = multiThrottleIndex;

    // writeOledSpeed(); // TOOD: check if this is needed
  }
}

int Throttles::getDisplaySpeed(int multiThrottleIndex) {
  if (speedDisplayAsPercent) {
    float speed = currentSpeed[multiThrottleIndex];
    speed = speed / 126 *100;
    int iSpeed = speed;
    if (iSpeed-speed >= 0.5) {
      iSpeed = iSpeed + 1;
    }
    return iSpeed;
  } else {
    if (speedDisplayAs0to28) {
      float speed = currentSpeed[multiThrottleIndex];
      speed = speed / 126 *28;
      int iSpeed = speed;
      if (iSpeed-speed >= 0.5) {
        iSpeed = iSpeed + 1;
      }
      return iSpeed;
    } else {
      return currentSpeed[multiThrottleIndex];
    }
  }
}

void Throttles::toggleLocoFacing(int multiThrottleIndex, int address) {
  debug_print("toggleLocoFacing(): "); debug_println(std::to_string(address).c_str()); 
  for (DCCExController::ConsistLoco* cl= throttles[currentThrottleIndex]->getFirst(); cl; cl=cl->getNext()) {
    if (cl->getLoco()->getAddress() == address) {
      if (cl->getFacing() == DCCExController::FacingForward) {
        cl->setFacing(DCCExController::FacingReversed);
      } else {
        cl->setFacing(DCCExController::FacingForward);
      }
      break;
    }
  }
}

DCCExController::Facing Throttles::getLocoFacing(int multiThrottleIndex, int address) {
  return throttles[currentThrottleIndex]->getLocoFacing(address);
}

void Throttles::toggleAdditionalMultiplier() {
  // if (speedStep != currentSpeedStep) {
  //   currentSpeedStep = speedStep;
  // } else {
  //   currentSpeedStep = speedStep * speedStepAdditionalMultiplier;
  // }
  auto speedStepCurrentMultiplier = Throttles::getInstance().speedStepCurrentMultiplier;
  switch (speedStepCurrentMultiplier) {
    case 1: 
      speedStepCurrentMultiplier = speedStepAdditionalMultiplier;
      break;
    case speedStepAdditionalMultiplier:
      speedStepCurrentMultiplier = speedStepAdditionalMultiplier*2;
      break;
    case speedStepAdditionalMultiplier*2:
      speedStepCurrentMultiplier = 1;
      break;
  }

  for (int i=0; i<maxThrottles; i++) {
    currentSpeedStep[i] = speedStep * speedStepCurrentMultiplier;
  }
  Throttles::getInstance().speedStepCurrentMultiplier = speedStepCurrentMultiplier;
  // writeOledSpeed(); // TOOD: check if this is needed
}

void Throttles::toggleDirection(int multiThrottleIndex) {
  if (throttles[currentThrottleIndex]->getLocoCount() > 0) {
    changeDirection(multiThrottleIndex, (currentDirection[multiThrottleIndex] == DCCExController::Forward) ? DCCExController::Reverse : DCCExController::Forward );
    // writeOledSpeed(); // TOOD: check if this is needed
  }
}

void Throttles::changeDirection(int multiThrottleIndex, DCCExController::Direction direction) {
  int locoCount = throttles[currentThrottleIndex]->getLocoCount();

  if (locoCount > 0) {
    currentDirection[multiThrottleIndex] = direction;
    debug_print("changeDirection(): "); debug_println( (direction==DCCExController::Forward) ? "Forward" : "Reverse");
    throttles[multiThrottleIndex]->setDirection(direction);
    debugLocoSpeed("changeDirection() first loco in consist: ", throttles[currentThrottleIndex]->getFirst()->getLoco() );

  }
  // writeOledSpeed(); // TOOD: check if this is needed
  // debug_println("Change direction(): end "); 
}

void Throttles::debugLocoSpeed(std::string txt, DCCExController::Loco* loco) {
  debugLocoSpeed(txt, loco->getAddress(), loco->getSource(), loco->getSpeed(), loco->getDirection()); 
}

void Throttles::debugLocoSpeed(std::string txt, int locoId, DCCExController::LocoSource source, int speed, DCCExController::Direction dir) {
  debug_print(txt.c_str());
  debug_print(" loco: "); debug_print(std::to_string(locoId).c_str()); 
  debug_print(" source: "); debug_print( (source == 0) ? "Roster" : "Entered"); 
  debug_print(" speed: "); debug_print(std::to_string(speed).c_str()); 
  debug_print(" dir: "); debug_print( (dir == DCCExController::Forward) ? "Forward" : "Reverse" ); 
  debug_println("");
}

// #define MAX_STATE_FUNCTIONS 32

void Throttles::loadFunctionLabels(int multiThrottleIndex) {  // from Roster entry
  debug_println("loadFunctionLabels()");
  GlobalValues &globalValues = GlobalValues::getInstance();
  
  if (throttles[multiThrottleIndex]->getLocoCount() > 0) {
    for (int i=0; i<MAX_STATE_FUNCTS; i++) {
      const char* fName = throttles[multiThrottleIndex]->getFirst()->getLoco()->getFunctionName(i);
      bool fMomentary = throttles[multiThrottleIndex]->getFirst()->getLoco()->isFunctionMomentary(i);
      if (fName != nullptr) {
        // debug_print("loadFunctionLabels() "); 
        // debug_println(fName);
        globalValues.functionLabels[multiThrottleIndex][i] = fName;
        globalValues.functionMomentary[multiThrottleIndex][i] = fMomentary;
      // } else {
      //   debug_println("loadFunctionLabels() blank"); 
      }
    }
  }
  debug_println("loadFunctionLabels() end"); 
}

void  Throttles::doDirectFunction(int multiThrottleIndex, int functionNumber, bool pressed) {
  debug_print("doDirectFunction(): "); debug_print(std::to_string(multiThrottleIndex).c_str());  debug_print(" , "); debug_println(std::to_string(functionNumber).c_str());
  if (throttles[currentThrottleIndex]->getLocoCount() > 0) {
    debug_print("direct fn: "); debug_println(std::to_string(functionNumber).c_str()); debug_println( pressed ? " Pressed" : " Released");
    doFunctionWhichLocosInConsist(multiThrottleIndex, functionNumber, pressed);
    OledDisplay::getInstance().writeOledSpeed(); 
  }
  // debug_print("doDirectFunction(): end"); 
}

void  Throttles::doFunction(int multiThrottleIndex, int functionNumber, bool pressed) {   // currently ignoring the pressed value
  debug_print("doFunction(): multiThrottleIndex "); debug_println(std::to_string(multiThrottleIndex).c_str());
  if (throttles[currentThrottleIndex]->getLocoCount() > 0) {
    doFunctionWhichLocosInConsist(multiThrottleIndex, functionNumber, true);
    GlobalValues &globalValues = GlobalValues::getInstance();

    if (!globalValues.functionStates[multiThrottleIndex][functionNumber]) {
      debug_print("fn: "); debug_print(std::to_string(functionNumber).c_str()); debug_println(" Pressed");
      // functionStates[functionNumber] = true;
    } else {
      sleep_ms(20);
      doFunctionWhichLocosInConsist(multiThrottleIndex, functionNumber, false);
      debug_print("fn: "); debug_print(std::to_string(functionNumber).c_str()); debug_println(" Released");
      // functionStates[functionNumber] = false;
    }
    OledDisplay::getInstance().writeOledSpeed(); 
  }
  // debug_println("doFunction(): ");
}

// Work out which locos in a consist should get the function
//
void  Throttles::doFunctionWhichLocosInConsist(int multiThrottleIndex, int functionNumber, bool pressed) {
  debug_print("doFunctionWhichLocosInConsist() multiThrottleIndex "); debug_print(std::to_string(multiThrottleIndex).c_str());
  debug_print(" fn: "); debug_println(std::to_string(functionNumber).c_str());
  GlobalValues &globalValues = GlobalValues::getInstance();

  if (globalValues.functionFollow[multiThrottleIndex][functionNumber]==CONSIST_LEAD_LOCO) {
    throttles[multiThrottleIndex]->setFunction(throttles[multiThrottleIndex]->getFirst()->getLoco(), functionNumber,pressed);
  } else {  // at the momemnt the only other option in CONSIST_ALL_LOCOS
    for (int i=0; i<throttles[multiThrottleIndex]->getLocoCount(); i++) {
      throttles[multiThrottleIndex]->setFunction(throttles[multiThrottleIndex]->getLocoAtPosition(i)->getLoco(), functionNumber,pressed);
    }
  }
  debug_println("doFunctionWhichLocosInConsist(): end fn: ");
}

void  Throttles::powerOnOff(DCCExController::TrackPower powerState) {
  debug_println("powerOnOff()");
  GlobalValues &globalValues = GlobalValues::getInstance();
  DccExController &dccExController = DccExController::getInstance();
  if (powerState == DCCExController::PowerOn) {
    dccExController.powerOn();
  } else {
    dccExController.powerOff();
  }
  globalValues.trackPower = powerState;
  OledDisplay::getInstance().writeOledSpeed();
}

void  Throttles::powerToggle() {
  debug_println("PowerToggle()");
  GlobalValues &globalValues = GlobalValues::getInstance();
  if (globalValues.trackPower==DCCExController::PowerOn) {
    powerOnOff(DCCExController::PowerOff);
  } else {
    powerOnOff(DCCExController::PowerOn);
  }
}

void Throttles::nextThrottle() {
  debug_print("nextThrottle(): "); 
  int wasThrottle = currentThrottleIndex;
  currentThrottleIndex++;
  if (currentThrottleIndex >= maxThrottles) {
    currentThrottleIndex = 0;
  }
  // currentThrottleIndexChar = getMultiThrottleChar(currentThrottleIndex);

  if (currentThrottleIndex!=wasThrottle) {
    OledDisplay::getInstance().writeOledSpeed();
  }
}

void Throttles::changeNumberOfThrottles(bool increase) {
  if (increase) {
    maxThrottles++;
    if (maxThrottles>6) maxThrottles = 6;   /// can't have more than 6
  } else {
    maxThrottles--;
    if (maxThrottles<1) {   /// can't have less than 1
      maxThrottles = 1;
    } else {
      releaseAllLocos(maxThrottles+1);
      if (currentThrottleIndex>=maxThrottles) {
        nextThrottle();
      }
    }
  }
  OledDisplay::getInstance().writeOledSpeed();
}

void Throttles::stopThenToggleDirection() {
  if (throttles[currentThrottleIndex]->getLocoCount() > 0) {
    if (currentSpeed[currentThrottleIndex] != 0) {
      // dccexProtocol.setSpeed(currentThrottleIndexChar, 0);
      speedSet(currentThrottleIndex,0);
    } else {
      if (TOGGLE_DIRECTION_ON_ENCODER_BUTTON_PRESSED_WHEN_STATIONAY){
        toggleDirection(currentThrottleIndex);
      }
    }
    currentSpeed[currentThrottleIndex] = 0;
  }
}