// OledDisplay.cpp
#include <memory>
#include <pico/stdlib.h>
#include <string>
#include <u8g2.h>
#include <vector>
#include <button.hpp>

#include "OledDisplay.h"

#include "GlobalValues.h"
#include "KeypadManager.h"
#include "Throttle.h"
#include "static.h"

bool oledDirectCommandsAreBeingDisplayed = false;
#ifdef HASH_SHOWS_FUNCTIONS_INSTEAD_OF_KEY_DEFS
bool hashShowsFunctionsInsteadOfKeyDefs = HASH_SHOWS_FUNCTIONS_INSTEAD_OF_KEY_DEFS;
#else
bool hashShowsFunctionsInsteadOfKeyDefs = false;
#endif



// text that will appear when you press #
const std::string directCommandText[4][3] = {
    {CHOSEN_KEYPAD_1_DISPLAY_NAME, CHOSEN_KEYPAD_2_DISPLAY_NAME, CHOSEN_KEYPAD_3_DISPLAY_NAME},
    {CHOSEN_KEYPAD_4_DISPLAY_NAME, CHOSEN_KEYPAD_5_DISPLAY_NAME, CHOSEN_KEYPAD_6_DISPLAY_NAME},
    {CHOSEN_KEYPAD_7_DISPLAY_NAME, CHOSEN_KEYPAD_8_DISPLAY_NAME, CHOSEN_KEYPAD_9_DISPLAY_NAME},
    {"* Menu", CHOSEN_KEYPAD_0_DISPLAY_NAME, "# This"}};

void OledDisplay::initialize(u8g2_t &display) {
   _display = display;
  u8g2_InitDisplay(&_display);
  u8g2_SetPowerSave(&_display, 0); // Wake up the display
  u8g2_ClearBuffer(&_display);
}

void OledDisplay::updateDisplay(const std::string &text) {
  u8g2_ClearBuffer(&_display);
  u8g2_SetFont(&_display, u8g2_font_ncenB08_tr);
  u8g2_DrawStr(&_display, 0, 10, text.c_str());
  u8g2_SendBuffer(&_display);
}

void OledDisplay::clearDisplay() {
  u8g2_ClearBuffer(&_display);
  u8g2_SendBuffer(&_display);
}

void OledDisplay::displayMenu(const std::string title, const std::vector<std::string> menuItems, int itemCount) {
  u8g2_ClearBuffer(&_display);
  u8g2_SetFont(&_display, u8g2_font_ncenB08_tr);
  u8g2_DrawStr(&_display, 0, 10, title.c_str());

  for (int i = 0; i < itemCount; i++) {
    u8g2_DrawStr(&_display, 0, 20 + (i * 10), menuItems[i].c_str());
  }
  u8g2_SendBuffer(&_display);
}

void OledDisplay::displayError(const std::string &errorMessage) {
  u8g2_ClearBuffer(&_display);
  u8g2_SetFont(&_display, u8g2_font_ncenB08_tr);
  u8g2_DrawStr(&_display, 0, 10, "Error:");
  u8g2_DrawStr(&_display, 0, 20, errorMessage.c_str());
  u8g2_SendBuffer(&_display);
}

void OledDisplay::setAppnameForOled() {
  oledText[0] = appName;
  oledText[6] = appVersion;
}

void OledDisplay::writeOledFoundSSids(const std::string &soFar) {
  menuIsShowing = true;
  GlobalValues &globalValues = GlobalValues::getInstance();
  globalValues.keypadUseType = KEYPAD_USE_SELECT_SSID_FROM_FOUND;

  if (soFar == "") { // nothing entered yet
    clearOledArray();
    for (int i = 0; i < 5 && i < globalValues.foundSsidsCount; i++) {
      if (globalValues.foundSsids[(globalValues.page * 5) + i].length() > 0) {
        oledText[i] = i + ": " + globalValues.foundSsids[(globalValues.page * 5) + i] + "   (" + globalValues.foundSsidRssis[(globalValues.page * 5) + i] + ")";
      }
    }
    oledText[5] = "(" + std::to_string(globalValues.page + 1) + ") " + menu_text[menu_select_ssids_from_found];
    writeOledArray(false, false);
    // } else {
    //   int cmd = menuCommand.substr(0, 1).toInt();
  }
}

