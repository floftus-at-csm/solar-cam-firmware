#pragma once

// Warning: This is a code example for the article "guideline for modular firmware".
//          It is code in a transition phase, do not use it as it is!

#include "esp_camera.h"
#include "soc/soc.h"           // Disable brownout problems
#include "soc/rtc_cntl_reg.h"  // Disable brownout problems
#include "driver/rtc_io.h"
#include <SPIFFS.h>
#include "Fe_Firebase.hpp"
#include "SD_MMC.h"                         // sd card - see https://randomnerdtutorials.com/esp32-cam-take-photo-save-microsd-card/
#include <SPI.h>
#include <FS.h>                             // gives file access
#define SD_CS 5                             // sd chip select pin = 5

/// The display module to control the attached LEDs

namespace Fe_cam {

// Check if photo capture was successful
bool checkPhoto(fs::FS& fs, String FILE_PHOTO);

// Capture Photo and Save it to SPIFFS
void gatherPhotoSaveSpiffs(String FILE_PHOTO);

void gatherPhotoSaveSD(String FILE_PHOTO);

void initSPIFFS();

void initSD();

void uploadImage(String FILE_PHOTO);

void SD_to_SPIFFS(String FILE_PHOTO);

void removePhoto(String FILE_PHOTO);

void initCamera();

void resetCamera(bool type);

void stopBrownout();

void gatheringLoop(int currentNum, int numPhotos);

int adjustSettings(Fe_Firebase::settingsInput currentSettings);

void standardAdjustExposure(int light);

void expAdjustExpossure(int light, int numLoops);

int testingAdjustExposure(int currentNum, Fe_Firebase::settingsInput currentSettings);
  }