// This file declares the AdditionalButtons class, responsible for managing additional button inputs and their associated actions.

#ifndef ADDITIONALBUTTONS_H
#define ADDITIONALBUTTONS_H

#include <button.hpp>
#include <vector>

class AdditionalButtons {
public:
      static AdditionalButtons &getInstance() {
    static AdditionalButtons instance;
    return instance;
  }

    void initialise();
    void additionalButtonLoop();
    void handleButtonPress(int buttonIndex);
    void addButtonAction(int buttonIndex, int action);
    void removeButtonAction(int buttonIndex);
private:
  AdditionalButtons() = default;
  ~AdditionalButtons() = default;
  AdditionalButtons(const AdditionalButtons &) = delete;
  AdditionalButtons &operator=(const AdditionalButtons &) = delete;

    std::vector<pimoroni::Button> additionalButtons;
    
};

#endif // ADDITIONALBUTTONS_H