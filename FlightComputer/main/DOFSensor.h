#pragma once
#include <cstdint>
#include "main.h"
#include "Sensor.h"

#define AVERAGE_DATA_BUFFER_SIZE 1000

class DOFSensor 
{
    public:
        float average;
        float peak;
        float averageDataArray[1000];
        bool doneFlying;
        per10mData_t per10mDataArray[100];
        bool         isCollectedArray[100]; //MAYBE NOT NEEDED I WILL LEAVE TO RF
        sensor_status_t initialize();
        sensor_status_t setInitialDataValues();
        sensor_status_t collectData( int altitude );
        void updateAverage(float currVal);
        void updatePeak(float currVal);
        void updatePer10MDataArray( float currVal, int altitude );
        void printPer10MData();
};