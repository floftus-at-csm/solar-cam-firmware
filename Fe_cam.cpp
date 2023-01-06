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

// array of filenames - to be used to delete files but also to post to firebase for uploading files
String filenames[100];  // e.g. you can only have 100 files saved at a time - this number is arbitrary but should be related to the size of your memory card

// String FILE_PHOTO = "/data/photo.jpg";

// Check if photo capture was successful
bool checkPhoto(fs::FS& fs, String FILE_PHOTO) {
  File f_pic = fs.open(FILE_PHOTO);
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

    fb = esp_camera_fb_get();
    if (!fb) {
      Serial.println("Camera capture failed");
      return;
    }
    // CHANGE THIS TO A LOOP - set loop val in web app
    delay(1000);
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
    Serial.printf("Picture file name: %s\n", FILE_PHOTO);
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

    // check if file has been correctly saved in SPIFFS
    ok = checkPhoto(SPIFFS, FILE_PHOTO);
  } while (!ok);
}

void initSPIFFS() {
  if (!SPIFFS.begin(true)) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    ESP.restart();
  } else {
    delay(500);
    Serial.println("SPIFFS mounted successfully");
  }
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
    s->set_exposure_ctrl(s, 1);               // 0 = disable , 1 = enable
    s->set_aec2(s, 1);                        // 0 = disable , 1 = enable
    s->set_gain_ctrl(s, 1);                   // 0 = disable , 1 = enable
  }else{
    s->set_exposure_ctrl(s, 0);               // 0 = disable , 1 = enable
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
  s->set_reg(s,0xff,0xff,0x01);//banksel 
  s->set_reg(s,0x11,0xff,01);//frame rate
  s->set_reg(s,0xff,0xff,0x00);//banksel 
  s->set_reg(s,0x86,0xff,1);//disable effects
  s->set_reg(s,0xd3,0xff,5);//clock
  s->set_reg(s,0x42,0xff,0x4f);//image quality (lower is bad)
  s->set_reg(s,0x44,0xff,1);//quality
  delay(1200);
  Serial.println("Settings Adjusted");

  light = s->get_reg(s,0x2f,0xff);
  return light;
}

void standardAdjustExposure(int light){
  if(light<140)
    {
      //here we are in night mode
      if(light<45)s->set_reg(s,0x11,0xff,1);//frame rate (1 means longer exposure)
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

void expAdjustExposure(int light, int numLoops){
  // use randomness to select values in the array - if the light value is really high or low maybe cut out some values at the top or bottom so as to not overexposure
  // potentially not full randomness as this might make automating the image processing difficult
  // step up the array?
  // s[;ot i[ tje]]
  if(light<140)
    {
      //here we are in night mode
      if(light<45)s->set_reg(s,0x11,0xff,1);//frame rate (1 means longer exposure)
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
void testingAdjustExposure(int currentNum){
      int * hexArray = [0x0, 0x0a, 0x14, 0x1e, 0x28, 0x32, 0x3f] // 0, 10, 20, 30, 40, 50, 63
      s->set_reg(s,0x2d,0xff,0x0);//extra lines
      s->set_reg(s,0x2e,0xff,0x0);//extra lines
      s->set_reg(s,0x47,0xff,0x0);//Frame Length Adjustment MSBs  

      s->set_reg(s, 0x46, 0xff, 0xd0);//Frame Length Adjustment LSBs  - start with this consistent to see whats happening
      s->set_reg(s, 0x2a, 0xff, 0xff);//line adjust MSB - start with this as consistent so I can see whats happening
      s->set_reg(s, 0x2b, 0xff, 0xff); //line adjust
      s->set_reg(s, 0x45, 0xff, hexArray[currentNum]); //exposure (doesn't seem to work) 

      // String imageSaveString = "0x45-0xff-" + String(hexArray[currentNum]);
      // return imageSaveString;
       // s->set_reg(s,0x45, hexArray[currentNum], hexArray[currentNum]);
}

}