void OledDisplay::writeOledRoster(const std::string &soFar) {
  lastOledScreen = last_oled_screen_roster;
  lastOledStringParameter = soFar;

  menuIsShowing = true;

  GlobalValues &globalValues = GlobalValues::getInstance();

  globalValues.keypadUseType = KEYPAD_USE_SELECT_ROSTER;
  if (soFar == "") { // nothing entered yet
    clearOledArray();
    for (int i = 0; i < 5 && i < globalValues.rosterSize; i++) {
      if (globalValues.rosterAddress[(globalValues.page * 5) + i] != 0) {
        oledText[i] = std::to_string(globalValues.rosterIndex[i]) + ": " + globalValues.rosterName[(globalValues.page * 5) + i] + " (" + std::to_string(globalValues.rosterAddress[(globalValues.page * 5) + i]) + ")";
      }
    }
    oledText[5] = "(" + std::to_string(globalValues.page + 1) + ") " + menu_text[menu_roster];
    writeOledArray(false, false);
    // } else {
    //   int cmd = menuCommand.substr(0, 1).toInt();
  }
}

void OledDisplay::writeOledTurnoutList(const std::string &soFar, bool action) {
  lastOledScreen = last_oled_screen_turnout_list;
  lastOledStringParameter = soFar;
  lastOledBooleanParameter = action;

  GlobalValues &globalValues = GlobalValues::getInstance();

  menuIsShowing = true;
  if (action) { // thrown
    globalValues.keypadUseType = KEYPAD_USE_SELECT_TURNOUTS_THROW;
  } else {
    globalValues.keypadUseType = KEYPAD_USE_SELECT_TURNOUTS_CLOSE;
  }
  if (soFar == "") { // nothing entered yet
    clearOledArray();
    int j = 0;
    for (int i = 0; i < 10 && i < globalValues.turnoutListSize; i++) {
      j = (i < 5) ? i : i + 1;
      if (globalValues.turnoutListUserName[(globalValues.page * 10) + i].length() > 0) {
        oledText[j] = std::to_string(globalValues.turnoutListIndex[i]) + ": " + globalValues.turnoutListUserName[(globalValues.page * 10) + i].substr(0, 10);
      }
    }
    oledText[5] = "(" + std::to_string(globalValues.page + 1) + ") " + menu_text[menu_turnout_list];
    writeOledArray(false, false);
    // } else {
    //   int cmd = menuCommand.substr(0, 1).toInt();
  }
}

void OledDisplay::writeOledRouteList(const std::string &soFar) {
  lastOledScreen = last_oled_screen_route_list;
  lastOledStringParameter = soFar;

  GlobalValues &globalValues = GlobalValues::getInstance();

  menuIsShowing = true;
  globalValues.keypadUseType = KEYPAD_USE_SELECT_ROUTES;
  if (soFar == "") { // nothing entered yet
    clearOledArray();
    int j = 0;
    for (int i = 0; i < 10 && i < globalValues.routeListSize; i++) {
      j = (i < 5) ? i : i + 1;
      if (globalValues.routeListUserName[(globalValues.page * 10) + i].length() > 0) {
        oledText[j] = std::to_string(globalValues.routeListIndex[i]) + ": " + globalValues.routeListUserName[(globalValues.page * 10) + i].substr(0, 10);
      }
    }
    oledText[5] = "(" + std::to_string(globalValues.page + 1) + ") " + menu_text[menu_route_list];
    writeOledArray(false, false);
    // } else {
    //   int cmd = menuCommand.substr(0, 1).toInt();
  }
}

void OledDisplay::setMenuTextForOled(int menuTextIndex) {
  oledText[5] = menu_text[menuTextIndex];
  if (broadcastMessageText != "") {
    if (pimoroni::millis() - broadcastMessageTime < 10000) {
      oledText[5] = "~" + broadcastMessageText;
    } else {
      broadcastMessageText = "";
    }
  }
}

void OledDisplay::writeOledEnterPassword() {
  lastOledScreen = last_oled_screen_enter_password;
  GlobalValues &globalValues = GlobalValues::getInstance();
  globalValues.keypadUseType = KEYPAD_USE_ENTER_SSID_PASSWORD;
  globalValues.encoderUseType = KEYPAD_USE_ENTER_SSID_PASSWORD;
  clearOledArray();
  std::string tempSsidPasswordEntered;
  tempSsidPasswordEntered = globalValues.ssidPasswordEntered + globalValues.ssidPasswordCurrentChar;
  if (tempSsidPasswordEntered.length() > 12) {
    tempSsidPasswordEntered = "\253" + tempSsidPasswordEntered.substr(tempSsidPasswordEntered.length() - 12);
  } else {
    tempSsidPasswordEntered = " " + tempSsidPasswordEntered;
  }
  oledText[0] = MSG_ENTER_PASSWORD;
  oledText[2] = tempSsidPasswordEntered;
  // oledText[5] = menu_enter_ssid_password;
  setMenuTextForOled(menu_enter_ssid_password);
  writeOledArray(false, true);
}

