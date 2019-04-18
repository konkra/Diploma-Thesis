Sd2Card card;
SdVolume volume;
SdFile root;// set up variables using the SD utility library functions:
const int chipSelect = D8;




char filename[50];
#define FOLDER_NAME "/CRLOGGER"
#define FILE_NAME_WRITE  "/INFOW"
#define FILE_NAME_READ  "/INFOR"
#define FILE_NAME_LOGFILE  "/logfile.txt"
#define FILE_NAME_MILES  "/miles.txt"

uint32_t lastFileSize = 0;
uint16_t fileIndex = 0;



bool SD_init()
{
lastFileSize = 0;
fileIndex = 0;

  SD.begin(chipSelect);

  if (debug == true) {
    Serial.print("Initializing SD card...");
  }
  if (!card.init(SPI_HALF_SPEED, chipSelect))  // SPI speed to downgraded from SPI_HALF_SPEED to quuarter
  {
    Serial.println("initialization failed.");
    return false;
    SDerror = true;
    delay(100); //digitalWrite(BUILTIN_LED, LOW);
    delay(100); //digitalWrite(BUILTIN_LED, HIGH);
  }
  else
  {
    if (debug == true) {
      Serial.println("Wiring is correct and a card is present.");
    }
  }
  // print the type of card
  if (debug == true) {
    Serial.print("Card type: ");

    switch (card.type()) {
      case SD_CARD_TYPE_SD1:
        Serial.print("SD1");
        break;
      case SD_CARD_TYPE_SD2:
        Serial.print("SD2");
        break;
      case SD_CARD_TYPE_SDHC:
        Serial.print("SDHC");
        break;
      default:
        Serial.print("Unknown");
    }
  }

  // Now we will try to open the 'volume'/'partition' - it should be FAT16 or FAT32
  if (!volume.init(card)) {
    if (debug == true) {
      Serial.println("Could not find FAT16/FAT32 partition.\nMake sure you've formatted the card");
    }
    delay(100); //digitalWrite(BUILTIN_LED, LOW);
    delay(100); //digitalWrite(BUILTIN_LED, HIGH);
    SDerror = true;
    return false;
  }
  else {
    // print the type and size of the first FAT-type volume
    uint32_t volumesize;
    if (debug == true) {
      Serial.print(", Volume type is FAT");
      Serial.println(volume.fatType(), DEC);
    }
    delay(100); //digitalWrite(BUILTIN_LED, LOW);
    delay(100); //digitalWrite(BUILTIN_LED, HIGH);
    delay(100); //digitalWrite(BUILTIN_LED, LOW);
    delay(100); //digitalWrite(BUILTIN_LED, HIGH);
    ////////////////////////////////////////////////////////////////////


    //////////////////////////////////////////
    sprintf(filename, "%s", FILE_NAME_MILES);
    if (debug == true) {
      Serial.print("Checking: "); Serial.print(filename);
    }

    if (!SD.exists(filename))
    { if (debug == true) {
        Serial.print("....Miles file does not exist.");
      }
      record = SD.open(filename, FILE_WRITE);
      record.close();

      if (SD.exists(filename) == true) {
        if (debug == true) {
          Serial.print("...");  Serial.println("Miles created");
        }
      }
    }

    else {
      if (debug == true) {
        Serial.println("...Miles file exists");
      }
    }
    ///////////////////Folder and file init IF DO NOT EXIST////////////////////////////////////////////////
    sprintf(filename, "%s", FOLDER_NAME);
    if (debug == true) {
      Serial.print("Checking: "); Serial.print(filename);
    }
    if (!SD.exists(filename))
    {
      SD.mkdir(filename);
      if (debug == true) {
        Serial.print("....Folder does not exist");
      }
      if (SD.exists(filename) == true) {
        if (debug == true) {
          Serial.print("...  ");  Serial.println("Folder created");
        }
        logfile.print("Folder created,");
      }
      else {
        if (debug == true) {
          Serial.print("...  ");  Serial.println("Folder not created");
          SDerror = true;
        }
      }
    }
    else {
      if (debug == true) {
        Serial.println("......../CRLOGGER Folder exists");
      }
      //logfile.print("Folder,");
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    sprintf(filename, "%s", FOLDER_NAME);
    sprintf(filename + strlen(filename), "%s", FILE_NAME_LOGFILE);
    if (debug == true) {
      Serial.print("Checking:");
      Serial.print(filename);
    }
    if (!SD.exists(filename))
    {
      if (debug == true) {
        Serial.print("....logfile file does not exist");
      }
      record = SD.open(filename, FILE_WRITE);
      record.println("Log file created");
      record.println("///////////////////////////////");
      record.close();
      if (SD.exists(filename))
      {
        if (debug == true) {
          Serial.println("....logfile file created");
        }
      } else {
        if (debug == true) {
          Serial.println("....logfile not created");
        }
        SDerror = true;
      }
    }
    else {
      if (debug == true) {
        Serial.println("....logfile exists");
      }
    }

    logfile = SD.open(filename, FILE_WRITE);

    if (!logfile) {
      if (debug == true) {
        Serial.println("Logfile not opened");
      }
      SDerror = true;
    }
    else {
      logfile.println("//////////////////////////////////////////");
      logfile.print(card.type()); logfile.print(volume.fatType(), DEC); logfile.print("/");
    }

    ///////////////////////////////////////////////////////////



    sprintf(filename, "%s", FOLDER_NAME);
    sprintf(filename + strlen(filename), "%s", FILE_NAME_WRITE);
    if (debug == true) {
      Serial.print("Checking: ");
      Serial.print(filename);
    }

    if (SD.exists(filename) == false)
    {
      if (debug == true) {
        //Serial.print(filename);
        Serial.print("... "); Serial.print("INFOW file does not exist");
      }
      record = SD.open(filename, FILE_WRITE);
      record.println('0');
      record.close();
      if (SD.exists(filename))
      {
        if (debug == true) {
          Serial.print("...  ");
          Serial.println("...INFOW file created");
        }
        logfile.print("INFOW created/");
      } else {
        if (debug == true) {
          Serial.print("...  ");
          Serial.println("...INFOW not created");
        }
        logfile.print("INFOW not created/");
        SDerror = true;


      }
    } else {
      if (debug == true) {
        Serial.println("...INFOW exists/");
      }
      logfile.print("INFOW/");
    }
    /////////////////////////////////////////////////////////
    sprintf(filename, "%s", FOLDER_NAME);
    sprintf(filename + strlen(filename), "%s", FILE_NAME_READ);

    //char filename2[24] = "/CRLOGGER";
    //sprintf(filename2 + 9, FILE_NAME_READ);
    if (debug == true) {
      Serial.print("Checking:");
      Serial.print(filename);
    }

    if (!SD.exists(filename))
    {
      if (debug == true) {
        Serial.print("....INFOR file does not exist");
      }
      record = SD.open(filename, FILE_WRITE);
      record.println('0');
      record.close();
      if (SD.exists(filename))
      {
        if (debug == true) {
          Serial.print("....INFOR file created/");
        }
        logfile.print("INFOR created/");
      } else {
        if (debug == true) {
          Serial.print("....INFOR not created/");
        }
        logfile.print("INFOR not created/");
        SDerror = true;
      }
    }
    else {
      if (debug == true) {
        Serial.println("....INFOR exists/");
      }
      logfile.println("INFOR/");
    }
    //////////////////////////////////////////


    //////////////////////////////////////////

    logfile.flush();
    return true;
  }
}
//////////////////// bool SD_init //////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
void flushData()
{
  // flush SD data every 2KB
  if (dataSize - lastFileSize >= 2048) {
    sdfile.flush();
    //flushFile();
    lastFileSize = dataSize;
    // display logged data size

    if (debug == true) {
      Serial.println();
      Serial.print("Logged KB:"); Serial.println((int)(dataSize >> 10));
    }
  }
}
///////////////////////////////////////////////////////////////////////////////
