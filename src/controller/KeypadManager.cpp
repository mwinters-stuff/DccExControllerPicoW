#include "KeypadManager.h"


KeypadManager::KeypadManager(char keys[4][4], byte rowPins[4], byte colPins[4]) 
    : keypad(Keypad(makeKeymap(keys), rowPins, colPins, 4, 4)) {
}

void KeypadManager::begin() {
    keypad.addEventListener(keypadEvent);
}

void KeypadManager::keypadEvent(KeypadEvent key) {
    switch (keypad.getState()) {
        case PRESSED:
            handleKeyPress(key);
            break;
        case RELEASED:
            handleKeyRelease(key);
            break;
        case HOLD:
            handleKeyHold(key);
            break;
        case IDLE:
            break;
        default:
            break;
    }
}

void KeypadManager::handleKeyPress(KeypadEvent key) {
    Serial.print("Key Pressed: ");
    Serial.println(key);
    // Add specific key press handling logic here
    // For example, you can map keys to specific actions
    switch (key) {
        case '1':
            // Action for key '1'
            break;
        case '2':
            // Action for key '2'
            break;
        // Add cases for other keys as needed
        default:
            break;
    }
}

void KeypadManager::handleKeyRelease(KeypadEvent key) {
    Serial.print("Key Released: ");
    Serial.println(key);
    // Add specific key release handling logic here
}

void KeypadManager::handleKeyHold(KeypadEvent key) {
    Serial.print("Key Held: ");
    Serial.println(key);
    // Add specific key hold handling logic here
}

void KeypadManager::update() {
    keypad.getKey();
}