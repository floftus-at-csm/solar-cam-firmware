#pragma once

#include "FS.h"
#include "Firebase_ESP_Client.h"

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
  String autoMode;
};

void initialize();

void uploadFromSPIFFS(String FILE_PHOTO);

void getArrayFromFirestore(FirebaseJson startingJson, String pathToArray);
settingsInput getSettings();

int checkIntVal(String fieldVal);

void writeVal(String fieldPath, int newValue);

}