/////////////////////////////////////////////////////////////////////////
#define PMTK_SET_NMEA_UPDATE_HALF_HZ "$PMTK220,2000*1C\r\n" // Every 2000ms (0.5Hz)
#define PMTK_SET_NMEA_UPDATE_1HZ      "$PMTK220,1000*1F\r\n" // Every 1000ms (1Hz)
#define PMTK_SET_NMEA_UPDATE_2HZ      "$PMTK220,500*2B\r\n>" // Every 500ms (2Hz)
#define PMTK_SET_NMEA_UPDATE_5HZ      "$PMTK220,200*2C\r\n"  // Every 200ms (5Hz)
#define PMTK_SET_NMEA_OUTPUT_BAUDRATE_14400   "$PMTK251,14400*29\r\n"
#define PMTK_SET_NMEA_OUTPUT_BAUDRATE_38400 "$PMTK251,38400*27\r\n" //SET TO 38400
#define PMTK_SET_NMEA_OUTPUT_BAUDRATE_115200 "$PMTK251,115200*1F\r\n" //SET TO 38400
#define PMTK_SET_NMEA_OUTPUT_BAUDRATE_DEFAULT   "$PMTK251,0*28\r\n" //SET TO DEFAULT
/* If the command is correct and executed, GPS module will output message $PMTK001,220,3*30<CR><LF>*/
#define PMTK_Set_NMEA_OUTPUT_GLL       "$PMTK314,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29\r\n" // turn on only GLL - Geographic position, latitude / longitude
#define PMTK_SET_NMEA_OUTPUT_RMC       "$PMTK314,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29\r\n" // turn on only RMC - Recommended minimum specific Loran-C data
#define PMTK_SET_NEMA_OUTPUT_VTG       "$PMTK314,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29\r\n" // turn on only VTG - Track made good and ground speed
#define PMTK_SET_NEMA_OUTPUT_GGA       "$PMTK314,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29\r\n" // turn on only GGA - Global Positioning System Fix Data
#define PMTK_SET_NMEA_OUTPUT_GSA       "$PMTK314,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29\r\n" // turn on only GSA - GPS DOP and active satellites
#define PMTK_SET_NMEA_OUTPUT_GSV       "$PMTK314,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0*29\r\n" // turn on only GSV - GPS Satellites in view
#define PMTK_SET_NEMA_OUTPUT_ZDA       "$PMTK314,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0*29\r\n" // turn on only ZDA - Date & Time - UTC, day, month, year, and local time zone.
#define PMTK_SET_NEMA_OUTPUT_RMCGAAGSA "$PMTK314,0,1,0,1,1,5,0,0,0,0,0,0,0,0,0,0,0,0,0*2C\r\n" // RMC, GGA, GSA at 1Hz and GSV at 0.2Hz
#define PMTK_SET_NMEA_OUTPUT_ALLDATA   "$PMTK314,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0*28\r\n" // turn on ALL THE DATA for the LS20031- except ZDA
#define PMTK_API_Q_NMEA_OUTPUT         "$PMTK414*33\r\n"                                      //Query current NMEA sentence output frequencies
#define PMTK_CMD_EASY_E                "$PMTK869,0*29\r\n""           //Query for EASY
#define PMTK_CMD_EASY_ENABLE           "$PMTK869, 1, 1 * 35\r\n"" //Enable EASY

/* If the command is correct and executed, GPS module will output message "$PMTK001,314,3*36<CR><LF>" See Flag upabove */
#define PMTK_ACK                        "$PMTK001,604,3*32\r\n"
#define PMTK_CMD_STANDBY_MODE           "$PMTK161,0*28\r\n"
#define PMTK_Q_RELEASE                  "$PMTK605*31\r\n"
/* Query FW release information -- MTK-3301s send firmware release name and version - Note: not sure of accuracy of this information */
/* if the command is correct and executed, GPS module will output message example >> "$PMTK705,AXN_1.30,29BF,MC-1513,*0E"
   Data field >> "$PMTK705,ReleaseStr,Mod eID,,*0E"
   ReleaseSTr: Firmware release name & version
   ModelID: Model ID
*/
#define  PMTK_CMD_WARM_START            "$PMTK102*31\r\n"
#define PMTK_TEST                       "$PMTK000*32\r\n"

#define PMTK_SET_NMEA_OUTPUT_OFF       "$PMTK314,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*28\r\n" // turn off output
#define PMTK_SET_NMEA_OUTPUT_HOT_START  "$PMTK101*32\r\n"
#define PMTK_SET_Nav_Speed             "$PMTK386,0*23\r\n"  //Set Nav threshold to 0
#define PMTK_SET_NMEA_UPDATE_10HZ     "$PMTK220,100*2F\r\n"  // Every 100ms (10Hz)
#define PMTK_SET_NMEA_OUTPUT_RMCGGA    "$PMTK314,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*28\r\n" // turn on GPRMC and GPGGA ----- TinyGPS only uses these two, all others can be turned off




///////////////////////////////////////////////////////////////////////
#define GPS_TX D2 //->  RXD 
#define GPS_RX D1  // -> TXD
SoftwareSerial gpsSerial(GPS_RX, GPS_TX); // RX, TX

