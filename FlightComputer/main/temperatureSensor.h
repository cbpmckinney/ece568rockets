#pragma once
#include <cstdint>
#include "main.h"
#include "Sensor.h"

class TemperatureSensor 
{
    public:
        float averageTemperature;
        float averageHumidity;
        float peakTemperature;
        float peakHumidity;
        float averageTemperatureDataArray[1000];
        unsigned int averageTempCount;
        unsigned int currTempIndex;
        float temperatureSum;
        float averageHumidityDataArray[1000];
        unsigned int averageHumidityCount;
        unsigned int currHumidityIndex;
        float humiditySum;
        float per1mTemperatureDataArray[100];
        int savedTemperatureAltitude;
        float per1mHumidityDataArray[100];
        int savedHumidityAltitude;
        dataPointStatus_t  isTemperatureCollectedArray[100]; //MAYBE NOT NEEDED I WILL LEAVE TO RF
        dataPointStatus_t  isHumidityCollectedArray[100];
        sensor_status_t initialize();
        sensor_status_t setInitialDataValues();
        sensor_status_t collectData( int altitude );
        void updateAverage(float currVal, float* toUpdate, float averageArray[], unsigned int* countToUse, unsigned int* currIndex, float* sum);
        void updatePeak(float currVal, float* toUpdate);
        void updatePer1mDataArray( float currVal, int altitude, float toUpdate[], int* currSavedAltitude, dataPointStatus_t isCollectedArray[] );
        void printPer1mData( float toPrint[], int currSavedAltitude );
};