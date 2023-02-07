
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
       DEEP_SLEEP_MODE,
       RESET } currentState;
enum { CONJURING,
       GATHERING,
       TEXTURING,
       SQUEEZING,
       REFERENCE,
       TEST,
       SWITCH } currentMode;

String FILE_PHOTO_NEW = "/data/photo.jpg";

String sensor_name = "OV5640";  // OV5640, OV2640
// image sizes (4:3)
framesize_t OV2640_imSizes_4_3[7] = { FRAMESIZE_QQVGA, FRAMESIZE_HQVGA, FRAMESIZE_QVGA, FRAMESIZE_CIF, FRAMESIZE_SVGA, FRAMESIZE_XGA, FRAMESIZE_UXGA };
framesize_t OV5640_imSizes_4_3[9] = { FRAMESIZE_QQVGA, FRAMESIZE_HQVGA, FRAMESIZE_QVGA, FRAMESIZE_CIF, FRAMESIZE_SVGA, FRAMESIZE_XGA, FRAMESIZE_UXGA, FRAMESIZE_QXGA, FRAMESIZE_QSXGA }; // FRAMESIZE_QXGA and FRAMESIZE_QsXGA are in wrong order to test

String currentFileNames[100];  // this might be too big - I need a way to keep track of memory usage
boolean takeNewPhoto = true;
int read_val = 1;
int num_loops = 0;
int moveOnVal = 0;
int sleepPeriod = 0;
bool debug = true;
bool wifi_connected = false;
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

void squeezingMode(int counter, int numLayers, int numPhotos, int sleepPeriod, int numSince) {

  preferences.begin("solar-cam", false);
  // for each photo gather 3 base images: setttings -> gather photo -> change settings -> gather photo -> change settings -> gather photo -> increase count -> sleep
  for (int i = 0; i < numLayers; i++) {
    String temp_photo = "/squeezing/" + String(numSince) + "_" + String(counter) + "-" + String(i) + ".jpg";
    // change settings
    Fe_cam::gatheringLoop(i, currentSettings.numPhotos);
    delay(1000);
    Fe_cam::gatherPhotoSaveSD(temp_photo);
  }
  if (counter < numPhotos || !wifi_connected) {
    if (debug) Serial.println("in counter if");
    counter++;
    preferences.putInt("counter", counter);
    preferences.putString("state", "imaging");
  } else {
    preferences.putInt("counter", counter);
    currentState = UPLOAD_MODE;
    preferences.putString("state", "upload");
    int numSinceUpload = preferences.getInt("sinceUpload", 0);
    numSinceUpload++;
    preferences.putInt("sinceUpload", numSinceUpload);
  }
  int sleepPeriod_i = preferences.getInt("sleepPeriod", 5000);
  preferences.end();  // Close the Preferences

  float time_to_sleep = sleepPeriod_i / 1000;  // is float ok here?
  esp_sleep_enable_timer_wakeup(time_to_sleep * uS_TO_S_FACTOR);
  // esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  if (debug) Serial.println("Setup ESP32 to sleep for every " + String(time_to_sleep) + " Seconds");
  if (debug) Serial.flush();
  esp_deep_sleep_start();
}


void gatheringMode(int counter, int numLayers, int numPhotos, int sleepPeriod, int numSince) {
  preferences.begin("solar-cam", false);
  // for each photo gather 3 base images: setttings -> gather photo -> change settings -> gather photo -> change settings -> gather photo -> increase count -> sleep
  for (int i = 0; i < numLayers; i++) {
    String temp_photo = "/gathering/" + String(numSince) + "_" + String(counter) + "-" + String(i) + ".jpg";
    // change settings
    Fe_cam::gatheringLoop(i, currentSettings.numPhotos);
    delay(1000);
    Fe_cam::gatherPhotoSaveSD(temp_photo);
  }
  if (counter < numPhotos || !wifi_connected) {
    if (debug) Serial.println("in counter if");
    counter++;
    preferences.putInt("counter", counter);
    preferences.putString("state", "imaging");
  } else {
    preferences.putInt("counter", counter);
    currentState = UPLOAD_MODE;
    preferences.putString("state", "upload");
    int numSinceUpload = preferences.getInt("sinceUpload", 0);
    numSinceUpload++;
    preferences.putInt("sinceUpload", numSinceUpload);
  }
  int sleepPeriod_i = preferences.getInt("sleepPeriod", 5000);
  preferences.end();  // Close the Preferences
  // internet disconnect
  // firebase disconnect
  // deep sleep x amount of times - what happens when it turns on again? - I need a preferences file - currentState = IMAGING, currentMode = gathering,
  // }

  float time_to_sleep = sleepPeriod_i / 1000;  // is float ok here?
  esp_sleep_enable_timer_wakeup(time_to_sleep * uS_TO_S_FACTOR);
  // esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  if (debug) Serial.println("Setup ESP32 to sleep for every " + String(time_to_sleep) + " Seconds");
  if (debug) Serial.flush();
  esp_deep_sleep_start();
  // change current file names
}

