#ifndef KEYPADMANAGER_H
#define KEYPADMANAGER_H

#include <pico_keypad.hpp>
#include <memory>
// #include "static.h"
//#include "config_buttons.h"

#include "config_buttons.h"
#include "config_keypad_etc.h"

#include "GlobalValues.h"
#include "OledDisplay.h"
#include <DCCEXProtocol.h>

class KeypadManager {
public:
   static KeypadManager &getInstance() {
    static KeypadManager instance;
    return instance;
  }

    // void begin();
    // void handleKeypadEvent(KeypadEvent key);
    void update();
    void setDccexProtocol(std::shared_ptr<DCCExController::DCCEXProtocol> dccexProtocol) { this->dccexProtocol = dccexProtocol; }


private:
    PicoKeypad keypad;
    
    std::shared_ptr<DCCExController::DCCEXProtocol> dccexProtocol;

    bool searchRosterOnEntryOfDccAddress = SEARCH_ROSTER_ON_ENTRY_OF_DCC_ADDRESS;

    // void processKeyPress(char key);
    // void processKeyRelease(char key);
    // void processKeyHold(char key);
    void resetMenu(GlobalValues &globalValues, OledDisplay &oledDisplay);
    void doMenu();
    void doDirectCommand(char key, bool isPressed);

    
    char keys[ROW_NUM][COLUMN_NUM] = {KEYPAD_KEYS};
    uint8_t pin_rows[ROW_NUM]      = KEYPAD_ROW_PINS; // GIOP19, GIOP18, GIOP5, GIOP17 connect to the row pins
    uint8_t pin_column[COLUMN_NUM] = KEYPAD_COLUMN_PINS;   // GIOP16, GIOP4, GIOP0 connect to the column pins

  KeypadManager(): keypad(PicoKeypad( MAKE_KEYMAP(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM )){};
  ~KeypadManager() = default;
  KeypadManager(const KeypadManager &) = delete;
  KeypadManager &operator=(const KeypadManager &) = delete;    
};

#endif // KEYPADMANAGER_H