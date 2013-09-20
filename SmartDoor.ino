#include <util/delay.h> 
#include <avr/wdt.h>

#define  ElectroLock      8      // Relay Controlling Electromagnetic Lock connected to Pin 8
#define  WarningLED       9      // Warning LED connected to Pin 9
#define  PressToExit      5      // Press-to-Exit pushbutton connected to Pin 5
#define  ManualCallPoint  2      // Manual Call Point connected to Pin 2
#define  SmokeSensor      1      // Connected to Interrupt 1, interrupt 0 is located on Pin 3
#define  DelayTime        3000   // Specify the delay time in milliseconds (Maximum = 15000), before electromagnetic lock unlocks
#define  UnlockTime       2000   // Specify the time(ms) the electromagnetic lock is unlocked for, before it locks again
#define  FlashLED         5      // LED will flash every 5 seconds

unsigned char   Smoke_Sensor          = SmokeSensor;  // Interrupt 1 located on Pin 3, which is used for Smoke Sensor (MQ-2)
unsigned char   seconds               = 0;
boolean         Exit_Switch_Operated  = false;        // Set Emergency Switch as FALSE (Not Pressed)
boolean         Man_Switch_Operated   = false;        // Set Emergency Switch as FALSE (Not Pressed)
boolean         toggle                = 0;

volatile char lock       = ElectroLock;
volatile char MCP        = ManualCallPoint;
volatile char man_flag   = 0;                  
volatile char smoke_flag = 0;

// ===========================================================================================
// Setup Routine - Executes only once when reset it pressed/arduino powered for first time
// ===========================================================================================

void setup(){
  // Configure as inputs
  pinMode(ManualCallPoint, INPUT);
  pinMode(PressToExit, INPUT);
  pinMode(Smoke_Sensor, INPUT);
  
  // Turn on internal Pull-Up Resistor
  digitalWrite(ManualCallPoint, HIGH);    
  digitalWrite(PressToExit, HIGH);
  pinMode(Smoke_Sensor, HIGH); 
  
  // Configure as outputs
  pinMode(ElectroLock, OUTPUT);
  pinMode(WarningLED, OUTPUT);

  // Attach Interrupts to their respective functions
  attachInterrupt(Smoke_Sensor, smoke_override_delay, LOW);    // LOW to trigger the interrupt whenever the pin is low
  
  /*
   *  Initialize timer 0
   *  Set interrupt to occur every 100Hz (10ms)
   */
  cli();       // Stop interrupts
  TCCR0A = 0;  // Set entire TCCR2A register to 0
  TCCR0B = 0;  // Same for TCCR2B
  TCNT0  = 0;  // Initialize counter value to 0
  
  // Set compare match register for 100Hz increments
  OCR0A   = 156;  // = (16*10^6) / (100*1024) - 1 (must be < 256)
  TCCR0A |= (1 << WGM01);  // Turn on CTC mode
  // Set CS00 and CS02 bits for 1024 prescaler
  TCCR0B |= (1 << CS00 | 1 << CS02);
  // Enable timer compare interrupt
  TIMSK0 |= (1 << OCIE0A);
  
  /*
   *  Initialize timer 1
   *  Set interrupt to occur every 2Hz (0.5s)
   */
    TCCR1A = 0;     // set entire TCCR1A register to 0
    TCCR1B = 0;     // same for TCCR1B
 
    // set compare match register to desired timer count:
    OCR1A = 7811;  // = (16*10^6) / (100*1024) - 1 (must be < 65536)
    // turn on CTC mode:
    TCCR1B |= (1 << WGM12);
    // Set CS10 and CS12 bits for 1024 prescaler:
    TCCR1B |= (1 << CS10);
    TCCR1B |= (1 << CS12);
    // enable timer compare interrupt:
    TIMSK1 |= (1 << OCIE1A);
    // enable global interrupts:
    
  sei();  // Enable global interrupts

  // Enable the watchdog timer
  wdt_enable(WDTO_500MS);
}

// ===========================================================================================
// Main Program - Repeats over and over again
// ===========================================================================================

void loop(){
  // Check whether manual call point or smoke sensor flag is set
  if(man_flag == 0 && smoke_flag == 0)
  {
    Exit_Switch_Operated = digitalRead(PressToExit);
    delay_ms(50);      // Used for debouncing of switch
  
    if (Exit_Switch_Operated == LOW){
        digitalWrite(ElectroLock, LOW);
        digitalWrite(WarningLED, LOW);
     }
     else{
        delay_ms(DelayTime);      // Specifies the time before the door unlocks
        digitalWrite(ElectroLock, HIGH);
        delay_ms(UnlockTime);           // Specifies the time before the door locks again
     }
  }
  
  if (man_flag == 1)
  {
      digitalWrite(ElectroLock, HIGH);
      led_flash();
   }
   
   if (smoke_flag == 1)
   {
     digitalWrite(ElectroLock, HIGH);
     led_flash2();
   }
   
}

// ===========================================================================================
// Interrupt Code
// ===========================================================================================

// Timer0 interrupt 100Hz 
// Reset watchdog timer & read Manual Call Point
ISR(TIMER0_COMPA_vect){
  wdt_reset(); 
// Generates pulse wave of frequency 100/2 = 50Hz (takes two cycles for full wave- toggle high then toggle low)
  if (digitalRead(MCP))
  {
    man_flag = 1;
    digitalWrite(lock, HIGH);
  }
}

// Timer1 interrupt 2Hz 
// Toggles pin 8 (WarningLED)
ISR(TIMER1_COMPA_vect){
//generates pulse wave of frequency 2Hz/2 = 1Hz (takes two cycles for full wave- toggle high then toggle low)
  wdt_reset(); 
  seconds++;
    if (man_flag == 0 && smoke_flag == 0 && seconds == FlashLED)
    {
        seconds = 0;
        if (toggle){
          digitalWrite(WarningLED,HIGH);
          toggle = 0;
        }
        else{
          digitalWrite(WarningLED,LOW);
          toggle = 1;
        }
    }
    
}

// Execute this code when external interrupt 1 is activated i.e. Smoke Sensor
void smoke_override_delay(){
  wdt_reset(); 
  smoke_flag = 1;
  digitalWrite(lock, HIGH);
}

// ===========================================================================================
// LED Flash Functions
// ===========================================================================================

void led_flash()
{
  digitalWrite(WarningLED, HIGH);
  delay_ms(1000);
  digitalWrite(WarningLED, LOW);
  delay_ms(1000);
}

void led_flash2()
{
  digitalWrite(WarningLED, HIGH);
  delay_ms(1000);
  digitalWrite(WarningLED, LOW);
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
