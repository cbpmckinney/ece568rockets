#ifndef _LocalDataSensors_
#define _LocalDataSensors_

#include <Adafruit_BMP3XX.h>
#include <Adafruit_AHTX0.h>
#include <Adafruit_GPS.h>
#include <Arduino.h>
#include <memory>

#define SEALEVELPRESSURE_HPA (1013.25)

struct LocalSensorData {
    // AHT20
    float aht_temp;
    float aht_humidity;
    //BMP390
    double bmp_temperature;
    double bmp_pressure;
    float bmp_altitude;
    //GPS
    uint8_t gps_hour;
    uint8_t gps_minute;
    uint8_t gps_seconds;
    uint8_t gps_milliseconds;
    uint8_t gps_day;
    uint8_t gps_month;
    uint8_t gps_year;
    int gps_fix;
    int gps_fixquality;
    nmea_float_t gps_latitude;
    char gps_lat;
    nmea_float_t gps_longitude;
    char gps_lon;
    nmea_float_t gps_speed;
    nmea_float_t gps_angle;
    nmea_float_t gps_altitude;
    uint8_t gps_satellites;
    // Combined
    double average_temp;
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