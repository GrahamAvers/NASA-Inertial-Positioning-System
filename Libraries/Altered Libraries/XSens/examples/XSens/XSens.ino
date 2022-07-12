#include <XSens.h>
#include <Wire.h>

//Initializing XSens at ADDRESS 0X6B
XSens xsens(0x6B);

void setup() {
  Serial.begin(9600);
  Wire.begin();
  //Wake up process
  xsens.begin();
}

void loop() {
  //Read mesurements (Reads all data given by the AHRS)
  xsens.updateMeasures();


  Serial.println("Outputting Orientation Data :");
  for(int i = 0 ; i < 4; ++i){
    Serial.print(xsens.getQuat()[i]);
    Serial.print(" ");
  }
  Serial.println(" ");

  
  Serial.println("Outputting Acceleration Data :");
  for(int i = 0 ; i < 3; ++i){
    Serial.print(xsens.getAccel()[i]);
    Serial.print(" ");
  }
  Serial.println(" ");

  
  Serial.println("Outputting Rate Of Turn Data :");
  for(int i = 0 ; i < 3; ++i){
    Serial.print(xsens.getRot()[i]);
    Serial.print(" ");
  }
  Serial.println(" ");

  
  Serial.println("Outputting Magnetic Field Vector Data :");
  for(int i = 0 ; i < 3; ++i){
    Serial.print(xsens.getMag()[i]);
    Serial.print(" ");
  }
  Serial.println(" ");
  
  Serial.println("Outputting Yaw :");
  Serial.println(xsens.getHeadingYaw());
  Serial.println(" ");
  Serial.println(" ");
  delay(100);
}
