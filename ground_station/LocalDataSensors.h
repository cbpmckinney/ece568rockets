#ifndef _LocalDataSensors_
#define _LocalDataSensors_

#include <Adafruit_BMP3XX.h>
#include <Adafruit_AHTX0.h>
#include <Arduino.h>
#include <memory>

#define SEALEVELPRESSURE_HPA (1013.25)

struct LocalSensorData {
    float aht_temp;
    float aht_humidity;
    double bmp_temperature;
    double bmp_pressure;
    float bmp_altitude;
};

class LocalDataSensors {
    public:
        Adafruit_BMP3XX bmp = Adafruit_BMP3XX();
        Adafruit_AHTX0 aht = Adafruit_AHTX0();
        uint32_t data_read_timer = millis();
        LocalSensorData data;
        LocalSensorData collectData();

        void initializeSensors();
};

#endif //_LocalDataSensors_