#ifndef _GLOBALVALUES_H
#define _GLOBALVALUES_H

#include <string>
#include <vector>
#include <DccExController.h>
#include "static.h"
#include "config_buttons.h"
#include "config_keypad_etc.h"

class GlobalValues {
public:
  static GlobalValues &getInstance() {
    static GlobalValues instance;
    return instance;
  }
  

  // function states
  bool functionStates[FUNCTION_STATES][MAX_STATE_FUNCTS]; // set to maximum possible (6 throttles)

  // function labels
  std::string functionLabels[FUNCTION_STATES][MAX_STATE_FUNCTS]; // set to maximum possible (6 throttles)
  // function momentary
  bool functionMomentary[FUNCTION_STATES][MAX_STATE_FUNCTS]; // set to maximum possible (6 throttles)

  // consist function follow
  int functionFollow[FUNCTION_STATES][MAX_STATE_FUNCTS]; // set to maximum possible (6 throttles)

  int keypadUseType = KEYPAD_USE_OPERATION;
  int encoderUseType = ENCODER_USE_OPERATION;
  int encoderButtonAction = ENCODER_BUTTON_ACTION;

  bool heartbeatCheckEnabled;
  bool useBatteryTest;
  int lastBatteryTestValue;
  bool useBatteryPercentAsWellAsIcon;
  DCCExController::TrackPower trackPower = DCCExController::PowerUnknown;

  int turnoutListSize;
  std::vector<int> turnoutListIndex;
  std::vector<std::string> turnoutListUserName;

  int foundSsidsCount;
  std::vector<std::string> foundSsids;
  std::vector<std::string> foundSsidRssis;

  int rosterSize;
  std::vector<int> rosterIndex;
  std::vector<std::string> rosterName;
  std::vector<int> rosterAddress;

  int routeListSize;
  std::vector<int> routeListIndex;
  std::vector<std::string> routeListUserName;


  std::string ssidPasswordEntered;
  std::string ssidPasswordCurrentChar;

  int functionPage = 0;
  int page = 0;


int buttonActions[14] = { CHOSEN_KEYPAD_0_FUNCTION,
                          CHOSEN_KEYPAD_1_FUNCTION,
                          CHOSEN_KEYPAD_2_FUNCTION,
                          CHOSEN_KEYPAD_3_FUNCTION,
                          CHOSEN_KEYPAD_4_FUNCTION,
                          CHOSEN_KEYPAD_5_FUNCTION,
                          CHOSEN_KEYPAD_6_FUNCTION,
                          CHOSEN_KEYPAD_7_FUNCTION,
                          CHOSEN_KEYPAD_8_FUNCTION,
                          CHOSEN_KEYPAD_9_FUNCTION,
                          CHOSEN_KEYPAD_A_FUNCTION,
                          CHOSEN_KEYPAD_B_FUNCTION,
                          CHOSEN_KEYPAD_C_FUNCTION,
                          CHOSEN_KEYPAD_D_FUNCTION
};

//optional additional buttons
int additionalButtonActions[MAX_ADDITIONAL_BUTTONS] = {
                          CHOSEN_ADDITIONAL_BUTTON_0_FUNCTION,
                          CHOSEN_ADDITIONAL_BUTTON_1_FUNCTION,
                          CHOSEN_ADDITIONAL_BUTTON_2_FUNCTION,
                          CHOSEN_ADDITIONAL_BUTTON_3_FUNCTION,
                          CHOSEN_ADDITIONAL_BUTTON_4_FUNCTION,
                          CHOSEN_ADDITIONAL_BUTTON_5_FUNCTION,
                          CHOSEN_ADDITIONAL_BUTTON_6_FUNCTION
};
unsigned long lastAdditionalButtonDebounceTime[MAX_ADDITIONAL_BUTTONS];  // the last time the output pin was toggled
unsigned long additionalButtonDebounceDelay = ADDITIONAL_BUTTON_DEBOUNCE_DELAY;    // the debounce time
bool additionalButtonRead[MAX_ADDITIONAL_BUTTONS];
bool additionalButtonLastRead[MAX_ADDITIONAL_BUTTONS];


    std::string selectedSsid;
    std::string selectedSsidPassword;
    int ssidConnectionState;
    // std::string foundSsids[10]; // Adjust size as needed
    // int foundSsidsCount;
    int ssidSelectionSource;


        std::string witServerIpAndPortConstructed;
    std::string witServerIpAndPortEntered;
    std::string witServerIpAndPortEntryMask;
    std::string selectedWitServerName;
    int maxFoundWitServers;
    bool autoConnectToFirstWiThrottleServer;
    bool witServerIpAndPortChanged;
    std::vector<std::string> foundWitServersNames;
    std::vector<std::string> foundWitServersIPs;
    std::vector<int> foundWitServersPorts;
    std::string selectedWitServerIP;
    int selectedWitServerPort;

    int dccexConnectionState;

private:
  GlobalValues() = default;
  ~GlobalValues() = default;
  GlobalValues(const GlobalValues &) = delete;
  GlobalValues &operator=(const GlobalValues &) = delete;
};

#endif