bool GPSfix = false;
bool GPSstrength = false;
unsigned long GPSstartTime;
unsigned long GPSfixTime;
bool GPS10Hz = false;
//uint32_t last_gpsTime = 0;
uint32_t gpsTime = 0;
//float deltaTime = 0.0;
double last_gps_location_lat = 0.0;
double last_gps_location_lng = 0.0;


 
void GPS_init()
{

 GPSfix = false;
 GPSstrength = false;
 GPSstartTime;
 GPSfixTime;
 GPS10Hz = false;
 gpsTime = 0;
 last_gps_location_lat = 0.0;
 last_gps_location_lng = 0.0;


  
  gpsSerial.begin(9600);  
  delay(100);

  gpsSerial.print(PMTK_SET_NMEA_OUTPUT_OFF);
  gpsSerial.flush();
  delay(100);


  for (;;)  {
    //if (gpsSerial.available()) {
      char c = gpsSerial.read();
      if (c == '*') {
        if (debug == true) {
        Serial.print(">");}
        logfile.print(">");
        break;
      }
    //}
      delay(1);
  }



  for (;;)       {
    if (gpsSerial.available()) {
      char c = gpsSerial.read();
      if (c == '\r') {
        if (debug == true) {
          Serial.print("/"); 
        }
        logfile.print("/");
        logfile.flush();
        // prompt char received
        break;
      } else {
        if (debug == true) {
          Serial.print(c);
        }
        logfile.print(c);        
      }
    }
    //delay(1);
  }
//Serial.println();
  
  
  gpsSerial.print(PMTK_SET_NMEA_OUTPUT_HOT_START);
  gpsSerial.flush();
  delay(100);

    for (;;)  {
    //if (gpsSerial.available()) {
      char c = gpsSerial.read();
      if (c == '*') {
        if (debug == true) {
        Serial.print(">");}
        logfile.print(">");
        break;
      }
    //}
      delay(1);
  }

  
  for (;;)       {
    if (gpsSerial.available()) {
      char c = gpsSerial.read();
      if (c == '\r') {
        if (debug == true) {
          Serial.print("/");
        }
        logfile.println("/");
        logfile.flush();
        // prompt char received
        break;
      } else {
        if (debug == true) {
          Serial.print(c);

        }
        //logData(c);
        logfile.print(c);
        
      }
    }
  }
  GPSstartTime = millis();
   if (debug == true) {
  Serial.println();}
  //sdfile.println();
//Serial.println();
}
//init



bool GPS_config()
{
  ///////////////////////////////////////////////////////////
  gpsSerial.print(PMTK_SET_Nav_Speed);
  gpsSerial.flush();
  delay(50);
  for (;;)  {
    //if (gpsSerial.available()) {
      char c = gpsSerial.read();
      if (c == '*') {
         if (debug == true) {
        Serial.print(">");}
        logfile.print(">");
        break;
      }
    //}
      delay(1);
  }

  
  for (;;)       {
    if (gpsSerial.available()) {
      char c = gpsSerial.read();
      if (c == '\r') {
        if (debug == true) {
          Serial.print("/");
        }
        logfile.print("/"); //logfile.flush();
        // prompt char received
        break;
      } else {
        if (debug == true) {
          Serial.print(c);
        }
        //logData(c);
        logfile.print(c);
        
        //delay(10);
      }
    }
  }
  //sdfile.println();
//Serial.println();

  ///////////////////////////////////////////////////////////
  gpsSerial.print(PMTK_SET_NMEA_UPDATE_10HZ);
  gpsSerial.flush();
  delay(50);
  String   GPSdata = "";

  for (;;)       {
    if (gpsSerial.available()) {
      char c = gpsSerial.read();
      if (c == '\r') {    // prompt char received
        if (debug == true) {
          Serial.print(">10Hz"); 
        }
        logfile.print(">10Hz"); //logfile.print(","); logfile.flush();
        break;
      } else {
        if (debug == true) {
          //Serial.print(c);
        }
        //logData(c);
        //logfile.print(c);
        
        GPSdata += c;
      }
    }
  }
  //sdfile.println();
  GPSdata += '\0';
//Serial.println();
//Serial.println(GPSdata);


  if (GPSdata.substring(0) == "$PMTK001,220,3*30")
  {
      if (debug == true) {
        Serial.print("-ok/"); 
      }
      logfile.print("-ok/");
    GPS10Hz = true;
  }

//Serial.println();
  //////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////
  gpsSerial.print(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  gpsSerial.flush();
  delay(50);
  GPSdata = "";
  for (;;)       {
    if (gpsSerial.available()) {
      char c = gpsSerial.read();
      if (c == '\r') {         // prompt char received
        if (debug == true) {
          Serial.print(">ON"); 
        }
        logfile.print(">ON"); //logfile.println(); logfile.flush();
        break;
      } else {
        if (debug == true) {
          //Serial.print(c);
        }
        //logData(c);
        //logfile.print(c);
        
        GPSdata += c;
      }
    }
  }

GPSdata += '\0';
//Serial.println();
//Serial.println(GPSdata);

  if (GPSdata.substring(0) == "$PMTK001,220,3*30")
  {
     if (debug == true) {
        Serial.print("-ok/");
      }
    GPS10Hz = true;
    logfile.print("-ok/"); 
  }

  delay(100); //digitalWrite(BUILTIN_LED, LOW); delay(100); digitalWrite(BUILTIN_LED, HIGH);

  if (GPS10Hz == false)
  {
    if (debug == true) {
   Serial.println("reseting..."); }
   logfile.println("reseting..."); logfile.flush(); logfile.close(); setup();
  }
  else {
     if (debug == true) {
    Serial.println("-ok/"); }
    logfile.println("-ok/");  logfile.flush();
    }
  return true;
}
//////////////// end of GPS init ///////////////////////////////////////


