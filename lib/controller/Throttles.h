#ifndef _THROTTLES_H
#define _THROTTLES_H

#include <DCCEXProtocol.h>
#include <vector>
#include <memory>

#include "Throttle.h"
#include "Base.h"
#include "static.h"

class Throttles : public Base{
public:


  void setup() override;
  void loop() override{};

  int throttleIndex;
  std::vector<std::shared_ptr<Throttle>> throttles;
  int maxThrottles = MAX_THROTTLES;
  int currentThrottleIndex = 0;
  int currentSpeed[6];
  DCCExController::Direction currentDirection[6]; // set to maximum possible (6)
  int speedStepCurrentMultiplier = 1;

  // speedstep
  int currentSpeedStep[6]; // set to maximum possible (6 throttles)


  int lastSpeedThrottleIndex;
  uint32_t lastSpeedSentTimeCurrentLoco;

// used to stop speed bounces
uint32_t lastSpeedSentTime = 0;
int lastSpeedSent = 0;

  void addThrottle(std::shared_ptr<Throttle> throttle) ;
  void deleteThrottle(int index);
  void deleteAllThrottles() ;

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

  // void doDirectFunction(int multiThrottleIndex, int functionNumber, bool pressed);
  // void doFunction(int multiThrottleIndex, int functionNumber, bool pressed);
  // void doFunctionWhichLocosInConsist(int multiThrottleIndex, int functionNumber, bool pressed);
  void powerOnOff(DCCExController::TrackPower powerState);
  void powerToggle();
  void nextThrottle();
  void changeNumberOfThrottles(bool increase);
  void stopThenToggleDirection();

};

#endif