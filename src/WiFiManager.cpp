#include "WiFiManager.h"

WiFiStages_t wifi_stage = NONE;

String network_string = "";
const char* pref_ssid = "";
const char* pref_pass = "";
long start_wifi_millis;
long wifi_timeout = 10000;
bool bluetooth_disconnect = false;

bool init_wifi(BluetoothSerial &SerialBT, Preferences &preferences)
{
  String temp_pref_ssid = preferences.getString("pref_ssid", "");
  String temp_pref_pass = preferences.getString("pref_pass");

  if(!temp_pref_ssid.length()) return false;

  pref_ssid = temp_pref_ssid.c_str();
  pref_pass = temp_pref_pass.c_str();

  Serial.print("Connecting to:  ");
  Serial.println(pref_ssid);

  start_wifi_millis = millis();
  WiFi.begin(pref_ssid, pref_pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if (millis() - start_wifi_millis > wifi_timeout) {
      WiFi.disconnect(true, true);
      return false;
    }
  }
  return true;
}

void scan_wifi_networks(BluetoothSerial &SerialBT)
{
  WiFi.mode(WIFI_STA);
  // WiFi.scanNetworks will return the number of networks found
  int n =  WiFi.scanNetworks();
  if (n == 0) {
    SerialBT.println("no networks found");
    SerialBT.flush();
  } else {
    //SerialBT.println();
    SerialBT.print(n);
    SerialBT.println(" networks found");
    SerialBT.flush();
    delay(1000);
    for (int i = 0; i < n; ++i) {
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.println(WiFi.SSID(i));
      
      network_string = i + 1;
      network_string = network_string + ": " + WiFi.SSID(i) + " (Strength:" + WiFi.RSSI(i) + ")";
      SerialBT.println(network_string);
      SerialBT.flush();
    }
    wifi_stage = SCAN_COMPLETE;
  }
}

void disconnect_bluetooth(BluetoothSerial &SerialBT)
{
  delay(1000);
  Serial.println("BT stopping");
  SerialBT.println("Bluetooth disconnecting...");
  delay(1000);
  SerialBT.flush();
  SerialBT.disconnect();
  SerialBT.end();
  Serial.println("BT stopped");
  delay(1000);
  bluetooth_disconnect = false;
}