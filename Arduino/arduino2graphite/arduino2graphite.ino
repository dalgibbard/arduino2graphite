//
//    FILE: dht22_test.ino
//  AUTHOR: Rob Tillaart
// VERSION: 0.1.01
// PURPOSE: DHT library test sketch for DHT22 && Arduino
//     URL:
//
// Released to the public domain
//
#include <Time.h>  
#include <dht.h>

#define TIME_MSG_LEN  11   // time sync to PC is HEADER followed by unix time_t as ten ascii digits
#define TIME_HEADER  'T'   // Header tag for serial time sync message
#define TIME_REQUEST  7    // ASCII bell character requests a time sync message 

dht DHT;

#define DHT22_PIN 5

void setup()
{
    Serial.begin(9600);
    //Serial.println("DHT TEST PROGRAM ");
    //Serial.print("LIBRARY VERSION: ");
    //Serial.println(DHT_LIB_VERSION);
    //Serial.println();
    //Serial.println("Type,\tstatus,\tHumidity (%),\tTemperature (C)\tTime (us)");
    //setSyncProvider( requestSync); 
}

void loop()
{
    //Check for time sync message
    //if(Serial.available() ) 
    //{
      processSyncMessage();
    //}
    if(timeStatus()!= timeNotSet)   
    {
      digitalWrite(13,timeStatus() == timeSet); // on if synced, off if needs refresh  
      //digitalClockDisplay();  
    }
    
    // READ DATA
    //Serial.print("DHT22, \t");

    uint32_t start = micros();
    int chk = DHT.read22(DHT22_PIN);
    uint32_t stop = micros();

    switch (chk)
    {
    case DHTLIB_OK:
        Serial.print("OK\t");
        break;
    case DHTLIB_ERROR_CHECKSUM:
        Serial.print("Checksum_error\t");
        break;
    case DHTLIB_ERROR_TIMEOUT:
        Serial.print("Timeout_error\t");
        break;
    default:
        Serial.print("Unknown_error\t");
        break;
    }
    // DISPLAY DATA
    //epoch  humidity  temp
    Serial.print(now());
    Serial.print("\t");
    Serial.print(DHT.humidity, 1);
    Serial.print("\t");
    Serial.print(DHT.temperature, 1);
    // Uncomment to also push time-taken
    //Serial.print("\t");
    //Serial.print(stop - start);
    Serial.println();

    delay(2000);
}

void digitalClockDisplay(){
  // digital clock display of the time
  Serial.print(hour());
  printDigits(minute());
  printDigits(second());
  Serial.print(" ");
  Serial.print(day());
  Serial.print(" ");
  Serial.print(month());
  Serial.print(" ");
  Serial.print(year()); 
  Serial.println(); 
}

void printDigits(int digits){
  // utility function for digital clock display: prints preceding colon and leading 0
  Serial.print(":");
  if(digits < 10)
    Serial.print('0');
  Serial.print(digits);
}

void processSyncMessage() {
  // if time sync available from serial port, update time and return true
  char c = Serial.read() ; 
  if( c == TIME_HEADER ) {       
    time_t pctime = 0;
    for(int i=0; i < TIME_MSG_LEN -1; i++){   
      c = Serial.read();          
      if( c >= '0' && c <= '9'){   
        pctime = (10 * pctime) + (c - '0') ; // convert digits to a number    
      }
    }   
    setTime(pctime);   // Sync Arduino clock to the time received on the serial port
  }  
}
//
// END OF FILE
//
