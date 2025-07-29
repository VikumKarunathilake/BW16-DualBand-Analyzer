#ifndef BUTTON_DEBOUNCER_H
#define BUTTON_DEBOUNCER_H

#include "main.h"

class ButtonDebouncer {
public:
  ButtonDebouncer(uint8_t pin, UIEvent event);
  void update();
  
private:
  uint8_t pin;
  UIEvent event;
  uint32_t last_press_time;
  bool last_state;
  
  static const uint32_t DEBOUNCE_DELAY = 50;
};

#endif // BUTTON_DEBOUNCER_H