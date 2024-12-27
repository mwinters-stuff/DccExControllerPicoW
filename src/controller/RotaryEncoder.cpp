// This file implements the methods declared in RotaryEncoder.h, providing the logic for processing rotary encoder input.

#include "RotaryEncoder.h"


// AiEsp32RotaryEncoder rotaryEncoder(ROTARY_ENCODER_A_PIN, ROTARY_ENCODER_B_PIN, ROTARY_ENCODER_BUTTON_PIN, ROTARY_ENCODER_VCC_PIN, ROTARY_ENCODER_STEPS);

RotaryEncoder::RotaryEncoder(uint8_t pinA, uint8_t pinB, uint8_t pinButton):
    encoder(pio0,0, {pinA, pinB}, PIN_UNUSED),
    button(pinButton, Polarity::ACTIVE_LOW, 0) {
}

void RotaryEncoder::begin() {
    encoder.init();
}

void RotaryEncoder::update() {
  int32_t delta = encoder.delta();
  if (delta != 0){
    if (rotationCallback != nullptr) {
      rotationCallback(delta);
    }
  }

  if(button.read()) {
    if (buttonCallback != nullptr) {
      buttonCallback(1);
    }
  }
}

void RotaryEncoder::setRotationCallback(rotationcallback_t callback) {
    rotationCallback = callback;
}

void RotaryEncoder::setButtonCallback(rotationcallback_t callback) {
    buttonCallback = callback;
}