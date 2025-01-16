#ifndef KEYPADMANAGER_H
#define KEYPADMANAGER_H

#include <memory>
#include <pico_keypad.hpp>

#include <vector>
#include <memory>
#include <string>

#include "static.h"

#include "config_buttons.h"
#include "config_keypad_etc.h"

#include "Base.h"

// #include <DCCEXProtocol.h>

class KeypadManager: public Base {
public:

  void setup() override;
  void loop() override;
  // void begin();
  // void handleKeypadEvent(KeypadEvent key);
  


  void doFunction(int multiThrottleIndex, int functionNumber, bool pressed);

  ~KeypadManager() = default;
  KeypadManager(const KeypadManager &) = delete;
  KeypadManager()
      : keypad(PicoKeypad(MAKE_KEYMAP(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM)) {};
private:
  PicoKeypad keypad;

  bool searchRosterOnEntryOfDccAddress = SEARCH_ROSTER_ON_ENTRY_OF_DCC_ADDRESS;

  // void processKeyPress(char key);
  // void processKeyRelease(char key);
  // void processKeyHold(char key);
  void resetMenu();
  void doMenu();
  void doDirectFunction(int multiThrottleIndex, int functionNumber, bool pressed);
  void doFunctionWhichLocosInConsist(int multiThrottleIndex, int functionNumber, bool pressed);

  void doDirectCommand(char key, bool isPressed);

  void doDirectCommandFunction(int currentThrottleIndex, int buttonAction, bool pressed);
  void doDirectAction(int buttonAction);

  char keys[ROW_NUM][COLUMN_NUM] = {KEYPAD_KEYS};
  uint8_t pin_rows[ROW_NUM] = KEYPAD_ROW_PINS;         // GIOP19, GIOP18, GIOP5, GIOP17 connect to the row pins
  uint8_t pin_column[COLUMN_NUM] = KEYPAD_COLUMN_PINS; // GIOP16, GIOP4, GIOP0 connect to the column pins

  // KeypadManager &operator=(const KeypadManager &) = delete;

};

#endif // KEYPADMANAGER_H