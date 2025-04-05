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
        float averageTemperatureDataArray[1000];
        unsigned int averageTempCount;
        unsigned int currTempIndex;
        float temperatureSum;
        float averagePressureDataArray[1000];
        unsigned int averagePressureCount;
        unsigned int currPressureIndex;
        float pressureSum;
        per10mData_t per10mTemperatureDataArray[100];
        int savedTemperatureAltitude;
        per10mData_t per10mPressureDataArray[100];
        int savedPressureAltitude;
        bool         isCollectedArray[100]; //MAYBE NOT NEEDED I WILL LEAVE TO RF
        sensor_status_t initialize();
        sensor_status_t setInitialDataValues();
        sensor_status_t collectData( );
        void updateAverage(float currVal, float* toUpdate, float averageArray[], unsigned int* countToUse, unsigned int* currIndex, float* sum);
        void updatePeak(float currVal, float* toUpdate);
        void updatePer10MDataArray( float currVal, int altitude, per10mData_t toUpdate[], int* currSavedAltitude );
        void printPer10MData( per10mData_t toPrint[], int currSavedAltitude );
        void printArray( float toPrint[], int size );
};