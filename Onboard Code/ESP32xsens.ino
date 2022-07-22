#include "SerialTransfer.h"

#define RXD2 16
#define TXD2 17

SerialTransfer myTransfer;

struct STRUCT {
  double y;
} testStruct;

TaskHandle_t Task1;
TaskHandle_t Task2;

#include "FS.h"
#include "SD.h"
#include "SPI.h"

#include <Wire.h>
#include <XSens.h>
#define VBATPIN A7

XSens xsens(0x6B);

const int chipSelect = 5;
double datas1[100][5];
double datas2[100][5];
int READY = 4;
int sig = 15;
String dataMessage;
int n = 0;
int d = 0;
int set = 0;

void setup() {
  Wire.begin();
  Serial.begin(115200);
  xsens.begin();
  SD.begin(chipSelect);
  pinMode(READY, INPUT);
  pinMode(sig, INPUT);

  Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);
  myTransfer.begin(Serial2);

  while (!SD.begin()) {
    Serial.println("Card Mount Failed");
  }
  uint8_t cardType = SD.cardType();

  if (cardType == CARD_NONE) {
    Serial.println("No SD card attached");
    return;
  }

  Serial.print("SD Card Type: ");
  if (cardType == CARD_MMC) {
    Serial.println("MMC");
  } else if (cardType == CARD_SD) {
    Serial.println("SDSC");
  } else if (cardType == CARD_SDHC) {
    Serial.println("SDHC");
  } else {
    Serial.println("UNKNOWN");
  }

  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  Serial.printf("SD Card Size: %lluMB\n", cardSize);
  deleteFile(SD, "/DATALOG.txt");
  writeFile(SD, "/DATALOG.txt", "NEW DATALOG START\n");

  Serial.printf("Total space: %lluMB\n", SD.totalBytes() / (1024 * 1024));
  Serial.printf("Used space: %lluMB\n", SD.usedBytes() / (1024 * 1024));

  //create a task that will be executed in the Task1code() function, with priority 1 and executed on core 1
  xTaskCreatePinnedToCore(
    Task1code,   /* Task function. */
    "Task1",     /* name of task. */
    10000,       /* Stack size of task */
    NULL,        /* parameter of the task */
    1,           /* priority of the task */
    &Task1,      /* Task handle to keep track of created task */
    1);          /* pin task to core 1 */
  delay(500);

  //create a task that will be executed in the Task2code() function, with priority 0 and executed on core 0
  xTaskCreatePinnedToCore(
    Task2code,   /* Task function. */
    "Task2",     /* name of task. */
    10000,       /* Stack size of task */
    NULL,        /* parameter of the task */
    0,           /* priority of the task */
    &Task2,      /* Task handle to keep track of created task */
    0);          /* pin task to core 0 */
  delay(500);
}

void Task1code( void * pvParameters ) {
  for (;;) {
    if (digitalRead(sig) == LOW)
    {
      if (digitalRead(READY) == HIGH)
      {
        xsens.updateMeasures();
        testStruct.y = double(xsens.getsamptime()[0]);
        myTransfer.sendDatum(testStruct);
        if (n <= 99)
        {
          datas1[d][0] = double(xsens.getsamptime()[0]);
          datas1[d][1] = double(xsens.getFAccel()[0]);
          datas1[d][2] = double(xsens.getFAccel()[1]);
          datas1[d][3] = double(xsens.getFAccel()[2]);
          datas1[d][4] = double(xsens.getHeadingYaw());
        }
        if (n >= 100)
        {
          datas2[d][0] = double(xsens.getsamptime()[0]);
          datas2[d][1] = double(xsens.getFAccel()[0]);
          datas2[d][2] = double(xsens.getFAccel()[1]);
          datas2[d][3] = double(xsens.getFAccel()[2]);
          datas2[d][4] = double(xsens.getHeadingYaw());
        }
        d = d + 1;
        n = n + 1;
      }
      if (d == 100)
      {
        d = 0;
      }
      if (n == 100)
      {
        set = 1;
      }
      if (n == 200)
      {
        set = 2;
        n = 0;
      }
    }
  }
}

void Task2code( void * pvParameters ) {
  for (;;) {
    if (set == 1)
    {
      dataMessage = String(datas1[0][0]) + "\r\n";;
      appendFileChunk1(SD, "/DATALOG.txt", dataMessage.c_str());

      //Needed?
      for (int i = 0; i < 100; i++) {
        for (int j = 0; j < 5; j++) {
          datas1[i][j] = 0;
        }
      }
      //Needed?

      set = 0;
    }
    if (set == 2)
    {
      dataMessage = String(datas2[0][0]) + "\r\n";;
      appendFileChunk2(SD, "/DATALOG.txt", dataMessage.c_str());

      //Needed?
      for (int i = 0; i < 100; i++) {
        for (int j = 0; j < 5; j++) {
          datas2[i][j] = 0;
        }
      }
      //Needed?

      set = 0;
    }
  }
}

void loop() {
}

void writeFile(fs::FS &fs, const char * path, const char * message) {
  Serial.printf("Writing file: %s\n", path);

  File file = fs.open(path, FILE_WRITE);
  if (!file) {
    Serial.println("Failed to open file for writing");
    return;
  }
  if (file.print(message)) {
    Serial.println("File written");
  } else {
    Serial.println("Write failed");
  }
  file.close();
}

void appendFileChunk1(fs::FS &fs, const char * path, const char * message) {
  Serial.println("Chunk 1 writing");
  File file = fs.open("/DATALOG.txt", FILE_APPEND);
  if (!file) {
    Serial.println("Failed to open file for appending");
    return;
  }
  for (int i = 0; i < 100; i++)
  {
    dataMessage = String(datas1[i][0]) + "," + String(datas1[i][1], 6) + "," + String(datas1[i][2], 6) + "," + String(datas1[i][3], 6) + "," + String(datas1[i][4], 6) + "\r\n";;
    if (file.print(dataMessage.c_str())) {
    } else {
      Serial.println("Append failed");
    }
  }
  file.close();
}

void appendFileChunk2(fs::FS &fs, const char * path, const char * message) {
  Serial.println("Chunk 2 writing");
  File file = fs.open("/DATALOG.txt", FILE_APPEND);
  if (!file) {
    Serial.println("Failed to open file for appending");
    return;
  }
  for (int i = 0; i < 100; i++)
  {
    dataMessage = String(datas2[i][0]) + "," + String(datas2[i][1], 6) + "," + String(datas2[i][2], 6) + "," + String(datas2[i][3], 6) + "," + String(datas2[i][4], 6) + "\r\n";;
    if (file.print(dataMessage.c_str())) {
    } else {
      Serial.println("Append failed");
    }
  }
  file.close();
}

void deleteFile(fs::FS &fs, const char * path) {
  Serial.printf("Deleting file: %s\n", path);
  if (fs.remove(path)) {
    Serial.println("File deleted");
  } else {
    Serial.println("Delete failed");
  }
}
