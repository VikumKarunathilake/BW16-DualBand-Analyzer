#include "wifi_analyzer.h"
#include "main.h"
#include <platform_stdlib.h>

extern QueueHandle_t wifi_ap_queue;
extern SemaphoreHandle_t oled_mutex;

WiFiAnalyzer::WiFiAnalyzer() : scan_results(nullptr) {
  // Initialize WiFi for BW16
  WiFi.disconnect();
  delay(100);
  // BW16 doesn't have enableSTA(), just use begin() if needed
}

void WiFiAnalyzer::scan_networks() {
  printf("[WiFi] Starting scan...\n");
  
  // Start synchronous scan
  int networksFound = WiFi.scanNetworks();
  
  if (networksFound > 0) {
    process_scan_results(networksFound);
  } else if (networksFound == 0) {
    printf("[WiFi] No networks found\n");
  } else {
    printf("[WiFi] Scan failed with error %d\n", networksFound);
  }
}

void WiFiAnalyzer::process_scan_results(int networksFound) {
  printf("[WiFi] Found %d networks\n", networksFound);
  
  for (int i = 0; i < networksFound; ++i) {
    WiFiAPInfo ap;
    memset(&ap, 0, sizeof(ap));
    
    // Get network info using BW16's WiFi methods
    String ssid = WiFi.SSID(i);
    strncpy(ap.ssid, ssid.c_str(), sizeof(ap.ssid) - 1);
    ap.rssi = WiFi.RSSI(i);
    ap.channel = WiFi.channel(i);
    
    // Determine band based on channel number
    ap.is_5ghz = (ap.channel > 14); // Channels > 14 are 5GHz
    
    // Get BSSID
    uint8_t* bssid = WiFi.BSSID(i);
    if (bssid) {
      memcpy(ap.bssid, bssid, 6);
    }
    
    print_ap_info(ap);
    
    // Send AP info to UI task
    if (xQueueSend(wifi_ap_queue, &ap, 10 / portTICK_PERIOD_MS) != pdTRUE) {
      printf("[WiFi] Queue full, dropping AP info\n");
    }
  }
}

void WiFiAnalyzer::print_ap_info(const WiFiAPInfo &ap) {
  printf("[AP] SSID: %s\n", ap.ssid);
  printf("     RSSI: %d dBm\n", ap.rssi);
  printf("     Channel: %d\n", ap.channel);
  printf("     Band: %s\n", ap.is_5ghz ? "5GHz" : "2.4GHz");
  
  printf("     BSSID: ");
  for (int i = 0; i < 6; i++) {
    printf("%02X", ap.bssid[i]);
    if (i < 5) printf(":");
  }
  printf("\n");
}

void WiFiAnalyzer::simulate_deauth(const WiFiAPInfo &ap) {
  printf("[SIM] Deauth to %s (", ap.ssid);
  for (int i = 0; i < 6; i++) {
    printf("%02X", ap.bssid[i]);
    if (i < 5) printf(":");
  }
  printf(")\n");
  
  // Actual deauth would use:
  // WiFi.deauth(ap.bssid, ...);
}

void WiFiAnalyzer::simulate_deauth_all() {
  printf("[SIM] Deauth to all visible networks\n");
}