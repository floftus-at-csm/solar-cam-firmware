#include "Fe_Wifi.hpp"

namespace Fe_Wifi {
//Replace with your network credentials
const char* ssid = "NOWTVP3MZZ";
const char* password = "65ImNYYg8Y3J";

void initWiFi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  // WiFi.persistent();  // stops ssid being written to flash - currently throwing an error
}

void turnOffWifi() {
  WiFi.disconnect();
  // WiFi.mode(WIFI_OFF);
  // WiFi.forceSleepBegin();
}

void reconnectWifi() {
  // unsigned long currentMillis = millis();
  // // if WiFi is down, try reconnecting
  // if ((WiFi.status() != WL_CONNECTED) && (currentMillis - previousMillis >= interval)) {
  //   Serial.print(millis());
  //   Serial.println("Reconnecting to WiFi...");
  //   WiFi.disconnect();
  //   WiFi.reconnect();
  //   previousMillis = currentMillis;
  // }
  WiFi.reconnect();
  // WiFi.mode(WIFI_STA);
  // WIFI_AP - esp comes up as a network to be connected to - this could be useful for connecting multiple cameras
}

// reconnecting https://randomnerdtutorials.com/esp32-useful-wi-fi-functions-arduino/

}