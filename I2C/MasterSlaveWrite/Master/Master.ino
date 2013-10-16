// I2C Master Code(UNO)

#include <Wire.h>

void setup(){
  Serial.begin(9600);
  
  Wire.begin();
}

void loop(){
  while(Serial.available()){
    char c = Serial.read();
    
    if(c == 'H')
    { // Can be any number from 1 - 127 (address), 0 is taken by the master device
      Wire.beginTransmission(5);
      Wire.write('H');
      Wire.endTransmission();
    }
    else if(c== 'L')
    {
      Wire.beginTransmission(5);
      Wire.write('L');
      Wire.endTransmission(); 
    }
  }
}
