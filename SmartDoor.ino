#include <util/delay.h> 

volatile int LED [] = {8,9};          // Pin 8 Electromagnetic Lock
                                      // Pin 9 Sensor 1
                                      // Pin 10 Sensor 2
int BUTTON [] = {0,5};                // Button[0] used as manual call point, interrupt 0 is located on Pin 2
                                      // Pin 5 used as switch to control electromagnetic lock
                                             
                                      
int Smoke_Sensor = 3;                 // Pin 3 used for Smoke Sensor (MQ-2)
        
int val [] = {0,0};                   // val to be used to store the state of input pin (BUTTON)
int old_val [] = {0,0};               // stores the previous value of "val"
int LEDstate [] = {0,0};              // 0 = LED OFF and 1 = LED ON       

// boolean Temp_Sensor = false;          // Set Reading of Sensors as FALSE (No Reading)
boolean Smoke_Sensor_Operating = false;              
boolean Sensors_Working = false;          
int Switch_Operated = false;    // Set Emergency Switch as FALSE (Not Pressed)

// The Setup Routine executes once when reset is pressed/arduino powered for first time
void setup(){
  for (int i = 0; i < 2; i++){
    pinMode(BUTTON[i], INPUT);
    digitalWrite(BUTTON[i], HIGH);     // Turn on internal Pull-Up Resistor
    pinMode(LED[i], OUTPUT);           
  }
  
  pinMode(Smoke_Sensor, INPUT);
//  digitalWrite(Smoke_Sensor, LOW);
  
  //Attach the interrupt to the input pin and monitor for ANY Change
  attachInterrupt(BUTTON[0], override_delay, CHANGE);
}

// The Loop Routine executes over and over again
void loop(){
  check_sensors();
  
  // Reads State of Emergency_Switch
  Switch_Operated = digitalRead(BUTTON[1]);
  
  // If sensors AREN'T working and emergency switch is operated
  // the electromagnet should unlock instantaneously, else
  // if the sensors are working and switch is operated delay occurs before unlocking
  if (Sensors_Working == false){
    // Checks whether Emergency Switch Operated
    if (Switch_Operated == LOW){
      digitalWrite(LED[0], HIGH);
    }
    else {
      digitalWrite(LED[0], LOW);
      delay_ms(2000);
    }
  } else
  {
    if (Switch_Operated == LOW){
      digitalWrite(LED[0], HIGH);
    }
    else {
      delay_ms(3000);
      digitalWrite(LED[0], LOW);
      delay_ms(2000);
    }
  }
}

// Execute this code when external interrupt 0 is activated
void override_delay(){
  digitalWrite(LED[0],LOW);
  delay_ms(2000);
}

void check_sensors(){
  Smoke_Sensor_Operating = digitalRead(Smoke_Sensor);

  // Checks Whether Sensors are working
  if (Smoke_Sensor_Operating == true){
    Sensors_Working = true;
    digitalWrite(LED[1], LOW);
  }
  else if (Smoke_Sensor_Operating == false){
    Sensors_Working = false;
    Smoke_Sensor_Operating = false;
    digitalWrite(LED[1], HIGH);
  }
}

void delay_ms(unsigned int time) 
{ 
  while (time--) 
    _delay_ms(1); 
}
