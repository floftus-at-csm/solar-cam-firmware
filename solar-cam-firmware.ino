
#include "Arduino.h"
#include "Fe_cam.hpp"
#include "Fe_Wifi.hpp"
#include "Fe_Firebase.hpp"
#include <Preferences.h>

// ------------------------------------------------------------
// variable definition
enum { NETWORK_SEARCH,
       SETTINGS_MODE,
       IMAGING_MODE,
       UPLOAD_MODE,
       DEEP_SLEEP_MODE } currentState;
enum { CONJURING,
       GATHERING,
       TEXTURING,
       REFERENCE,
       SWITCH } currentMode;

String FILE_PHOTO_NEW = "/data/photo.jpg";

String currentFileNames[100];  // this might be too big - I need a way to keep track of memory usage
boolean takeNewPhoto = true;
int read_val = 1;
int num_loops = 0;
int moveOnVal = 0;


Fe_Firebase::settingsInput currentSettings;
//--------------------------

// ------------------------------------------------------------
// function declaration
void conjuringMode(int num) {
  if (num == 1) {
    Fe_Wifi::turnOffWifi();
  }
  //  save this for when I have an audio stream
  // change current file names
}

void gatheringMode(int num) {
  if (num == 1) {
    Fe_Wifi::turnOffWifi();
  }
  // take image on trigger (timer or sensor)
  // repeat for set amount of photos?
  // change current file names
}

void texturingMode(int num, int cams) {
  if (num == 1) {
    Fe_Wifi::turnOffWifi();
  }
  // change current file names
}

void referenceMode() {
  String temp_photo = "/data/photo.jpg";
  Fe_cam::capturePhotoSaveSpiffs(temp_photo);
  delay(1);
  Fe_Firebase::uploadFromSPIFFS(temp_photo);  // upload image
  Fe_Firebase::writeVal("read", 0);
  currentState = SETTINGS_MODE;
}

void networkSearch() {
  // init wifi
  // search through a set of possible wifis
  // once connected to one then change state to SETTINGS_MODE
}


void settingsMode() {
  Serial.println("in settings loop");
  delay(3000);
  read_val = Fe_Firebase::checkIntVal("read");
  Serial.print("the read val is: ");
  Serial.println(read_val);
  if(read_val == 1){
    currentSettings = Fe_Firebase::getSettings();
    Serial.println(currentSettings.brightness);
    Fe_cam::adjustSettings(currentSettings);
    String tempCurrentMode = currentSettings.mode;
    Serial.print("the image mode is: ");
    Serial.println(currentSettings.mode);
    currentState = IMAGING_MODE;
  }else{
    delay(2000);
  }
}

void imagingMode(Fe_Firebase::settingsInput current) {
  // potentially convert to string
  Serial.println("in imaging mode loop");
  if (current.mode == "conjuring") {
    currentMode = CONJURING;
  } else if (current.mode == "gathering") {
    currentMode = GATHERING;
  } else if (current.mode == "texturing") {
    currentMode = TEXTURING;
  } else if (current.mode == "reference") {
    currentMode = REFERENCE;
  }

  switch (currentMode) {
    case CONJURING:
      conjuringMode(current.numPhotos);
      break;
    case GATHERING:
      gatheringMode(current.numPhotos);
      break;
    case TEXTURING:
      texturingMode(current.numPhotos, current.numCamera);
      break;
    case REFERENCE:
      referenceMode();
      break;
    case SWITCH:

      break;
  }
}

void uploadMode(String file_names[]) {
}

void deepSleep() {
}

void setup() {
  // Serial port for debugging purposes
  Serial.begin(115200);
  Serial.println("In Setup");
  Fe_Wifi::initWiFi();
  Serial.println("Wifi initialized");
  Fe_cam::initSPIFFS();
  Serial.println("Spiffs initialized");
  Fe_cam::stopBrownout();
  Fe_cam::initCamera();
  Serial.println("Camera Initialised");
  Fe_Firebase::initialize();
  Serial.println("firebase initialized");
  currentState = SETTINGS_MODE;
}

void loop() {
  switch (currentState) {
    case NETWORK_SEARCH:
      networkSearch();
      break;
    case SETTINGS_MODE:
      // currentSettings = settingsMode();
      settingsMode();
      break;
    case IMAGING_MODE:
      imagingMode(currentSettings);
      break;
    case UPLOAD_MODE:
      uploadMode(currentFileNames);
      break;
    case DEEP_SLEEP_MODE:
      deepSleep();
      break;
  }
}