void OledDisplay::writeOledMenu(const std::string &soFar) {
  lastOledScreen = last_oled_screen_menu;
  lastOledStringParameter = soFar;
  Throttles &throttles = Throttles::getInstance();

  debug_print("writeOledMenu() : ");
  debug_println(soFar.c_str());
  menuIsShowing = true;
  bool drawTopLine = false;
  if (soFar == "") { // nothing entered yet
    clearOledArray();
    int j = 0;
    for (int i = 1; i < 10; i++) {
      j = (i < 6) ? i : i + 1;
      oledText[j - 1] = std::to_string(i) + ": " + menuText[i][0];
    }
    oledText[10] = "0: " + menuText[0][0];
    // oledText[5] = menu_cancel;
    setMenuTextForOled(menu_cancel);
    writeOledArray(false, false);
  } else {
    int cmd = std::stoi(menuCommand.substr(0, 1));

    clearOledArray();

    oledText[0] = ">> " + menuText[cmd][0] + ":";
    oledText[6] = menuCommand.substr(1, menuCommand.length());
    oledText[5] = menuText[cmd][1];

    switch (soFar.at(0)) {
    case MENU_ITEM_DROP_LOCO: {
      if (throttles.throttles[throttles.currentThrottleIndex]->getLocoCount() > 0) {
        writeOledAllLocos(false);
        drawTopLine = true;
      }
    } // fall through
    case MENU_ITEM_FUNCTION:
    case MENU_ITEM_TOGGLE_DIRECTION: {
      if (throttles.throttles[throttles.currentThrottleIndex]->getLocoCount() <= 0) {
        oledText[2] = MSG_THROTTLE_NUMBER + std::to_string(throttles.currentThrottleIndex + 1);
        oledText[3] = MSG_NO_LOCO_SELECTED;
        // oledText[5] = menu_cancel;
        setMenuTextForOled(menu_cancel);
      }
      break;
    }
    case MENU_ITEM_EXTRAS: { // extra menu
      writeOledExtraSubMenu();
      drawTopLine = true;
      break;
    }
    }

    writeOledArray(false, false, true, drawTopLine);
  }
}

void OledDisplay::writeOledExtraSubMenu() {
  lastOledScreen = last_oled_screen_extra_submenu;

  int j = 0;
  for (int i = 0; i < 8; i++) {
    j = (i < 4) ? i : i + 2;
    oledText[j + 1] = (extraSubMenuText[i].length() == 0) ? "" : std::to_string(i) + ": " + extraSubMenuText[i];
  }
}

void OledDisplay::writeHeartbeatCheck() {
  menuIsShowing = false;
  clearOledArray();
  oledText[0] = menuText[10][0];
  if (GlobalValues::getInstance().heartbeatCheckEnabled) {
    oledText[1] = MSG_HEARTBEAT_CHECK_ENABLED;
  } else {
    oledText[1] = MSG_HEARTBEAT_CHECK_DISABLED;
  }
  oledText[5] = menuText[10][1];
  writeOledArray(false, false);
}

