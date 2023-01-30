#include "Fe_Firebase.hpp"
#include "Fe_cam.hpp"

namespace Fe_cam {

// OV2640 camera module pins (CAMERA_MODEL_AI_THINKER)
#define PWDN_GPIO_NUM 32
#define RESET_GPIO_NUM -1
#define XCLK_GPIO_NUM 0
#define SIOD_GPIO_NUM 26
#define SIOC_GPIO_NUM 27
#define Y9_GPIO_NUM 35
#define Y8_GPIO_NUM 34
#define Y7_GPIO_NUM 39
#define Y6_GPIO_NUM 36
#define Y5_GPIO_NUM 21
#define Y4_GPIO_NUM 19
#define Y3_GPIO_NUM 18
#define Y2_GPIO_NUM 5
#define VSYNC_GPIO_NUM 25
#define HREF_GPIO_NUM 23
#define PCLK_GPIO_NUM 22
#define SPI_CS_PIN 13
#define SPI_SCK_PIN 14
#define SPI_MISO_PIN 2
#define SPI_MOSI_PIN 15
#define SPI_CLOCK_IN_MHz 16

// array of filenames - to be used to delete files but also to post to firebase for uploading files
String filenames[100];  // e.g. you can only have 100 files saved at a time - this number is arbitrary but should be related to the size of your memory card

// String FILE_PHOTO = "/data/photo.jpg";

// Check if photo capture was successful
bool checkPhoto(fs::FS& fs, String FILE_PHOTO) {
  File f_pic = fs.open(FILE_PHOTO.c_str());
  unsigned int pic_sz = f_pic.size();
  return (pic_sz > 100);
}

// gather Photo and Save it to SPIFFS
void gatherPhotoSaveSpiffs(String FILE_PHOTO) {
  camera_fb_t* fb = NULL;  // pointer
  bool ok = 0;             // Boolean indicating if the picture has been taken correctly
  do {
    // Take a photo with the camera
    Serial.println("Taking a photo...");
    delay(1000);
    fb = esp_camera_fb_get();
    delay(1000);
    if (!fb) {
      Serial.println("Camera capture failed");
      return;
    }
    // CHANGE THIS TO A LOOP - set loop val in web app
    
    // esp_camera_fb_return(fb);
    // delay(1000);
    // fb = NULL;
    // delay(1000);
    // fb = esp_camera_fb_get();
    // delay(1000);
    // esp_camera_fb_return(fb);
    // delay(1000);
    // fb = NULL;
    // delay(750);
    // fb = esp_camera_fb_get();
    // Photo file name
    Serial.print("Picture file name: ");
    Serial.println(FILE_PHOTO);
    SPIFFS.remove(FILE_PHOTO);
    File file = SPIFFS.open(FILE_PHOTO, FILE_WRITE);
    // Insert the data in the photo file
    if (!file) {
      Serial.println("Failed to open file in writing mode");
    } else {
      file.write(fb->buf, fb->len);  // payload (image), payload length
      Serial.print("The picture has been saved in ");
      Serial.print(FILE_PHOTO);
      Serial.print(" - Size: ");
      Serial.print(file.size());
      Serial.println(" bytes");
    }
    // Close the file
    file.close();
    esp_camera_fb_return(fb);
    fb = NULL;

    // check if file has been correctly saved in SPIFFS
    ok = checkPhoto(SPIFFS, FILE_PHOTO);
  } while (!ok);
}

void initSPIFFS() {
  if (!SPIFFS.begin(true)) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    ESP.restart();
  } else {
    // SPIFFS.format();      // wipe spiffs
    delay(500);
    Serial.println("SPIFFS mounted successfully");
    Serial.printf("total bytes: %d , used: %d \n", SPIFFS.totalBytes(), SPIFFS.usedBytes());
    if(SPIFFS.usedBytes() > 200000){
      SPIFFS.format();
      Serial.println("Wiped SPIFFS");
      Serial.printf("total bytes: %d , used: %d \n", SPIFFS.totalBytes(), SPIFFS.usedBytes());
    }
  }
}

