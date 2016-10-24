// Title:  GY-80 3-axis Magnetometer (HMC5883L)
// Author: ScottC http://arduinobasics.blogspot.com
// Version: 1.0
// Arduino IDE: 1.6.9
// Attribution: Inspired by Sparkfun Quickstart guide : https://www.sparkfun.com/tutorials/301

#include <Wire.h> //I2C Arduino Library

#define address 0x1E   //I2C address of the Magnetometer

int x,y,z; //triple axis data
int xCal, yCal, zCal;  //Value used to calibrate the triple axis data.


//=====================================================================================================
void setup(){
  //Initialize Serial and I2C communications
  Serial.begin(9600);
  Wire.begin();

  //Setup the magnetometer
  magSetting(0x00, B01101000); // 8 samples averaged per measurement output. Data output rate = 3Hz (rate is not important in single measurement mode)
  magSetting(0x01, B11000000); // set a gain of 5.6

  calibrateTo1000();  // Set all axis variables to 1000 (at the current sensor location)
}



//=====================================================================================================
void loop(){
 //Read data for each axis (x, y and z)
  getReadings();
  
//Display readings in serial monitor
  printReadings();
}


//=====================================================================================================
void magSetting(byte regLoc, byte setting){
  Wire.beginTransmission(address);
  Wire.write(regLoc); 
  Wire.write(setting);
  Wire.endTransmission();
  delay(10);
}


//=====================================================================================================
void getReadings(){
  magSetting(0x02, 0x01);  //wake up and prepare to take reading (Single measurement mode) - this populates the registers with data
  Wire.requestFrom(address, 6);  //Request 6 bytes. Each axis uses 2 bytes.
  if (Wire.available()>5){
     x = readValue()- xCal;
     z = readValue()- zCal;
     y = readValue()- yCal;
  } else {
    Serial.println("****Error: Less than 6 bytes available for reading*****");
  }
}


//=====================================================================================================
int readValue(){
  int val = Wire.read()<<8; 
      val |= Wire.read();

  return val;
}


//=====================================================================================================
void printReadings(){
 
  //Print out values of each axis
  Serial.print("x: ");
  Serial.print(x);
  Serial.print("  y: ");
  Serial.print(y);
  Serial.print("  z: ");
  Serial.println(z);
}


//=====================================================================================================
void calibrateTo1000(){
  //Calibrate all axis to zero
  xCal = 0;
  yCal = 0;
  zCal = 0;
  getReadings();
  xCal = x-1000;
  yCal = y-1000;
  zCal = z-1000;
}

