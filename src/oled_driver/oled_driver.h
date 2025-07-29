#ifndef OLED_DRIVER_H
#define OLED_DRIVER_H

#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <SPI.h>
#include "main.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 128
#define OLED_RESET -1

class OLEDDriver {
public:
  OLEDDriver();
  void init();
  void clear();
  void draw_ap_list(const WiFiAPInfo *aps, int count, int selected_index);
  void draw_ap_details(const WiFiAPInfo &ap);
  
private:
  Adafruit_SH1107 display;
  void draw_header(const char *title);
};

#endif // OLED_DRIVER_H