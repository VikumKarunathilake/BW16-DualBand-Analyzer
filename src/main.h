#ifndef MAIN_H
#define MAIN_H

#include <Arduino.h>
#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include <semphr.h>

// Button GPIO Pins
#define BUTTON_UP_PIN    10
#define BUTTON_DOWN_PIN  11
#define BUTTON_SELECT_PIN 12

// OLED SPI Pins
#define OLED_CS_PIN     5
#define OLED_DC_PIN     6
#define OLED_RESET_PIN  7
#define OLED_MOSI_PIN   MOSI
#define OLED_SCK_PIN    SCK

// WiFi AP Information Structure
typedef struct {
  char ssid[33];
  int8_t rssi;
  uint8_t channel;
  bool is_5ghz;
  uint8_t bssid[6];
} WiFiAPInfo;

// UI Events
typedef enum {
  UI_EVENT_NONE,
  UI_EVENT_UP,
  UI_EVENT_DOWN,
  UI_EVENT_SELECT,
  UI_EVENT_BACK
} UIEvent;

// Function prototypes
void init_buttons();
void wifi_task(void *pvParameters);
void ui_task(void *pvParameters);
void uart_task(void *pvParameters);

#endif // MAIN_H