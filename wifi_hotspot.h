#ifndef WIFI_HOTSPOT_H

#define WIFI_HOTSPOT_H

#include <Arduino.h>
#include <fstream>

#define HOSTAPD_CONFIG_PATH "/etc/hostapd/hostapd.conf"
#define HOSTAPD_TMP_CONFIG_FILE "/tmp/hostapd.conf"

void hostapd_cfg_change_variable(std::string name, std::string value);
void start_hotspot(std::string ssid, std::string password);

#endif