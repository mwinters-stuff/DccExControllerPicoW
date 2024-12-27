// OledDisplay.h
#ifndef _OLEDDISPLAY_H
#define _OLEDDISPLAY_H

#include <u8g2.h>
#include <string>
#include <vector>
#include <memory>
#include <DCCEXProtocol.h>
#include "KeypadManager.h"
#include "../u8g2functions.h"
#include "Throttles.h"
#include "FunctionStates.h"

class OledDisplay {
public:
    OledDisplay(u8g2_t& display, 
        std::shared_ptr<KeypadManager> keypadManager, 
        std::shared_ptr<Throttles> throttles,
        std::shared_ptr<FunctionStates> functionStates
    ): _display(display), 
    _keypadManager(keypadManager),
    _throttles(throttles),
    _functionStates(functionStates) {};
    
    void initialize();
    void updateDisplay(const std::string& text);
    void clearDisplay();
    void displayMenu(const std::string title, const std::vector<std::string> menuItems, int itemCount);
    void displayError(const std::string& errorMessage);
    

    void setAppnameForOled(void);
    void writeOledFoundSSids(int foundSsidsCount, std::string foundSsids[], std::string foundSsidRssis[], int page);
    void writeOledRoster(int rosterSize, int rosterIndex[], std::string rosterName[], int rosterAddress[], int page);
    void writeOledTurnoutList(int turnoutListSize, int turnoutListIndex[], std::string turnoutListUserName[], int page, bool action);
    void writeOledRouteList(int routeListSize, int routeListIndex[], std::string routeListUserName[],int page);
    void writeOledEnterPassword(std::string ssidPasswordEntered, std::string ssidPasswordCurrentChar);
    void writeOledMenu(int throttleIndex, std::vector<std::shared_ptr<Throttle> > throttles, int maxThrottles, int currentThrottleIndex, std::string currentSpeed[], DCCExController::Direction currentDirection[]);
    void writeOledExtraSubMenu();
    void writeHeartbeatCheck(void);
    void writeOledSpeed(int throttleIndex, std::vector<std::shared_ptr<Throttle> > throttles, int maxThrottles, int currentThrottleIndex, std::string currentSpeed[], DCCExController::Direction currentDirection[]);
    void writeOledFunctions(void);
    void writeOledFunctionList(std::string);
    void writeOledAllLocos(bool);
    void writeOledEditConsist();
    void writeOledArray(bool, bool);
    void writeOledArray(bool, bool, bool);
    void writeOledArray(bool, bool, bool, bool);
    void clearOledArray(void);
    void writeOledDirectCommands(void);
    void writeOledBattery();
    void setMenuTextForOled(int menuTextIndex);
private:
    u8g2_t& _display;
    std::shared_ptr<KeypadManager> _keypadManager;
    std::shared_ptr<Throttles> _throttles;
    std::shared_ptr<FunctionStates> _functionStates;
    std::vector<std::string> oledText{18, ""};
    std::vector<bool> oledTextInvert{18, false};
    bool menuCommandStarted = false;
    std::string menuCommand = "";
    bool menuIsShowing = false;
    std::string soFar = "";
    int lastOledScreen = 0;
    std::string lastOledStringParameter = "";
    int lastOledIntParameter = 0;
    bool lastOledBooleanParameter = false;
    // Broadcast msessage
    std::string broadcastMessageText = "";
    uint32_t broadcastMessageTime = 0;

    bool heartbeatCheckEnabled;
    bool useBatteryTest;
     int lastBatteryTestValue;
     bool useBatteryPercentAsWellAsIcon;
     DCCExController::TrackPower trackPower = DCCExController::PowerUnknown;
};

#endif // _OLEDDISPLAY_H