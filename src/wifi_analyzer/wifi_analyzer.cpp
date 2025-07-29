#include "wifi_analyzer.h"
#include "main.h"
#include <platform_stdlib.h>

extern QueueHandle_t wifi_ap_queue;
extern SemaphoreHandle_t oled_mutex;

WiFiAnalyzer::WiFiAnalyzer() : scan_results(NULL)
{
  // Initialize WiFi for BW16
  WiFi.disconnect();
  delay(100);
  WiFi.enableSTA(true);
}

void WiFiAnalyzer::scan_networks()
{
  printf("[WiFi] Starting scan...\n");

  // Start synchronous scan (BW16 doesn't support async scan)
  int networksFound = WiFi.scanNetworks();

  if (networksFound > 0)
  {
    process_scan_results(networksFound);
  }
  else if (networksFound == 0)
  {
    printf("[WiFi] No networks found\n");
  }
  else
  {
    printf("[WiFi] Scan failed with error %d\n", networksFound);
  }
}

void WiFiAnalyzer::process_scan_results(int networksFound)
{
  printf("[WiFi] Found %d networks\n", networksFound);

  // Get scan results
  scan_results = (rtw_scan_result_t *)WiFi.getScanInfo();

  for (int i = 0; i < networksFound; ++i)
  {
    WiFiAPInfo ap;
    memset(&ap, 0, sizeof(ap));

    // Copy network info
    strncpy(ap.ssid, (const char *)scan_results[i].SSID, sizeof(ap.ssid) - 1);
    ap.rssi = scan_results[i].signal_strength;
    ap.channel = scan_results[i].channel;
    ap.is_5ghz = (scan_results[i].bandwidth == RTW_IEEE80211_BAND_5GHZ);
    memcpy(ap.bssid, scan_results[i].BSSID.octet, 6);

    print_ap_info(ap);

    // Send AP info to UI task
    if (xQueueSend(wifi_ap_queue, &ap, 10 / portTICK_PERIOD_MS) != pdTRUE)
    {
      printf("[WiFi] Queue full, dropping AP info\n");
    }
  }
}

void WiFiAnalyzer::print_ap_info(const WiFiAPInfo &ap)
{
  printf("[AP] SSID: %s\n", ap.ssid);
  printf("     RSSI: %d dBm\n", ap.rssi);
  printf("     Channel: %d\n", ap.channel);
  printf("     Band: %s\n", ap.is_5ghz ? "5GHz" : "2.4GHz");

  printf("     BSSID: ");
  for (int i = 0; i < 6; i++)
  {
    printf("%02X", ap.bssid[i]);
    if (i < 5)
      printf(":");
  }
  printf("\n");
}

void WiFiAnalyzer::simulate_deauth(const WiFiAPInfo &ap)
{
  printf("[SIM] Deauth to %s (", ap.ssid);
  for (int i = 0; i < 6; i++)
  {
    printf("%02X", ap.bssid[i]);
    if (i < 5)
      printf(":");
  }
  printf(")\n");

  // Actual deauth would use:
  // WiFi.deauth(ap.bssid, ...);
}

void WiFiAnalyzer::simulate_deauth_all()
{
  printf("[SIM] Deauth to all visible networks\n");
}