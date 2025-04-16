#pragma once
#include <cstdint>
#include "main.h"
#include "Sensor.h"

class AltitudeSensor 
{
    public:
        float averageTemperature;
        float averagePressure;
        float peakTemperature;
        float peakPressure;
        float currAltitude;
        float startingAltitude;
        uint32_t currAltitudeDifferenceSinceStart = 0;
        float averageTemperatureDataArray[1000];
        unsigned int averageTempCount;
        unsigned int currTempIndex;
        float temperatureSum;
        float averagePressureDataArray[1000];
        unsigned int averagePressureCount;
        unsigned int currPressureIndex;
        float pressureSum;
        float per1mTemperatureDataArray[100];
        int savedTemperatureAltitude;
        float per1mPressureDataArray[100];
        int savedPressureAltitude;
        dataPointStatus_t isTemperatureCollectedArray[100]; //MAYBE NOT NEEDED I WILL LEAVE TO RF
        dataPointStatus_t isPressureCollectedArray[100];
        sensor_status_t initialize();
        sensor_status_t setInitialDataValues();
        sensor_status_t collectData( );
        void updateAverage(float currVal, float* toUpdate, float averageArray[], unsigned int* countToUse, unsigned int* currIndex, float* sum);
        void updatePeak(float currVal, float* toUpdate);
        void updatePer1MDataArray( float currVal, int altitude, float toUpdate[], int* currSavedAltitude, dataPointStatus_t isCollectedArray[] );
        void printPer1MData( float toPrint[], int currSavedAltitude );
        void printArray( float toPrint[], int size );
};