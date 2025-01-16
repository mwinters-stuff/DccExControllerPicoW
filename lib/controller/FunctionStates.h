#ifndef _FUNCTIONSTATES_H
#define _FUNCTIONSTATES_H

#include "Base.h"

class FunctionStates: public Base {
public:
  void setup() override;
  void loop() override;

  void resetFunctionStates(int multiThrottleIndex);

  void loadDefaultFunctionLabels(int multiThrottleIndex);

  void resetFunctionLabels(int multiThrottleIndex);

  void resetAllFunctionLabels();

  void resetAllFunctionFollow();

// FunctionStates() = default;
//   ~FunctionStates() = default;
private:
  
  // FunctionStates(const FunctionStates &) = delete;
  // FunctionStates &operator=(const FunctionStates &) = delete;

  // std::shared_ptr<GlobalValues> globalValues;
  // std::shared_ptr<Throttles> throttles;
};

#endif