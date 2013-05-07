#include <util/delay.h> 

int LED [] = {8,9,10};                // Pin 8 Electromagnetic Lock
                                      // Pin 9 Sensor 1
                                      // Pin 10 Sensor 2
int BUTTON [] = {0,3,4,5};            // Pin 5 used as switch to control electromagnetic lock
                                      // Pin 3 used to control sensor 1
                                      // Pin 4 used to control sensor 2
                                      // Button[0] used as manual call point, interrupt 0 is located on Pin 2                                    
        
int val [] = {0,0};                   // val to be used to store the state of input pin (BUTTON)
int old_val [] = {0,0};               // stores the previous value of "val"
int LEDstate [] = {0,0};              // 0 = LED OFF and 1 = LED ON       

boolean Temp_Sensor = false;          // Set Reading of Sensors as FALSE (No Reading)
boolean Smoke_Sensor = false;              
boolean Sensors_Working = false;          
int Emergency_Switch_Operated = 0;    // Set Emergency Switch as FALSE (Not Pressed)

// The Setup Routine executes once when reset is pressed
void setup(){
  for (int i = 0; i < 4; i++){
    pinMode(BUTTON[i], INPUT);
    digitalWrite(BUTTON[i], HIGH);     // Turn on internal Pull-Up Resistor
    pinMode(LED[i], OUTPUT);           
  }
  
  //Attach the interrupt to the input pin and monitor for ANY Change
  attachInterrupt(BUTTON[0], override_delay, CHANGE);
}

// The Loop Routine executes over and over again
void loop(){
  read_buttons();
  check_sensors();
  
  // Reads State of Emergency_Switch
  Emergency_Switch_Operated = digitalRead(BUTTON[3]);
  
  // If sensors AREN'T working and emergency switch is operated
  // the electromagnet should unlock instantaneously, else
  // if the sensors are working and switch is operated delay occurs before unlocking
  if (Sensors_Working == false){
    // Checks whether Emergency Switch Operated
    if (Emergency_Switch_Operated == LOW){
      digitalWrite(LED[0], HIGH);
    }
    else {
      digitalWrite(LED[0], LOW);
      delay_ms(2000);
    }
  } else
  {
    if (Emergency_Switch_Operated == LOW){
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

void read_buttons(){
  for (int i = 0; i < 2; i++){
    val[i] = digitalRead(BUTTON[i+1]);      // Read Push Button
    
    // Check if there was a transition
    if ((val[i] == HIGH) && (old_val[i] == LOW)){
      LEDstate[i] = 1 - LEDstate[i];
      delay(10);
    }
    old_val[i] = val[i];    // val is now old, let's store it  
  }

  if (LEDstate[0] == 0){
    digitalWrite(LED[1], HIGH); // Turn LED ON
    Smoke_Sensor = false; // Set Smoke Sensor as NOT working
  } else{
    digitalWrite(LED[1], LOW); // Turn LED OFF
    Smoke_Sensor = true; // Set Smoke Sensor as working
  }  
  
  if (LEDstate[1] == 0){
    digitalWrite(LED[2], HIGH); // Turn LED ON
    Temp_Sensor = false; // Set Smoke Sensor as NOT working
  } else{
    digitalWrite(LED[2], LOW); // Turn LED OFF
    Temp_Sensor = true; // Set Smoke Sensor as working
  }
}

void check_sensors(){
   // Checks Whether Sensors are working
  if (Temp_Sensor == true && Smoke_Sensor == true){
    Sensors_Working = true;
  }
  else {
    Sensors_Working = false;
  }
}

void delay_ms(unsigned int time) 
{ 
  while (time--) 
    _delay_ms(1); 
}
