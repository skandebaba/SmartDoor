// Simple program to turn a relay that is connected to pin 8 on and off

void setup(){
  pinMode(8, OUTPUT);
}

void loop(){
  digitalWrite(8,HIGH);
  delay(2000);
  digitalWrite(8,LOW);
  delay(2000);
}