// int throttleIndex, std::vector<std::shared_ptr<Throttle> > throttles, int maxThrottles, int currentThrottleIndex, std::string currentSpeed[], DCCExController::Direction currentDirection[]
void OledDisplay::writeOledSpeed() {
  lastOledScreen = last_oled_screen_speed;

  // debug_println("writeOledSpeed() ");
  menuIsShowing = false;
  std::string sLocos = "";
  std::string sSpeed = "";
  std::string sDirection = "";

  bool foundNextThrottle = false;
  std::string sNextThrottleNo = "";
  std::string sNextThrottleSpeedAndDirection = "";

  Throttles &throttles = Throttles::getInstance();

  clearOledArray();

  bool drawTopLine = false;

  if (throttles.throttles[throttles.currentThrottleIndex]->getLocoCount() > 0) {
    // oledText[0] = label_locos; oledText[2] = label_speed;

    for (int i = 0; i < throttles.throttles[throttles.currentThrottleIndex]->getLocoCount(); i++) {
      sLocos = sLocos + " " + throttles.getDisplayLocoString(throttles.currentThrottleIndex, i);
    }
    // sSpeed = std::string(currentSpeed[currentThrottleIndex]);
    sSpeed = std::to_string(throttles.getDisplaySpeed(throttles.currentThrottleIndex));
    sDirection = (throttles.currentDirection[throttles.currentThrottleIndex] == DCCExController::Forward) ? DIRECTION_FORWARD_TEXT : DIRECTION_REVERSE_TEXT;

    // find the next Throttle that has any locos selected - if there is one
    if (throttles.maxThrottles > 1) {
      int nextThrottleIndex = throttles.currentThrottleIndex + 1;

      for (int i = nextThrottleIndex; i < throttles.maxThrottles; i++) {
        if (throttles.throttles[throttles.currentThrottleIndex]->getLocoCount() > 0) {
          foundNextThrottle = true;
          nextThrottleIndex = i;
          break;
        }
      }
      if ((!foundNextThrottle) && (throttles.currentThrottleIndex > 0)) {
        for (int i = 0; i < throttles.currentThrottleIndex; i++) {
          if (throttles.throttles[throttles.currentThrottleIndex]->getLocoCount() > 0) {
            foundNextThrottle = true;
            nextThrottleIndex = i;
            break;
          }
        }
      }
      if (foundNextThrottle) {
        sNextThrottleNo = std::to_string(nextThrottleIndex + 1);
        int speed = throttles.getDisplaySpeed(nextThrottleIndex);
        sNextThrottleSpeedAndDirection = std::to_string(speed);
        // if (speed>0) {
        if (throttles.currentDirection[nextThrottleIndex] == DCCExController::Forward) {
          sNextThrottleSpeedAndDirection = sNextThrottleSpeedAndDirection + DIRECTION_FORWARD_TEXT_SHORT;
        } else {
          sNextThrottleSpeedAndDirection = DIRECTION_REVERSE_TEXT_SHORT + sNextThrottleSpeedAndDirection;
        }
        // }
        // + " " + ((currentDirection[nextThrottleIndex]==Forward) ? DIRECTION_FORWARD_TEXT_SHORT : DIRECTION_REVERSE_TEXT_SHORT);
        sNextThrottleSpeedAndDirection = "     " + sNextThrottleSpeedAndDirection;
        sNextThrottleSpeedAndDirection = sNextThrottleSpeedAndDirection.substr(sNextThrottleSpeedAndDirection.length() - 5);
      }
    }

    oledText[0] = "   " + sLocos;
    // oledText[7] = "     " + sDirection;  // old function state format

    drawTopLine = true;

  } else {
    setAppnameForOled();
    oledText[2] = MSG_THROTTLE_NUMBER + std::to_string(throttles.currentThrottleIndex + 1);
    oledText[3] = MSG_NO_LOCO_SELECTED;
    drawTopLine = true;
  }

  if (!hashShowsFunctionsInsteadOfKeyDefs) {
    setMenuTextForOled(menu_menu);
  } else {
    setMenuTextForOled(menu_menu_hash_is_functions);
  }

  writeOledArray(false, false, false, drawTopLine);

  if (throttles.throttles[throttles.currentThrottleIndex]->getLocoCount() > 0) {
    writeOledFunctions();

    // throttle number

    u8g2_ClearBuffer(&_display);
    u8g2_SetDrawColor(&_display, 0);
    u8g2_DrawBox(&_display, 0, 0, 12, 16);
    u8g2_SetDrawColor(&_display, 1);
    u8g2_SetFont(&_display, FONT_THROTTLE_NUMBER); // medium
    u8g2_DrawStr(&_display, 2, 15, std::to_string(throttles.currentThrottleIndex + 1).c_str());
  }

  writeOledBattery();

  if (speedStep != throttles.currentSpeedStep[throttles.currentThrottleIndex]) {
    // oledText[3] = "X " + std::string(speedStepCurrentMultiplier);
    u8g2_SetDrawColor(&_display, 1);
    u8g2_SetFont(&_display, FONT_SPEED_STEP);
    u8g2_DrawGlyph(&_display, 1, 38, glyph_speed_step);
    u8g2_SetFont(&_display, FONT_DEFAULT);
    // u8g2_DrawStr(&_display, 0, 37, ("X " + std::string(speedStepCurrentMultiplier)).c_str());
    u8g2_DrawStr(&_display, 9, 37, std::to_string(throttles.speedStepCurrentMultiplier).c_str());
  }

  GlobalValues &globalValues = GlobalValues::getInstance();

  if (globalValues.trackPower == DCCExController::PowerOn) {
    // u8g2_DrawBox(&_display,0,41,15,8);
    u8g2_DrawRBox(&_display, 0, 40, 9, 9, 1);
    u8g2_SetDrawColor(&_display, 0);
  }
  u8g2_SetFont(&_display, FONT_TRACK_POWER);
  // u8g2_DrawStr(&_display, 0, 48, label_track_power.c_str());
  u8g2_DrawGlyph(&_display, 1, 48, glyph_track_power);
  u8g2_SetDrawColor(&_display, 1);

  if (!globalValues.heartbeatCheckEnabled) {
    u8g2_SetFont(&_display, FONT_HEARTBEAT);
    u8g2_DrawGlyph(&_display, 13, 49, glyph_heartbeat_off);
    u8g2_SetDrawColor(&_display, 2);
    u8g2_DrawLine(&_display, 13, 48, 20, 41);
    // u8g2_DrawLine(&_display, 13, 48, 21, 40);
    u8g2_SetDrawColor(&_display, 1);
  }

  // direction
  // needed for new function state format
  u8g2_SetFont(&_display, FONT_DIRECTION); // medium
  u8g2_DrawStr(&_display, 79, 36, sDirection.c_str());

  // speed
  const char *cSpeed = sSpeed.c_str();
  // u8g2_SetFont(&_display,u8g2_font_inb21_mn); // big
  u8g2_SetFont(&_display, FONT_SPEED); // big
  int width = u8g2_GetStrWidth(&_display, cSpeed);
  u8g2_DrawStr(&_display, 22 + (55 - width), 45, cSpeed);

  // speed and direction of next throttle
  if ((throttles.maxThrottles > 1) && (foundNextThrottle)) {
    u8g2_SetFont(&_display, FONT_NEXT_THROTTLE);
    u8g2_DrawStr(&_display, 85 + 34, 38, sNextThrottleNo.c_str());
    u8g2_DrawStr(&_display, 85 + 12, 48, sNextThrottleSpeedAndDirection.c_str());
  }

  u8g2_SendBuffer(&_display);

  // debug_println("writeOledSpeed(): end");
}

