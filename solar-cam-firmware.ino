
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
       SWITCH } currentMode;

String FILE_PHOTO_NEW = "/data/photo.jpg";

String currentFileNames[100];  // this might be too big - I need a way to keep track of memory usage
boolean takeNewPhoto = true;
int read_val = 0;
int num_loops = 0;
int moveOnVal = 0;

// struct settingsInput {
//   // should this be in the camera module? probably
//   int brightness;
//   int contrast;
//   int saturation;
//   int autoExposureControl;  // 0 - 1600
//   int whiteBalance;
//   String mode;
//   int numPhotos;
//   int numCameras;
// };
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

void networkSearch() {
  // init wifi
  // search through a set of possible wifis
  // once connected to one then change state to SETTINGS_MODE
}

void settingsLoop(){
    currentSettings = Fe_Firebase::getSettings();
    Fe_cam::adjustSettings(currentSettings);
    String temp_photo = "/data/photo.jpg";
    Fe_cam::capturePhotoSaveSpiffs(temp_photo);
    delay(1);
    Fe_Firebase::uploadFromSPIFFS(temp_photo); // upload image
}

// settingsInput settingsMode() {
void settingsMode() {
  Serial.println("in settings loop");
  // readval = 1 means change settings
  // readval = 0 means wait
  // moveonval = 1 means next state
  // moveonval = 0 means wait
  delay(500);
  if (num_loops == 0) {
    settingsLoop();
  }
  if (read_val == 1 && moveOnVal == 0) {
    settingsLoop();
    Fe_Firebase::writeVal("read", 0);
    read_val = 0;
  }

  if (read_val == 0) {
    // read_val = Fe_Firebase::checkIntVal("read");
    moveOnVal = Fe_Firebase::checkIntVal("moveOn");
    Serial.print("the moveOn val is: ");
    Serial.println(moveOnVal);
    if(moveOnVal == 1){
      // reset moveOn val in database
      Fe_Firebase::writeVal("moveOn", 0);
      moveOnVal = 0;
      read_val = 0;
      currentState = IMAGING_MODE;
    }
    read_val = Fe_Firebase::checkIntVal("read");
    Serial.print("the read val is: ");
    Serial.println(read_val);
  }

  num_loops++;
  Serial.print("the number of loops is: ");
  Serial.println(num_loops);
  delay(1000);
  // state 1: check the database

  // state 2: change settings, upload, change back to 1

  // take_image - name as test image


  // receive settings
  // how can I check from the database when to receive settings? realtime database?
  //implement settings or go to next mode

}

void imagingMode(Fe_Firebase::settingsInput current) {
  if (current.mode == "conjuring") {
    currentMode = CONJURING;
  } else if (current.mode == "gathering") {
    currentMode = GATHERING;
  } else if (current.mode == "texturing") {
    currentMode = TEXTURING;
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
  // if (takeNewPhoto) {
  //   Fe_cam::capturePhotoSaveSpiffs(FILE_PHOTO_NEW);
  //   takeNewPhoto = false;
  // }
  // delay(1);
  // Fe_Firebase::uploadFromSPIFFS(FILE_PHOTO_NEW);
}