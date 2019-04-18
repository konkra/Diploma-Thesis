
/* DONE:
  Record in EEPROM total milees,   DONE V.6.1
  Disconnect feature with logging to EEPROM, number of discconnection (address = 4) and
  duration of disconnections in seconds at addresses 5, 9, 13...type os uint_32_t
*/
//check disconnectons in OBD
/*   move trip file to FLASH => version 6.3
     when SD errors, contunue to work writting km to flash.
     avoid hard restart wehn awake when travel starts. awake GPS instead hot start. but wifi AP???
     GSM to Serial
     OBD to SPI
*/
//////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <stdlib.h>
#include <Wire.h>
#include <SPI.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Arduino.h>
#include <WiFiClientSecure.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <SD.h>
#include <WiFiUdp.h>
#include <TinyGPSPlus.h>
#include <Adafruit_GPS.h>
#include <SoftwareSerial.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>

bool debug = true;
bool SDerror = false;
bool Sleep = false;
bool DISCONNECT = false;
volatile bool interuptFlag = false;

uint8_t NUM_DISCONNECTIONS = 0;
uint32_t DISCONNECT_TIME;
uint32_t SleepTimer;
uint32_t Time_to_Sleep = 60000;
uint32_t TIME_TO_CONNECT = 10000;
uint32_t dataTime;
uint32_t dataSize=0;
float firmware_v = 6.6;
float total_distance = 0.0;
float Trip_distance = 0.0;
float temp_distance = 0.0;
//////////////////////////
float batt_level = 5.0;
float MIN_SPEED = 3.0;
/////////////////////////
static File sdfile;
static File record;
static File logfile;

#define MotionInterruptPin 3
#include "WiFiconnect.h"
#include "firm_update.h"
#include "MPU6050_init.h"
#include "UDPmessage.h"
#include "SDinit.h"
#include "SDconfig.h"
#include "GPSconfig.h"
#include "FTPhandling.h"
#include "WiFiserverUDP.h"
#include "Logfile_upload.h"
#include "SleepMode.h"
#include "BatteryMon.h"
extern "C" {
#include <user_interface.h>
}
char outstr[9];
bool upload = false;

TinyGPSPlus gps;
WiFiServer Webserver(80);



MpuSensorData data;

String ChipID; //String(ESP.getChipId());MpuSensorData data;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool open_file_for_write()
{
  uint32_t index = openFile();
  if (index) {
    if (debug == true) {
      Serial.print("FileID:"); Serial.print(index);
      Serial.print(", Chip ID: "); Serial.println((int)ESP.getChipId());
    }
    sdfile.print("File ID: "); sdfile.println(index);
    sdfile.print("Chip ID: "); sdfile.println((int)ESP.getChipId()); sdfile.flush();
    logfile.print("File ID: "); logfile.print(index);  logfile.print("/");
    logfile.println((int)ESP.getChipId()); logfile.flush();

    UDPtransmit("SD Initialized, File ID:" + String(index, DEC) + "\r\n");

    delay(100);
    return true;

    //flushFile();
  } else {
    if (debug == true) {
      Serial.println("Opening File error");
    }
    SDerror = true;
    delay(100); //digitalWrite(BUILTIN_LED, LOW);
    delay(100); //digitalWrite(BUILTIN_LED, HIGH);
    return false;
  }
}
//////////////////// END of bool open file ////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////setup ////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

