#ifndef WIFISCANNER_H
#define WIFISCANNER_H
#include <WiFi.h>
#include <Preferences.h>
#include "BluetoothSerial.h"

typedef enum wifi_setup_stages { NONE, SCAN_START, SCAN_COMPLETE, SSID_ENTERED, WAIT_PASS, PASS_ENTERED, WAIT_CONNECT, LOGIN_FAILED } WiFiStages_t;
extern WiFiStages_t wifi_stage;

extern bool bluetooth_disconnect;
extern const char* pref_ssid;
extern const char* pref_pass;

void scan_wifi_networks(BluetoothSerial &SerialBT);
bool init_wifi(BluetoothSerial &SerialBT, Preferences &preferences);
void disconnect_bluetooth(BluetoothSerial &SerialBT);

#endif