#ifndef OLED_DRIVER_H
#define OLED_DRIVER_H

#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <Adafruit_NeoPixel.h>
#include <U8g2lib.h>
#include <SPI.h>
#include "main.h"

// Display configuration
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 128
#define OLED_RESET -1

// NeoPixel configuration (if used)
#define NEOPIXEL_PIN 8
#define NUMPIXELS 1

class OLEDDriver
{
public:
  OLEDDriver();
  void init();
  void clear();
  void draw_ap_list(const WiFiAPInfo *aps, int count, int selected_index);
  void draw_ap_details(const WiFiAPInfo &ap);

  // NeoPixel control
  void setStatusLED(uint32_t color);

private:
  Adafruit_SH1107 display;
  U8G2_SH1107_128X128_1_4W_HW_SPI u8g2; // Alternative driver
  Adafruit_NeoPixel statusLED;

  void draw_header(const char *title);
  void u8g2_draw_test(); // Example U8g2 usage
};

#endif // OLED_DRIVER_H