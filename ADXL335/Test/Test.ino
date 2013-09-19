int x,y,z;

int x_low_thres, x_up_thres;
int y_low_thres, y_up_thres;
int z_low_thres, z_up_thres;

int x_limit = 800;
int y_limit = 800;
int z_limit = 800;

int led = 9;

int init_flag = 0;

// Simply change this value to change threshold
int threshold = 2;

void setup()
{
  Serial.begin(9600);      // sets the serial port to 9600
  
  pinMode(led, OUTPUT);
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  digitalWrite(A0, HIGH);  // set pullup on analog pin 0 
  digitalWrite(A1, HIGH);  // set pullup on analog pin 1
  digitalWrite(A2, HIGH);  // set pullup on analog pin 2
}

void loop()
{
    delay(100);
    read_accel();

// Set upper and lower threshold values for x-axis, y-axis, z-axis
  if(init_flag == 0){
    
    x_low_thres = x - threshold;
    x_up_thres = x + threshold;
    
    y_low_thres = y - threshold;
    y_up_thres = y + threshold;
    
    z_low_thres = z - threshold;
    z_up_thres = z + threshold;
    
    Serial.print("x is: ");
    Serial.println(x);
    Serial.print("Lower Threshold is: ");
    Serial.println(x_low_thres);
    Serial.print("Upper Threshold is: ");
    Serial.println(x_up_thres);
    
    Serial.print("y is: ");
    Serial.println(y);
    Serial.print("Lower Threshold is: ");
    Serial.println(y_low_thres);
    Serial.print("Upper Threshold is: ");
    Serial.println(y_up_thres);
    
    Serial.print("z is: ");
    Serial.println(z);
    Serial.print("Lower Threshold is: ");
    Serial.println(z_low_thres);
    Serial.print("Upper Threshold is: ");
    Serial.println(z_up_thres);
    
    init_flag++;
  }
  
  // Check whether sensor readings above limit specified
  if (x < x_limit & y < y_limit & z < z_limit){ // x > x_limit & 
// Check whether a tap occured or whether activity is taking place
    if ((x < x_low_thres) || (x > x_up_thres) || (y < y_low_thres) || (y > y_up_thres) || (z < z_low_thres) || (z > z_up_thres)){
      delay(250); // This delay is used to see if only a tap or something occured
    
      read_accel();
    
//      Serial.print("First x-value: ");
//      Serial.println(x);
//    
//      Serial.print("First y-value: ");
//      Serial.println(y);
//    
//      Serial.print("First z-value: ");
//      Serial.println(z);
    
        // This code will execute only if there is activity
        if ((x < x_low_thres) || (x > x_up_thres) || (y < y_low_thres) || (y > y_up_thres) || (z < z_low_thres) || (z > z_up_thres))
        {
          Serial.println("EARTHQUAKE DETECTED ");
          Serial.print("Earthquake x-value: ");
          Serial.println(x);
          
          Serial.print("Earthquake y-value: ");
          Serial.println(y);
          
          Serial.print("Earthquake z-value: ");
          Serial.println(z);
          
          digitalWrite(led, HIGH);
          delay(1000);
          digitalWrite(led, LOW);
        }
      }
  }
  else{
    Serial.println("Sensor not working");
//    Serial.print("x: ");
//    Serial.println(x);
//    Serial.print("y: ");
//    Serial.println(y);
//    Serial.print("z: ");
//    Serial.println(z);
    delay(500);
  }
}

void read_accel(){
  x = analogRead(0);       // read analog input pin 0
  delay(1);
  y = analogRead(1);       // read analog input pin 1
  delay(1);
  z = analogRead(2);       // read analog input pin 1
  delay(1);
}
