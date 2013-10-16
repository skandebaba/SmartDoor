// I2C Slave Code (UNO R3)

#include <Wire.h>

void setup(){
  Wire.begin(5);
  Wire.onRequest(requestEvent);
}

void loop(){
  delay(100);
}

// When master asks slave for information this is executed
void requestEvent(){
  Wire.write("1234567890");
}
