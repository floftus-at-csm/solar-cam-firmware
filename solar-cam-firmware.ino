
enum currentState { NETWORK_SEARCH,
                    SETTINGS_MODE,
                    IMAGING_MODE,
                    UPLOAD_MODE };
// #include "WiFi.h"
#include "Arduino.h"
#include "Fe_cam.hpp"
#include "Fe_Wifi.hpp"
#include "Fe_Firebase.hpp"

String FILE_PHOTO_NEW = "/data/photo.jpg";

boolean takeNewPhoto = true;

void setup() {
  // Serial port for debugging purposes
  Serial.begin(115200);
  Fe_Wifi::initWiFi();
  Fe_cam::initSPIFFS();
  Fe_cam::stopBrownout();
  Fe_cam::initCamera();
  Fe_Firebase::initialize();

}

void loop() {
  if (takeNewPhoto) {
    Fe_cam::capturePhotoSaveSpiffs(FILE_PHOTO_NEW);
    takeNewPhoto = false;
  }
  delay(1);
  Fe_Firebase::uploadFromSPIFFS(FILE_PHOTO_NEW);

}