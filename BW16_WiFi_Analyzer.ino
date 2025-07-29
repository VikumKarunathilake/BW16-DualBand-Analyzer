#include "main.h"
#include "src/wifi_analyzer/wifi_analyzer.h"
#include "src/oled_driver/oled_driver.h"
#include "src/ui_manager/ui_manager.h"
#include "src/utils/button_debouncer.h"

// FreeRTOS Task Handles
TaskHandle_t wifi_task_handle = NULL;
TaskHandle_t ui_task_handle = NULL;
TaskHandle_t uart_task_handle = NULL;

// Shared resources
QueueHandle_t wifi_ap_queue;
QueueHandle_t ui_event_queue;
SemaphoreHandle_t oled_mutex;

// Button instances
ButtonDebouncer btn_up(BUTTON_UP_PIN, UI_EVENT_UP);
ButtonDebouncer btn_down(BUTTON_DOWN_PIN, UI_EVENT_DOWN);
ButtonDebouncer btn_select(BUTTON_SELECT_PIN, UI_EVENT_SELECT);

// OLED instance
OLEDDriver oled;

void init_buttons()
{
  // Buttons are initialized in their constructor
}

void setup()
{
  Serial.begin(115200);
  printf("\n\nBW16 Wi-Fi Analyzer Firmware\n");
  printf("Initializing...\n");

  // Initialize shared resources
  wifi_ap_queue = xQueueCreate(10, sizeof(WiFiAPInfo));
  ui_event_queue = xQueueCreate(10, sizeof(UIEvent));
  oled_mutex = xSemaphoreCreateMutex();

  // Initialize hardware
  init_buttons();
  oled.init();

  // Create FreeRTOS tasks
  xTaskCreate(wifi_task, "WiFiTask", 4096, NULL, 2, &wifi_task_handle);
  xTaskCreate(ui_task, "UITask", 4096, NULL, 2, &ui_task_handle);
  xTaskCreate(uart_task, "UARTTask", 2048, NULL, 1, &uart_task_handle);

  printf("Initialization complete. Starting scheduler...\n");
}

void loop()
{
  // Not used - FreeRTOS scheduler handles tasks
  vTaskDelay(portMAX_DELAY);
}

// FreeRTOS Tasks
void wifi_task(void *pvParameters)
{
  printf("WiFi Task started\n");
  WiFiAnalyzer analyzer;

  while (1)
  {
    analyzer.scan_networks();
    vTaskDelay(10000 / portTICK_PERIOD_MS); // Scan every 10 seconds
  }
}

void ui_task(void *pvParameters)
{
  printf("UI Task started\n");
  UIManager ui_manager;

  while (1)
  {
    ui_manager.handle_events();
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}

void uart_task(void *pvParameters)
{
  printf("UART Task started\n");

  while (1)
  {
    // Handle communication with secondary BW16
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}