// gather Photo and Save it to SPIFFS
void gatherPhotoSaveSD(String FILE_PHOTO) {
  fs::FS &fs = SD_MMC;          // sd card file system
  camera_fb_t* fb = NULL;  // pointer
  // bool ok = 0;             // Boolean indicating if the picture has been taken correctly
  // do {
    // Take a photo with the camera
    Serial.println("Taking a photo...");
    delay(1000);
    fb = esp_camera_fb_get();
    delay(1000);
    esp_camera_fb_return(fb);
    fb = NULL;
    delay(1000);
    fb = esp_camera_fb_get();
    if (!fb) {
      Serial.println("Camera capture failed");
      return;
    }

    // Photo file name
    Serial.print("Picture file name: ");
    Serial.println(FILE_PHOTO);
    // SPIFFS.remove(FILE_PHOTO);
    // File file = fs.open(FILE_PHOTO.c_str(), FILE_WRITE);                                  // create file on sd card
    File file = fs.open(FILE_PHOTO.c_str(), FILE_APPEND);                                  // create file on sd card
    // Insert the data in the photo file
    if (!file) {
      Serial.println("Failed to open file in writing mode");
    } else {
      file.write(fb->buf, fb->len);  // payload (image), payload length
      Serial.print("The picture has been saved in ");
      Serial.print(FILE_PHOTO);
      Serial.print(" - Size: ");
      Serial.print(file.size());
      Serial.println(" bytes");
    }
    // Close the file
    file.close();
    esp_camera_fb_return(fb);
    fb = NULL;

    // check if file has been correctly saved in SPIFFS
  //   ok = checkPhoto(fs, FILE_PHOTO);
    
  // } while (!ok);
}


void initSD(){
  // SD Card - if one is detected set 'sdcardPresent' High
     if (!SD_MMC.begin("/sdcard", true)) {        // if loading sd card fails
       // note: ('/sdcard", true)' = 1bit mode - see: https://www.reddit.com/r/esp32/comments/d71es9/a_breakdown_of_my_experience_trying_to_talk_to_an/
      //  if (serialDebug) Serial.println("No SD Card detected");
      Serial.println("No SD Card detected");
      //  sdcardPresent = 0;                        // flag no sd card available
     } else {
       uint8_t cardType = SD_MMC.cardType();
       if (cardType == CARD_NONE) {              // if invalid card found
            Serial.println("SD Card type detect failed");
          //  sdcardPresent = 0;                    // flag no sd card available
       } else {
         // valid sd card detected
         uint16_t SDfreeSpace = (uint64_t)(SD_MMC.totalBytes() - SD_MMC.usedBytes()) / (1024 * 1024);
         Serial.printf("SD Card found, free space = %dmB \n", SDfreeSpace);
        //  sdcardPresent = 1;                      // flag sd card available
       }
     }
     fs::FS &fs = SD_MMC;                        // sd card file system

 // discover the number of image files already stored in '/img' folder of the sd card and set image file counter accordingly
  //  imageCounter = 0;
     int tq=fs.mkdir("/test");
     tq=fs.mkdir("/gathering");
     tq=fs.mkdir("/conjuring");                    // create the '/img' folder on sd card (in case it is not already there)
     tq=fs.mkdir("/reference");
     tq=fs.mkdir("/data");

}

void SD_to_SPIFFS(String FILE_PHOTO){
  Serial.println("converting from sd to spiffs");
  fs::FS &fs = SD_MMC;
  SPIFFS.begin();
  File source_file = fs.open(FILE_PHOTO.c_str(), FILE_READ);
  File SPIFFS_file = SPIFFS.open(FILE_PHOTO, FILE_WRITE);
  static uint8_t buf[512];
// Serialprintln(source_file) - check source file size??  
  while( source_file.read( buf, 512) ) {
      SPIFFS_file.write( buf, 512 );
      // maybe count up and if this gets stuck stop - count buffer size?
      // Serial.println("writing to spiffs");
  }
  Serial.println("converted file");
  source_file.close();
  SPIFFS_file.close();
  delay(500);
}

void removePhoto(String FILE_PHOTO){
  SPIFFS.format(); // deletes everything
  Serial.println("Wiped SPIFFS");
  fs::FS &fs = SD_MMC;
  fs.remove(FILE_PHOTO.c_str());
}

void uploadImage(String FILE_PHOTO){
  fs::FS &fs = SD_MMC; 
  SD_MMC.begin("/sdcard", true);
  // I think I need to call this from the camera module because this is where the 
    Fe_Firebase::uploadFromSD(FILE_PHOTO);
}

