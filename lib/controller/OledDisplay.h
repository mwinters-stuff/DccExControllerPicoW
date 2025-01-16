#ifndef _OLEDDISPLAY_H
#define _OLEDDISPLAY_H

#include <u8g2.h>
#include <vector>
#include <string>

#include "Base.h"

class OledDisplay: public Base {
public:
  // static std::shared_ptr<OledDisplay> getInstance() {
  //   static std::shared_ptr<OledDisplay> instance = std::make_shared<OledDisplay>();
  //   return instance;
  // }

  std::string menuCommand = "";
  bool menuCommandStarted = false;
  bool menuIsShowing = false;

  std::vector<std::string> oledText{18, ""};
  int lastOledScreen = 0;
  std::string lastOledStringParameter = "";
  int lastOledIntParameter = 0;
  bool lastOledBooleanParameter = false;
  // Broadcast msessage
  std::string broadcastMessageText = "";
  uint32_t broadcastMessageTime = 0;


  // Delete copy constructor and assignment operator to prevent copies
  // OledDisplay(const OledDisplay &) = delete;
  // OledDisplay &operator=(const OledDisplay &) = delete;
  OledDisplay()
      : oledTextInvert(18, false) {
    // Initialization code here
  }

  void setup() override;
  void loop() override{};
  void initialize(u8g2_t &display);
  void updateDisplay(const std::string &text);
  void clearDisplay();
  void displayMenu(const std::string title, const std::vector<std::string> menuItems, int itemCount);
  void displayError(const std::string &errorMessage);

  void setAppnameForOled(void);
  void writeOledFoundSSids(const std::string &soFar);
  void writeOledRoster(const std::string &soFar);
  void writeOledTurnoutList(const std::string &soFar, bool action);
  void writeOledRouteList(const std::string &soFar);
  void writeOledEnterPassword();
  void writeOledMenu(const std::string &soFar);
  void writeOledExtraSubMenu();
  void writeHeartbeatCheck(void);
  void writeOledSpeed();
  void writeOledFunctions(void);
  void writeOledFunctionList(const std::string &soFar);
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

  u8g2_t _display;

  std::vector<bool> oledTextInvert;
  // std::string soFar = "";
};

#endif // _OLEDDISPLAY_H