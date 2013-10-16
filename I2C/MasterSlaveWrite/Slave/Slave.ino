// I2C Slave Code (UNO R3)

#include <Wire.h>

void setup(){
  Wire.begin(5); // Give slave device an address
  Wire.onReceive(receiveEvent);
  
  pinMode(13, OUTPUT);
  digitalWrite(13,LOW);
}

void loop(){
}

// Can call event anything we want

void receiveEvent(int howMany){ // howMany stores how many bytes we received
  while(Wire.available()){
    char c = Wire.read();
    
    if(c == 'H'){
      digitalWrite(13,HIGH);
    }
    else if(c == 'L'){
      digitalWrite(13,LOW);
    }
  }
}
