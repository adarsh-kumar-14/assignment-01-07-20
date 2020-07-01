/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    Adarsh Kumar
    01 July 2020 07:30PM

When the system starts the latest data is downloaded from the API. 
The data is stored in non-volatile storage like SD card.
In case the API fails, the system uses the old stored persistent data.
Tasks:
  a. When anyone types a key from the above API e.g. "HYBpDQVhoCni2wuyCT" the console replies with the value e.g. 56
  b. Printing the query time for the above e.g. “Time for query: 50msec”
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* ******************************************************************************
  Connect the SD card to the following pins:

   SD Card | ESP32
     D2       -
     D3       SS
     CMD      MOSI
     VSS      GND
     VDD      3.3V
     CLK      SCK
     VSS      GND
     D0       MISO
     D1       -
******************************************************************************** */

// // // // // // // // // //      HEADER FILES      // // // // // // // // // //
#include <WiFi.h>
#include <HTTPClient.h>
#include <Arduino_JSON.h>
#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include <Regexp.h>

// // // // // // // // // //  Declaring Constants  // // // // // // // // // //
const char* ssid     = "Enter your SSID";
const char* password = "Enter Password";
const char* host = "http://demo4657392.mockable.io/list-tag-ids";
String jsonBuffer;
JSONVar valueKeyPair;
char* value;
//////////////////////////////////////////////////////////////////////////////////
void appendFile(fs::FS &fs, const char * path, JSONVar message){
    Serial.printf("Appending to file: %s\n", path);
    File file = fs.open(path, FILE_APPEND);
    if(!file){
        Serial.println("Failed to open file for appending");
        return;
    }
    if(file.print(message)){
        Serial.println("Message appended");
    } else {
        Serial.println("Append failed");
    }
    file.close();
}
//////////////////////////////////////////////////////////////////////////////////
void renameFile(fs::FS &fs, const char * path1, const char * path2){
    Serial.printf("Renaming file %s to %s\n", path1, path2);
    if (fs.rename(path1, path2)) {
        Serial.println("File renamed");
    } else {
        Serial.println("Rename failed");
    }
}
//////////////////////////////////////////////////////////////////////////////////
void deleteFile(fs::FS &fs, const char * path){
    Serial.printf("Deleting file: %s\n", path);
    if(fs.remove(path)){
        Serial.println("File deleted");
    } else {
        Serial.println("Delete failed");
    }
}
//////////////////////////////////////////////////////////////////////////////////
void findValue(fs::FS &fs, const char * path, JSONVar message){
    File file = fs.open(path);
    if(!file){
        Serial.println("Failed to open file for reading");
        return;
    }
    while(file.available()){
        MatchState ms;
        ms.Target(file.read());            // where to search
        char result = ms.Match(message);   // look for message
    }
    file.close();
    return result;
} 
//////////////////////////////////////////////////////////////////////////////////
void setup()
{
    // Serial Connection
    Serial.begin(115200);
    while (!Serial){ 
     continue;
    }

    
    // Connecting to WiFi Network
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED){
     continue;
    }
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());


    // Mounting SD Card
    if(!SD.begin()){
     Serial.println("Card Mount Failed");
     return;
    }
    uint8_t cardType = SD.cardType();
    if(cardType == CARD_NONE){
     Serial.println("No SD card attached");
     return;
    }
    Serial.print("SD Card Type: ");
    if(cardType == CARD_MMC){
     Serial.println("MMC");
    } 
    else if(cardType == CARD_SD){
     Serial.println("SDSC");
    } 
    else if(cardType == CARD_SDHC){
     Serial.println("SDHC");
    }
    else {
     Serial.println("UNKNOWN");
    }

 
    //Connecting to API
    if(WiFi.status()== WL_CONNECTED){
      String serverPath = host;
      
      HTTPClient http;
      http.begin(serverPath.c_str());
      jsonBuffer = http.getString();
      JSONVar myArray = JSON.parse(jsonBuffer);
      
      if (JSON.typeof(myArray) == "undefined") {
       Serial.println("Parsing input failed!");
       return;
      }
            
      for (int i=0; i<myArray.length(); i++){
       // myObject.hasOwnProperty(key) checks if the object contains an entry for key
       valueKeyPair = myArray[i];
       appendFile(SD, "/data_1.txt", valueKeyPair );
       
       //if (myObject.hasOwnProperty("i")){               // It means at place 0 of list, look for 1 and so on
        //valueKeyPair = myObject("i") ;
        //appendFile(SD, "/data_1.txt", valueKeyPair );
       }
      deleteFile(SD, "/data.txt");
      renameFile(SD, "/data_1.txt", "/data.txt");
    }
    else {
      Serial.println("WiFi Disconnected");
    }

}
//////////////////////////////////////////////////////////////////////////////////
void loop()
{
    // Input data
    char userEnteredKey[20];
    Serial.println("Enter the Value of Key :");
    Serial.readBytes(userEnteredKey,20);

    
    // For Query Time
    uint32_t start = millis();
    uint32_t end = start;

    //Finding the Value
    start = millis();
    value = findFile(SD, "/data.txt", userEnteredKey );
    end = millis() - start;
    Serial.printf("Key:");
    Serial.printf(value);
    Serial.printf("Time for Query: %u msec\n", end);
}