void setup() {
  //delay(1000);
/*EEPROM.begin(512);
EEPROM.put(0,0);
EEPROM.put(40,1);
EEPROM.commit();
EEPROM.end();*/
pinMode(BUILTIN_LED, OUTPUT);
SDerror = false;
Sleep = false;
DISCONNECT = false;
upload = false;
Trip_distance = 0.0;
detachInterrupt(MotionInterruptPin);
dataSize=0;
upload = false; 
interuptFlag = false;
EEPROM.begin(512);
EEPROM.get(0,total_distance);
EEPROM.get(40,NUM_DISCONNECTIONS);


  
  if (debug == true) {
    Serial.begin(115200); Serial.println();
  }
  
  SD_init();
  
  if (battery_voltage() <= batt_level)
  {
    if (debug == true) {
      Serial.println("Device is disconnected");
    }
    
    logfile.println(); logfile.print("Device is disconnected");logfile.print("Bat:"); logfile.print((float)battery_voltage(), 1); logfile.print(",");
    NUM_DISCONNECTIONS = NUM_DISCONNECTIONS+1; EEPROM.put(40,NUM_DISCONNECTIONS);EEPROM.commit();
    logfile.print("Dis:"); logfile.print(NUM_DISCONNECTIONS);logfile.print(",");
    
    DISCONNECT == true;
    WiFi.mode(WIFI_OFF);
    WiFi.forceSleepBegin(); // = 17-19mA !!!
    delay(500);
  }
  
  dataTime = millis();
  while (battery_voltage() <= batt_level) {
    DISCONNECT_TIME = (millis() / 1000);
    delay(1000);
    if (debug == true) {
      Serial.print("Please connect device! Time elapsed: "); Serial.println(DISCONNECT_TIME);
    }
    if ((millis()-dataTime) >=900000)
    { dataTime = millis(); logfile.print((float)battery_voltage(), 1);logfile.print("15min/"); logfile.flush();
      }
    }
  if (battery_voltage() >= batt_level)
  {
    if (debug == true) {
      Serial.println("Device is connected"); }
    DISCONNECT == false;
  }


WiFi.forceSleepWake();
//Wire.begin(D3, D4);
  

   if (debug == true) {
   Serial.print("Total: "); Serial.print((float)total_distance, 3); Serial.print(",");
   Serial.print("Bat:"); Serial.print((float)battery_voltage(), 1); Serial.print(",");
   Serial.print("Dis:"); Serial.print(NUM_DISCONNECTIONS);   
   }
   logfile.print("Total:"); logfile.print((float)total_distance, 3); logfile.print(",");
   logfile.print("Dis:"); logfile.print(NUM_DISCONNECTIONS); logfile.print(",");
   logfile.print("Bat:"); logfile.print((float)battery_voltage(), 1); logfile.print(",");

GPS_init();
WiFi_init();
UDPinit();

  if (debug == true) {
    Serial.print(",Waiting..."); logfile.print(",Waiting...");
  }

  dataTime = millis();
  while ((millis() - dataTime) < TIME_TO_CONNECT)
  {
    if (wifi_softap_get_station_num() > 0)
    { 
      if (debug == true) {
        Serial.print("Client Connected.");
        logfile.print("Client Connected."); logfile.flush();
      }
      break;
    }
    delay(100);

   // if (gpsSerial.available() > 0)
    //{
     // gps.encode(gpsSerial.read());
   // }
    //if (gps.location.isUpdated())
    //{ break;
    //}

  }
//int packetSize = 0;
//packetBuffer = {};
  while (wifi_softap_get_station_num() > 0)
  {
   
   int packetSize = UDP.parsePacket();   
    if (packetSize) {
      outstr[9]={};
      int len = UDP.read(packetBuffer, 255);
      if (len > 0) packetBuffer[len] = 0;
      if (debug == true) {
        Serial.print(packetBuffer); Serial.print(","); Serial.print(UDP.remoteIP()); Serial.print(","); Serial.println(UDP.remotePort());
      }
    
      sprintf(outstr, "%s", packetBuffer);
      //Serial.println(outstr);  
    if (strcmp(outstr, "121") == 0)
    {
      logfile.println("Uploading...");  logfile.flush();
      UDP.beginPacket(UDP.remoteIP(), UDP.remotePort());
      UDP.print("Uploading....");
      UDP.endPacket();
      upload = true;
      break;
    }
    if (strcmp(outstr, "122") == 0)
    {
      logfile.println("Uploading...");  logfile.flush();//logfile.close();
      UDP.beginPacket(UDP.remoteIP(), UDP.remotePort());
      UDP.print("Uploading all....\r\n");
      UDP.endPacket();
      logfile_send();
      upload = true;
      break;
    }
     if (strcmp(outstr, "123") == 0)
    {
      logfile.println("Updating...");  logfile.flush();//logfile.close();
      UDP.beginPacket(UDP.remoteIP(), UDP.remotePort());
      UDP.print("Updating....");
      UDP.endPacket();
      firmware_update();
      break;
    }
    
     if (strcmp(outstr, "124") == 0)
    {
      UDP.beginPacket(UDP.remoteIP(), UDP.remotePort());
      UDP.print("Firmware Ver." + String(firmware_v,1) + "\r\n");
      UDP.endPacket();
    }    
     if (strcmp(outstr, "125") == 0)
    {
      UDP.beginPacket(UDP.remoteIP(), UDP.remotePort());
      UDP.print("Reseting km and files");
      UDP.endPacket();
      total_distance = 0;
      NUM_DISCONNECTIONS = 0;
      EEPROM.put(0,0);
      EEPROM.put(40,NUM_DISCONNECTIONS);
      EEPROM.commit();

      
    }       
    
    }  // if packet size   

  }

  //////////////////////////UPLOAD//////////////////////////////////////////
  if (upload == true)
  { uint32_t index3 = openFile_forFTP(1); uint32_t index4 = openFile_forFTP(2);

    if (index3 == index4) {
      if (debug == true) {
        Serial.println("Nothing to Transfer");
      }
    }
    if (index3 > index4)
    {
      if (debug == true) {
        Serial.print("Files to transfer: "); Serial.println(index3 - index4);
        //logfile.print("Files to transfer:");logfile.flush();
      }
    }
    if (index4 > index3)
    { index3 = index3 + 100000;
      if (debug == true) {
        Serial.print("More than 100kfiles");
      }
    }
    while (index3 - index4 > 0)
    {
      if (doFTP(index4 + 1, index3) == true) {
        if (debug == true) {
          Serial.println(F("FTP OK"));
        }
        sprintf(filename, "%s", FOLDER_NAME); sprintf(filename + strlen(filename), "%s", FILE_NAME_READ); record = SD.open(filename, FILE_WRITE);
        if (record)
        { record.seek(0); record.println(index4 + 1); record.close();
        }
        if (index4 == 99998)
        { index4 = 0; index3 = index3 - 100000;
        }
        else {
          index4 = index4 + 1;
        }
      } else {
        if (debug == true) {
          Serial.print("FTP Error, Retrying....");   delay(1000);
        }
        if (wifi_softap_get_station_num() > 0)    {
          delay(1000);
        } else {
          break;
        }
      }
    }
  }
  /////////////////////////////////// end of upload ///////////////////////////////////


  /////////////// continue setup after upload /////////////////
  if (open_file_for_write() == true)
  {  if (debug == true) {
    Serial.println("File opened ok");  
      }  
  }
  else {
    if (debug == true) {
      Serial.println("File Error");
    }
  }
  
/*      if (MPU6050_init() == true) {
      logfile.print("68/");
      if (debug == true) {
        Serial.print("68/");
      }
    }
    else {
      logfile.print("FF/");
      if (debug == true) {
        Serial.print("FF/");
      }
    }*/ 
  WiFi.mode(WIFI_OFF); WiFi.forceSleepBegin(); delay(500);  
  GPS_config();
    if (debug == false) {
    Serial.end();
  }
  SleepTimer = millis();
  
}    //SETUP END


