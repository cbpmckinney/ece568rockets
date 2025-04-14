#pragma once
#include <cstdint>
#include "main.h"
#include "Sensor.h"

class DOFSensor 
{
    public:
        float average;
        float peak;
        float averageDataArray[1000];
        bool doneFlying;
        bool startedFlying;
        float per1mDataArray[100];
        dataPointStatus_t         isCollectedArray[100]; //MAYBE NOT NEEDED I WILL LEAVE TO RF
        bool         isFlying();
        sensor_status_t initialize();
        sensor_status_t setInitialDataValues();
        sensor_status_t collectData( int altitude );
        void updateAverage(float currVal);
        void updatePeak(float currVal);
        void updatePer1mDataArray( float currVal, int altitude );
        void printPer1mData();
};