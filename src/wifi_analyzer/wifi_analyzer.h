#ifndef WIFI_ANALYZER_H
#define WIFI_ANALYZER_H

#include "main.h"
#include <WiFi.h>

class WiFiAnalyzer
{
public:
  WiFiAnalyzer();
  void scan_networks();
  void simulate_deauth(const WiFiAPInfo &ap);
  void simulate_deauth_all();

private:
  void process_scan_results(int networksFound);
  void print_ap_info(const WiFiAPInfo &ap);
  void *scan_results; // Opaque pointer for scan results
};

#endif // WIFI_ANALYZER_H