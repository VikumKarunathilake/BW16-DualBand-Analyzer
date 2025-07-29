#include "oled_driver.h"
#include "main.h"

extern SemaphoreHandle_t oled_mutex;

OLEDDriver::OLEDDriver()
    : display(SCREEN_WIDTH, SCREEN_HEIGHT, &SPI, OLED_CS_PIN, OLED_DC_PIN, OLED_RESET_PIN),
      u8g2(U8G2_R0, OLED_CS_PIN, OLED_DC_PIN, OLED_RESET_PIN),
      statusLED(NUMPIXELS, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800) {}

void OLEDDriver::init()
{
  if (xSemaphoreTake(oled_mutex, portMAX_DELAY) == pdTRUE)
  {
    // Initialize SH1107 display
    display.begin();
    display.setRotation(1);
    display.clearDisplay();
    display.setTextColor(SH110X_WHITE);
    display.setTextSize(1);
    display.cp437(true);

    // Initialize U8g2 (alternative driver)
    u8g2.begin();
    u8g2.setFont(u8g2_font_6x10_tf);
    u8g2.setFontRefHeightExtendedText();
    u8g2.setDrawColor(1);
    u8g2.setFontPosTop();
    u8g2.setFontDirection(0);

    // Initialize NeoPixel
    statusLED.begin();
    statusLED.setBrightness(50);
    setStatusLED(statusLED.Color(0, 0, 255)); // Blue initial status

    xSemaphoreGive(oled_mutex);
  }
}

void OLEDDriver::clear()
{
  if (xSemaphoreTake(oled_mutex, portMAX_DELAY) == pdTRUE)
  {
    display.clearDisplay();
    u8g2.clearBuffer();
    xSemaphoreGive(oled_mutex);
  }
}

void OLEDDriver::setStatusLED(uint32_t color)
{
  statusLED.setPixelColor(0, color);
  statusLED.show();
}

void OLEDDriver::draw_header(const char *title)
{
  // Using Adafruit_SH110X
  display.fillRect(0, 0, SCREEN_WIDTH, 12, SH110X_WHITE);
  display.setTextColor(SH110X_BLACK);
  display.setCursor(5, 2);
  display.print(title);
  display.setTextColor(SH110X_WHITE);

  // Alternative using U8g2
  u8g2.setDrawColor(0);
  u8g2.drawBox(0, 0, SCREEN_WIDTH, 12);
  u8g2.setDrawColor(1);
  u8g2.setCursor(5, 2);
  u8g2.print(title);
}

void OLEDDriver::draw_ap_list(const WiFiAPInfo *aps, int count, int selected_index)
{
  if (xSemaphoreTake(oled_mutex, portMAX_DELAY) == pdTRUE)
  {
    clear();
    draw_header("Wi-Fi Networks");

    // Using Adafruit_SH110X as primary display
    for (int i = 0; i < count && i < 7; i++)
    {
      int y_pos = 15 + (i * 15);

      if (i == selected_index)
      {
        display.fillRect(0, y_pos - 2, SCREEN_WIDTH, 14, SH110X_WHITE);
        display.setTextColor(SH110X_BLACK);
      }
      else
      {
        display.setTextColor(SH110X_WHITE);
      }

      display.setCursor(5, y_pos);
      display.print(aps[i].ssid);

      display.setCursor(5, y_pos + 8);
      display.print("Ch:");
      display.print(ap.channel);
      display.print(" ");
      display.print(ap.ssid);
      display.print(" ");
      display.print(ap.rssi);
      display.println("dBm");
    }

    // Mirror to U8g2 for demonstration
    u8g2_draw_test();

    display.display();
    u8g2.sendBuffer();
    setStatusLED(statusLED.Color(0, 255, 0)); // Green when active

    xSemaphoreGive(oled_mutex);
  }
}

void OLEDDriver::draw_ap_details(const WiFiAPInfo &ap)
{
  if (xSemaphoreTake(oled_mutex, portMAX_DELAY) == pdTRUE)
  {
    clear();
    draw_header("AP Details");

    // Using Adafruit_SH110X
    display.setCursor(5, 15);
    display.print("SSID:");
    display.setCursor(5, 25);
    display.print(ap.ssid);

    display.setCursor(5, 40);
    display.print("BSSID:");
    display.setCursor(5, 50);
    for (int i = 0; i < 6; i++)
    {
      if (ap.bssid[i] < 0x10)
        display.print("0");
      display.print(ap.bssid[i], HEX);
      if (i < 5)
        display.print(":");
    }

    display.setCursor(5, 65);
    display.printf("Channel: %d (%s)", ap.channel, ap.is_5ghz ? "5GHz" : "2.4GHz");

    display.setCursor(5, 80);
    display.printf("Signal: %d dBm", ap.rssi);

    display.setCursor(5, 110);
    display.print("[SELECT] to deauth");

    display.display();
    setStatusLED(statusLED.Color(255, 0, 0)); // Red when in details

    xSemaphoreGive(oled_mutex);
  }
}

void OLEDDriver::u8g2_draw_test()
{
  // Example of using U8g2 alongside Adafruit
  u8g2.setCursor(0, 110);
  u8g2.print("U8g2 Secondary");
  u8g2.drawFrame(0, 100, 128, 20);
}