void initCamera() {
  // OV2640 camera module
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  // config.xclk_freq_hz = 20000000;
  config.xclk_freq_hz = 16500000; 
  config.pixel_format = PIXFORMAT_JPEG;
  // config.pixel_format = PIXFORMAT_GRAYSCALE;

  if (psramFound()) {
    Serial.println("psram found");
    config.frame_size = FRAMESIZE_UXGA;
    config.jpeg_quality = 3;  //0-63 lower number means higher quality - it was at 12
    config.fb_count = 3;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 2;
    config.fb_count = 1;
  }
  // Camera init
  esp_err_t err = esp_camera_init(&config);
  ESP_ERROR_CHECK(esp_wifi_set_ps(WIFI_PS_NONE));
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    ESP.restart();
  }
}

void resetCamera(bool type = 0) {
  // 1 = hardware reset - 0 = software reset
  if (type == 1) {
    // power cycle the camera module (handy if camera stops responding)
      digitalWrite(PWDN_GPIO_NUM, HIGH);    // turn power off to camera module
      delay(300);
      digitalWrite(PWDN_GPIO_NUM, LOW);
      delay(300);
      initCamera();
    } else {
    // reset via software (handy if you wish to change resolution or image type etc. - see test procedure)
      esp_camera_deinit();
      delay(50);
      initCamera();
    }
}

void stopBrownout() {
  // Turn-off the 'brownout detector'
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);
}

int adjustSettings(Fe_Firebase::settingsInput currentSettings) {
  int light = 0;
  Serial.println("Adjusting settings");
  sensor_t* s = esp_camera_sensor_get();

  s->set_brightness(s, currentSettings.brightness);  // -2 to 2
  // s->set_brightness(s, 2);  // -2 to 2
  s->set_contrast(s, currentSettings.contrast);                    // -2 to 2
  s->set_saturation(s, currentSettings.saturation);                  // -2 to 2
  s->set_special_effect(s, 0);              // 0 to 6 (0 - No Effect, 1 - Negative, 2 - Grayscale, 3 - Red Tint, 4 - Green Tint, 5 - Blue Tint, 6 - Sepia)
  s->set_whitebal(s, 1);                    // 0 = disable , 1 = enable
  s->set_awb_gain(s, 1);                    // 0 = disable , 1 = enable
  s->set_wb_mode(s, currentSettings.whiteBalance);                     // 0 to 4 - if awb_gain enabled (0 - Auto, 1 - Sunny, 2 - Cloudy, 3 - Office, 4 - Home)
  if(currentSettings.autoMode == "autoOn"){
    s->set_exposure_ctrl(s, 0);               // 0 = disable , 1 = enable
    s->set_aec2(s, 1);                        // 0 = disable , 1 = enable
    s->set_gain_ctrl(s, 1);                   // 0 = disable , 1 = enable
  }else{
    Serial.println(" =====  in exposure else condition ======== ");
    s->set_exposure_ctrl(s, 1);               // 0 = disable , 1 = enable
    s->set_aec2(s, 0);                        // 0 = disable , 1 = enable
    s->set_gain_ctrl(s, 0);                   // 0 = disable , 1 = enable
  }
  s->set_ae_level(s, 2);                    // -2 to 2
  // s->set_aec_value(s, currentSettings.autoExposureControl);                 // 0 to 1200
  s->set_aec_value(s, 400);
  s->set_agc_gain(s, 0);                    // 0 to 30
  s->set_gainceiling(s, (gainceiling_t)6);  // 0 to 6
  s->set_bpc(s, 1);                         // 0 = disable , 1 = enable
  s->set_wpc(s, 1);                         // 0 = disable , 1 = enable
  s->set_raw_gma(s, 1);                     // 0 = disable , 1 = enable
  s->set_lenc(s, 1);                        // 0 = disable , 1 = enable
  s->set_hmirror(s, 0);                     // 0 = disable , 1 = enable
  s->set_vflip(s, 0);                       // 0 = disable , 1 = enable
  s->set_dcw(s, 1);                         // 0 = disable , 1 = enable
  s->set_colorbar(s, 0);                    // 0 = disable , 1 = enable
  // s->set_reg(s,0xff,0xff,0x00);//banksel
  // s->set_reg(s,0xff,0xff,0x01);//banksel 
  // s->set_reg(s,0x11,0xff,01);//frame rate
  // s->set_reg(s,0xff,0xff,0x00);//banksel 
  // s->set_reg(s,0x86,0xff,1);//disable effects
  // s->set_reg(s,0xd3,0xff,5);//clock
  // s->set_reg(s,0x42,0xff,0x4f);//image quality (lower is bad)

  // s->set_reg(s,0x44,0xff,1);//quality
  light = s->get_reg(s,0x2f,0xff);
  delay(1200);
  Serial.println("Settings Adjusted");

  return light;
}

