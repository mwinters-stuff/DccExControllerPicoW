#ifndef _THROTTLES_H
#define _THROTTLES_H

#include <string>
#include <memory>
#include <vector>
#include <DCCEXProtocol.h> 

#include "Throttle.h"
#include "static.h"
#include "FunctionStates.h"

class Throttles {
  public:
    std::shared_ptr<FunctionStates> _functionStates;
    int throttleIndex;
    std::vector<std::shared_ptr<Throttle> > throttles;
    int maxThrottles;
    int currentThrottleIndex;
    int currentSpeed[6];
    DCCExController::Direction currentDirection[6];   // set to maximum possible (6)
    int speedStepCurrentMultiplier = 1;


// speedstep
    int currentSpeedStep[6];   // set to maximum possible (6 throttles)

uint32_t lastSpeedSentTime;
int lastSpeedSent;
int lastSpeedThrottleIndex;
uint32_t lastSpeedSentTimeCurrentLoco;



    Throttles(std::shared_ptr<FunctionStates> functionStates): _functionStates(functionStates) {
      throttleIndex = 0;
      maxThrottles = MAX_THROTTLES;
      currentThrottleIndex = 0;
    }

    void addThrottle(std::shared_ptr<Throttle> throttle){
      if(throttleIndex < maxThrottles){
        throttles.push_back(throttle);
        throttleIndex++;
      }
    }
    void deleteThrottle(int index){
      if(index < maxThrottles){
        throttles.erase(throttles.begin() + index);
        throttleIndex--;
      }
    }
    void deleteAllThrottles(){
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
    void toggleAdditionalMultiplier(int speedStepCurrentMultiplier);
    DCCExController::Facing getLocoFacing(int multiThrottleIndex, int address);
    std::string getDisplayLocoString(int multiThrottleIndex, int index);
    void debugLocoSpeed(std::string txt, DCCExController::Loco* loco);
    void debugLocoSpeed(std::string txt, int locoId, DCCExController::LocoSource source, int speed, DCCExController::Direction dir);


    void loadFunctionLabels(int multiThrottleIndex);

};

#endif