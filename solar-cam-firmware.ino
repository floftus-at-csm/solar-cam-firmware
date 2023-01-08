
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
       TEST,
       SWITCH } currentMode;

String FILE_PHOTO_NEW = "/data/photo.jpg";

String currentFileNames[100];  // this might be too big - I need a way to keep track of memory usage
boolean takeNewPhoto = true;
int read_val = 1;
int num_loops = 0;
int moveOnVal = 0;
#define uS_TO_S_FACTOR 1000000 /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP 5        /* Time ESP32 will go to sleep (in seconds) */
Preferences preferences;

Fe_Firebase::settingsInput currentSettings;
//--------------------------

// ------------------------------------------------------------
// function declaration
void conjuringMode(int num) {
  if (num == 1) {
    Fe_Wifi::turnOffWifi();
  }
  // loop over the number of photos that you're doing
  // create a string for each one

  //  save this for when I have an audio stream
  // change current file names
}

void gatheringMode(int counter, int numLayers, int numPhotos, int light, int sleepPeriod) {

  if (counter == 0) {
    // potentially just leave this
    Fe_Wifi::turnOffWifi();
  }
  // for each photo gather 3 base images: setttings -> gather photo -> change settings -> gather photo -> change settings -> gather photo -> increase count -> sleep
  for (int i = 0; i < numLayers; i++) {
    String temp_photo = "/gathering/" + String(counter) + "/" + String(i) + ".jpg";
    // change settings
    Fe_cam::gatherPhotoSaveSpiffs(temp_photo);
  }
  if (counter < numPhotos) {
    counter++;
    preferences.putInt("counter", counter);
  } else {
    counter = 0;
    preferences.putInt("counter", counter);
    currentState = UPLOAD_MODE;
  }
  preferences.end();  // Close the Preferences
  // internet disconnect
  // firebase disconnect
  // deep sleep x amount of times - what happens when it turns on again? - I need a preferences file - currentState = IMAGING, currentMode = gathering,
  // }


  // change current file names
}

void testMode(int counter, int numLayers, int numPhotos, int light, int sleepPeriod) {

  if (counter == 0) {
    // potentially just leave this
    Fe_Wifi::turnOffWifi();
  }
  preferences.begin("solar-cam", false);
  int numSinceUpload = preferences.getInt("sinceUpload", 0);
  preferences.end();
  // for each photo gather 3 base images: setttings -> gather photo -> change settings -> gather photo -> change settings -> gather photo -> increase count -> sleep
  for (int i = 0; i < numLayers; i++) {
    // change settings
    // Fe_cam::testingAdjustExposure(counter);
    String temp_photo = "/test/" + String(numSinceUpload) + "/" + String(counter) + "/" + String(i) + ".jpg";
    Fe_cam::gatherPhotoSaveSpiffs(temp_photo);
    delay(1000);
    // save a record of the
    // normally I'll just do this with numbers so you don't need to continuously save an array of strings to memory
    // ie normally work out the string using layerVal, counterVal, and numCycles since upload
  }
  if (counter < numPhotos) {
    counter++;
    preferences.begin("solar-cam", false);
    preferences.putInt("counter", counter);
    preferences.end();  // Close the Preferences
  } else {
    counter = 0;                              // I think reset counter in upload mode
    preferences.begin("solar-cam", false);
    preferences.putInt("counter", counter);  // move this
    preferences.putString("mode", "upload");   
    preferences.end();  // Close the Preferences
    // currentState = UPLOAD_MODE;
    // write current state to preferences then go to sleep
  }
  // internet disconnect
  // firebase disconnect
  float time_to_sleep = sleepPeriod / 1000;  // is float ok here?
  esp_sleep_enable_timer_wakeup(time_to_sleep * uS_TO_S_FACTOR);
}

void texturingMode(int num, int cams) {
  if (num == 1) {
    Fe_Wifi::turnOffWifi();
  }
  // change current file names
}

void referenceMode() {
  String temp_photo = "/data/photo.jpg";
  Fe_cam::gatherPhotoSaveSpiffs(temp_photo);
  delay(1000);
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
  Serial.println("=== in settings loop ===");
  delay(3000);
  read_val = Fe_Firebase::checkIntVal("read");
  Serial.print("the read val is: ");
  Serial.println(read_val);
  if (read_val == 1) {
    currentSettings = Fe_Firebase::getSettings();
    Serial.println(currentSettings.brightness);
    Fe_cam::adjustSettings(currentSettings);
    String tempCurrentMode = currentSettings.mode;
    Serial.print("the image mode is: ");
    Serial.println(currentSettings.mode);

    preferences.begin("solar-cam", false);
    preferences.putInt("brightness", currentSettings.brightness);
    preferences.putInt("saturation", currentSettings.saturation);
    preferences.putInt("aec", currentSettings.autoExposureControl);
    preferences.putInt("wb", currentSettings.whiteBalance);
    preferences.putString("imagingMode", currentSettings.mode);
    preferences.putInt("numPhotos", currentSettings.numPhotos);
    preferences.putInt("numCams", currentSettings.numCamera);
    preferences.putInt("layerVal", currentSettings.layerVal);
    preferences.putString("auto", currentSettings.autoMode);
    preferences.end();
    currentState = IMAGING_MODE;
  } else {
    delay(2000);
  }
}