void standardAdjustExposure(int light){
  sensor_t* s = esp_camera_sensor_get();
  if(light<140)
    {
      //here we are in night mode
      if(light<45){
        s->set_reg(s,0x11,0xff,1);//frame rate (1 means longer exposure)
      }
      s->set_reg(s,0x13,0xff,0);//manual everything
      s->set_reg(s,0x0c,0x6,0x8);//manual banding
           
      s->set_reg(s,0x45,0x3f,0x3f);//really long exposure (but it doesn't really work)
    }
    else
    {
      //here we are in daylight mode
      
      s->set_reg(s,0x2d,0xff,0x0);//extra lines
      s->set_reg(s,0x2e,0xff,0x0);//extra lines

      s->set_reg(s,0x47,0xff,0x0);//Frame Length Adjustment MSBs

    if(light<150)
    {
      s->set_reg(s,0x46,0xff,0xd0);//Frame Length Adjustment LSBs
      s->set_reg(s,0x2a,0xff,0xff);//line adjust MSB
      s->set_reg(s,0x2b,0xff,0xff);//line adjust
      s->set_reg(s,0x45,0xff,0xff);//exposure (doesn't seem to work)
    }
    else if(light<160)
    {
      s->set_reg(s,0x46,0xff,0xc0);//Frame Length Adjustment LSBs
      s->set_reg(s,0x2a,0xff,0xb0);//line adjust MSB
      s->set_reg(s,0x2b,0xff,0xff);//line adjust
      s->set_reg(s,0x45,0xff,0x10);//exposure (doesn't seem to work)
    }    
    else if(light<170)
    {
      s->set_reg(s,0x46,0xff,0xb0);//Frame Length Adjustment LSBs
      s->set_reg(s,0x2a,0xff,0x80);//line adjust MSB
      s->set_reg(s,0x2b,0xff,0xff);//line adjust
      s->set_reg(s,0x45,0xff,0x10);//exposure (doesn't seem to work)
    }    
    else if(light<180)
    {
      s->set_reg(s,0x46,0xff,0xa8);//Frame Length Adjustment LSBs
      s->set_reg(s,0x2a,0xff,0x80);//line adjust MSB
      s->set_reg(s,0x2b,0xff,0xff);//line adjust
      s->set_reg(s,0x45,0xff,0x10);//exposure (doesn't seem to work)
    } 
    else if(light<190)
    {
      s->set_reg(s,0x46,0xff,0xa6);//Frame Length Adjustment LSBs
      s->set_reg(s,0x2a,0xff,0x80);//line adjust MSB
      s->set_reg(s,0x2b,0xff,0xff);//line adjust
      s->set_reg(s,0x45,0xff,0x90);//exposure (doesn't seem to work)
    } 
    else if(light<200)
    {
      s->set_reg(s,0x46,0xff,0xa4);//Frame Length Adjustment LSBs
      s->set_reg(s,0x2a,0xff,0x80);//line adjust MSB
      s->set_reg(s,0x2b,0xff,0xff);//line adjust
      s->set_reg(s,0x45,0xff,0x10);//exposure (doesn't seem to work)
    } 
    else if(light<210)
    {
      s->set_reg(s,0x46,0xff,0x98);//Frame Length Adjustment LSBs
      s->set_reg(s,0x2a,0xff,0x60);//line adjust MSB
      s->set_reg(s,0x2b,0xff,0xff);//line adjust
      s->set_reg(s,0x45,0xff,0x10);//exposure (doesn't seem to work)
    } 
    else if(light<220)
    {
      s->set_reg(s,0x46,0xff,0x80);//Frame Length Adjustment LSBs
      s->set_reg(s,0x2a,0xff,0x20);//line adjust MSB
      s->set_reg(s,0x2b,0xff,0xff);//line adjust
      s->set_reg(s,0x45,0xff,0x10);//exposure (doesn't seem to work)
    } 
    else if(light<230)
    {
      s->set_reg(s,0x46,0xff,0x70);//Frame Length Adjustment LSBs
      s->set_reg(s,0x2a,0xff,0x20);//line adjust MSB
      s->set_reg(s,0x2b,0xff,0xff);//line adjust
      s->set_reg(s,0x45,0xff,0x10);//exposure (doesn't seem to work)
    } 
    else if(light<240)
    {
      s->set_reg(s,0x46,0xff,0x60);//Frame Length Adjustment LSBs
      s->set_reg(s,0x2a,0xff,0x20);//line adjust MSB
      s->set_reg(s,0x2b,0xff,0x80);//line adjust
      s->set_reg(s,0x45,0xff,0x10);//exposure (doesn't seem to work)
    } 
    else if(light<253)
    {
      s->set_reg(s,0x46,0xff,0x10);//Frame Length Adjustment LSBs
      s->set_reg(s,0x2a,0xff,0x0);//line adjust MSB
      s->set_reg(s,0x2b,0xff,0x40);//line adjust
      s->set_reg(s,0x45,0xff,0x10);//exposure (doesn't seem to work)
    }
    else
    {
      s->set_reg(s,0x46,0xff,0x0);//Frame Length Adjustment LSBs
      s->set_reg(s,0x2a,0xff,0x0);//line adjust MSB
      s->set_reg(s,0x2b,0xff,0x0);//line adjust
      s->set_reg(s,0x45,0xff,0x0);//exposure (doesn't seem to work)
      s->set_reg(s,0x10,0xff,0x0);//exposure (doesn't seem to work)
    }
    }
}

