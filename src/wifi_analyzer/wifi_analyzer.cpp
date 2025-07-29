#include "wifi_analyzer.h"
#include "main.h"

extern QueueHandle_t wifi_ap_queue;
extern SemaphoreHandle_t oled_mutex;

WiFiAnalyzer::WiFiAnalyzer() : scan_results(NULL)
{
  // Initialize WiFi - BW16 doesn't use mode() like ESP boards
  WiFi.disconnect();
  delay(100);
}

void WiFiAnalyzer::scan_networks()
{
  printf("Starting WiFi scan...\n");

  // BW16 uses a different scanning approach
  int networksFound = WiFi.scanNetworks();

  if (networksFound > 0)
  {
    process_scan_results(networksFound);
  }
  else
  {
    printf("No networks found\n");
  }

  // BW16 doesn't have scanDelete()
}

void WiFiAnalyzer::process_scan_results(int networksFound)
{
  printf("Found %d networks\n", networksFound);

  // Get scan results - BW16 provides direct access
  scan_results = (rtw_scan_result_t *)WiFi.getScanInfo();

  for (int i = 0; i < networksFound; ++i)
  {
    WiFiAPInfo ap;
    memset(&ap, 0, sizeof(ap));

    strncpy(ap.ssid, scan_results[i].SSID, sizeof(ap.ssid) - 1);
    ap.rssi = scan_results[i].signal_strength;
    ap.channel = scan_results[i].channel;
    ap.is_5ghz = (scan_results[i].bandwidth == 5);
    memcpy(ap.bssid, scan_results[i].BSSID.octet, 6);

    print_ap_info(ap);

    // Send AP info to UI task
    if (xQueueSend(wifi_ap_queue, &ap, portMAX_DELAY) != pdTRUE)
    {
      printf("Failed to send AP info to queue\n");
    }
  }
}

void WiFiAnalyzer::print_ap_info(const WiFiAPInfo &ap)
{
  printf("SSID: %s\n", ap.ssid);
  printf("RSSI: %d dBm\n", ap.rssi);
  printf("Channel: %d\n", ap.channel);
  printf("Band: %s\n", ap.is_5ghz ? "5GHz" : "2.4GHz");

  printf("BSSID: ");
  for (int i = 0; i < 6; i++)
  {
    printf("%02X", ap.bssid[i]);
    if (i < 5)
      printf(":");
  }
  printf("\n\n");
}

void WiFiAnalyzer::simulate_deauth(const WiFiAPInfo &ap)
{
  printf("[SIMULATED] Sending deauthentication packets to %s (%02X:%02X:%02X:%02X:%02X:%02X)\n",
         ap.ssid, ap.bssid[0], ap.bssid[1], ap.bssid[2], ap.bssid[3], ap.bssid[4], ap.bssid[5]);
}

void WiFiAnalyzer::simulate_deauth_all()
{
  printf("[SIMULATED] Sending deauthentication packets to all visible networks\n");
}