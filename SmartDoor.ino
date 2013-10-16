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
#define  X_AXIS           A0     // X-Axis of ADXL335 connected to A0
#define  Y_AXIS           A1     // Y-Axis of ADXL335 connected to A1
#define  Z_AXIS           A2     // Z-Axis of ADXL335 connected to A2

unsigned char   Smoke_Sensor            = SmokeSensor;  // Interrupt 1 located on Pin 3, which is used for Smoke Sensor (MQ-2)
unsigned char   seconds                 = 0;
boolean         Exit_Switch_Operated    = false;        // Set Press-To-Exit as FALSE (Not Pressed)
boolean         Man_Switch_Operated     = false;        // Set Manual Call Point as FALSE (Not Pressed)
boolean         toggle                  = 0;
int             x,y,z                   = 0;            // Variables to store readings from ADXL335

// Variables used to store threshold values for each axis
int x_low_thres, x_up_thres;
int y_low_thres, y_up_thres;
int z_low_thres, z_up_thres;

// Limits for ADXL335, if above these values then something wrong with ADXL335
int x_limit = 800;
int y_limit = 800;
int z_limit = 800;

// Flags used for ADXL335
char   init_flag  = 0;    // Used to make sure thresholding is only done once
char   quake_flag = 0;

// Simply change this value to change threshold of ADXL335
char threshold = 2;

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
  pinMode(X_AXIS, INPUT);
  pinMode(Y_AXIS, INPUT);
  pinMode(Z_AXIS, INPUT);
  
  // Turn on internal Pull-Up Resistor
  digitalWrite(ManualCallPoint, HIGH);    
  digitalWrite(PressToExit, HIGH);
  digitalWrite(X_AXIS, HIGH);  // set pullup on analog pin 0 
  digitalWrite(Y_AXIS, HIGH);  // set pullup on analog pin 1
  digitalWrite(Z_AXIS, HIGH);  // set pullup on analog pin 2
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
  delay_ms(100);
  read_accel();
  // Set upper and lower threshold values for x-axis, y-axis, z-axis
  if(init_flag == 0){
    x_low_thres = x - threshold;
    x_up_thres = x + threshold;
    
    y_low_thres = y - threshold;
    y_up_thres = y + threshold;
    
    z_low_thres = z - threshold;
    z_up_thres = z + threshold;
    
    init_flag++;
  }
  
  // Check whether manual call point or smoke sensor flag is set
  if(man_flag == 0 && smoke_flag == 0 && quake_flag == 0)
  {
    Exit_Switch_Operated = digitalRead(PressToExit);
    delay_ms(50);      // Used for debouncing of switch
  
    if (Exit_Switch_Operated == LOW){
        digitalWrite(ElectroLock, LOW);
        digitalWrite(WarningLED, LOW);
     }
     else{
        delay_ms(DelayTime);            // Specifies the time before the door unlocks
        digitalWrite(ElectroLock, HIGH);
        delay_ms(UnlockTime);           // Specifies the time before the door locks again
     }
     
      // Check whether sensor readings above limit specified
      if (x < x_limit & y < y_limit & z < z_limit)
      {
        // Check whether a tap occured or whether activity is taking place
        if ((x < x_low_thres) || (x > x_up_thres) || (y < y_low_thres) || (y > y_up_thres) || (z < z_low_thres) || (z > z_up_thres))
        {
          delay_ms(250);   // This delay is used to see if a tap occured
          read_accel();
      
          // This code will execute only if there is activity
          if ((x < x_low_thres) || (x > x_up_thres) || (y < y_low_thres) || (y > y_up_thres) || (z < z_low_thres) || (z > z_up_thres))
          {  
            quake_flag = 1;
          }
        }
      }
      else{
        quake_flag = 1;
      }
  }
  
  if (man_flag == 1)
  {
    for(;;)
    {
      digitalWrite(ElectroLock, HIGH);
      led_flash();
    }
   }
   
   if (smoke_flag == 1)
   {
     for(;;)
     {
       digitalWrite(ElectroLock, HIGH);
       led_flash2();
     }
   }
   
   if (quake_flag == 1)
   {
     for(;;)
     {
       digitalWrite(ElectroLock, HIGH);
       led_flash3();
     }
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
    digitalWrite(ElectroLock, HIGH);
  }
}

// Timer1 interrupt 2Hz 
// Toggles pin 8 (WarningLED)
ISR(TIMER1_COMPA_vect){
//generates pulse wave of frequency 2Hz/2 = 1Hz (takes two cycles for full wave- toggle high then toggle low)
  wdt_reset(); 
  seconds++;
    if (man_flag == 0 && smoke_flag == 0 && quake_flag == 0 && seconds == FlashLED)
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
  digitalWrite(ElectroLock, HIGH);
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

void led_flash3()
{
  digitalWrite(WarningLED, HIGH);
  delay_ms(2000);
  digitalWrite(WarningLED, LOW);
  delay_ms(2000);
}

// ===========================================================================================
// Read Accelerometer (ADXL335)
// ===========================================================================================

void read_accel(){
  x = analogRead(X_AXIS);       // read analog input pin 0
  delay_ms(1);
  y = analogRead(Y_AXIS);       // read analog input pin 1
  delay_ms(1);
  z = analogRead(Z_AXIS);       // read analog input pin 1
  delay_ms(1);
}

// ===========================================================================================
// Delay Function
// ===========================================================================================

void delay_ms(unsigned int time) 
{ 
  while (time--) 
    _delay_ms(1); 
}
