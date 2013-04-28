int LED = 9;                            // LED on PIN 9 (OUTPUT)
int BUTTON = 3;                         // Push Button on PIN 3 (INPUT)

int val = 0;                            // val to be used to store the state of input pin (BUTTON)
int old_val = 0;                        // stores the previous value of "val"
int state = 0;                          // 0 = LED OFF and 1 = LED ON

int Electro_Lock = 8;                    // Define which pin electromagnet will use (PIN 8) OUTPUT
int Emergency_Switch = 2;                // Push Button on PIN 2 (INPUT)            

boolean Temp_Sensor = true;              // Set Reading of Sensors as FALSE (No Reading)
boolean Smoke_Sensor = false;              
boolean Sensors_Working = false;          // Set Sensors as not working
int Emergency_Switch_Operated = 0;          // Set Emergency Switch as FALSE (Not Pressed)

// The Setup Routine executes once when reset is pressed
void setup(){
  pinMode(Electro_Lock, OUTPUT);           // Set Pin 8 as output
  pinMode(Emergency_Switch, INPUT);        // Set Pin 2 as input
  digitalWrite(Emergency_Switch, HIGH);    // Turn on internal Pull-Up Resistor
 
  pinMode(LED, OUTPUT);                    // Set Pin 9 as output
  pinMode(BUTTON, INPUT);                  // Set Pin 3 as inputs
  digitalWrite(BUTTON, HIGH);              // Turn on internal Pull-Up Resistor
}  

// STIL HAVE TO WRITE CODE TO READ PUSH BUTTONS

// The Loop Routine executes over and over again
void loop(){
  read_button();
  check_sensors();
  
  // Reads State of Emergency_Switch
  Emergency_Switch_Operated = digitalRead(Emergency_Switch);
 
 // Checks Whether Sensors Are Working
//  if (Temp_Sensor == true && Smoke_Sensor == true){
//    Sensors_Working = true;
 // }
  
  // If sensors AREN'T working and emergency switch is operated
  // the electromagnet should unlock instantaneously
  if (Sensors_Working == false){
    // Checks whether Emergency Switch Operated
    if (Emergency_Switch_Operated == LOW){
      digitalWrite(Electro_Lock, HIGH);
    }
    else {
      digitalWrite(Electro_Lock, LOW);
      delay(2000);
    }
  }
//  else if (Sensors_Working == false && Emergency_Switch_Operated == false){
//    digitalWrite(Electro_Lock, HIGH);
//  }

  // If sensors ARE working and emergency switch is operated
  // the electromagnet unlocks after a delay period
  if (Sensors_Working == true){
    // Checks Whether Emergency Switch Operated
    if (Emergency_Switch_Operated == LOW){
      digitalWrite(Electro_Lock, HIGH);
    }
    else {
      delay(3000);
      digitalWrite(Electro_Lock, LOW);
      delay(2000);
    }
  }
//  else if (Sensors_Working == true && Emergency_Switch_Operated == false){
//    digitalWrite(Electro_Lock, HIGH);
//  }
}

void read_button(){
    val = digitalRead(BUTTON);      // Read Push Button

  // Check if there was a transition
  if ((val == HIGH) && (old_val == LOW)){
    state = 1 - state;
    delay(10);
  }
  
  old_val = val;    // val is now old, let's store it
  
  if (state == 1){
    digitalWrite(LED, HIGH); // Turn LED ON
    Smoke_Sensor = false; // Set Smoke Sensor as NOT working
  } else{
    digitalWrite(LED, LOW); // Turn LED OFF
    Smoke_Sensor = true; // Set Smoke Sensor as working
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
