//================================================================================================
//       Title: GY-80 3-axis Magnetometer (HMC5883L)
//      Author: ScottC,  http://arduinobasics.blogspot.com
//     Version: 1.0
//        Date: 24th Oct 2016
// Arduino IDE: 1.6.9
// Attribution: Inspired by Sparkfun Quickstart guide : https://www.sparkfun.com/tutorials/301
//   Datasheet: https://cdn.sparkfun.com/datasheets/Sensors/Magneto/HMC5883L-FDS.pdf
//              or
//              https://cdn-shop.adafruit.com/datasheets/HMC5883L_3-Axis_Digital_Compass_IC.pdf
//                
// Description: This is a simple sketch which aims to quickly get you up and running with the GY-80 Magnetometer sensor.
//              It does not go into too much detail, however, this sketch can be modified easily to
//              accomodate changes in
//                   a) Output Rate (Hz)
//                   b) Number of samples per measurement output
//                   c) Amount of Gain
//             This sketch uses Single measurement mode rather than continuous measurement mode.
//             Single measurement mode data output can be quicker than continuous measurement mode.
//             See datasheet for continuous measurement mode.
//================================================================================================

#include <Wire.h>      //I2C Arduino Library - inbuilt into Arduino IDE (no library download required)
#define address 0x1E   //I2C address of the Magnetometer

//Global variables
int x,y,z;                   //Variables used to hold the triple axis data
int xCal=0, yCal=0, zCal=0;  //Variables used to calibrate the triple axis data.
int calValue = 1000;         //On startup, all axis variables will equal this number.



//=====================================================================================================
// setup(): 
//    This runs once only. 
//    Setup Serial and I2C communication. 
//    Configure the Magnetometer to use 8 samples (averaged) per measurement output. (Register A)
//    Change the output rate of Magnetometer to 3Hz : This is only useful in continuous-measurement mode. (Register A)
//    Set the gain to +/- 2.5 Ga, which changes the digital resolution to 1.52 (Register B)
//    Calibrate all axis values to 1000. Which means that all axis variables will equal 1000 on startup, no matter what position the sensor is in.
//-----------------------------------------------------------------------------------------------------
void setup(){
  //Initialize Serial and I2C communications
  Serial.begin(9600);
  Wire.begin();

  //Setup the magnetometer 
  magSetting(0x00, B01101000); // Magnetometer settings associated with Register A. See datasheet for acceptable values. 
  magSetting(0x01, B01100000); // Magnetometer settings associated with Register B. See datasheet for acceptable values. 
  calibrateMag();              // Set or calibrate all axis variables to the calValue.
}



//=====================================================================================================
// loop():
//    This code runs in an endless loop
//    getReadings(): is used to read the data for each axis and assign these values to variables x,y and z.
//    printReadings(): is used to display these values in the Serial Monitor window.
//-----------------------------------------------------------------------------------------------------
void loop(){
  getReadings();
  printReadings();
}


//=====================================================================================================
// magSetting(regLoc, setting):
//    regLoc: is the register address location that we want to interact with (RegA = 0x00, RegB = 0x01, Mode = 0x02)
//    setting: is the 8bit code used to configure the magnetometer (see datasheet)
//
//    This function allows you to configure the magnetometer to suit your specific application
//    A delay of 10 is used, but anything above 7 is recommended.
//-----------------------------------------------------------------------------------------------------
void magSetting(byte regLoc, byte setting){
  Wire.beginTransmission(address);
  Wire.write(regLoc); 
  Wire.write(setting);
  Wire.endTransmission();
  delay(10);
}


//=====================================================================================================
// getReadings():
//    It is necessary to send the bytes (0x02 and 0x01) to instruct the magnetometer to prepare for single measurement mode
//    Request 6 bytes from the magnetometer and then read 2 bytes per axis (assigned to variables x, y and z).
//    The xCal, yCal, and zCal variables adjust the magnetometer's readings relative to it's initial state.
//    Set the global "calValue" variable to 0 if you don't want to "calibrate" the magnetometer on startup.
//    The magnetometer should send back 6 bytes of positional data, if not, an Error message will be printed to the serial monitor
//-----------------------------------------------------------------------------------------------------
void getReadings(){
  magSetting(0x02, 0x01);        //prepare to take reading (Single measurement mode) - this populates the registers with data
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
// readValue():
//    This reads the magnetometer's data registers - 2 bytes at a time.
//    All 6 data registers must be read properly before new data can be placed into any of these data registers
//    This function reads two bytes (8bit + 8bit) and joins them together to make a 16bit integer value.
//    This value is returned and assigned to one of the axis variables x, y and/or z.
//    With every read() called, the data register pointer is incremented.
//    When magSetting(0x02, 0x01) is called in the getReadings() function, the pointer is reset to the first data register (03).
//-----------------------------------------------------------------------------------------------------
int readValue(){
  int val = Wire.read()<<8; 
      val |= Wire.read();

  return val;
}


//=====================================================================================================
// printReadings():
//    This funtion is used to print the 3 axis values (x, y and z) to the Serial monitor window.
//-----------------------------------------------------------------------------------------------------
void printReadings(){
  Serial.print("x: ");
  Serial.print(x);
  Serial.print("  y: ");
  Serial.print(y);
  Serial.print("  z: ");
  Serial.println(z);
}


//=====================================================================================================
// calibrateMag():
//    This function is used to calibrate or set each axis value to calValue.
//    calValue is defined in the global variables section (=1000)
//    After calibration, each axis variable (x, y and z) will equal the calValue, no matter what position the magnetometer is in.
//-----------------------------------------------------------------------------------------------------
void calibrateMag(){
  getReadings();
  xCal = x-calValue;
  yCal = y-calValue;
  zCal = z-calValue;
}

