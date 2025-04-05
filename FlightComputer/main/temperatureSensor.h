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
        per10mData_t per10mTemperatureDataArray[100];
        int savedTemperatureAltitude;
        per10mData_t per10mHumidityDataArray[100];
        int savedHumidityAltitude;
        bool         isCollectedArray[100]; //MAYBE NOT NEEDED I WILL LEAVE TO RF
        sensor_status_t initialize();
        sensor_status_t setInitialDataValues();
        sensor_status_t collectData( int altitude );
        void updateAverage(float currVal, float* toUpdate, float averageArray[], unsigned int* countToUse, unsigned int* currIndex, float* sum);
        void updatePeak(float currVal, float* toUpdate);
        void updatePer10MDataArray( float currVal, int altitude, per10mData_t toUpdate[], int* currSavedAltitude );
        void printPer10MData( per10mData_t toPrint[], int currSavedAltitude );
};