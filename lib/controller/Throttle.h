#ifndef _THROTTLE_H
#define _THROTTLE_H

#include <DCCEXProtocol.h> 


class Throttle {
public:
  Throttle(DCCExController::DCCEXProtocol* dccexProtocol);
  void addLoco(DCCExController::Loco* loco, DCCExController::Facing facing);
  void removeLoco(DCCExController::Loco* loco);
  void removeLoco(int address);
  void removeAllLocos();
  DCCExController::Consist* getConsist();
  int getLocoCount();
  DCCExController::Loco* getByAddress(int address);
  DCCExController::ConsistLoco* getConLocoByAddress(int address);
  DCCExController::ConsistLoco* getFirst();
  DCCExController::ConsistLoco* getLocoAtPosition(int index);
  void setSpeed(int speed);
  DCCExController::Direction getDirection();
  void setDirection(DCCExController::Direction direction);
  DCCExController::Facing getLocoFacing(int address);
  void setLocoFacing(int address, DCCExController::Facing facing);
  void setFunction(DCCExController::Loco* loco, int function, bool state);
  void process();

private:
  DCCExController::DCCEXProtocol* _dccexProtocol;
  DCCExController::Consist* _consist;

};

#endif