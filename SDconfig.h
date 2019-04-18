#define FILE_NAME_FORMAT "/DAT%05d.CSV"
//String  path = "/CRLOGGER/INFOW.txt";
//String  path_2 = "/CRLOGGER/INFOR.txt";


static const char* idstr = "DeviceID:1023";

//////////////////////////////////////////////////////////////////////////////////////////
uint32_t openFile(uint16_t logFlags = 0, uint32_t dateTime = 0)
{
  uint32_t fileIndex, record_number;
  size_t clientCount = 0;
  int clientBuf[36];
  String inString = "";
  //char filename[24] = "/CRLOGGER";
  //char filename[24] = "/CRLOGGER";
  //Serial.println();

  sprintf(filename, "%s", FOLDER_NAME);
  sprintf(filename + strlen(filename), "%s", FILE_NAME_WRITE);
  if (SD.exists(filename)) {
    record = SD.open(filename, FILE_READ);

    if (record)
    {
      while (record.available()) {
        int inChar = record.read();
        //Serial.println(inChar);
        if (isDigit(inChar))
        {
          // convert the incoming byte to a char
          // and add it to the string:
          inString += (char)inChar;
        }
        if (inChar == '\n') {
          if (debug == true) {
            Serial.print("Last File:"); Serial.println(inString);
          }

          fileIndex = inString.toInt();
          if (fileIndex == 99999)
          { fileIndex = 1;
          }
          else {
            fileIndex = fileIndex + 1;
          }


          // clear the string for new input:
          inString = "";
          break;
        }
      }
      record.close();
    }
    else {
      Serial.println("record error");
    }


    sprintf(filename, "%s", FOLDER_NAME);
    sprintf(filename + 9, FILE_NAME_FORMAT, fileIndex);
    if (SD.exists(filename))
    { SD.remove(filename);
    }
    sdfile = SD.open(filename, FILE_WRITE);

    if (!sdfile) {
      Serial.print(filename);  Serial.println("   File NOT opened");
      return 0;
    }
    else {
      if (debug == true) {
        Serial.print(filename); Serial.println(":  File Opened");
      }

      sprintf(filename, "%s", FOLDER_NAME);
      sprintf(filename + strlen(filename), "%s", FILE_NAME_WRITE);
      record = SD.open(filename, FILE_WRITE);
      if (record)
      {
        record.seek(0);
        record.println(fileIndex);
        record.close();
      }
      //dataSize = sdfile.print(idstr); ???
      //sdfile.println(); ????
      return fileIndex;
    }

  }
  else {
    Serial.println("INFOW error");
  }
}//end of openfile



//////////////////////////////////////////////////////////////////////////////////////////
byte getChecksum(char* buffer, byte len)
{
  uint8_t checksum = 0;
  for (byte i = 0; i < len; i++) {
    checksum ^= buffer[i];
  }
  return checksum;
}
/////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////
void logData(char c)
{
  char buf[20];
  if (c >= ' ') {
    byte n = sprintf(buf, "%c", c);
    dataSize +=  sdfile.write(buf, n);
  }
}////////////////////////////////////////////////////////////////
void logData(uint16_t value)
{
  char buf[20];
  byte n = sprintf(buf, "%ld\r", value);
  dataSize += sdfile.write((uint8_t*)buf, n);
}
////////////////////////////////////////////////////////////////////
void logData(uint32_t value)
{
  char buf[40];
  byte n = sprintf(buf, "%ld", value);
  dataSize += sdfile.write((uint8_t*)buf, n);
}

/////////////////////////////////////////////////////////////////
void logData(char s[20])
{
  char buffer [20];

  byte n = sprintf(buffer, "%s", s);
  dataSize +=  sdfile.write(buffer, n);
}
////////////////////////////////////////////////////////////////
void logData(float d)
{
  char buffer [50];

  byte n = sprintf(buffer, "%lf", d);
  dataSize +=  sdfile.write(buffer, n);
}


