int Switch [] = {3,4};
int Led [] = {9,10};

int LedState [] = {LOW,LOW};
int SwitchDebounce [] = {0,0};
int LastSwitchState [] = {HIGH, HIGH};
int LastSwitchDebounce [] = {LOW, LOW};

unsigned long LastDebounceTime = 0;
unsigned long DebounceDelay = 50;

void setup()
{
  for (int index = 0; index < 2; index++){
    pinMode(Switch[index],INPUT);
    digitalWrite(Switch[index],HIGH);
    pinMode(Led[index],OUTPUT);
  }
}

void loop() 
{
 for (int index = 0; index < 2; index++) 
  {
  int CurrentSwitch = digitalRead(Switch[index]);
  if (CurrentSwitch != LastSwitchDebounce[index])
  {
    LastDebounceTime = millis();
  } 
  if ((millis() - LastDebounceTime) > DebounceDelay) 
  {
    if (CurrentSwitch != LastSwitchState[index]) 
    {
      if (CurrentSwitch == LOW)
      {
        LedState[index] = !LedState[index];
      } 
    }
    LastSwitchState[index] = CurrentSwitch;
  }
  digitalWrite(Led[index],LedState[index]);
  LastSwitchDebounce[index] = CurrentSwitch;
  }
}
