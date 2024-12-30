#ifndef _FUNCTIONSTATES_H
#define _FUNCTIONSTATES_H

#include "static.h"
#include <string>

class FunctionStates {
public:
  static FunctionStates &getInstance() {
    static FunctionStates instance;
    return instance;
  }

  void resetFunctionStates(int multiThrottleIndex);

  void loadDefaultFunctionLabels(int multiThrottleIndex);

  void resetFunctionLabels(int multiThrottleIndex);

  void resetAllFunctionLabels();

  void resetAllFunctionFollow();

private:
  FunctionStates() = default;
  ~FunctionStates() = default;
  FunctionStates(const FunctionStates &) = delete;
  FunctionStates &operator=(const FunctionStates &) = delete;
};

#endif