void OledDisplay::writeOledBattery() {
  GlobalValues &globalValues = GlobalValues::getInstance();
  if (globalValues.useBatteryTest) {
    // int lastBatteryTestValue = random(0,100);
    u8g2_SetFont(&_display, FONT_HEARTBEAT);
    u8g2_SetDrawColor(&_display, 1);
    u8g2_DrawStr(&_display, 1, 30, "Z");
    if (globalValues.lastBatteryTestValue > 10)
      u8g2_DrawLine(&_display, 2, 24, 2, 27);
    if (globalValues.lastBatteryTestValue > 25)
      u8g2_DrawLine(&_display, 3, 24, 3, 27);
    if (globalValues.lastBatteryTestValue > 50)
      u8g2_DrawLine(&_display, 4, 24, 4, 27);
    if (globalValues.lastBatteryTestValue > 75)
      u8g2_DrawLine(&_display, 5, 24, 5, 27);
    if (globalValues.lastBatteryTestValue > 90)
      u8g2_DrawLine(&_display, 6, 24, 6, 27);

    u8g2_SetFont(&_display, FONT_FUNCTION_INDICATORS);
    if (globalValues.useBatteryPercentAsWellAsIcon) {
      u8g2_DrawStr(&_display, 1, 22, (std::to_string(globalValues.lastBatteryTestValue) + "%").c_str());
    }
    if (globalValues.lastBatteryTestValue < 5) {
      u8g2_DrawStr(&_display, 11, 29, "LOW");
    }
    u8g2_SetFont(&_display, FONT_DEFAULT);
  }
}

// #define MAX_STATE_FUNCTIONS 32

