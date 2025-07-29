#include "button_debouncer.h"
#include "main.h"

extern QueueHandle_t ui_event_queue;

ButtonDebouncer::ButtonDebouncer(uint8_t pin, UIEvent event) 
  : pin(pin), event(event), last_press_time(0), last_state(HIGH) {
  pinMode(pin, INPUT_PULLUP);
}

void ButtonDebouncer::update() {
  bool current_state = digitalRead(pin);
  uint32_t now = millis();
  
  if (current_state != last_state) {
    last_press_time = now;
    last_state = current_state;
  }
  
  if (current_state == LOW && 
      (now - last_press_time) > DEBOUNCE_DELAY && 
      last_state == LOW) {
    xQueueSend(ui_event_queue, &event, 0);
    last_press_time = now; // Prevent rapid repeats
  }
}