void loop()
{

  if (battery_voltage() <= batt_level)
  {
    if (Sleep == true) {
      DISCONNECT = true; setup();
    }
    else {
      DISCONNECT = true; Sleep = true; close_files();
    }
  }
/*  if (interuptFlag == true)
  {
    interuptFlag = false;
    if (debug == true) {
      Serial.begin(115200);
      //val = MpuReadReg(INT_STATUS);  // see page 29 factory set to '1' = Data ready
      Serial.print("INT_STATUS : "); Serial.println(MpuReadReg(INT_STATUS), HEX);
      //val = MpuReadReg(0x61);  // see page 29 factory set to '1' = Data ready
      Serial.print("MOT_DETECT_STATUS  : "); Serial.println(MpuReadReg(0x61));
      Serial.flush();
      Serial.end();
    }
    delay(100);
    setup();
  }*/
  
    if (Sleep == true)
    {
      if (gps.speed.kmph() > MIN_SPEED)
      {    
        setup();
      }
  
    }
  /////////////////////////////////////////////////////////////////////////////////////
  if (Sleep == false)
  {
    while (gpsSerial.available() > 0)
    {
      gps.encode(gpsSerial.read());
      //delay(5);
    }
    if (gps.location.isUpdated())
    {
      digitalWrite(BUILTIN_LED, HIGH);

      if ((GPSfix == false) && (gps.date.value() != 0)) {
        GPSfixTime = millis();
        if (debug == true) {
          Serial.print((int)(GPSfixTime - GPSstartTime)/1000); Serial.print("/");
          Serial.print(gps.satellites.value()); Serial.print("/");
        }
        //logfile.print("GPS fix: ");
        logfile.print("Fix:");logfile.print((int)(GPSfixTime - GPSstartTime)/1000);  logfile.print("/");
        logfile.print("Sat:");logfile.print(gps.satellites.value()); logfile.print("/");
        /////////////////////////////////////////////////////////////////////////////////////////////////
        if (debug == true) {
          //Serial.print("Time:");
          if (gps.time.hour() >= 22) {
            Serial.print(gps.time.hour() - 22);
          }
          else {Serial.print(gps.time.hour() + 2);
          }     Serial.print(":");
          Serial.print(gps.time.minute()); Serial.print(":");Serial.print(gps.time.second()); Serial.print("/");

          //Serial.print("Date: ");
          //Serial.print(date);
          Serial.print(gps.date.day()); Serial.print("-"); Serial.print(gps.date.month()); Serial.print("-");Serial.println(gps.date.year());
        }

        //logfile.print("Time: ");
        if (gps.time.hour() >= 22) {
          logfile.print(gps.time.hour() - 22);
        }
        else {
          logfile.print(gps.time.hour() + 2);
        }  logfile.print(":");
        logfile.print(gps.time.minute()); logfile.print(":"); logfile.print(gps.time.second()); logfile.print("/");
        //logfile.print(", Date: ");
        logfile.print(gps.date.day()); logfile.print("-"); logfile.print(gps.date.month()); logfile.print("-");
        logfile.println(gps.date.year());logfile.flush(); logfile.close();
        ///////////////////////////////////////
        GPSfix = true;
      }

      if (GPSstrength == false)
      { if (gps.satellites.value() >= 4)
        { GPSstrength = true;
          last_gps_location_lat = gps.location.lat();
          last_gps_location_lng = gps.location.lng() ;
        }
      }

      if (gps.speed.kmph() > MIN_SPEED)
      {   
        
        SleepTimer = millis();

        //check if
        //sdfile.print((float)gps.location.lat(),6);
        dtostrf(gps.location.lat(), 9, 6, outstr);
        logData(outstr); sdfile.print("  ");
        dtostrf(gps.location.lng(),  9, 6, outstr);
        logData(outstr); sdfile.print("  ");
        logData(gps.satellites.value()); sdfile.print("  ");
        dtostrf(gps.speed.kmph(), 6, 2, outstr);
        logData(outstr); sdfile.print("  ");
        //logData(gps.date.value()); sdfile.print("  ");
        logData(gps.time.value()); sdfile.print("  ");
        /////////////////////////////////////////////////////////////////////////////////////
/*        MpuReadSensorData(&data);
        data.x_accel -= g_ax_base;
        data.y_accel -= g_ay_base;
        data.z_accel -= g_az_base;
        sdfile.print(data.x_accel); sdfile.print("  "); sdfile.print(data.y_accel); sdfile.print("  "); sdfile.print(data.z_accel); sdfile.print("  ");
*/
        if (GPSstrength == true)
        {
          temp_distance = gps.distanceBetween(last_gps_location_lat, last_gps_location_lng, gps.location.lat(), gps.location.lng()) / 1000.0;

          if (temp_distance < 200) {
            Trip_distance = Trip_distance + temp_distance;
            total_distance = total_distance + temp_distance;
            last_gps_location_lat = gps.location.lat();
            last_gps_location_lng = gps.location.lng() ;
          }
          else {
            temp_distance = 0.0;
          }
          //dtostrf(temp_distance,  5, 2, outstr);  logData(outstr); sdfile.print("  ");
          // dtostrf(Trip_distance,  7, 2, outstr); logData(outstr);
          sdfile.print((float)Trip_distance,3); //sdfile.print("  ");

        }
        //sdfile.print((float)battery_voltage(), 1);

        sdfile.println();
        flushData();

        if (debug == true) {
          Serial.print("LAT=");           Serial.print(gps.location.lat(), 6); Serial.print(",");
          Serial.print("LONG=");          Serial.print(gps.location.lng(), 6); Serial.print(",");
          Serial.print("Sat:");           Serial.print(gps.satellites.value()); Serial.print(",") ;
          Serial.print("Speed:");         Serial.print(gps.speed.kmph()); Serial.print(",") ;
          //Serial.print("Date:");
          //Serial.print(gps.date.value());
          //Serial.print(gps.date.day());Serial.print("/"); Serial.print(gps.date.month()); Serial.print("/"); Serial.print(gps.date.year()); Serial.print(",") ;
          Serial.print("Time:");         Serial.print(gps.time.value()); Serial.print(",") ;
          //Serial.print("TempDist:");   Serial.print(temp_distance, 6); Serial.print(",") ;
          Serial.print("SumDist:");      Serial.print((float)Trip_distance, 3); Serial.print(",") ;
         // Serial.print("AccelX.Y.Z:");   Serial.print(data.x_accel);  Serial.print(","); Serial.print(data.y_accel); Serial.print(","); Serial.print(data.z_accel); Serial.print(",");
          Serial.print("Batt:");         Serial.println((float)battery_voltage(), 1);
        }
      }
      // if gps <3km
      else {
        if ((millis() - SleepTimer) >= Time_to_Sleep)
        { Sleep = true;
          close_files();
        }
      }
    }  //if gps updated!
    else {
      if ((millis() - SleepTimer) >= Time_to_Sleep)
      { Sleep = true;
        close_files();
      }
    }
  } //end of if !sleep
  delay(10);
  digitalWrite(BUILTIN_LED, LOW);
 
} // end of loop





