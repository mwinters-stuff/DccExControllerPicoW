// This file declares the AdditionalButtons class, responsible for managing additional button inputs and their associated actions.

#ifndef ADDITIONALBUTTONS_H
#define ADDITIONALBUTTONS_H

#include <button.hpp>

#include <vector>
// #include <memory>
// #include <string>

#include "static.h"
#include "Base.h"

#include "config_keypad_etc.h"

class AdditionalButtons: public Base {
public:
  // static std::shared_ptr<AdditionalButtons> getInstance() {
  //   static std::shared_ptr<AdditionalButtons> instance = std::make_shared<AdditionalButtons>();
  //   return instance;
  // }

  void setup() override;
  void loop() override;

  void handleButtonPress(int buttonIndex);
  void addButtonAction(int buttonIndex, int action);
  void removeButtonAction(int buttonIndex);

  // AdditionalButtons() = default;
  // ~AdditionalButtons() = default;

private:
  // AdditionalButtons(const AdditionalButtons &) = delete;
  // AdditionalButtons &operator=(const AdditionalButtons &) = delete;

  std::vector<pimoroni::Button> additionalButtons;

uint additionalButtonPin[MAX_ADDITIONAL_BUTTONS] =          ADDITIONAL_BUTTONS_PINS;
Polarity additionalButtonType[MAX_ADDITIONAL_BUTTONS] =         ADDITIONAL_BUTTONS_TYPE;

  // std::shared_ptr<Base> globalValues;
};

#endif // ADDITIONALBUTTONS_H