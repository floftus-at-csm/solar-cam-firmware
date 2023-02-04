#include "Fe_Wifi.hpp"
#include "secrets.h"
namespace Fe_Wifi {
//Replace with your network credentials
const char* ssid = secrets::secret_ssid;
const char* password = secrets::secret_password;

bool initWiFi() {
  int counter = 0;
  bool connected = true;
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
    counter ++;
    Serial.print("the counter is: ");
    Serial.println(counter);
    if(counter >= 7){
      connected = false;
      break;
    }
  }
  // WiFi.persistent();  // stops ssid being written to flash - currently throwing an error
  return connected;
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