void expAdjustExposure(int light, int numLoops){
  // use randomness to select values in the array - if the light value is really high or low maybe cut out some values at the top or bottom so as to not overexposure
  // potentially not full randomness as this might make automating the image processing difficult
  // step up the array?
  // s[;ot i[ tje]]
  sensor_t* s = esp_camera_sensor_get();
  if(light<140)
    {
      //here we are in night mode
      if(light<45){
        s->set_reg(s,0x11,0xff,1);//frame rate (1 means longer exposure)
      }
      s->set_reg(s,0x13,0xff,0);//manual everything
      s->set_reg(s,0x0c,0x6,0x8);//manual banding
           
      s->set_reg(s,0x45,0x3f,0x3f);//really long exposure (but it doesn't really work)
    }
    else
    {
      //here we are in daylight mode
      
      s->set_reg(s,0x2d,0xff,0x0);//extra lines
      s->set_reg(s,0x2e,0xff,0x0);//extra lines
      s->set_reg(s,0x47,0xff,0x0);//Frame Length Adjustment MSBs          

      s->set_reg(s,0x46,0xff,0xd0);//Frame Length Adjustment LSBs         - this changes from [0xd0, 0xc0, 0xb0, 0xa8, 0xa6, 0xa4, 0x98, 0x80, 0x70, 0x60, 0x10, 0x0]
      s->set_reg(s,0x2a,0xff,0xff);//line adjust MSB                      - this changes from [0xff, 0xb0, 0x80, 0x80, 0x80, 0x80, 0x60, 0x20, 0x20, 0x20, 0x0, 0x0]
      s->set_reg(s,0x2b,0xff,0xff);//line adjust                          - this changes from [0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x80, 0x40, 0x0]
      s->set_reg(s,0x45,0xff,0xff);//exposure (doesn't seem to work)      - this changes from [0xff, 0x10, 0x10, 0x10, 0x90, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x0] - 0x3f (63) - can I potentitally go up to this? on both of last two

      s->set_reg(s,0x0f,0xff,0x4b);//no idea
      s->set_reg(s,0x03,0xff,0xcf);//no idea
      s->set_reg(s,0x3d,0xff,0x34);//changes the exposure somehow, has to do with frame rate - exp with other values?

      s->set_reg(s,0x11,0xff,0x0);//frame rate
      s->set_reg(s,0x43,0xff,0x11);//11 is the default value 

      // sometimes do this with lower brightess, even if brightness is too high
      s->set_brightness(s, -2);  // -2 to 2
      // s->set_reg(s,0x45,0x3f,0x3f);//really long exposure (but it doesn't really work)
    }
}


