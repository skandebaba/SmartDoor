#include <util/delay.h> 

volatile unsigned char LED [] = {8,9};          // Pin 8 Electromagnetic Lock
                                                // Pin 9 Warning LED
volatile unsigned char Relay = 8;               // Relay is connected to pin 8

unsigned char BUTTON [] = {0,5};                // Button[0] used as manual call point, interrupt 0 is located on Pin 2
                                                // Pin 5 used as switch to control electromagnetic lock
                                                                            
unsigned char Smoke_Sensor = 3;                 // Pin 3 used for Smoke Sensor (MQ-2)  

// boolean Temp_Sensor = false;          // Set Reading of Sensors as FALSE (No Reading)
boolean Smoke_Sensor_Operating = false;              
boolean Sensors_Working = false;          
boolean Switch_Operated = false;    // Set Emergency Switch as FALSE (Not Pressed)

// ===========================================================================================
// Setup Routine - Executes only once when reset it pressed/arduino powered for first time
// ===========================================================================================

void setup(){
  for (int i = 0; i < 2; i++){
    pinMode(BUTTON[i], INPUT);
    digitalWrite(BUTTON[i], HIGH);     // Turn on internal Pull-Up Resistor          
  }
  
  pinMode(LED[1], OUTPUT);
  pinMode(Relay, OUTPUT);
  pinMode(Smoke_Sensor, INPUT);
//  digitalWrite(Smoke_Sensor, LOW);
  
  //Attach the interrupt to the input pin and monitor for ANY Change
  attachInterrupt(BUTTON[0], override_delay, CHANGE);
}

// ===========================================================================================
// Main Program
// ===========================================================================================

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
      digitalWrite(Relay, LOW);
    }
    else{
      digitalWrite(Relay, HIGH);
      digitalWrite(LED[1], LOW);
      delay_ms(2000);
    }
  } 
  else
  {
    if (Switch_Operated == LOW){
      digitalWrite(Relay, LOW);
    }
    else {
      delay_ms(3000);
      digitalWrite(Relay, HIGH);
      delay_ms(2000);
    }
  }
}

// ===========================================================================================
// Interrupt Code
// ===========================================================================================

// Execute this code when external interrupt 0 is activated i.e. Manual Call Point
void override_delay(){
  digitalWrite(Relay,HIGH);
  delay_ms(2000);
}

// ===========================================================================================
// Code to check whether sensor(s) are working
// ===========================================================================================

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

// ===========================================================================================
// Delay function
// ===========================================================================================

void delay_ms(unsigned int time) 
{ 
  while (time--) 
    _delay_ms(1); 
}
