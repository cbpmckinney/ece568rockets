#pragma once
#include <cstdint>
#include "main.h"

class Sensor 
{
    public:
        float average;
        float peak;
        float per1mDataArray[100];
        dataPointStatus_t         isCollectedArray[100]; //MAYBE NOT NEEDED I WILL LEAVE TO RF
        sensor_status_t initialize();
        sensor_status_t setInitialDataValues();
        sensor_status_t collectData();

};