void OledDisplay::writeOledFunctions() {
  lastOledScreen = last_oled_screen_speed;
  GlobalValues &globalValues = GlobalValues::getInstance();
  Throttles &throttles = Throttles::getInstance();
  // debug_println("writeOledFunctions():");
  //  int x = 99;
  // bool anyFunctionsActive = false;
  for (int i = 0; i < MAX_STATE_FUNCTS; i++) {
    if (globalValues.functionStates[throttles.currentThrottleIndex][i]) {
      // old function state format
      //     //  debug_print("Fn On "); debug_println(i);
      //     if (i < 12) {
      //     int y = (i+2)*10-8;
      //     if ((i>=4) && (i<8)) {
      //       x = 109;
      //       y = (i-2)*10-8;
      //     } else if (i>=8) {
      //       x = 119;
      //       y = (i-6)*10-8;
      //     }

      //     u8g2_DrawBox(&_display,x,y,8,8);
      //     u8g2_SetDrawColor(&_display,0);
      //     u8g2_SetFont(&_display,u8g2_font_profont10_tf);
      //     u8g2_DrawStr(&_display,  x+2, y+7, std::string( (i<10) ? i : i-10 ).c_str());
      //     u8g2_SetDrawColor(&_display,1);
      //   //  } else {
      //   //    debug_print("Fn Off "); debug_println(i);

      // new function state format
      // anyFunctionsActive = true;
      // u8g2_DrawBox(&_display,i*4+12,12,5,7);
      u8g2_DrawRBox(&_display, i * 4 + 12, 12 + 1, 5, 7, 2);
      u8g2_SetDrawColor(&_display, 0);
      u8g2_SetFont(&_display, FONT_FUNCTION_INDICATORS);
      u8g2_DrawStr(&_display, i * 4 + 1 + 12, 18 + 1, std::to_string((i < 10) ? i : ((i < 20) ? i - 10 : i - 20)).c_str());
      u8g2_SetDrawColor(&_display, 1);
    }
    //  if (anyFunctionsActive) {
    //     u8g2_DrawStr(&_display,  0, 18, (function_states).c_str());
    // //     u8g2_DrawHLine(&_display0,19,128);
    //  }
  }
  // debug_println("writeOledFunctions(): end");
}

void OledDisplay::writeOledFunctionList(const std::string &soFar) {
  lastOledScreen = last_oled_screen_function_list;
  lastOledStringParameter = soFar;

  GlobalValues &globalValues = GlobalValues::getInstance();
  Throttles &throttles = Throttles::getInstance();

  menuIsShowing = true;
  globalValues.keypadUseType = KEYPAD_USE_SELECT_FUNCTION;

  if (soFar == "") { // nothing entered yet
    clearOledArray();
    if (throttles.throttles[throttles.currentThrottleIndex]->getLocoCount() > 0) {
      int j = 0;
      int k = 0;
      int truncateAt = 9;
      for (int i = 0; i < 10; i++) {
        k = (globalValues.functionPage * 10) + i;
        if (k < MAX_STATE_FUNCTS) {
          j = (i < 5) ? i : i + 1;
          // if (functionLabels[currentThrottleIndex][k].length()>0) {
          oledText[j] = std::to_string(i) + ": ";
          if (k >= 10) {
            oledText[j] = oledText[j] + std::to_string(k);
            truncateAt = 7;
          }
          oledText[j] = oledText[j] + ((globalValues.functionMomentary[throttles.currentThrottleIndex][k]) ? "." : " ");
          oledText[j] = oledText[j] + globalValues.functionLabels[throttles.currentThrottleIndex][k].substr(0, truncateAt);

          if (globalValues.functionStates[throttles.currentThrottleIndex][k]) {
            oledTextInvert[j] = true;
          }
          // }
        }
      }
      oledText[5] = "(" + std::to_string(globalValues.functionPage) + ") " + menu_text[menu_function_list];
      // setMenuTextForOled("(" + String(functionPage) +  ") " + menu_function_list);
    } else {
      oledText[0] = MSG_NO_FUNCTIONS;
      oledText[2] = MSG_THROTTLE_NUMBER + std::to_string(throttles.currentThrottleIndex + 1);
      oledText[3] = MSG_NO_LOCO_SELECTED;
      // oledText[5] = menu_cancel;
      setMenuTextForOled(menu_cancel);
    }
    writeOledArray(false, false);
    // } else {
    //   int cmd = menuCommand.substring(0, 1).toInt();
  }
}

