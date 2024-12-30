// DccExController.h
#ifndef DCCEXCONTROLLER_H
#define DCCEXCONTROLLER_H


#include "Throttle.h"

class DccExController {
public:

    static DccExController &getInstance() {
        static DccExController instance;
        return instance;
    }


    void setup();
    void loop();

    void selectRoster(int index);
    void selectTurnoutList(int index, bool state);
    void selectRouteList(int index);
    void selectFunctionList(int index);
    void selectEditConsistList(int index);

    void toggleHeartbeatCheck();
    void changeNumberOfThrottles(bool increase);

    void powerToggle();
    void powerOn();
    void powerOff();

    void _loadRoster();
    void _loadTurnoutList();
    void _loadRouteList();

    
    void deepSleepStart();
    void deepSleepStart(int);

    char getMultiThrottleChar(int);
    int getMultiThrottleIndex(char);
private:
    DccExController() = default;
    ~DccExController() = default;
    DccExController(const DccExController &) = delete;
    DccExController &operator=(const DccExController &) = delete;
};

#endif // DCCEXCONTROLLER_H