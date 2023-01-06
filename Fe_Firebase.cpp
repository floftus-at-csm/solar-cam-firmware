#include "Fe_Firebase.hpp"
#include "addons/TokenHelper.h"

namespace Fe_Firebase {

// Insert Firebase project API Key
#define API_KEY "AIzaSyCaIcgv017fvFyiGoaXIsfVKeE5WTB_1i0"

// Insert Authorized Email and Corresponding Password
#define USER_EMAIL "floftus@hotmail.co.uk"
#define USER_PASSWORD "dragonshield"

// Insert Firebase storage bucket ID e.g bucket-name.appspot.com
#define STORAGE_BUCKET_ID "solar-cam-9cb5c.appspot.com"

#define FIREBASE_PROJECT_ID "solar-cam-9cb5c"

//Define Firebase Data objects
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig configF;

bool taskCompleted = false;

settingsInput fromWebSettings;

void initialize() {
  //Firebase
  // Assign the api key
  configF.api_key = API_KEY;
  //Assign the user sign in credentials
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;
  //Assign the callback function for the long running token generation task
  configF.token_status_callback = tokenStatusCallback;  //see addons/TokenHelper.h
  fbdo.setResponseSize(2048);
  Firebase.begin(&configF, &auth);
  Firebase.reconnectWiFi(true);
}

settingsInput getSettings() {
  if (Firebase.ready()) {
    String documentPath = "Fe-cam1/settings";
    String mask = "current";
    if (Firebase.Firestore.getDocument(&fbdo, FIREBASE_PROJECT_ID, "", documentPath.c_str(), mask.c_str())) {
      Serial.printf("ok\n%s\n\n", fbdo.payload().c_str());
      FirebaseJson payload;
      payload.setJsonData(fbdo.payload().c_str());
      getArrayFromFirestore(payload, "fields/current/arrayValue/values");
      return fromWebSettings;
    } else {
      Serial.println(fbdo.errorReason());
    }
  }
}
void getArrayFromFirestore (FirebaseJson startingJson, String pathToArray) {
  FirebaseJsonData jsonData;
  startingJson.get(jsonData, pathToArray, true);
  FirebaseJsonArray arr;

  // //Get array data
  jsonData.get<FirebaseJsonArray>(arr);
  // String outputArray[arr.size()];
  Serial.print("The size of the array is: ");
  Serial.println(arr.size());

  Serial.println("the array data is: ");
  FirebaseJsonData tempJson;

  arr.get(tempJson, "/[0]/integerValue");
  Serial.print("the brightness is: ");
  Serial.println(tempJson.to<int>());
  fromWebSettings.brightness = tempJson.to<int>();
  tempJson.clear();
  
  arr.get(tempJson, "/[1]/integerValue");  
  Serial.print("the contrast is: ");
  Serial.println(tempJson.to<int>());
  fromWebSettings.contrast = tempJson.to<int>();
  tempJson.clear();
  
  arr.get(tempJson, "/[2]/integerValue");
  Serial.print("the saturation is: ");
  Serial.println(tempJson.to<int>());
  fromWebSettings.saturation = tempJson.to<int>();
  tempJson.clear();

  arr.get(tempJson, "/[3]/integerValue");
  Serial.print("the autoExposureControl is: ");
  Serial.println(tempJson.to<int>());
  fromWebSettings.autoExposureControl = tempJson.to<int>();  // 0 - 1600
  tempJson.clear();
  
  arr.get(tempJson, "/[4]/integerValue");
  Serial.print("the white balance mode is: ");
  Serial.println(tempJson.to<int>());
  fromWebSettings.whiteBalance = tempJson.to<int>();
  tempJson.clear();

  arr.get(tempJson,"/[5]/stringValue");
  Serial.print("the imaging mode is: ");
  Serial.println(tempJson.to<String>());
  fromWebSettings.mode = tempJson.to<String>();
  tempJson.clear();

  arr.get(tempJson, "/[6]/integerValue");
  Serial.print("the number of photos is: ");
  Serial.println(tempJson.to<int>());
  fromWebSettings.numPhotos = tempJson.to<int>();
  tempJson.clear();

  arr.get(tempJson, "/[7]/integerValue");
    Serial.print("the number of cameras is: ");
  Serial.println(tempJson.to<int>());
  fromWebSettings.numCamera = tempJson.to<int>();
  tempJson.clear();
  
  arr.get(tempJson, "/[8]/integerValue");
  Serial.print("the layer val: ");
  Serial.println(tempJson.to<int>());
  fromWebSettings.layerVal = tempJson.to<int>();
  tempJson.clear();

  arr.get(tempJson, "/[9]/integerValue");
  Serial.print("the sleep period in Millis is: ");
  Serial.println(tempJson.to<int>());
  fromWebSettings.sleepPeriod = tempJson.to<int>();
  tempJson.clear();

  arr.get(tempJson, "/[10]/stringValue");
  Serial.print("the automode is: ");
  Serial.println(tempJson.to<String>());
  fromWebSettings.autoMode = tempJson.to<String>();
  tempJson.clear();


}
int checkIntVal(String fieldVal){
  Serial.println("Checking the read value");
  if(Firebase.ready()){
    String documentPath = "Fe-cam1/settings";
    // String mask = "read";
    String mask = fieldVal;
    if (Firebase.Firestore.getDocument(&fbdo, FIREBASE_PROJECT_ID, "", documentPath.c_str(), mask.c_str())) {
      FirebaseJson payload;
      Serial.println(fbdo.payload().c_str());
      payload.setJsonData(fbdo.payload().c_str());
      FirebaseJsonData jsonData;
      payload.get(jsonData, "fields/read/integerValue", true);
      Serial.println("the read value is: ");
      Serial.println(jsonData.to<int>());
      // Serial.println(jsonData.intValue);
      return jsonData.to<int>();
    }

  }
}

void writeVal(String fieldPath, int newValue){
  Serial.println("writing value");
  if(Firebase.ready()){
    // v2
    // currently this is completely reseting the document - rather than just updating one value
    // The dyamic array of write object fb_esp_firestore_document_write_t.
    std::vector<struct fb_esp_firestore_document_write_t> writes;
    struct fb_esp_firestore_document_write_t update_write;
    update_write.type = fb_esp_firestore_document_write_type_transform; // fb_esp_firestore_document_write_type_update, fb_esp_firestore_document_write_type_transform
    FirebaseJson content;
    String documentPath = "Fe-cam1/settings";
    String combinedFieldPath = "fields/" + fieldPath + "/integerValue";
    content.set(combinedFieldPath, newValue);

    // if (Firebase.Firestore.commitDocument(&fbdo, FIREBASE_PROJECT_ID, "" /* databaseId can be (default) or empty */, writes /* dynamic array of fb_esp_firestore_document_write_t */, "" /* transaction */))
    taskCompleted = false;
    if (Firebase.Firestore.patchDocument(&fbdo, FIREBASE_PROJECT_ID, "" /* databaseId can be (default) or empty */, documentPath.c_str(), content.raw(), fieldPath /* updateMask */)) 
          Serial.printf("ok\n%s\n\n", fbdo.payload().c_str());
    else
          Serial.println(fbdo.errorReason()); 
  }
}

void uploadFromSPIFFS(String FILE_PHOTO) {
  if (Firebase.ready() && !taskCompleted) {
    taskCompleted = true;
    Serial.print("Uploading picture... ");

    //MIME type should be valid to avoid the download problem.
    //The file systems for flash and SD/SDMMC can be changed in FirebaseFS.h.
    if (Firebase.Storage.upload(&fbdo, STORAGE_BUCKET_ID /* Firebase Storage bucket id */, FILE_PHOTO /* path to local file */, mem_storage_type_flash /* memory storage type, mem_storage_type_flash and mem_storage_type_sd */, FILE_PHOTO /* path of remote file stored in the bucket */, "image/jpeg" /* mime type */)) {
      Serial.printf("\nDownload URL: %s\n", fbdo.downloadURL().c_str());
    } else {
      Serial.println(fbdo.errorReason());
    }
  }
}
}