#include "AdditionalButtons.h"
#include "config_keypad_etc.h"
#include "pico/stdlib.h"

void AdditionalButtons::initialise() {
    // Initialize additional buttons here
    for (int i = 0; i < MAX_ADDITIONAL_BUTTONS; ++i) {
         gpio_set_function(additionalButtonPins[i], GPIO_FUNC_SIO);
      gpio_set_dir(additionalButtonPins[i], GPIO_IN);
      if (polarity == Polarity::ACTIVE_LOW) {
        gpio_pull_up(additionalButtonPins[i]);
      }
      else {
        gpio_pull_down(additionalButtonPins[i]);
      }
        // pinMode(additionalButtonPins[i], INPUT_PULLUP);
        buttonActions[i] = FUNCTION_NULL;
    }
}

void AdditionalButtons::additionalButtonLoop() {
    // Handle additional button actions in the loop
    for (int i = 0; i < MAX_ADDITIONAL_BUTTONS; ++i) {
        if (gpio_get(additionalButtonPins[i]) == false) {
            handleButtonPress(i);
        }
    }
}

void AdditionalButtons::addButtonAction(int buttonIndex, int action) {
    if (buttonIndex >= 0 && buttonIndex < MAX_ADDITIONAL_BUTTONS) {
        buttonActions[buttonIndex] = action;
    }
}

void AdditionalButtons::removeButtonAction(int buttonIndex) {
    if (buttonIndex >= 0 && buttonIndex < MAX_ADDITIONAL_BUTTONS) {
        buttonActions[buttonIndex] = FUNCTION_NULL;
    }
}

void AdditionalButtons::handleButtonPress(int buttonIndex) {
    if (buttonIndex >= 0 && buttonIndex < MAX_ADDITIONAL_BUTTONS) {
        int action = buttonActions[buttonIndex];
        // Implement the logic for the action
        switch (action) {
            case FUNCTION_NULL:
                // No action assigned
                break;
            case FUNCTION_1:
                // Implement action for FUNCTION_1
                break;
            case FUNCTION_2:
                // Implement action for FUNCTION_2
                break;
            // Add cases for other functions as needed
            default:
                break;
        }
    }
}