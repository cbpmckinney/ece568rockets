#pragma once
#include <cstdint>
#include "main.h"
#include "Sensor.h"

class DOFSensor 
{
    public:
        float average;
        float peak;
        per10mData_t per10mDataArray[100];
        bool         isCollectedArray[100]; //MAYBE NOT NEEDED I WILL LEAVE TO RF
        sensor_status_t initialize();
        sensor_status_t setInitialDataValues();
        sensor_status_t collectData();
};