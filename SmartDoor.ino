#include <util/delay.h> 

volatile unsigned char LED [] = {8,9};          // Pin 8 Electromagnetic Lock
                                                // Pin 9 Warning LED
unsigned char BUTTON [] = {2,5};                // Button[0] used as manual call point, interrupt 0 is located on Pin 2
                                                // Pin 5 used as switch to control electromagnetic lock                                                                                
unsigned char Smoke_Sensor = 1;                 // Interrupt 1 located on Pin 3, which is used for Smoke Sensor (MQ-2)      
boolean Exit_Switch_Operated = false;           // Set Emergency Switch as FALSE (Not Pressed)
boolean Man_Switch_Operated = false;            // Set Emergency Switch as FALSE (Not Pressed)

volatile char man_flag = 0;
volatile char smoke_flag = 0;

// ===========================================================================================
// Setup Routine - Executes only once when reset it pressed/arduino powered for first time
// ===========================================================================================

void setup(){
  for (int i = 0; i < 2; i++){
    pinMode(BUTTON[i], INPUT);
    digitalWrite(BUTTON[i], HIGH);    // Turn on internal Pull-Up Resistor
    pinMode(LED[i], OUTPUT);           
  }

  pinMode(Smoke_Sensor, INPUT);
  pinMode(Smoke_Sensor, HIGH);

//  attachInterrupt(BUTTON[0], override_delay, RISING);          // RISING to trigger when the pin goes from low to high
  attachInterrupt(Smoke_Sensor, smoke_override_delay, LOW);    // LOW to trigger the interrupt whenever the pin is low
  
  cli();//stop interrupts

//set timer0 interrupt at 2kHz
  TCCR0A = 0;// set entire TCCR2A register to 0
  TCCR0B = 0;// same for TCCR2B
  TCNT0  = 0;//initialize counter value to 0
  // set compare match register for 2khz increments
  OCR0A = 124;// = (16*10^6) / (2000*64) - 1 (must be <256)
  // turn on CTC mode
  TCCR0A |= (1 << WGM01);
  // Set CS01 and CS00 bits for 64 prescaler
  TCCR0B |= (1 << CS01) | (1 << CS00);   
  // enable timer compare interrupt
  TIMSK0 |= (1 << OCIE0A);

sei();//allow interrupts
}

ISR(TIMER0_COMPA_vect){//timer0 interrupt 2kHz toggles pin 8
//generates pulse wave of frequency 2kHz/2 = 1kHz (takes two cycles for full wave- toggle high then toggle low)
  if (digitalRead(BUTTON[0]))
    man_flag = 1;
}

// ===========================================================================================
// Main Program - Repeats over and over again
// ===========================================================================================

void loop(){

  if(man_flag == 0 && smoke_flag == 0)
  {
    
    Exit_Switch_Operated = digitalRead(BUTTON[1]);
    delay_ms(20);      // Used for debouncing of switch

    if (Exit_Switch_Operated == LOW){
      digitalWrite(LED[0], LOW);
      digitalWrite(LED[1], LOW);
    }
    else{
      delay_ms(3000);
      digitalWrite(LED[0], HIGH);
      led_flash();
      delay_ms(3000);
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
//void override_delay(){
//  man_flag = 1;
//  digitalWrite(LED[0], HIGH);
//}

// Execute this code when external interrupt 1 is activated i.e. Smoke Sensor
void smoke_override_delay(){
  smoke_flag = 1;
  digitalWrite(LED[0], HIGH);
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

