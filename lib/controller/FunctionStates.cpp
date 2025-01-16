#include "FunctionStates.h"

#include "ExternalSingletons.h"
#include "GlobalValues.h"
#include "Throttles.h"
#include "static.h"

// #define MAX_STATE_FUNCTIONS 32

void FunctionStates::setup() {

  for (int i = 0; i < MAX_THROTTLES; i++) {
    resetFunctionStates(i);
    loadDefaultFunctionLabels(i);
  }
  resetAllFunctionFollow();
}

void FunctionStates::loop() {
}

void FunctionStates::resetFunctionStates(int multiThrottleIndex) {
  
  for (int i = 0; i < MAX_STATE_FUNCTS; i++) {
    globalValues->functionStates[multiThrottleIndex][i] = false;
  }
}
void FunctionStates::loadDefaultFunctionLabels(int multiThrottleIndex) {

  if (throttles->throttles[multiThrottleIndex]->getLocoCount() == 1) { // only do this for the first loco in the consist
    resetFunctionLabels(multiThrottleIndex);
    globalValues->functionLabels[multiThrottleIndex][0] = F0_LABEL;
    globalValues->functionMomentary[multiThrottleIndex][0] = !F0_LATCHING;
    globalValues->functionLabels[multiThrottleIndex][1] = F1_LABEL;
    globalValues->functionMomentary[multiThrottleIndex][1] = !F1_LATCHING;
    globalValues->functionLabels[multiThrottleIndex][2] = F2_LABEL;
    globalValues->functionMomentary[multiThrottleIndex][2] = !F2_LATCHING;
    for (int i = 3; i < MAX_STATE_FUNCTS; i++) {
      globalValues->functionMomentary[multiThrottleIndex][i] = false;
    }
  }
}

void FunctionStates::resetFunctionLabels(int multiThrottleIndex) {
  debug_print("resetFunctionLabels(): ");
  debug_println(std::to_string(multiThrottleIndex).c_str());
  
  for (int i = 0; i < MAX_STATE_FUNCTS; i++) {
    globalValues->functionLabels[multiThrottleIndex][i] = "";
    globalValues->functionMomentary[multiThrottleIndex][i] = "";
  }
  // functionPage = 0; // TODO: check if this is needed
}

void FunctionStates::resetAllFunctionLabels() {
  for (int i = 0; i < MAX_THROTTLES; i++) {
    resetFunctionLabels(i);
  }
}

void FunctionStates::resetAllFunctionFollow() {

  for (int i = 0; i < 6; i++) {
    globalValues->functionFollow[i][0] = CONSIST_FUNCTION_FOLLOW_F0;
    globalValues->functionFollow[i][1] = CONSIST_FUNCTION_FOLLOW_F1;
    globalValues->functionFollow[i][2] = CONSIST_FUNCTION_FOLLOW_F2;
    for (int j = 3; j < MAX_STATE_FUNCTS; j++) {
      globalValues->functionFollow[i][j] = CONSIST_FUNCTION_FOLLOW_OTHER_FUNCTIONS;
    }
  }
}
