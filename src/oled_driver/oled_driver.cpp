#include "oled_driver.h"
#include "main.h"

extern SemaphoreHandle_t oled_mutex;

OLEDDriver::OLEDDriver() 
  : display(SCREEN_WIDTH, SCREEN_HEIGHT, &SPI, OLED_CS_PIN, OLED_DC_PIN, OLED_RESET_PIN) {}

void OLEDDriver::init() {
  if (xSemaphoreTake(oled_mutex, portMAX_DELAY) == pdTRUE) {
    display.begin();
    display.setRotation(1); // Adjust rotation if needed
    display.clearDisplay();
    display.setTextColor(SH110X_WHITE);
    display.setTextSize(1);
    display.cp437(true);
    xSemaphoreGive(oled_mutex);
  }
}

void OLEDDriver::clear() {
  if (xSemaphoreTake(oled_mutex, portMAX_DELAY) == pdTRUE) {
    display.clearDisplay();
    xSemaphoreGive(oled_mutex);
  }
}

void OLEDDriver::draw_header(const char *title) {
  display.fillRect(0, 0, SCREEN_WIDTH, 12, SH110X_WHITE);
  display.setTextColor(SH110X_BLACK);
  display.setCursor(5, 2);
  display.print(title);
  display.setTextColor(SH110X_WHITE);
}

void OLEDDriver::draw_ap_list(const WiFiAPInfo *aps, int count, int selected_index) {
  if (xSemaphoreTake(oled_mutex, portMAX_DELAY) == pdTRUE) {
    clear();
    draw_header("Wi-Fi Networks");
    
    for (int i = 0; i < count && i < 7; i++) {
      int y_pos = 15 + (i * 15);
      
      // Highlight selected item
      if (i == selected_index) {
        display.fillRect(0, y_pos - 2, SCREEN_WIDTH, 14, SH110X_WHITE);
        display.setTextColor(SH110X_BLACK);
      } else {
        display.setTextColor(SH110X_WHITE);
      }
      
      display.setCursor(5, y_pos);
      display.print(aps[i].ssid);
      
      display.setCursor(5, y_pos + 8);
      display.print("Ch:");
      display.print(aps[i].channel);
      display.print(" ");
      display.print(aps[i].is_5ghz ? "5G" : "2G");
      display.print(" ");
      display.print(aps[i].rssi);
      display.print("dBm");
    }
    
    display.display();
    xSemaphoreGive(oled_mutex);
  }
}

void OLEDDriver::draw_ap_details(const WiFiAPInfo &ap) {
  if (xSemaphoreTake(oled_mutex, portMAX_DELAY) == pdTRUE) {
    clear();
    draw_header("AP Details");
    
    display.setCursor(5, 15);
    display.print("SSID:");
    display.setCursor(5, 25);
    display.print(ap.ssid);
    
    display.setCursor(5, 40);
    display.print("BSSID:");
    display.setCursor(5, 50);
    for (int i = 0; i < 6; i++) {
      if (ap.bssid[i] < 0x10) display.print("0");
      display.print(ap.bssid[i], HEX);
      if (i < 5) display.print(":");
    }
    
    display.setCursor(5, 65);
    display.print("Channel: ");
    display.print(ap.channel);
    display.print(" (");
    display.print(ap.is_5ghz ? "5GHz" : "2.4GHz");
    display.print(")");
    
    display.setCursor(5, 80);
    display.print("Signal: ");
    display.print(ap.rssi);
    display.print(" dBm");
    
    display.setCursor(5, 110);
    display.print("[SELECT] to deauth");
    
    display.display();
    xSemaphoreGive(oled_mutex);
  }
}