void imagingMode() {
  //     preferences.putInt("brightness", currentSettings.brightness);
  //   preferences.putInt("saturation", currentSettings.saturation);
  //   preferences.putInt("aec", currentSettings.autoExposureControl);
  //   preferences.putInt("wb", currentSettings.whiteBalance );
  //   preferences.putString("mode", currentSettings.mode);
  //   preferences.putInt("numPhotos", currentSettings.numPhotos);
  //   preferences.putInt("numCams", currentSettings.numCamera);
  //   preferences.putString("auto", currentSettings.autoMode);
  //   int brightness;
  // int contrast;
  // int saturation;
  // int autoExposureControl; // 0 - 1600
  // int whiteBalance;
  // String mode;
  // int numPhotos;
  // int numCamera;
  // String autoMode;
  Serial.println("=== in imaging mode ===");
  Fe_Firebase::settingsInput current;
  preferences.begin("solar-cam", false);
  // imaging mode settings
  current.numPhotos = preferences.getInt("numPhotos", 1);
  int counter = preferences.getInt("counter", 0);
  current.numCamera = preferences.getInt("numCams", 1);
  current.sleepPeriod = preferences.getInt("sleepPeriod", 5000);
  // standard settings
  current.layerVal = preferences.getInt("layerVal", 5);
  current.brightness = preferences.getInt("brightness", 0);
  current.saturation = preferences.getInt("saturation", 0);
  current.autoExposureControl = preferences.getInt("aec", 400);
  current.whiteBalance = preferences.getInt("wb", 0);
  current.mode = preferences.getString("imagingMode", "reference");
  current.autoMode = preferences.getString("auto", "off");
  preferences.end();
  int lightVal = Fe_cam::adjustSettings(current);
  Serial.print("The light value is: ");
  Serial.println(lightVal);
  Serial.print("The current mode is: ");
  Serial.println(current.mode);
  if (current.mode == "conjuring") {
    currentMode = CONJURING;
  } else if (current.mode == "gathering") {
    currentMode = GATHERING;
  } else if (current.mode == "texturing") {
    currentMode = TEXTURING;
  } else if (current.mode == "reference") {
    currentMode = REFERENCE;
  } else if (current.mode == "test") {
    currentMode = TEST;
  }

  switch (currentMode) {
    case CONJURING:
      conjuringMode(current.numPhotos);
      break;
    case GATHERING:
      gatheringMode(counter, current.layerVal, current.numPhotos, lightVal, current.sleepPeriod);
      break;
    case TEXTURING:
      texturingMode(current.numPhotos, current.numCamera);
      break;
    case REFERENCE:
      referenceMode();
      break;
    case TEST:
      testMode(counter, current.layerVal, current.numPhotos, lightVal, current.sleepPeriod);
      break;
    case SWITCH:

      break;
  }
}

void uploadMode() {
  // we only get here if wifi is connected
  // Fe_Firebase::settingsInput current;
  Serial.println("==== in upload mode ===");
  // Get values we need to work out file names
  preferences.begin("solar-cam", false);
  int numSinceUpload = preferences.getInt("sinceUpload", 0);
  Serial.print("num since upload is: ");
  Serial.println(numSinceUpload);
  int numLayers = preferences.getInt("layerVal", 5);
  Serial.print("num layers are: ");
  Serial.println(numLayers);
  int counter = preferences.getInt("counter", 5);
  Serial.print("the counter is: ");
  Serial.println(counter);

  for (int i = 0; i < numLayers; i++) {
    String temp_photo = "/test/" + String(numSinceUpload) + "/" + String(counter) + "/" + String(i) + ".jpg";
    Fe_Firebase::uploadFromSPIFFS(temp_photo);
  }
  int sleepPeriod = preferences.getInt("sleepPeriod", 5000);
  numSinceUpload ++;
  preferences.putInt("sinceUpload", numSinceUpload);
  preferences.end(); 
  float time_to_sleep = sleepPeriod / 1000;  // is float ok here?
  esp_sleep_enable_timer_wakeup(time_to_sleep * uS_TO_S_FACTOR);
}

void deepSleep() {
}

void setup() {
  // Serial port for debugging purposes
  Serial.begin(115200);
  Serial.println("In Setup");

  // !!! check battery level !!!

  preferences.begin("solar-cam", false);
  // Get the counter value, if the key does not exist, return a default value of SETTINGS_MODE
  // Note: Key name is limited to 15 chars.
  String state = preferences.getString("state", "settings"); // this is overall state not imaging mode! differentiate!
  // state = "settings";
  // preferences.putString("mode", state);
  Serial.print("the current mode is: ");
  Serial.println(state);
  if (state == "settings") {
    currentState = SETTINGS_MODE;
  } else if (state == "imaging") {
    currentState = IMAGING_MODE;
  } else if (state == "upload") {
    currentState = UPLOAD_MODE;
  } else if (state == "deep_sleep") {
    currentState = DEEP_SLEEP_MODE;
  } else if (state == "network") {
    currentState = NETWORK_SEARCH;
  }
  // currentState = SETTINGS_MODE;
  preferences.end();  // Close the Preferences
  // if (currentState == SETTINGS_MODE || currentState == UPLOAD_MODE) {
    Fe_Wifi::initWiFi();
    Serial.println("Wifi initialized");
    Fe_Firebase::initialize();
    Serial.println("firebase initialized");
    // what about if this fails? either turn of or go to imaging mode
    // I need a method for checking if initWiFi succeeds or fails
    // if unsuccessful then increment the number of loops since upload, check battery levels and then either restart the loop again or switch off for a while
  // }
  Fe_cam::initSPIFFS();
  Serial.println("Spiffs initialized");
  Fe_cam::stopBrownout();
  Fe_cam::initCamera();
  Serial.println("Camera Initialised");
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
      imagingMode();
      break;
    case UPLOAD_MODE:
      uploadMode();
      break;
    case DEEP_SLEEP_MODE:
      deepSleep();
      break;
  }
}