#include <WiFi.h>
#include <Preferences.h>
#include "BluetoothSerial.h"
#include "WiFiManager.h"

#define FORMAT 23

String connected_string;


String client_wifi_ssid;
String client_wifi_password;

const char* bluetooth_name = "ESP33";


BluetoothSerial SerialBT;
Preferences preferences;


void setup()
{
  Serial.begin(115200);
  pinMode(FORMAT, INPUT_PULLUP);
  Serial.println("Booting...");

  preferences.begin("wifi_access", false);

  if (!init_wifi(SerialBT, preferences)) { // Connect to Wi-Fi fails
    SerialBT.register_callback(callback);
  } else {
    Serial.println("");
    Serial.print("ESP32 IP: ");
    Serial.println(WiFi.localIP());
    SerialBT.register_callback(callback_show_ip);
  }

  SerialBT.begin(bluetooth_name);
}



void callback(esp_spp_cb_event_t event, esp_spp_cb_param_t *param)
{
  
  if (event == ESP_SPP_SRV_OPEN_EVT) {
    wifi_stage = SCAN_START;
  }

  if (event == ESP_SPP_DATA_IND_EVT && wifi_stage == SCAN_COMPLETE) { // data from phone is SSID
    int client_wifi_ssid_id = SerialBT.readString().toInt();
    client_wifi_ssid = WiFi.SSID(client_wifi_ssid_id - 1);
    wifi_stage = SSID_ENTERED;
  }

  if (event == ESP_SPP_DATA_IND_EVT && wifi_stage == WAIT_PASS) { // data from phone is password
    client_wifi_password = SerialBT.readString();
    client_wifi_password.trim();
    wifi_stage = PASS_ENTERED;
  }

}

void callback_show_ip(esp_spp_cb_event_t event, esp_spp_cb_param_t *param)
{
  if (event == ESP_SPP_SRV_OPEN_EVT) {
    SerialBT.print("ESP32 IP: ");
    SerialBT.println(WiFi.localIP());
    SerialBT.flush();
    bluetooth_disconnect = true;
  }
}


void loop()
{
  
  if (bluetooth_disconnect)
  {
    disconnect_bluetooth(SerialBT);
  }

  switch (wifi_stage)
  {
    case SCAN_START:
      SerialBT.println("Scanning Wi-Fi networks");
      SerialBT.flush();
      Serial.println("Scanning Wi-Fi networks");
      scan_wifi_networks(SerialBT);
      SerialBT.println("Please enter the number for your Wi-Fi");
      SerialBT.flush();
      wifi_stage = SCAN_COMPLETE;
      break;

    case SSID_ENTERED:
      SerialBT.println("Please enter your Wi-Fi password");
      SerialBT.flush();
      Serial.println("Please enter your Wi-Fi password");
      wifi_stage = WAIT_PASS;
      break;

    case PASS_ENTERED:
      SerialBT.println("Please wait for Wi-Fi connection...");
      SerialBT.flush();
      Serial.println("Please wait for Wi_Fi connection...");
      wifi_stage = WAIT_CONNECT;
      preferences.putString("pref_ssid", client_wifi_ssid);
      preferences.putString("pref_pass", client_wifi_password);
      if (init_wifi(SerialBT, preferences)) { // Connected to WiFi
        connected_string = "ESP32 IP: ";
        connected_string = connected_string + WiFi.localIP().toString();
        SerialBT.println(connected_string);
        Serial.println("\n" + connected_string);
        bluetooth_disconnect = true;
      } else { // try again
        wifi_stage = LOGIN_FAILED;
      }
      WiFi.scanDelete();
      break;

    case LOGIN_FAILED:
      SerialBT.println("Wi-Fi connection failed");
      Serial.println("Wi-Fi connection failed");
      delay(2000);
      wifi_stage = SCAN_START;
      break;
  }
  
  if(digitalRead(FORMAT)==LOW){
    Serial.println("Resetting configuration");
    preferences.clear();
    ESP.restart();
  } 
}