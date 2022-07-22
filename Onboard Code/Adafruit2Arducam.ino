#include "SerialTransfer.h"

SerialTransfer myTransfer;

struct STRUCT {
  double y;
} testStruct;

#include <Wire.h>
#include <ArduCAM.h>
#include <SPI.h>
#include <SD.h>
#include "memorysaver.h"
//This demo can only work on OV5640_MINI_5MP_PLUS or OV5642_MINI_5MP_PLUS platform.
#if !(defined (OV5640_MINI_5MP_PLUS)||defined (OV5642_MINI_5MP_PLUS))
#error Please select the hardware platform and camera module in the ../libraries/ArduCAM/memorysaver.h file
#endif
#define   FRAMES_NUM    0x00
const int CS = 6;
#define SD_CS 4
int CAMP1 = 17;
//int CAMP2 = 11;
bool is_header = false;
int total_time = 0;
#if defined (OV5640_MINI_5MP_PLUS)
ArduCAM myCAM(OV5640, CS);
#else
ArduCAM myCAM(OV5642, CS);
#endif
uint8_t read_fifo_burst(ArduCAM myCAM);

int sig = 13;

void setup() {
  pinMode(CAMP1, OUTPUT);
  //pinMode(CAMP2, OUTPUT);
  digitalWrite(CAMP1, LOW);
  //digitalWrite(CAMP2, LOW);
  delay(1000);
  digitalWrite(CAMP1, HIGH);
  //digitalWrite(CAMP2, HIGH);
  // put your setup code here, to run once:
  Serial1.begin(115200);
  myTransfer.begin(Serial1);
  pinMode(sig, INPUT);

}
void loop() {
  while (digitalRead(sig) == LOW)
  {
    Serial.println("e");
    digitalWrite(CAMP1, LOW);
    //digitalWrite(CAMP2, LOW);
    delay(2500);
    uint8_t vid, pid;
    uint8_t temp;
    Wire.begin();
    Serial.begin(115200);
    Serial.println(F("ArduCAM Start!"));

    SPI.begin();
    //Reset the CPLD
    myCAM.write_reg(0x07, 0x80);
    delay(500);
    myCAM.write_reg(0x07, 0x00);
    delay(500);


    //Check if the ArduCAM SPI bus is OK
    myCAM.write_reg(ARDUCHIP_TEST1, 0x55);
    temp = myCAM.read_reg(ARDUCHIP_TEST1);
    if (temp != 0x55)
    {
      Serial.println(F("SPI interface Error!"));
      SD.end();
      Wire.end();
      SPI.end();
      digitalWrite(CAMP1, HIGH);
      //digitalWrite(CAMP2, HIGH);
      break;
      delay(1000);
    }
    Serial.println(F("SPI interface OK."));


    myCAM.rdSensorReg16_8(OV5642_CHIPID_HIGH, &vid);
    myCAM.rdSensorReg16_8(OV5642_CHIPID_LOW, &pid);
    while ((vid != 0x56) || (pid != 0x42)) {
      Serial.println(F("Can't find OV5642 module!"));
      delay(1000);
    }
    Serial.println(F("OV5642 detected."));
    //Initialize SD Card
    if (!SD.begin(SD_CS))
    {
      Serial.println(F("SD Card Error!"));
      delay(1000);
      break;
    }
    Serial.println(F("SD Card detected."));
    //Change to JPEG capture mode and initialize the OV5640 module
    myCAM.set_format(JPEG);
    myCAM.InitCAM();
    myCAM.set_bit(ARDUCHIP_TIM, VSYNC_LEVEL_MASK);
    myCAM.clear_fifo_flag();
    myCAM.write_reg(ARDUCHIP_FRAMES, FRAMES_NUM);
    // put your main code here, to run repeatedly:
    myCAM.flush_fifo();
    myCAM.clear_fifo_flag();
    myCAM.OV5642_set_JPEG_size(OV5642_2592x1944); delay(1000);

    //Start capture
    myCAM.start_capture();
    Serial.println(F("start capture."));

    total_time = millis();

    Serial1.end();
    Serial1.begin(115200);
    delay(14);
    if (myTransfer.available())
    {
      myTransfer.rxObj(testStruct);
    }

    while ( !myCAM.get_bit(ARDUCHIP_TRIG, CAP_DONE_MASK));
    total_time = millis() - total_time;

    Serial.println(F("CAM Capture Done."));
    Serial.print(F("capture total_time used (in miliseconds):"));
    Serial.println(total_time, DEC);
    total_time = millis();
    read_fifo_burst(myCAM);
    total_time = millis() - total_time;
    Serial.print(F("save capture total_time used (in miliseconds):"));
    Serial.println(total_time, DEC);
    //Clear the capture done flag
    myCAM.clear_fifo_flag();
    SD.end();
    Wire.end();
    SPI.end();
    delay(1000);
    digitalWrite(CAMP1, HIGH);
    //digitalWrite(CAMP2, HIGH);
    delay(5000);
  }
}


uint8_t read_fifo_burst(ArduCAM myCAM)
{
  uint8_t temp = 0, temp_last = 0;
  uint32_t length = 0;
  static int i = 0;
  static int k = 0;
  char str[8];
  File outFile;
  byte buf[256];
  length = myCAM.read_fifo_length();
  Serial.print(F("The fifo length is :"));
  Serial.println(length, DEC);
  if (length >= MAX_FIFO_SIZE) //8M
  {
    Serial.println("Over size.");
    return 0;
  }
  if (length == 0 ) //0 kb
  {
    Serial.println(F("Size is 0."));
    return 0;
  }
  myCAM.CS_LOW();
  myCAM.set_fifo_burst();//Set fifo burst mode
  i = 0;
  while ( length-- )
  {
    temp_last = temp;
    temp =  SPI.transfer(0x00);
    //Read JPEG data from FIFO
    if ( (temp == 0xD9) && (temp_last == 0xFF) ) //If find the end ,break while,
    {
      buf[i++] = temp;  //save the last  0XD9
      //Write the remain bytes in the buffer
      myCAM.CS_HIGH();
      outFile.write(buf, i);
      //Close the file
      outFile.close();
      Serial.println(F("OK"));
      is_header = false;
      myCAM.CS_LOW();
      myCAM.set_fifo_burst();
      i = 0;
    }
    if (is_header == true)
    {
      //Write image data to buffer if not full
      if (i < 256)
        buf[i++] = temp;
      else
      {
        //Write 256 bytes image data to file
        myCAM.CS_HIGH();
        outFile.write(buf, 256);
        i = 0;
        buf[i++] = temp;
        myCAM.CS_LOW();
        myCAM.set_fifo_burst();
      }
    }
    else if ((temp == 0xD8) & (temp_last == 0xFF))
    {
      is_header = true;
      myCAM.CS_HIGH();
      //Create a avi file
      k = k + 1;
      //Changed k to testStruct.y in line below for file naming to match timestamp
      itoa(testStruct.y, str, 10);
      strcat(str, ".jpg");
      //Open the new file
      outFile = SD.open(str, O_WRITE | O_CREAT | O_TRUNC);
      if (! outFile)
      {
        Serial.println(F("File open failed"));
        break;
      }
      myCAM.CS_LOW();
      myCAM.set_fifo_burst();
      buf[i++] = temp_last;
      buf[i++] = temp;
    }
  }
  myCAM.CS_HIGH();
  return 1;
}
