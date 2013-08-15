#include <util/delay.h> 

volatile unsigned char LED [] = {8,9};          // Pin 8 Electromagnetic Lock
                                                // Pin 9 Warning LED
unsigned char BUTTON [] = {0,5};                // Button[0] used as manual call point, interrupt 0 is located on Pin 2
                                                // Pin 5 used as switch to control electromagnetic lock                                                                                
unsigned char Smoke_Sensor = 1;                 // Interrupt 1 located on Pin 3, which is used for Smoke Sensor (MQ-2)      
boolean Exit_Switch_Operated = false;           // Set Emergency Switch as FALSE (Not Pressed)
boolean Man_Switch_Operated = false;           // Set Emergency Switch as FALSE (Not Pressed)

volatile char man_flag = 0;
volatile char smoke_flag = 0;

// ===========================================================================================
// Setup Routine - Executes only once when reset it pressed/arduino powered for first time
// ===========================================================================================

void setup(){
  for (int i = 0; i < 2; i++){
    pinMode(BUTTON[i], INPUT);
    digitalWrite(BUTTON[i], INPUT_PULLUP);    // Turn on internal Pull-Up Resistor
    pinMode(LED[i], OUTPUT);           
  }
  
  pinMode(Smoke_Sensor, INPUT);
  pinMode(Smoke_Sensor, HIGH);
  
  attachInterrupt(BUTTON[0], override_delay, RISING);          // RISING to trigger when the pin goes from low to high
  attachInterrupt(Smoke_Sensor, smoke_override_delay, LOW);    // LOW to trigger the interrupt whenever the pin is low
}

// ===========================================================================================
// Main Program - Repeats over and over again
// ===========================================================================================

void loop(){
  
  if(man_flag == 0 && smoke_flag == 0)
  {
    Exit_Switch_Operated = digitalRead(BUTTON[1]);
    delay_ms(50);      // Used for debouncing of switch
  
    if (Exit_Switch_Operated == LOW){
        digitalWrite(LED[0], LOW);
        digitalWrite(LED[1], LOW);
     }
     else{
        delay_ms(3000);
        digitalWrite(LED[0], HIGH);
        delay_ms(2000);
     }
  }
  
  if (man_flag == 1){
      digitalWrite(LED[0], HIGH);
      led_flash();
   }
  if (smoke_flag == 1)
   {
     digitalWrite(LED[0], HIGH);
     led_flash2();
   }
    
}

// ===========================================================================================
// Interrupt Code
// ===========================================================================================

// Execute this code when external interrupt 0 is activated i.e. Manual Call Point
void override_delay(){
  man_flag = 1;
}

// Execute this code when external interrupt 1 is activated i.e. Smoke Sensor
void smoke_override_delay(){
  smoke_flag = 1;
}

// ===========================================================================================
// LED Flash Functions
// ===========================================================================================

void led_flash()
{
  digitalWrite(LED[1], HIGH);
  delay_ms(1000);
  digitalWrite(LED[1], LOW);
  delay_ms(1000);
}

void led_flash2()
{
  digitalWrite(LED[1], HIGH);
  delay_ms(1000);
  digitalWrite(LED[1], LOW);
  delay_ms(2000);
}

// ===========================================================================================
// Delay Function
// ===========================================================================================

void delay_ms(unsigned int time) 
{ 
  while (time--) 
    _delay_ms(1); 
}
