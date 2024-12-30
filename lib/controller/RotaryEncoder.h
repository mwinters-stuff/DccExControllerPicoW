#ifndef ROTARYENCODER_H
#define ROTARYENCODER_H

// RotaryEncoder.h

#include <encoder.hpp>
#include <button.hpp>

typedef void (*rotationcallback_t)(int);

class RotaryEncoder {
public:
    RotaryEncoder(uint8_t pinA, uint8_t pinB, uint8_t pinButton);
    void begin();
    void update();
    // bool isButtonPressed();
    // int getValue();
    // void reset();

    void setRotationCallback(rotationcallback_t callback);
    void setButtonCallback(rotationcallback_t callback);
private:
    encoder::Encoder encoder;
    Button button;
    int lastValue;
    rotationcallback_t rotationCallback;
    rotationcallback_t buttonCallback;
};

#endif // ROTARYENCODER_H