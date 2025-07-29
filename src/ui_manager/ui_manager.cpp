#include "ui_manager.h"
#include "main.h"

extern QueueHandle_t wifi_ap_queue;
extern QueueHandle_t ui_event_queue;
extern SemaphoreHandle_t oled_mutex;

UIManager::UIManager() 
  : ap_count(0), selected_index(0), show_details(false) {
  memset(ap_list, 0, sizeof(ap_list));
}

void UIManager::handle_events() {
  // Check for new WiFi scan results
  WiFiAPInfo new_ap;
  while (xQueueReceive(wifi_ap_queue, &new_ap, 0) == pdTRUE) {
    if (ap_count < MAX_APS) {
      memcpy(&ap_list[ap_count], &new_ap, sizeof(WiFiAPInfo));
      ap_count++;
    }
  }
  
  // Handle button events
  handle_button_events();
  
  // Update display
  update_display();
}

void UIManager::handle_button_events() {
  UIEvent event;
  while (xQueueReceive(ui_event_queue, &event, 0) == pdTRUE) {
    switch (event) {
      case UI_EVENT_UP:
        if (show_details) break;
        selected_index = (selected_index > 0) ? selected_index - 1 : 0;
        break;
        
      case UI_EVENT_DOWN:
        if (show_details) break;
        selected_index = (selected_index < ap_count - 1) ? selected_index + 1 : ap_count - 1;
        break;
        
      case UI_EVENT_SELECT:
        if (ap_count > 0) {
          if (show_details) {
            // In details view, select triggers deauth simulation
            wifi_analyzer.simulate_deauth(ap_list[selected_index]);
          } else {
            show_details = true;
          }
        }
        break;
        
      case UI_EVENT_BACK:
        show_details = false;
        break;
        
      default:
        break;
    }
  }
}

void UIManager::update_display() {
  if (show_details && ap_count > 0) {
    oled.draw_ap_details(ap_list[selected_index]);
  } else {
    oled.draw_ap_list(ap_list, ap_count, selected_index);
  }
}