void testMode(int counter, int numLayers, int numPhotos, int sleepPeriod) {
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
    String temp_photo = "/t/" + String(numSinceUpload) + "/" + String(counter) + "/" + String(i) + ".jpg";
    Fe_cam::gatherPhotoSaveSD(temp_photo);
    delay(1000);
    // save a record of the
    // normally I'll just do this with numbers so you don't need to continuously save an array of strings to memory
    // ie normally work out the string using layerVal, counterVal, and numCycles since upload
  }
  if (counter < numPhotos || !wifi_connected) {
    if (debug) Serial.println("in counter if");
    counter++;
    preferences.begin("solar-cam", false);
    preferences.putInt("counter", counter);
    preferences.end();  // Close the Preferences
  } else {
    counter = 0;  // I think reset counter in upload mode
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

void referenceMode(Fe_Firebase::settingsInput currentSettings) {
  for (int i = 0; i < currentSettings.numPhotos; i++) {
    Fe_cam::initCamera(sensor_name, OV5640_imSizes_4_3[i * 3]);  // testing
    delay(1500);                                                 // testing
    String temp_photo = "/data/photo" + String(i) + ".jpg";
    Fe_cam::gatheringLoop(i, currentSettings.numPhotos);
    // Fe_cam::testingAdjustExposure(i, currentSettings);
    Fe_cam::gatherPhotoSaveSD(temp_photo);
    delay(1000);
    Fe_cam::SD_to_SPIFFS(temp_photo);
    if (wifi_connected) Fe_Firebase::uploadFromSPIFFS(temp_photo);  // upload image
    if (wifi_connected) Fe_cam::removePhoto(temp_photo);
    if (wifi_connected) Fe_Firebase::writeVal("read", 0);
    currentState = SETTINGS_MODE;
  }
}
// void referenceMode() {
//   String temp_photo = "/data/photo.jpg";
//   Fe_cam::testingAdjustExposure(0);
//   Fe_cam::gatherPhotoSaveSD(temp_photo);
//   delay(1000);
//   Fe_cam::SD_to_SPIFFS(temp_photo);
//   Fe_Firebase::uploadFromSPIFFS(temp_photo);  // upload image
//   Fe_cam::removePhoto(temp_photo);
//   Fe_Firebase::writeVal("read", 0);
//   currentState = SETTINGS_MODE;
// }
void networkSearch() {
  // init wifi
  // search through a set of possible wifis
  // once connected to one then change state to SETTINGS_MODE
}



void imagingMode() {
  if (debug) Serial.println("=== in imaging mode ===");
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
  int numSinceUpload = preferences.getInt("sinceUpload", 0);
  preferences.end();

  // if(debui)
  Serial.print("counter is: ");
  Serial.println(counter);
  if (current.mode == "squeezing") {
    // if (counter <= 3) {
      if(debug)Serial.print("the sensor name is: ");
      if(debug)Serial.println(sensor_name);
      if (sensor_name == "OV5640") {
        // map counter between 0 and size of array - use counter modulo numLayers
        if(debug)Serial.print("the image size is: ");
        if(debug)Serial.println(OV5640_imSizes_4_3[(counter%3) * 3]);
        Fe_cam::initCamera(sensor_name, OV5640_imSizes_4_3[(counter%3) * 3]);
      } else if (sensor_name == "OV2640") {
        Fe_cam::initCamera(sensor_name, OV2640_imSizes_4_3[((counter%3) * 2) + 1]);
        if(debug)Serial.print("the image size is: ");
        if(debug)Serial.println(OV2640_imSizes_4_3[((counter%3) * 2) + 1]);
      }
    // } 
    // else {
    //   if (sensor_name == "OV5640") {
    //     Fe_cam::initCamera(sensor_name, OV5640_imSizes_4_3[9]);
    //   } else if (sensor_name == "OV2640") {
    //     Fe_cam::initCamera(sensor_name, OV2640_imSizes_4_3[7]);
    //   }
    // }
  } else if (current.mode == "gathering") {
    Fe_cam::initCamera(sensor_name, OV5640_imSizes_4_3[random(9)]);
  } else {
    if (sensor_name == "OV5640") {
      Fe_cam::initCamera(sensor_name, OV5640_imSizes_4_3[9]);
    } else if (sensor_name == "OV2640") {
      Fe_cam::initCamera(sensor_name, OV2640_imSizes_4_3[7]);
    }
  }
  Fe_cam::stopBrownout();
  delay(1000);
  // Fe_cam::adjustSettings(current);

  if (debug) Serial.println("Camera Initialised");

  // Fe_cam::resetCamera(0); // testing to see if something has built up in the frame buffer that needs resetting - might be worth resetting the camera every once in a while?
  // int lightVal = Fe_cam::adjustSettings(current);
  // if (debug) Serial.print("The light value is: ");
  // if (debug) Serial.println(lightVal);
  if (debug) Serial.print("The current mode is: ");
  if (debug) Serial.println(current.mode);
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
  } else if (current.mode == "squeezing") {
    currentMode = SQUEEZING;
  }

  switch (currentMode) {
    case CONJURING:
      conjuringMode(current.numPhotos);
      break;
    case GATHERING:
      gatheringMode(counter, current.layerVal, current.numPhotos, current.sleepPeriod, numSinceUpload);
      break;
    case TEXTURING:
      texturingMode(current.numPhotos, current.numCamera);
      break;
    case REFERENCE:
      referenceMode(current);
      break;
    case TEST:
      testMode(counter, current.layerVal, current.numPhotos, current.sleepPeriod);
      break;
    case SQUEEZING:
      squeezingMode(counter, current.layerVal, current.numPhotos, current.sleepPeriod, numSinceUpload);
      break;
    case SWITCH:

      break;
  }
}