void OledDisplay::writeOledAllLocos(bool hideLeadLoco) {
  lastOledScreen = last_oled_screen_all_locos;
  lastOledBooleanParameter = hideLeadLoco;

  Throttles &throttles = Throttles::getInstance();

  int startAt = (hideLeadLoco) ? 1 : 0; // for the loco heading menu, we don't want to show the loco 0 (lead) as an option.
  debug_println("writeOledAllLocos(): ");
  int address;
  int j = 0;
  int i = 0;
  if (throttles.throttles[throttles.currentThrottleIndex]->getLocoCount() > 0) {
    for (int index = 0; ((index < throttles.throttles[throttles.currentThrottleIndex]->getLocoCount()) && (i < 8)); index++) { // can only show first 8
      j = (i < 4) ? i : i + 2;
      address = throttles.throttles[throttles.currentThrottleIndex]->getLocoAtPosition(index)->getLoco()->getAddress();
      if (i >= startAt) {
        oledText[j + 1] = std::to_string(i) + ": " + std::to_string(address);
        if (throttles.throttles[throttles.currentThrottleIndex]->getLocoFacing(address) == DCCExController::FacingReversed) {
          oledTextInvert[j + 1] = true;
        }
      }
      i++;
    }
  }
}

void OledDisplay::writeOledEditConsist() {
  lastOledScreen = last_oled_screen_edit_consist;

  menuIsShowing = false;
  clearOledArray();
  debug_println("writeOledEditConsist(): ");
  GlobalValues::getInstance().keypadUseType = KEYPAD_USE_EDIT_CONSIST;
  writeOledAllLocos(true);
  oledText[0] = menuText[11][0];
  oledText[5] = menuText[11][1];
  writeOledArray(false, false);
}

void OledDisplay::writeOledArray(bool isThreeColums, bool isPassword) {
  writeOledArray(isThreeColums, isPassword, true, false);
}

void OledDisplay::writeOledArray(bool isThreeColums, bool isPassword, bool sendBuffer) {
  writeOledArray(isThreeColums, isPassword, sendBuffer, false);
}

void OledDisplay::writeOledArray(bool isThreeColums, bool isPassword, bool sendBuffer, bool drawTopLine) {
  // debug_println("Start writeOledArray()");
  u8g2_ClearBuffer(&_display); // clear the internal memory

  u8g2_SetFont(&_display, FONT_DEFAULT); // small

  int x = 0;
  int y = 10;
  int xInc = 64;
  int max = 12;
  if (isThreeColums) {
    xInc = 42;
    max = 18;
  }

  for (int i = 0; i < max; i++) {
    const char *cLine1 = oledText[i].c_str();
    if ((isPassword) && (i == 2))
      u8g2_SetFont(&_display, FONT_PASSWORD);

    if (oledTextInvert[i]) {
      u8g2_DrawBox(&_display, x, y - 8, 62, 10);
      u8g2_SetDrawColor(&_display, 0);
    }
    u8g2_DrawStr(&_display, x, y, cLine1);
    u8g2_SetDrawColor(&_display, 1);

    if ((isPassword) && (i == 2))
      u8g2_SetFont(&_display, FONT_DEFAULT);
    y = y + 10;
    if ((i == 5) || (i == 11)) {
      x = x + xInc;
      y = 10;
    }
  }

  if (drawTopLine)
    u8g2_DrawHLine(&_display, 0, 11, 128);
  u8g2_DrawHLine(&_display, 0, 51, 128);

  if (sendBuffer)
    u8g2_SendBuffer(&_display); // transfer internal memory to the display
  // debug_println("writeOledArray(): end ");
}

void OledDisplay::clearOledArray() {
  for (int i = 0; i < 15; i++) {
    oledText[i] = "";
    oledTextInvert[i] = false;
  }
}

void OledDisplay::writeOledDirectCommands() {
  lastOledScreen = last_oled_screen_direct_commands;

  oledDirectCommandsAreBeingDisplayed = true;
  clearOledArray();
  oledText[0] = DIRECT_COMMAND_LIST;
  for (int i = 0; i < 4; i++) {
    oledText[i + 1] = directCommandText[i][0];
  }
  int j = 0;
  for (int i = 6; i < 10; i++) {
    oledText[i + 1] = directCommandText[j][1];
    j++;
  }
  j = 0;
  for (int i = 12; i < 16; i++) {
    oledText[i + 1] = directCommandText[j][2];
    j++;
  }
  writeOledArray(true, false);
  menuCommandStarted = false;
}