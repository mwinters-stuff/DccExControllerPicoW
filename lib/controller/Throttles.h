#ifndef _THROTTLES_H
#define _THROTTLES_H

#include <DCCEXProtocol.h>
#include <memory>
#include <string>
#include <vector>

#include "Throttle.h"
#include "static.h"

class Throttles {
public:
  static Throttles &getInstance() {
    static Throttles instance;
    return instance;
  }

  int throttleIndex;
  std::vector<std::shared_ptr<Throttle>> throttles;
  int maxThrottles = MAX_THROTTLES;
  int currentThrottleIndex = 0;
  int currentSpeed[6];
  DCCExController::Direction currentDirection[6]; // set to maximum possible (6)
  int speedStepCurrentMultiplier = 1;

  // speedstep
  int currentSpeedStep[6]; // set to maximum possible (6 throttles)

  uint32_t lastSpeedSentTime;
  int lastSpeedSent;
  int lastSpeedThrottleIndex;
  uint32_t lastSpeedSentTimeCurrentLoco;

  void addThrottle(std::shared_ptr<Throttle> throttle) {
    if (throttleIndex < maxThrottles) {
      throttles.push_back(throttle);
      throttleIndex++;
    }
  }
  void deleteThrottle(int index) {
    if (index < maxThrottles) {
      throttles.erase(throttles.begin() + index);
      throttleIndex--;
    }
  }
  void deleteAllThrottles() {
    throttles.clear();
    throttleIndex = 0;
  }

  void releaseAllLocos(int multiThrottleIndex);
  void releaseOneLoco(int multiThrottleIndex, int address);
  void speedEstop();
  void speedEstopCurrentLoco();
  void speedDown(int multiThrottleIndex, int amt);
  void speedUp(int multiThrottleIndex, int amt);
  void speedSet(int multiThrottleIndex, int speed);
  int getDisplaySpeed(int multiThrottleIndex);
  void toggleLocoFacing(int multiThrottleIndex, int address);

  void toggleDirection(int multiThrottleIndex);
  void changeDirection(int multiThrottleIndex, DCCExController::Direction direction);
  void toggleAdditionalMultiplier();
  DCCExController::Facing getLocoFacing(int multiThrottleIndex, int address);
  std::string getDisplayLocoString(int multiThrottleIndex, int index);
  void debugLocoSpeed(std::string txt, DCCExController::Loco *loco);
  void debugLocoSpeed(std::string txt, int locoId, DCCExController::LocoSource source, int speed, DCCExController::Direction dir);

  void loadFunctionLabels(int multiThrottleIndex);

  void doDirectFunction(int multiThrottleIndex, int functionNumber, bool pressed);
  void doFunction(int multiThrottleIndex, int functionNumber, bool pressed);
  void doFunctionWhichLocosInConsist(int multiThrottleIndex, int functionNumber, bool pressed);
  void powerOnOff(DCCExController::TrackPower powerState);
  void powerToggle();
  void nextThrottle();
  void changeNumberOfThrottles(bool increase);
  void stopThenToggleDirection();

private:
  Throttles() = default;
  ~Throttles() = default;
  Throttles(const Throttles &) = delete;
  Throttles &operator=(const Throttles &) = delete;
};

#endif