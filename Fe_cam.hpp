#pragma once

// Warning: This is a code example for the article "guideline for modular firmware".
//          It is code in a transition phase, do not use it as it is!

#include "esp_camera.h"
#include "soc/soc.h"           // Disable brownout problems
#include "soc/rtc_cntl_reg.h"  // Disable brownout problems
#include "driver/rtc_io.h"
#include <SPIFFS.h>
#include "Fe_Firebase.hpp"

/// The display module to control the attached LEDs
///
namespace Fe_cam {

// struct settings{
//   // should this be in the camera module? probably 
//   int brightness;
//   int contrast;
//   int saturation;
//   int autoExposureControl; // 0 - 1600
//   String mode;
//   int numPhotos;
//   int numCamera;
// };

// Check if photo capture was successful
bool checkPhoto(fs::FS& fs, String FILE_PHOTO);

// Capture Photo and Save it to SPIFFS
void capturePhotoSaveSpiffs(String FILE_PHOTO);

void initSPIFFS();

void initCamera();

void stopBrownout();

void adjustSettings(Fe_Firebase::settingsInput currentSettings);

  }