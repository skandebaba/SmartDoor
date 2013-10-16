#include "Wire.h"

#define DEVICE (0x53) // Device address as specified in data sheet

//To learn more about these and the rest of the registers on the ADXL345, read the datasheet!
#define POWER_CTL 0x2D         // Power Control Register
#define DATA_FORMAT 0x31       // Data Format Control
#define DATAX0 0x32            // X-Axis Data 0
#define INT_MAP 0x2F           // Interrupt Mapping Control
#define INT_ENABLE 0x2E        // Interrupt Enable Control
#define INT_SOURCE 0x30        // Source of Interrupts
#define ACT_INACT_CTL 0x27     // Axis Enable Control for Activity and Inactivity Detection
#define THRESH_ACT 0x24        // Activity Threshold
#define THRESH_INACT 0x25;     // Inactivity Threshold

//This buffer will hold values read from the ADXL345 registers.
char values[6];
//These variables will be used to hold the x,y and z axis accelerometer values.
int x,y,z = 0;
double xg, yg, zg = 0;

void setup(){
    // Join I2C bus
    Wire.begin();
  
   // Initialize serial communication 
    Serial.begin(9600);
  
    //Create an interrupt that will trigger when a tap is detected.
    attachInterrupt(0, activity, RISING);
  
    pinMode(13, OUTPUT);
    digitalWrite(2, HIGH);
  
    // AC-Coupled Operation, ACT_X, ACT_Y, ACT_Z enabled
    writeTo(ACT_INACT_CTL, 0xF0);
  
    //Set the Activity Threshold
    writeTo(THRESH_ACT, 0x0F);
  
    //Put the ADXL345 into +/- 4G range by writing the value 0x01 to the DATA_FORMAT register.
    writeTo(DATA_FORMAT, 0x01);
  
    //Put the ADXL345 into Measurement Mode by writing 0x08 to the POWER_CTL register.
    writeTo(POWER_CTL, 0x08);  //Measurement mode
  
    //Enable the Activity Interrupt.
    writeTo(INT_ENABLE, 0x90);
  
    // Activity interrupt takes place at INT1
    writeTo(INT_MAP, 0xEF);
  
    readFrom(INT_SOURCE, 1, values); //Clear the interrupts from the INT_SOURCE register.
}

void activity(void){
//  readFrom(INT_SOURCE, 1, values); //Clear the interrupts from the INT_SOURCE register
//  Serial.print(values[0]);
//  if (values[0] & 0x90 == 1){
  digitalWrite(13,HIGH);
//  }
//  readFrom(INT_SOURCE, 1, values);
//  if(values[0] & 0x10 == 1)digitalWrite(13,HIGH);
}

void loop(){
    //Reading 6 bytes of data starting at register DATAX0 will retrieve the x,y and z acceleration values from the ADXL345.
  //The results of the read operation will get stored to the values[] buffer.
  readFrom(DATAX0, 6, values);

  //The ADXL345 gives 10-bit acceleration values, but they are stored as bytes (8-bits). To get the full value, two bytes 
  //must be combined for each axis.
  //The X value is stored in values[0] and values[1].
  x = ((int)values[1]<<8)|(int)values[0];
  //The Y value is stored in values[2] and values[3].
  y = ((int)values[3]<<8)|(int)values[2];
  //The Z value is stored in values[4] and values[5].
  z = ((int)values[5]<<8)|(int)values[4];
  
      Serial.print(x);
      Serial.print(',');
      Serial.print(y);
      Serial.print(',');
      Serial.println(z);
  
  //Convert the accelerometer value to G's. 
  //With 10 bits measuring over a +/-4g range we can find how to convert by using the equation:
  // Gs = Measurement Value * (G-range/(2^10)) or Gs = Measurement Value * (8/1024)
  xg = x * 0.0078;
  yg = y * 0.0078;
  zg = z * 0.0078;
}

void writeTo(byte address, byte val) {
  Wire.beginTransmission(DEVICE); // start transmission to device 
  Wire.write(address);             // send register address
  Wire.write(val);                 // send value to write
  Wire.endTransmission();         // end transmission
}

// Reads num bytes starting from address register on device in to _buff array
void readFrom(byte address, int num, char * value) {
  Wire.beginTransmission(DEVICE); // start transmission to device 
  Wire.write(address);             // sends address to read from
  Wire.endTransmission();         // end transmission

  Wire.beginTransmission(DEVICE); // start transmission to device
  Wire.requestFrom(DEVICE, num);    // request 6 bytes from device

  int i = 0;
  while(Wire.available())         // device may send less than requested (abnormal)
  { 
    value[i] = Wire.read();    // receive a byte
    i++;
  }
  Wire.endTransmission();         // end transmission
}
