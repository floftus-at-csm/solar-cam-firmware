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
  Serial.println("the JSON array is: ");

  Serial.println("the array data is: ");
  FirebaseJsonData tempJson;
  arr.get(tempJson, "/[0]/stringValue");
  fromWebSettings.brightness = tempJson.to<int>();
  tempJson.clear();
  // FirebaseJsonData tempJson;
  arr.get(tempJson, "/[1]/stringValue");  
  fromWebSettings.contrast = tempJson.to<int>();
  tempJson.clear();
  // FirebaseJsonData tempJson;
  arr.get(tempJson, "/[2]/stringValue");
  fromWebSettings.saturation = tempJson.to<int>();
  tempJson.clear();
    // FirebaseJsonData tempJson;
  arr.get(tempJson, "/[3]/stringValue");
  fromWebSettings.autoExposureControl = tempJson.to<int>();  // 0 - 1600
  tempJson.clear();
    // FirebaseJsonData tempJson;
  arr.get(tempJson, "/[4]/stringValue");
  fromWebSettings.mode = tempJson.to<String>();
  tempJson.clear();
    // FirebaseJsonData tempJson;
  arr.get(tempJson, "/[5]/stringValue");
  fromWebSettings.numPhotos = tempJson.to<int>();
  tempJson.clear();
    // FirebaseJsonData tempJson;
  arr.get(tempJson, "/[6]/stringValue");
  fromWebSettings.numCamera = tempJson.to<int>();
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
      // The dyamic array of write object fb_esp_firestore_document_write_t.
      std::vector<struct fb_esp_firestore_document_write_t> writes;

      // A write object that will be written to the document.
      struct fb_esp_firestore_document_write_t transform_write;

      transform_write.type = fb_esp_firestore_document_write_type_transform;
      // Set the document path of document to write (transform)
      transform_write.document_transform.transform_document_path = "Fe-cam1/settings";

      // Set a transformation of a field of the document.
      struct fb_esp_firestore_document_write_field_transforms_t field_transforms;
      // Set field path to write.
      field_transforms.fieldPath = "read";
      // Set the transformation type.
      // fb_esp_firestore_transform_type_set_to_server_value,
      // fb_esp_firestore_transform_type_increment,
      // fb_esp_firestore_transform_type_maaximum,
      // fb_esp_firestore_transform_type_minimum,
      // fb_esp_firestore_transform_type_append_missing_elements,
      // fb_esp_firestore_transform_type_remove_all_from_array
      field_transforms.transform_type = fb_esp_firestore_transform_type_set_to_server_value;
      
      // Set the transformation content, server value for this case.
      // See https://firebase.google.com/docs/firestore/reference/rest/v1/Write#servervalue
      field_transforms.transform_content = 0; // set timestamp to "test_collection/test_document/server_time"

      // Add a field transformation object to a write object.
      transform_write.document_transform.field_transforms.push_back(field_transforms);

      // Add a write object to a write array.
      writes.push_back(transform_write);

      if (Firebase.Firestore.commitDocument(&fbdo, FIREBASE_PROJECT_ID, "" /* databaseId can be (default) or empty */, writes /* dynamic array of fb_esp_firestore_document_write_t */, "" /* transaction */))
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