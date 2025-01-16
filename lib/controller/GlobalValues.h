#ifndef _GLOBALVALUES_H
#define _GLOBALVALUES_H

#include "config_buttons.h"
#include "config_keypad_etc.h"
#include "static.h"
#include "Base.h"
#include <DccExController.h>
#include <string>
#include <vector>
#include <DCCEXProtocol.h>

class GlobalValues: public Base {
public:

  void setup() override {};
  void loop() override {};

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

  bool useBatteryTest;
  int lastBatteryTestValue;
  bool useBatteryPercentAsWellAsIcon;
  DCCExController::TrackPower trackPower = DCCExController::PowerUnknown;

  // turnout variables
  int turnoutListSize;
  std::vector<int> turnoutListIndex;
  std::vector<std::string> turnoutListUserName;
  std::vector<int> turnoutListSysName;
  std::vector<bool> turnoutListState;

  
  std::vector<std::string> foundSsids;
  std::vector<std::string> foundSsidRssis;
int foundSsidsCount = 0;
int ssidSelectionSource;
double startWaitForSelection;

  int rosterSize;
  std::vector<int> rosterIndex;
  std::vector<std::string> rosterName;
  std::vector<int> rosterAddress;

  int routeListSize;
  std::vector<int> routeListIndex;
  std::vector<std::string> routeListUserName;
  std::vector<int> routeListSysName;

  std::string ssidPasswordEntered;
  std::string ssidPasswordCurrentChar;

  int functionPage = 0;
  int page = 0;

  int buttonActions[14] = {CHOSEN_KEYPAD_0_FUNCTION,
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
                           CHOSEN_KEYPAD_D_FUNCTION};

  // optional additional buttons
  int additionalButtonActions[MAX_ADDITIONAL_BUTTONS] = {
      CHOSEN_ADDITIONAL_BUTTON_0_FUNCTION,
      CHOSEN_ADDITIONAL_BUTTON_1_FUNCTION,
      CHOSEN_ADDITIONAL_BUTTON_2_FUNCTION,
      CHOSEN_ADDITIONAL_BUTTON_3_FUNCTION,
      CHOSEN_ADDITIONAL_BUTTON_4_FUNCTION,
      CHOSEN_ADDITIONAL_BUTTON_5_FUNCTION,
      CHOSEN_ADDITIONAL_BUTTON_6_FUNCTION};
  unsigned long lastAdditionalButtonDebounceTime[MAX_ADDITIONAL_BUTTONS];         // the last time the output pin was toggled
  unsigned long additionalButtonDebounceDelay = ADDITIONAL_BUTTON_DEBOUNCE_DELAY; // the debounce time
  bool additionalButtonRead[MAX_ADDITIONAL_BUTTONS];
  bool additionalButtonLastRead[MAX_ADDITIONAL_BUTTONS];

  std::string selectedSsid;
  std::string selectedSsidPassword;
  int ssidConnectionState;
  

  std::string witServerIpAndPortConstructed;
  std::string witServerIpAndPortEntered;
  std::string selectedWitServerName;
  int maxFoundWitServers;
  bool autoConnectToFirstWiThrottleServer;
  bool witServerIpAndPortChanged;
  std::vector<std::string> foundWitServersNames;
  std::vector<std::string> foundWitServersIPs;
  std::vector<int> foundWitServersPorts;
  std::string selectedWitServerIP;
  int selectedWitServerPort;
  int noOfWitServices = 0;
int foundWitServersCount = 0;

  int dccexConnectionState;


int heartBeatPeriod = 10; // default to 10 seconds
long lastHeartBeatSentTime;
long lastServerResponseTime;
bool heartbeatCheckEnabled = true;

  // ~GlobalValues() = default;

  // GlobalValues() = default;
private:
  // GlobalValues(const GlobalValues &) = delete;
  // GlobalValues &operator=(const GlobalValues &) = delete;
};

#endif