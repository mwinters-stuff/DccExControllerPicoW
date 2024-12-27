#ifndef KEYPADMANAGER_H
#define KEYPADMANAGER_H

#include <pico_keypad.hpp>
#include "static.h"
#include "config_buttons.h"

class KeypadManager {
public:
    KeypadManager();
    void begin();
    void handleKeypadEvent(KeypadEvent key);
    void update();


    int keypadUseType = KEYPAD_USE_OPERATION;
    int encoderUseType = ENCODER_USE_OPERATION;
    int encoderButtonAction = ENCODER_BUTTON_ACTION;

private:
    PicoKeypad keypad;
    void processKeyPress(char key);
    void processKeyRelease(char key);
    void processKeyHold(char key);
};

#endif // KEYPADMANAGER_H