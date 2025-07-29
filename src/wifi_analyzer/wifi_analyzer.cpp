#include "wifi_analyzer.h"
#include "main.h"

extern QueueHandle_t wifi_ap_queue;
extern SemaphoreHandle_t oled_mutex;

WiFiAnalyzer::WiFiAnalyzer() {
  // Initialize WiFi in promiscuous mode
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
}

void WiFiAnalyzer::scan_networks() {
  printf("Starting WiFi scan...\n");
  
  // Start async scan for both 2.4GHz and 5GHz networks
  WiFi.scanNetworks(true, true);
  
  // Wait for scan to complete
  int networksFound = 0;
  while ((networksFound = WiFi.scanComplete()) == WIFI_SCAN_RUNNING) {
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
  
  if (networksFound > 0) {
    process_scan_results(networksFound);
  } else {
    printf("No networks found\n");
  }
  
  WiFi.scanDelete();
}

void WiFiAnalyzer::process_scan_results(int networksFound) {
  printf("Found %d networks\n", networksFound);
  
  for (int i = 0; i < networksFound; ++i) {
    WiFiAPInfo ap;
    memset(&ap, 0, sizeof(ap));
    
    strncpy(ap.ssid, WiFi.SSID(i).c_str(), sizeof(ap.ssid) - 1);
    ap.rssi = WiFi.RSSI(i);
    ap.channel = WiFi.channel(i);
    ap.is_5ghz = (WiFi.is5GHz(i) == 1);
    memcpy(ap.bssid, WiFi.BSSID(i), 6);
    
    print_ap_info(ap);
    
    // Send AP info to UI task
    if (xQueueSend(wifi_ap_queue, &ap, portMAX_DELAY) != pdTRUE) {
      printf("Failed to send AP info to queue\n");
    }
  }
}

void WiFiAnalyzer::print_ap_info(const WiFiAPInfo &ap) {
  printf("SSID: %s\n", ap.ssid);
  printf("RSSI: %d dBm\n", ap.rssi);
  printf("Channel: %d\n", ap.channel);
  printf("Band: %s\n", ap.is_5ghz ? "5GHz" : "2.4GHz");
  
  printf("BSSID: ");
  for (int i = 0; i < 6; i++) {
    printf("%02X", ap.bssid[i]);
    if (i < 5) printf(":");
  }
  printf("\n\n");
}

void WiFiAnalyzer::simulate_deauth(const WiFiAPInfo &ap) {
  printf("[SIMULATED] Sending deauthentication packets to %s (%02X:%02X:%02X:%02X:%02X:%02X)\n",
         ap.ssid, ap.bssid[0], ap.bssid[1], ap.bssid[2], ap.bssid[3], ap.bssid[4], ap.bssid[5]);
  
  // In a real implementation, this would send actual deauth packets
  // For safety, we only simulate this action in the logs
}

void WiFiAnalyzer::simulate_deauth_all() {
  printf("[SIMULATED] Sending deauthentication packets to all visible networks\n");
  // Simulated action only
}