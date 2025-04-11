#include <Wire.h>
#include <Adafruit_Sensor.h>
#include "temperatureSensor.h"
#include <Adafruit_AHTX0.h>

//#define DEBUG_TEMPERATURE_SENSOR 1

#define AVERAGE_DATA_BUFFER_SIZE 1000
Adafruit_AHTX0 aht;

static unsigned long timeOfLastPoll;
static bool firstPoll;
static bool isInitialized = false;
static unsigned int count = 0;

static uint16_t PRINT_DELAY_MS = 1000; // how often to print the data
static uint16_t printCount = 0; //counter to avoid printing every 10MS sample

void TemperatureSensor::updateAverage(float currVal, float* toUpdate, float averageArray[], unsigned int* countToUse, unsigned int* currIndex, float* sum)
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

void TemperatureSensor::updatePeak(float currVal, float* toUpdate)
{
  if( currVal > (*toUpdate))
    (*toUpdate) = currVal;
}

void TemperatureSensor::updatePer1mDataArray( float currVal, int altitude, float toUpdate[], int* currSavedAltitude, dataPointStatus_t isCollectedArray[] )
{
    if( altitude > *currSavedAltitude )
    {
      if( *currSavedAltitude >= 100 )
          return;
      isCollectedArray[(*currSavedAltitude) ] = SET_DATA;
      toUpdate[ (*currSavedAltitude)++ ] = currVal;
    }
}
void TemperatureSensor::printPer1mData( float toPrint[], int currSavedAltitude )
{
  for( int i = 0; i < currSavedAltitude; i++ )
  {
      Serial.print("VAL AT: ");
      Serial.print(i);
      Serial.print(" ");
      Serial.println( toPrint[i] );
  }
}

sensor_status_t TemperatureSensor::initialize()
{
    if( isInitialized)
      return SENSOR_WORKING;

    if (!aht.begin())
    {
        Serial.println("No temperature detected");
        return SENSOR_NOT_WORKING;
    }
    return SENSOR_WORKING;
}

sensor_status_t TemperatureSensor::setInitialDataValues()
{
    this->averageTemperature = 0;
    this->averageHumidity = 0;
    this->peakTemperature = 0;
    this->peakHumidity = 0;
    for (int i = 0; i < 1000; i++) {
        this->averageTemperatureDataArray[i] = 0;
        this->averageHumidityDataArray[i] = 0;
    }
    float zero = { 0xff };
    for (int i = 0; i < 100; i++) {
        this->per1mTemperatureDataArray[i] = zero;
        this->per1mHumidityDataArray[i] = zero;
        this->isTemperatureCollectedArray[i] = UNSET_DATA; //MAYBE NOT NEEDED I WILL LEAVE TO RF
        this->isHumidityCollectedArray[i] = UNSET_DATA;
    }
    this->averageTempCount = 0;
    this->currTempIndex = 0;
    this->temperatureSum = 0;
    this->averageHumidityCount = 0;
    this->currHumidityIndex = 0;
    this->humiditySum = 0;
    this->savedTemperatureAltitude = 0;
    this->savedHumidityAltitude = 0;
    return SENSOR_WORKING;
}

sensor_status_t TemperatureSensor::collectData( int altitude )
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

  sensors_event_t humidity, temp;
  aht.getEvent(&humidity, &temp);

  float currTemp = temp.temperature;
  updateAverage(currTemp, &(this->averageTemperature), averageTemperatureDataArray, &(this->averageTempCount), &(this->currTempIndex), &(this->temperatureSum));
  updatePeak( currTemp,  &(this->peakTemperature));
  updatePer1mDataArray( currTemp, altitude, per1mTemperatureDataArray, &(this->savedTemperatureAltitude), isTemperatureCollectedArray );
  
  float currHumidity = humidity.relative_humidity;
  updateAverage(currHumidity, &(this->averageHumidity), averageHumidityDataArray, &(this->averageHumidityCount), &(this->currHumidityIndex), &(this->humiditySum));
  updatePeak( currHumidity,  &(this->peakHumidity));
  updatePer1mDataArray( currHumidity, altitude, per1mHumidityDataArray, &(this->savedHumidityAltitude), isHumidityCollectedArray );

#ifdef DEBUG_TEMPERATURE_SENSOR
  if (printCount * 10 >= PRINT_DELAY_MS) {
    Serial.print("PEAK TEMP: ");
    Serial.println(this->peakTemperature);
    Serial.print("AVERAGE TEMP: ");
    Serial.println(this->averageTemperature);
    Serial.print("PEAK HUMIDITY: ");
    Serial.println(this->peakHumidity);
    Serial.print("AVERAGE HUMIDITY: ");
    Serial.println(this->averageHumidity);
    
    printCount = 0;
  }
  else {
    printCount = printCount + 1;
  }
#endif


  return SENSOR_WORKING;
}