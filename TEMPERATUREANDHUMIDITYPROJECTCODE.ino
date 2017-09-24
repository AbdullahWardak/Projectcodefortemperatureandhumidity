
  //Libraries
  #include <DHT.h>
  #include "SD.h"
  #include <Wire.h>
  #include "RTClib.h"

  
  #define LOG_INTERVAL  1000 // mills between entries. 
  #define SYNC_INTERVAL 1000 // mills between calls to flush() - to write data to the card
  uint32_t syncTime = 0; // time of last sync()

  #define ECHO_TO_SERIAL   1 // echo data to serial port. 


  //define the Constants
  #define DHTPIN 2     
  #define DHTTYPE DHT22   
  DHT dht(DHTPIN, DHTTYPE); 

  //define the Variables
  int chk;
  float hum;  
  float temp; 
  RTC_PCF8523 RTC; 


  const int chipSelect = 10;


  File logfile;

  void setup()
  {
    Serial.begin(9600);

  
    initSDcard();


    createFile();


    initRTC();

  // print header 
    logfile.println("millis,stamp,datetime,hum,temp");
   #if ECHO_TO_SERIAL
    Serial.println("millis,stamp,datetime,hum,temp");
  #endif //ECHO_TO_SERIAL



    dht.begin();

  }

  void loop()
  {
    DateTime now;

  
    delay((LOG_INTERVAL - 1) - (millis() % LOG_INTERVAL));

  
    uint32_t m = millis();
    logfile.print(m);          
    logfile.print(", ");
  #if ECHO_TO_SERIAL
    Serial.print(m);         
    Serial.print(", ");
  #endif

  
    now = RTC.now();
  
    logfile.print(now.unixtime()); 
    logfile.print(", ");
    logfile.print(now.year(), DEC);
    logfile.print("/");
    logfile.print(now.month(), DEC);
    logfile.print("/");
    logfile.print(now.day(), DEC);
    logfile.print(" ");
    logfile.print(now.hour(), DEC);
    logfile.print(":");
    logfile.print(now.minute(), DEC);
    logfile.print(":");
    logfile.print(now.second(), DEC);
  #if ECHO_TO_SERIAL
    Serial.print(now.unixtime()); 
    Serial.print(", ");
    Serial.print(now.year(), DEC);
    Serial.print("/");
    Serial.print(now.month(), DEC);
    Serial.print("/");
    Serial.print(now.day(), DEC);
    Serial.print(" ");
    Serial.print(now.hour(), DEC);
    Serial.print(":");
    Serial.print(now.minute(), DEC);
    Serial.print(":");
    Serial.print(now.second(), DEC);
  #endif 


  
    hum = dht.readHumidity();
    temp = dht.readTemperature();

    logfile.print(", ");
    logfile.print(hum);
    logfile.print(", ");
    logfile.println(temp);
  #if ECHO_TO_SERIAL
    Serial.print(", ");
    Serial.print(hum);
    Serial.print(", ");
    Serial.println(temp);
  #endif //ECHO_TO_SERIAL

    if ((millis() - syncTime) < SYNC_INTERVAL) return;
    syncTime = millis();

    logfile.flush();
  }


  // create the error map
  void error(char const *str)
  {
    Serial.print("error: ");
    Serial.println(str);

    while (1);
  }

  void initSDcard()
  {
    Serial.print("Initializing SD card...");
 
    pinMode(10, OUTPUT);

  
    if (!SD.begin(chipSelect)) {
      Serial.println("Card failed, or not present");
      
      return;
    }
    Serial.println("card initialized.");

  }

  void createFile()
  {
   
    char filename[] = "ABCDEF001.CSV";
    for (uint8_t i = 0; i < 100; i++) {
      filename[5] = i / 10 + '0';
      filename[6] = i % 10 + '0';
      if (! SD.exists(filename)) {
        logfile = SD.open(filename, FILE_WRITE);
        break;  // leave the loop!
      }
    } 

    if (! logfile) {
      error("couldnt create file");
    }

    Serial.print("Logging to: ");
    Serial.println(filename);
  }

  void initRTC()
  {
    Wire.begin();
    if (!RTC.begin()) {
      logfile.println("RTC failed");
  #if ECHO_TO_SERIAL
      Serial.println("RTC failed");
  #endif  //ECHO_TO_SERIAL

    }
  }
 

