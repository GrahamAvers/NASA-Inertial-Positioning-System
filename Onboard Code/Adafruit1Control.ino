#include "SerialTransfer.h"

SerialTransfer myTransfer;

struct STRUCT {
  double y;
} testStruct;

#include <Wire.h>
#include <SparkFun_MS5803_I2C.h>
MS5803 sensor(ADDRESS_HIGH);
float temperature_c, temperature_f;
double pressure_abs, pressure_relative, altitude_delta, pressure_baseline;
double base_altitude = 1655.0;

#include <SPI.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_BME680.h"
#define BME_SCK 13
#define BME_MISO 12
#define BME_MOSI 11
#define BME_CS 10
#define SEALEVELPRESSURE_HPA (1013.25)
Adafruit_BME680 bme(BME_CS, BME_MOSI, BME_MISO,  BME_SCK);

#include <SD.h>
const int chipSelect = 4;
int swch = 6;
int xsen = 15;
int acam = 16;

void setup() {
  pinMode(swch, INPUT);
  pinMode(xsen, OUTPUT);
  pinMode(acam, OUTPUT);
  Wire.begin();
  Serial.begin(115200);
  sensor.reset();
  sensor.begin();
  pressure_baseline = sensor.getPressure(ADC_4096);

  Serial1.begin(115200);
  myTransfer.begin(Serial1);

  while (!bme.begin()) {
    Serial.println("Could not find a valid BME680 sensor, check wiring!");
    delay(500);
  }

  bme.setTemperatureOversampling(BME680_OS_8X);
  bme.setHumidityOversampling(BME680_OS_2X);
  bme.setPressureOversampling(BME680_OS_4X);
  bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
  bme.setGasHeater(320, 150); // 320*C for 150 ms

  while (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    delay(1000);
  }
  Serial.println("card ready.");
  Serial.println("");

  digitalWrite(xsen, HIGH);
  digitalWrite(acam, HIGH);
  digitalRead(swch);
}

void loop()
{
  while (!SD.begin(chipSelect)) {
  }
  if (digitalRead(swch) == LOW)
  {
    digitalWrite(acam, LOW);
    digitalWrite(xsen, LOW);
    temperature_c = sensor.getTemperature(CELSIUS, ADC_512);
    temperature_f = sensor.getTemperature(FAHRENHEIT, ADC_512);
    pressure_abs = sensor.getPressure(ADC_4096);
    pressure_relative = sealevel(pressure_abs, base_altitude);
    altitude_delta = altitude(pressure_abs , pressure_baseline);
    //    Serial.println("SparkFun Sensor:");
    //    Serial.print("Temperature C = ");
    //    Serial.println(temperature_c);
    //    Serial.print("Temperature F = ");
    //    Serial.println(temperature_f);
    //    Serial.print("Pressure abs (mbar)= ");
    //    Serial.println(pressure_abs);
    //    Serial.print("Pressure relative (mbar)= ");
    //    Serial.println(pressure_relative);
    //    Serial.print("Altitude change (m) = ");
    Serial.println(altitude_delta);
    //    Serial.println(" ");//padding between outputs

    if (! bme.performReading()) {
      Serial.println("Failed to perform reading :(");
      return;
    }
    //    Serial.println("BME Sensor:");
    //    Serial.print("Temperature = ");
    //    Serial.print(bme.temperature);
    //    Serial.println(" *C");
    //    Serial.print("Pressure = ");
    //    Serial.print(bme.pressure / 100.0);
    //    Serial.println(" hPa");
    //    Serial.print("Humidity = ");
    //    Serial.print(bme.humidity);
    //    Serial.println(" %");
    //    Serial.print("Gas = ");
    //    Serial.print(bme.gas_resistance / 1000.0);
    //    Serial.println(" KOhms");
    //    Serial.print("Approx. Altitude = ");
    Serial.println(bme.readAltitude(SEALEVELPRESSURE_HPA));
    //    Serial.println(" m");
    //    Serial.println();

    Serial1.end();
    Serial1.begin(115200);
    delay(19);
    if (myTransfer.available())
    {
      myTransfer.rxObj(testStruct);
      Serial.println(testStruct.y);
      Serial.println();
    }

    File dataFile = SD.open("datalog.txt", FILE_WRITE);
    dataFile.print(altitude_delta);
    dataFile.print(", ");
    dataFile.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
    dataFile.print(", ");
    dataFile.println(testStruct.y);
    dataFile.close();
  }
  else {
    digitalWrite(acam, HIGH);
    digitalWrite(xsen, HIGH);
    delay(1000);
    Serial.println("e");
  }
}
double sealevel(double P, double A)
{
  return (P / pow(1 - (A / 44330.0), 5.255));
}

double altitude(double P, double P0)
{
  return (44330.0 * (1 - pow(P / P0, 1 / 5.255)));
}
