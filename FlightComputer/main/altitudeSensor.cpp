#include <Wire.h>
#include <Adafruit_Sensor.h>
#include "altitudeSensor.h"
#include "Adafruit_BMP3XX.h"
#include "main.h"
//#define DEBUG_ALTITUDE 1

# define BMP_SCK 13
# define BMP_MISO 12
# define BMP_MOSI 11
# define BMP_CS 10

#define AVERAGE_DATA_BUFFER_SIZE 1000
Adafruit_BMP3XX bmp;

static uint16_t PRINT_DELAY_MS = 1000; // how often to print the data

static unsigned long timeOfLastPoll;
static bool firstPoll;
static bool isInitialized = false;
static uint16_t printCount = 0; //counter to avoid printing every 10MS sample

void AltitudeSensor::updateAverage(float currVal, float* toUpdate, float averageArray[], unsigned int* countToUse, unsigned int* currIndex, float* sum)
{
    if (*countToUse < AVERAGE_DATA_BUFFER_SIZE) {
        (*sum) += currVal;
        averageArray[(*currIndex)] = currVal; 
        (*countToUse)++;
    }
    else {
        *sum = (*sum) - averageArray[(*currIndex)] + currVal;
        averageArray[(*currIndex)] = currVal;
        *countToUse = AVERAGE_DATA_BUFFER_SIZE;
    }
    *currIndex = ((*currIndex) + 1) % AVERAGE_DATA_BUFFER_SIZE;
    (*toUpdate) = (*sum)/(*countToUse);
}

void AltitudeSensor::updatePeak(float currVal, float* toUpdate, bool invert)
{
  if (invert)
  {
    if( currVal < (*toUpdate))
    (*toUpdate) = currVal;
  }
  else
  {
    if( currVal > (*toUpdate))
    (*toUpdate) = currVal;
  }

}

void AltitudeSensor::updatePer1MDataArray( float currVal, int altitude, float toUpdate[], int* currSavedAltitude, dataPointStatus_t isCollectedArray[] )
{
    if( altitude > *currSavedAltitude )
    {
      if( *currSavedAltitude >= 100 )
          return;
      isCollectedArray[ (*currSavedAltitude) ] = SET_DATA;
      toUpdate[ (*currSavedAltitude)++ ]= currVal;
    }
}
void AltitudeSensor::printPer1MData( float toPrint[], int currSavedAltitude )
{
  for( int i = 0; i < currSavedAltitude; i++ )
  {
      Serial.print("VAL AT: ");
      Serial.print(i);
      Serial.print(" ");
      Serial.println( toPrint[i] );
  }
}

void AltitudeSensor::printArray( float toPrint[], int size )
{
  for( int i = 0; i < size; i++ )
  {
      Serial.print("VAL AT: ");
      Serial.print(i);
      Serial.print(" ");
      Serial.println( toPrint[i] );
  }
}

sensor_status_t AltitudeSensor::initialize()
{
    if( isInitialized)
      return SENSOR_WORKING;

    if (!bmp.begin_I2C()) {   // hardware I2C mode, can pass in address & alt Wire
      Serial.println("Could not find a valid BMP3 sensor, check wiring!");
      return SENSOR_NOT_WORKING;
    }
  // Set up oversampling and filter initialization
    bmp.setTemperatureOversampling(BMP3_OVERSAMPLING_8X);
    bmp.setPressureOversampling(BMP3_OVERSAMPLING_4X);
    bmp.setIIRFilterCoeff(BMP3_IIR_FILTER_COEFF_3);
    bmp.setOutputDataRate(BMP3_ODR_50_HZ);

    return SENSOR_WORKING;
}

sensor_status_t AltitudeSensor::setInitialDataValues()
{

    this->averageTemperature = 0;
    this->averagePressure = 0;
    this->peakTemperature = 0;
    this->peakPressure = 1000000;
    this->currAltitude = 0;
    for (int i = 0; i < 1000; i++) {
        this->averageTemperatureDataArray[i] = 0;
        this->averagePressureDataArray[i] = 0;
    }
    float zero = { 0xff };
    for (int i = 0; i < 100; i++) {
        this->per1mTemperatureDataArray[i] = zero;
        this->per1mPressureDataArray[i] = zero;
        this->isTemperatureCollectedArray[i] = UNSET_DATA; //MAYBE NOT NEEDED I WILL LEAVE TO RF
        this->isPressureCollectedArray[i] = UNSET_DATA;
    }
    this->averageTempCount = 0;
    this->currTempIndex = 0;
    this->temperatureSum = 0;
    this->averagePressureCount = 0;
    this->currPressureIndex = 0;
    this->pressureSum = 0;
    this->savedTemperatureAltitude = 0;
    this->savedPressureAltitude = 0;
    bmp.performReading();
    this->startingAltitude = bmp.readAltitude(1020.0);
    return SENSOR_WORKING;
}

sensor_status_t AltitudeSensor::collectData( )
{
  if( firstPoll )
  {
    timeOfLastPoll = micros();
    firstPoll = false;
  }
  else if( (micros() - timeOfLastPoll) < (10 * 1000) )
  {
    return SENSOR_WORKING; // skip this. It is too fast
  }
  else
  {
    timeOfLastPoll = micros();
  }
  if (! bmp.performReading()) {
  #ifdef DEBUG_ALTITUDE
    Serial.println("Failed to perform reading :(");
  #endif
    return SENSOR_NOT_WORKING;
  }

  this->currAltitude = bmp.readAltitude(1020.0);
  this->currAltitudeDifferenceSinceStart = this->currAltitude - this->startingAltitude;

  #ifdef TEST_MODE_ON_GROUND
  this->currAltitude = simAltitude;
  #endif

  float currTemp = bmp.temperature;
  updateAverage(currTemp, &(this->averageTemperature), averageTemperatureDataArray, &(this->averageTempCount), &(this->currTempIndex), &(this->temperatureSum));
  updatePeak( currTemp,  &(this->peakTemperature), false);
  updatePer1MDataArray( currTemp, currAltitudeDifferenceSinceStart, per1mTemperatureDataArray, &(this->savedTemperatureAltitude), isTemperatureCollectedArray );
  
  float currPressure = (bmp.pressure / 100.0);
  updateAverage(currPressure, &(this->averagePressure), averagePressureDataArray, &(this->averagePressureCount), &(this->currPressureIndex), &(this->pressureSum));
  updatePeak( currPressure,  &(this->peakPressure), true);
  updatePer1MDataArray( currPressure, currAltitudeDifferenceSinceStart, per1mPressureDataArray, &(this->savedPressureAltitude), isPressureCollectedArray );

#ifdef DEBUG_ALTITUDE
if (printCount * 10 >= PRINT_DELAY_MS) {
    Serial.print("PEAK TEMP: ");
    Serial.println(this->peakTemperature);
    Serial.print("AVERAGE TEMP: ");
    Serial.println(this->averageTemperature);
    Serial.print("PEAK PRESSURE: ");
    Serial.println(this->peakPressure);
    Serial.print("AVERAGE PRESSURE: ");
    Serial.println(this->averagePressure);
    Serial.print("CURR ALTITUDE: ");
    Serial.println(this->currAltitude);
    printCount = 0;
  }
  else {
    printCount = printCount + 1;
  }
#endif


  return SENSOR_WORKING;
}