///////////////////////////////////////////////////////////////////////////////////////////
void close_files()
{
EEPROM.put(0,total_distance);
EEPROM.commit();
EEPROM.end();
  ////////////////////////////////////////////////
  if (debug == true) {
    if (gps.time.hour() >= 22) {
      Serial.print(gps.time.hour() - 22);
    }
    else {
      Serial.print(gps.time.hour() + 2);
    } Serial.print(":"); Serial.print(gps.time.minute()); Serial.print(":"); Serial.print(gps.time.second()); Serial.print("/"); 
    //Serial.print("Date:");    //Serial.print(date);
    Serial.print(gps.date.day()); Serial.print("-"); Serial.print(gps.date.month()); Serial.print("-"); Serial.print(gps.date.year()); Serial.print("/");
    //Serial.print("Distance:");
    Serial.print((float)Trip_distance, 3); Serial.print("/"); Serial.println("End of Trip");
    if (DISCONNECT == true) {
      Serial.println("Device disconnected");
    }
    if (Sleep == true) {
      Serial.println("Sleep enabled");
    }
  }
  ////////////////////////////////////////////////
  sdfile.println("Trip Ends");
  if (DISCONNECT == true) {
    sdfile.println("Device disconnected");
  }
  sdfile.close();
  ////////////////////////////////////////////////
  sprintf(filename, "%s", FOLDER_NAME);
  sprintf(filename + strlen(filename), "%s", FILE_NAME_LOGFILE);
  logfile = SD.open(filename, FILE_WRITE);
  //logfile.print("Time: ");
  if (gps.time.hour() >= 22) { logfile.print(gps.time.hour() - 22); } else {logfile.print(gps.time.hour() + 2); } 
  logfile.print(":");   logfile.print(gps.time.minute()); logfile.print(":"); logfile.print(gps.time.second()); logfile.print("/");
  logfile.print(gps.date.day()); logfile.print("-"); logfile.print(gps.date.month()); logfile.print("-"); logfile.print(gps.date.year()); logfile.print("/");
  logfile.print("Distance:"); logfile.println((float)Trip_distance, 3); logfile.println("Trip Ends");
  
  if (DISCONNECT == true) {
    logfile.println("Device disconnected");
  }
  logfile.close();
  Sleep_mode();
}

//loop

