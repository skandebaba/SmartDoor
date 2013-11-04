
int sensorPin = 0;      // The analog pin the TMP36's Vout (sense) pin is connected to
                        // the resolution is 10 mV / degree centigrade with a
                        // 500 mV offset to allow for negative temperatures
 
void setup()
{
  Serial.begin(9600);  // Start the serial connection with the computer
                       // to view the result open the serial monitor 
}
 
void loop()                     // run over and over again
{
 // Getting the voltage reading from the temperature sensor
 int reading = analogRead(sensorPin);  
 
 // Converting that reading to voltage, for 3.3v arduino use 3.3
 float voltage = reading * 5.0;
 voltage /= 1024.0; 
 
 // Print out the voltage
 Serial.print(voltage); Serial.println(" volts");
 
 // Now print out the temperature
 float temperatureC = (voltage - 0.5) * 100 ;  // Converting from 10 mv per degree wit 500 mV offset
                                               // to degrees ((voltage - 500mV) times 100)
 Serial.print(temperatureC); Serial.println(" degrees C");
 
 // Now convert to Fahrenheit
 float temperatureF = (temperatureC * 9.0 / 5.0) + 32.0;
 Serial.print(temperatureF); Serial.println(" degrees F");
 
 delay(1000);                                     // Waiting a second
}
