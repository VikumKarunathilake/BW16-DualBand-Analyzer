#ifndef OLED_DRIVER_H
#define OLED_DRIVER_H

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1351.h>
#include <SPI.h>
#include "main.h"

class OLEDDriver {
public:
  OLEDDriver();
  void init();
  void clear();
  void draw_ap_list(const WiFiAPInfo *aps, int count, int selected_index);
  void draw_ap_details(const WiFiAPInfo &ap);
  
private:
  Adafruit_SSD1351 display;
  void draw_header(const char *title);
};

#endif // OLED_DRIVER_H