void uploadMode() {
  // we only get here if wifi is connected
  // Fe_Firebase::settingsInput current;
  if (debug) Serial.println("==== in upload mode ===");
  // Get values we need to work out file names
  preferences.begin("solar-cam", false);
  int numSinceUpload = preferences.getInt("sinceUpload", 0);
  if (debug) Serial.print("num since upload is: ");
  if (debug) Serial.println(numSinceUpload);
  int numLayers = preferences.getInt("layerVal", 3);
  if (debug) Serial.print("num layers are: ");
  if (debug) Serial.println(numLayers);
  int counter = preferences.getInt("counter", 5);
  if (debug) Serial.print("the counter is: ");
  if (debug) Serial.println(counter);
  String mode = preferences.getString("imagingMode", "reference");
  if (debug) Serial.print("the imaging mode is: ");
  if (debug) Serial.println(mode);
  String temp_photo = "";
  for (int i = 0; i < numLayers; i++) {
    for (int j = 1; j < counter + 1; j++) {
      // String temp_photo = "/test/" + String(numSinceUpload) + "/" + String(counter) + "/" + String(i) + ".jpg";
      temp_photo = "/" + mode + "/" + String(numSinceUpload - 1) + "_" + String(j) + "-" + String(i) + ".jpg";
      if (debug) Serial.print("the current photo is: ");
      if (debug) Serial.println(temp_photo);
      bool exists = Fe_cam::SD_to_SPIFFS(temp_photo);  // check if file exists
      if (exists) {
        // temp_photo = "/" + mode + "/" + String(numSinceUpload - 1) + "/" + String(j) + "-" + String(i) + ".jpg";
        Fe_Firebase::uploadFromSPIFFS(temp_photo);
        // temp_photo = "/" + mode + "/" + String(numSinceUpload - 1) + "_" + String(j) + "-" + String(i) + ".jpg";
        Fe_cam::removePhoto(temp_photo);
      }
      Fe_cam::wipeSPIFFS();
    }
  }
  int sleepPeriod_i = preferences.getInt("sleepPeriod", 5000);

  counter = 0;
  preferences.putInt("counter", counter);
  preferences.putInt("sinceUpload", numSinceUpload);
  preferences.putString("state", "settings");
  preferences.end();
  Fe_Firebase::writeVal("read", 0);
  currentState = SETTINGS_MODE;
  float time_to_sleep = sleepPeriod_i / 1000;  // is float ok here?
  esp_sleep_enable_timer_wakeup(time_to_sleep * uS_TO_S_FACTOR);
}

