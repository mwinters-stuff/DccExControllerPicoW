#include "AdditionalButtons.h"

#include "ExternalSingletons.h"
#include "static.h"
#include "GlobalValues.h"

//extern std::unique_ptr<GlobalValues> globalValues;

void AdditionalButtons::setup() {

  for (int i = 0; i < MAX_ADDITIONAL_BUTTONS; i++) {
    if (globalValues->additionalButtonActions[i] != FUNCTION_NULL) {
      debug_print("Additional Button: ");
      debug_print(std::to_string(i).c_str());
      debug_print(" pin:");
      debug_println(std::to_string(additionalButtonPin[i]).c_str());

      additionalButtons.push_back(Button(additionalButtonPin[i], additionalButtonType[i], 200, 1000));
      // pimoroni::Button::Button(additionalButtonPin[i], additionalButtonType[i], 200, 1000));

      if (additionalButtonType[i] == Polarity::ACTIVE_HIGH) {
        globalValues->additionalButtonLastRead[i] = true;
      } else {
        globalValues->additionalButtonLastRead[i] = false;
      }

      globalValues->lastAdditionalButtonDebounceTime[i] = 0;
    }

    // pinMode(additionalButtonPins[i], INPUT_PULLUP);
    // buttonActions[i] = FUNCTION_NULL;
  }
}

void AdditionalButtons::loop() {
  // Handle additional button actions in the loop
  for (int i = 0; i < MAX_ADDITIONAL_BUTTONS; ++i) {
    if (additionalButtons[i].read()) {
      handleButtonPress(i);
    }
  }
}

void AdditionalButtons::addButtonAction(int buttonIndex, int action) {
  if (buttonIndex >= 0 && buttonIndex < MAX_ADDITIONAL_BUTTONS) {
    globalValues->additionalButtonActions[buttonIndex] = action;
  }
}

void AdditionalButtons::removeButtonAction(int buttonIndex) {
  if (buttonIndex >= 0 && buttonIndex < MAX_ADDITIONAL_BUTTONS) {
    globalValues->additionalButtonActions[buttonIndex] = FUNCTION_NULL;
  }
}

void AdditionalButtons::handleButtonPress(int buttonIndex) {
  if (buttonIndex >= 0 && buttonIndex < MAX_ADDITIONAL_BUTTONS) {
    int action = globalValues->additionalButtonActions[buttonIndex];
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