#ifndef _FUNCTIONSTATES_H
#define _FUNCTIONSTATES_H


#include <string>
#include "static.h"


class FunctionStates {
  public:
    // function states
bool functionStates[6][MAX_FUNCTIONS];   // set to maximum possible (6 throttles)

// function labels
std::string functionLabels[6][MAX_FUNCTIONS];   // set to maximum possible (6 throttles)
// function momentary
bool functionMomentary[6][MAX_FUNCTIONS]; // set to maximum possible (6 throttles)

// consist function follow
  int functionFollow[6][MAX_FUNCTIONS];   // set to maximum possible (6 throttles)

void resetFunctionStates(int multiThrottleIndex) {
  for (int i=0; i<MAX_FUNCTIONS; i++) {
    functionStates[multiThrottleIndex][i] = false;
  }
}
void loadDefaultFunctionLabels(int multiThrottleIndex) {
  if (throttles[multiThrottleIndex]->getLocoCount() == 1) {  // only do this for the first loco in the consist
    resetFunctionLabels(multiThrottleIndex);
    functionLabels[multiThrottleIndex][0] = F0_LABEL;
    functionMomentary[multiThrottleIndex][0] = !F0_LATCHING;
    functionLabels[multiThrottleIndex][1] = F1_LABEL;
    functionMomentary[multiThrottleIndex][1] = !F1_LATCHING;
    functionLabels[multiThrottleIndex][2] = F2_LABEL;
    functionMomentary[multiThrottleIndex][2] = !F2_LATCHING;
    for (int i=3; i<MAX_FUNCTIONS; i++) {
      functionMomentary[multiThrottleIndex][i] = false;
    }
  }
}

void resetFunctionLabels(int multiThrottleIndex) {
  debug_print("resetFunctionLabels(): "); debug_println(multiThrottleIndex);
  for (int i=0; i<MAX_FUNCTIONS; i++) {
    functionLabels[multiThrottleIndex][i] = "";
    functionMomentary[multiThrottleIndex][i] = "";
  }
  // functionPage = 0; // TODO: check if this is needed
}

void resetAllFunctionLabels() {
  for (int i=0; i<maxThrottles; i++) {
    resetFunctionLabels(i);
  }
}

void resetAllFunctionFollow() {
  for (int i=0; i<6; i++) {
    functionFollow[i][0] = CONSIST_FUNCTION_FOLLOW_F0;
    functionFollow[i][1] = CONSIST_FUNCTION_FOLLOW_F1;
    functionFollow[i][2] = CONSIST_FUNCTION_FOLLOW_F2;
    for (int j=3; j<MAX_FUNCTIONS; j++) {
      functionFollow[i][j] = CONSIST_FUNCTION_FOLLOW_OTHER_FUNCTIONS;
    }
  }
}
};


#endif