void gatheringLoop(int currentNum, int numPhotos){
  int light = 0;
  sensor_t* s = esp_camera_sensor_get();
  // potentially invert hex order
  unsigned int hexArray2[] = {0x3f,  0x28, 0x0 }; // 0, 10, 20, 30, 40, 50, 63
  // [0xd0, 0xc0, 0xb0, 0xa8, 0xa6, 0xa4, 0x98, 0x80, 0x70, 0x60, 0x10, 0x0]
  unsigned int hexArray[] = {0x0, 0x10,  0x98, 0xa8, 0xd0};
  int exp_ctrl[] = {0, 0, 1};
  // int gain_tester[] = {0, 15, 30};
  // int aec[] = {400, 600, 1200};
  int contrasts[] = {2, 0, -2};
  // int wb_mode[] = {0, 2, 4};
    s->set_brightness(s, 0);  // -2 to 2
  // s->set_brightness(s, 2);  // -2 to 2
  s->set_contrast(s, contrasts[currentNum]);                    // -2 to 2
  s->set_saturation(s, 0);                  // -2 to 2
  s->set_special_effect(s, 0);              // 0 to 6 (0 - No Effect, 1 - Negative, 2 - Grayscale, 3 - Red Tint, 4 - Green Tint, 5 - Blue Tint, 6 - Sepia)
  s->set_whitebal(s, 1);                    // 0 = disable , 1 = enable
  s->set_awb_gain(s, 1);                    // 0 = disable , 1 = enable
  s->set_wb_mode(s, 0);                     // 0 to 4 - if awb_gain enabled (0 - Auto, 1 - Sunny, 2 - Cloudy, 3 - Office, 4 - Home)


    s->set_exposure_ctrl(s, exp_ctrl[currentNum]);               // 0 = disable , 1 = enable
    s->set_aec2(s, 0);                        // 0 = disable , 1 = enable
    s->set_gain_ctrl(s, 0);                   // 0 = disable , 1 = enable

  s->set_ae_level(s, 0);                    // -2 to 2
  // s->set_aec_value(s, currentSettings.autoExposureControl);                 // 0 to 1200
  s->set_aec_value(s, 400);
  s->set_agc_gain(s, 0);                    // 0 to 30
  s->set_gainceiling(s, (gainceiling_t)6);  // 0 to 6
  s->set_bpc(s, 1);                         // 0 = disable , 1 = enable
  s->set_wpc(s, 1);                         // 0 = disable , 1 = enable
  s->set_raw_gma(s, 1);                     // 0 = disable , 1 = enable
  s->set_lenc(s, 1);                        // 0 = disable , 1 = enable
  s->set_hmirror(s, 0);                     // 0 = disable , 1 = enable
  s->set_vflip(s, 0);                       // 0 = disable , 1 = enable
  s->set_dcw(s, 1);                         // 0 = disable , 1 = enable
  s->set_colorbar(s, 0);                    // 0 = disable , 1 = enable
  s->set_reg(s,0xff,0xff,0x01);//banksel
  s->set_reg(s,0x2d,0xff,0x0);//extra lines
  s->set_reg(s,0x2e,0xff,0x0);//extra lines
  s->set_reg(s,0x47,0xff,0x0);//Frame Length Adjustment MSBs  

  // s->set_reg(s, 0x46, 0xff, 0xd0);//Frame Length Adjustment LSBs  - start with this consistent to see whats happening
  s->set_reg(s, 0x46, 0xff, hexArray[currentNum]);
  s->set_reg(s, 0x2a, 0xff, 0xff);//line adjust MSB - start with this as consistent so I can see whats happening
  s->set_reg(s, 0x2b, 0xff, 0xff); //line adjust
  s->set_reg(s, 0x45, 0xff, hexArray2[currentNum]); //exposure (doesn't seem to work) 
  s->set_reg(s,0x11,0xff,1);//frame rate (1 means longer exposure)

  // potentially change set_aec_value(s, 400) and 
  //  s->set_brightness(s, -2);  // -2 to 2
  light = s->get_reg(s,0x2f,0xff);
  Serial.print("the hex value is: ");
  Serial.println(hexArray2[currentNum]);
  Serial.print("the light value is: ");
  Serial.println(light);
  delay(1400);
}
int testingAdjustExposure(int currentNum, Fe_Firebase::settingsInput currentSettings){
  //   int brightness;
  // int contrast;
  // int saturation;
  // int autoExposureControl; // 0 - 1600
  // int whiteBalance;
  // String mode;
  // int numPhotos;
  // int numCamera;
  // int layerVal;
  // int sleepPeriod;
  // String autoMode;
  int light = 0;
  sensor_t* s = esp_camera_sensor_get();
  unsigned int hexArray2[] = {0x0, 0x0a, 0x14, 0x1e, 0x28, 0x32, 0x3f}; // 0, 10, 20, 30, 40, 50, 63
  // [0xd0, 0xc0, 0xb0, 0xa8, 0xa6, 0xa4, 0x98, 0x80, 0x70, 0x60, 0x10, 0x0]
  unsigned int hexArray[] = {0xd0, 0xc0, 0xb0, 0xa8, 0xa6, 0xa4, 0x98, 0x80, 0x70, 0x60, 0x10, 0x0};
    s->set_brightness(s, 0);  // -2 to 2
  // s->set_brightness(s, 2);  // -2 to 2
  s->set_contrast(s, 0);                    // -2 to 2
  s->set_saturation(s,2);                  // -2 to 2
  s->set_special_effect(s, 0);              // 0 to 6 (0 - No Effect, 1 - Negative, 2 - Grayscale, 3 - Red Tint, 4 - Green Tint, 5 - Blue Tint, 6 - Sepia)
  s->set_whitebal(s, 1);                    // 0 = disable , 1 = enable
  s->set_awb_gain(s, 1);                    // 0 = disable , 1 = enable
  s->set_wb_mode(s, 0);                     // 0 to 4 - if awb_gain enabled (0 - Auto, 1 - Sunny, 2 - Cloudy, 3 - Office, 4 - Home)


    s->set_exposure_ctrl(s, currentSettings.brightness);               // 0 = disable , 1 = enable
    s->set_aec2(s, currentSettings.contrast);                        // 0 = disable , 1 = enable
    s->set_gain_ctrl(s, currentSettings.saturation);                   // 0 = disable , 1 = enable

  s->set_ae_level(s, 0);                    // -2 to 2
  // s->set_aec_value(s, currentSettings.autoExposureControl);                 // 0 to 1200
  s->set_aec_value(s, currentSettings.autoExposureControl);
  s->set_agc_gain(s, 0);                    // 0 to 30
  s->set_gainceiling(s, (gainceiling_t)6);  // 0 to 6
  s->set_bpc(s, 1);                         // 0 = disable , 1 = enable
  s->set_wpc(s, 1);                         // 0 = disable , 1 = enable
  s->set_raw_gma(s, 1);                     // 0 = disable , 1 = enable
  s->set_lenc(s, 1);                        // 0 = disable , 1 = enable
  s->set_hmirror(s, 0);                     // 0 = disable , 1 = enable
  s->set_vflip(s, 0);                       // 0 = disable , 1 = enable
  s->set_dcw(s, 1);                         // 0 = disable , 1 = enable
  s->set_colorbar(s, 0);                    // 0 = disable , 1 = enable
  s->set_reg(s,0xff,0xff,0x01);//banksel
  s->set_reg(s,0x2d,0xff,0x0);//extra lines
  s->set_reg(s,0x2e,0xff,0x0);//extra lines
  s->set_reg(s,0x47,0xff,0x0);//Frame Length Adjustment MSBs  

  // s->set_reg(s, 0x46, 0xff, 0xd0);//Frame Length Adjustment LSBs  - start with this consistent to see whats happening
  s->set_reg(s, 0x46, 0xff, hexArray[currentNum]);
  s->set_reg(s, 0x2a, 0xff, 0xff);//line adjust MSB - start with this as consistent so I can see whats happening
  s->set_reg(s, 0x2b, 0xff, 0xff); //line adjust
  s->set_reg(s, 0x45, 0xff, hexArray2[currentNum]); //exposure (doesn't seem to work) 
  s->set_reg(s,0x11,0xff,1);//frame rate (1 means longer exposure)

  // potentially change set_aec_value(s, 400) and 
  //  s->set_brightness(s, -2);  // -2 to 2
  light = s->get_reg(s,0x2f,0xff);
  Serial.print("the hex value is: ");
  Serial.println(hexArray2[currentNum]);
  Serial.print("the light value is: ");
  Serial.println(light);
  delay(1200);
  // String imageSaveString = "0x45-0xff-" + String(hexArray[currentNum]);
  // return imageSaveString;
    // s->set_reg(s,0x45, hexArray[currentNum], hexArray[currentNum]);
    return light;
}

}

