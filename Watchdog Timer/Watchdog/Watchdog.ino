/*
 *  A simple program to test the watchdog timer
 *  If we define TEST, we will see the watchdog timer reset the program
 *  every 2 seconds (LED turns off quickly for a brief moment)
 */
 
#include <avr/wdt.h>

#define  LED    9
// #define  TEST    // Uncomment to see watchdog timer reset program

void setup()
{
  pinMode(LED, OUTPUT);
  wdt_enable(WDTO_2S);    // Check every 2 seconds
}

void loop()
{
#ifndef TEST
  wdt_reset();
#endif
  
  digitalWrite(LED, HIGH);
}
