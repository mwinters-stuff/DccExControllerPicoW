// DccExController.h
#ifndef DCCEXCONTROLLER_H
#define DCCEXCONTROLLER_H


#include "Throttle.h"

class DccExController {
public:
    void selectTurnoutList(int, bool);
    void selectRouteList(int);
    void selectFunctionList(int);
    void selectEditConsistList(int);

    void _loadRoster();
    void _loadTurnoutList();
    void _loadRouteList();

    
    void debugLocoSpeed(String txt, Loco* loco);

    void deepSleepStart();
    void deepSleepStart(int);

    char getMultiThrottleChar(int);
    int getMultiThrottleIndex(char);
};

#endif // DCCEXCONTROLLER_H