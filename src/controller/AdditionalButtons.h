// This file declares the AdditionalButtons class, responsible for managing additional button inputs and their associated actions.

#ifndef ADDITIONALBUTTONS_H
#define ADDITIONALBUTTONS_H

class AdditionalButtons {
public:
    AdditionalButtons();
    void initialise();
    void update();
    void handleButtonPress(int buttonIndex);
    void setButtonAction(int buttonIndex, void (*action)());
    
private:
    int additionalButtonPins[10]; // Array to hold additional button pins
    void (*buttonActions[10])(); // Array to hold actions for additional buttons
};

#endif // ADDITIONALBUTTONS_H