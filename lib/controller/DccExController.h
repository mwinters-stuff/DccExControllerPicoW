// DccExController.h
#ifndef DCCEXCONTROLLER_H
#define DCCEXCONTROLLER_H

#include <vector>
#include <memory>
#include <string>
#include <DCCEXLoco.h>

#include "static.h"
#include "Base.h"

class DccExController: public Base {
public:
  void setup() override;
  void loop() override;

  void selectRoster(int selection);
  void selectTurnoutList(int selection, bool state);
  void selectRouteList(int selection);
  void selectFunctionList(int selection);
  void selectEditConsistList(int selection);

  void toggleHeartbeatCheck();
  void changeNumberOfThrottles(bool increase);

  void powerToggle();
  void powerOn();
  void powerOff();

  void _loadRoster();
  void _loadTurnoutList();
  void _loadRouteList();
  void _processLocoUpdate(DCCExController::Loco* loco);

  char getMultiThrottleChar(int);
  int getMultiThrottleIndex(char);
private:
};

#endif // DCCEXCONTROLLER_H