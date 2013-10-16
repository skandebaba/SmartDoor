// I2C Master Code (UNO)

#include <Wire.h>

void setup(){
  Wire.begin();
  Serial.begin(9600);
  
  // Wire.requestFrom(device_id,number_of_bytes_to_receieve,true/false)
  // true will send a stop bit
  // false it won't send stop bit, will be useful if I want to send more data
  // don't want to cut off communication just yet
  Wire.requestFrom(5,10);
  
  while(Wire.available()){
    char c = Wire.read(); // Help if you store to external variable
    Serial.print(c);
  }
}

void loop(){
}
