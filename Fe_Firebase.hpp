#pragma once

#include "FS.h"
#include "Firebase_ESP_Client.h"
// #include "Fe_cam.hpp"
// Provide the SD card interfaces setting and mounting
// #include <addons/SDHelper.h>

namespace Fe_Firebase{

struct settingsInput{
  // should this be in the camera module? probably 
  int brightness;
  int contrast;
  int saturation;
  int autoExposureControl; // 0 - 1600
  int whiteBalance;
  String mode;
  int numPhotos;
  int numCamera;
  int layerVal;
  int sleepPeriod;
  String autoMode;
};

void initialize();

void uploadFromSPIFFS(String FILE_PHOTO);

void uploadFromSD(String FILE_PHOTO);

void getArrayFromFirestore(FirebaseJson startingJson, String pathToArray);
settingsInput getSettings();

int checkIntVal(String fieldVal);

void writeVal(String fieldPath, int newValue);

}