void deepSleep(int sleep_period) {
  float time_to_sleep = sleep_period / 1000;  // is float ok here?
  esp_sleep_enable_timer_wakeup(time_to_sleep * uS_TO_S_FACTOR);
}

void reset() {
  preferences.begin("solar-cam", false);
  preferences.putInt("sinceUpload", 0);
  preferences.putString("state", "settings");
  preferences.putInt("counter", 0);
  preferences.end();
  deepSleep(5);
}

void settingsMode() {
  if (debug) Serial.println("=== in settings loop ===");
  delay(3000);
  if (wifi_connected) {
    read_val = Fe_Firebase::checkIntVal("read");
    if (debug) Serial.print("the read val is: ");
    if (debug) Serial.println(read_val);
    if (read_val == 1) {
      currentSettings = Fe_Firebase::getSettings();
      if (debug) Serial.println(currentSettings.brightness);
      // Fe_cam::adjustSettings(currentSettings);
      String tempCurrentMode = currentSettings.mode;
      if (debug) Serial.print("the image mode is: ");
      if (debug) Serial.println(currentSettings.mode);

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
      preferences.putInt("sleepPeriod", currentSettings.sleepPeriod);
      preferences.end();

      if (currentSettings.mode == "reset") {
        currentState = RESET;
      } else {
        currentState = IMAGING_MODE;
      }

    } else {
      delay(2000);
    }
  }else{
    currentState = IMAGING_MODE;
  }
}

void setup() {
  // if(debug)Serial port for debugging purposes
  if (debug) Serial.begin(115200);
  if (debug) Serial.println("In Setup");

  // !!! check battery level !!!
  preferences.begin("solar-cam", false);
  // Get the counter value, if the key does not exist, return a default value of SETTINGS_MODE
  // Note: Key name is limited to 15 chars.
  String state = preferences.getString("state", "settings");  // this is overall state not imaging mode! differentiate!
  sleepPeriod = preferences.getInt("sleepPeriod", 5000);
  if (debug) Serial.print("the sleep period is: ");
  if (debug) Serial.println(sleepPeriod);

  // preferences.putInt("counter", 0);
  // preferences.putInt("sinceUpload", 0);
  // state = "settings"; // use this line and the one below to reset the camera
  // preferences.putString("state", "settings");

  if (debug) Serial.print("the current state is: ");
  if (debug) Serial.println(state);
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
  delay(1000);
  wifi_connected = Fe_Wifi::initWiFi();
  if(wifi_connected && debug) Serial.println("Wifi initialized");
  if (wifi_connected) Fe_Firebase::initialize();
  if (wifi_connected &&debug) Serial.println("firebase initialized");
  // what about if this fails? either turn of or go to imaging mode
  // I need a method for checking if initWiFi succeeds or fails
  // if unsuccessful then increment the number of loops since upload, check battery levels and then either restart the loop again or switch off for a while
  // }
  Fe_cam::initSPIFFS();
  // if(debug)Serial.println("Spiffs initialized");
  Fe_cam::initSD();
  if (debug) Serial.println("Spiffs initialized");
  // if mode is imageing mode
  // Fe_cam::stopBrownout();
  // Fe_cam::initCamera(sensor_name);
  // if(debug)Serial.println("Camera Initialised");
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
      deepSleep(sleepPeriod);
      break;
    case RESET:
      reset();
      break;
  }
}