#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_BMP3XX.h"
#include <Adafruit_AHTX0.h>

# define BMP_SCK 13
# define BMP_MISO 12
# define BMP_MOSI 11
# define BMP_CS 10

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BMP3XX bmp;
Adafruit_AHTX0 aht;


void setup() {
  Serial.begin(115200);
  while (!Serial);
  Serial.println("Adafruit BMP390 + AHT20 Test");

  if (!bmp.begin_I2C()) {   // hardware I2C mode, can pass in address & alt Wire
  //if (! bmp.begin_SPI(BMP_CS)) {  // hardware SPI mode  
  //if (! bmp.begin_SPI(BMP_CS, BMP_SCK, BMP_MISO, BMP_MOSI)) {  // software SPI mode
    Serial.println("Could not find a valid BMP3 sensor, check wiring!");
    while (1);
  }
  Serial.println("BMP390 Found");

// Set up oversampling and filter initialization
  bmp.setTemperatureOversampling(BMP3_OVERSAMPLING_8X);
  bmp.setPressureOversampling(BMP3_OVERSAMPLING_4X);
  bmp.setIIRFilterCoeff(BMP3_IIR_FILTER_COEFF_3);
  bmp.setOutputDataRate(BMP3_ODR_50_HZ);

  if (! aht.begin()) {
    Serial.println("Could not find AHT? Check wiring");
    while (1) delay(10);
  }
  Serial.println("AHT10 or AHT20 found");

}

void loop() {
  if (! bmp.performReading()) {
    Serial.println("Failed to perform reading :(");
    return;
  }

  sensors_event_t humidity, temp;
  aht.getEvent(&humidity, &temp);

  Serial.print("AHT Temp = ");
  Serial.print(temp.temperature);
  Serial.println(" deg C");

  Serial.print("AHT Humidity = ");
  Serial.print(humidity.relative_humidity);
  Serial.println ("% rH");

  Serial.print("BMP Temp = ");
  Serial.print(bmp.temperature);
  Serial.println(" deg C");

  Serial.print("BMP Pressure = ");
  Serial.print(bmp.pressure / 100.0);
  Serial.println(" hPa");

  Serial.print("BMP Altitude = ");
  Serial.print(bmp.readAltitude(1020.0));
  Serial.println(" m ASL");

  Serial.println();
  delay(2000);
}
