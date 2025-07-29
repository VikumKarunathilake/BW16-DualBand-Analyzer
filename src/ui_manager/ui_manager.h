#ifndef UI_MANAGER_H
#define UI_MANAGER_H

#include "main.h"
#include "oled_driver.h"
#include "wifi_analyzer.h"

#define MAX_APS 20

class UIManager {
public:
  UIManager();
  void handle_events();
  
private:
  OLEDDriver oled;
  WiFiAnalyzer wifi_analyzer;
  
  WiFiAPInfo ap_list[MAX_APS];
  int ap_count;
  int selected_index;
  bool show_details;
  
  void handle_button_events();
  void update_display();
};

#endif // UI_MANAGER_H