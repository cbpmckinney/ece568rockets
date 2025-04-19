#include "LocalDataSensors.h"

Adafruit_GPS GPS(&Wire);

void LocalDataSensors::initializeSensors() {

    if (!bmp.begin_I2C()) {   // hardware I2C mode, can pass in address & alt Wire
        //if (! bmp.begin_SPI(BMP_CS)) {  // hardware SPI mode  
        //if (! bmp.begin_SPI(BMP_CS, BMP_SCK, BMP_MISO, BMP_MOSI)) {  // software SPI mode
          Serial.println("Could not find a valid BMP3 sensor");
    } else {
        Serial.println("Found and began BMP390 Sensor!");
        bmp.setTemperatureOversampling(BMP3_OVERSAMPLING_8X);
        bmp.setPressureOversampling(BMP3_OVERSAMPLING_4X);
        bmp.setIIRFilterCoeff(BMP3_IIR_FILTER_COEFF_3);
        bmp.setOutputDataRate(BMP3_ODR_50_HZ);
    }

    if (!aht.begin()) {
        Serial.println("Could not find a valid AHT20 sensor");
    } else {
        Serial.println("Found and began AHT20 Sensor!");
    }

    GPS.begin(0x10);
    GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
    GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);
    GPS.sendCommand(PGCMD_ANTENNA);
    GPS.println(PMTK_Q_RELEASE);
}

LocalSensorData LocalDataSensors::collectData() {
    
    char c = GPS.read();
    bool skipGPS = false;
    
    if (GPS.newNMEAreceived()) {
        // a tricky thing here is if we print the NMEA sentence, or data
        // we end up not listening and catching other sentences!
        // so be very wary if using OUTPUT_ALLDATA and trying to print out data
        //Serial.println(GPS.lastNMEA()); // this also sets the newNMEAreceived() flag to false
        if (!GPS.parse(GPS.lastNMEA())) // this also sets the newNMEAreceived() flag to false
            skipGPS = true; // we can fail to parse a sentence in which case we should just wait for another
      }

    if (millis() - data_read_timer > 1000) {
        data_read_timer = millis(); // reset the timer
        if (!skipGPS) {
            data.gps_hour = GPS.hour;
            data.gps_minute = GPS.minute;
            data.gps_seconds = GPS.seconds;
            data.gps_milliseconds = GPS.milliseconds;
            data.gps_day = GPS.day;
            data.gps_month = GPS.month;
            data.gps_year = GPS.year;
            data.gps_fix = GPS.fix;
            data.gps_fixquality = GPS.fixquality;
            data.gps_latitude = GPS.latitude;
            data.gps_lat = GPS.lat;
            data.gps_longitude = GPS.longitude;
            data.gps_lon = GPS.lon;
            data.gps_speed = GPS.speed;
            data.gps_angle = GPS.angle;
            data.gps_altitude = GPS.altitude;
            data.gps_satellites = GPS.satellites;
        }
        
        
        if (! bmp.performReading()) {
            Serial.println("Failed to perform BMP reading");
        } else {
            data.bmp_temperature = ((bmp.temperature*1.8)+32);
            data.bmp_pressure = bmp.pressure/100.0;
            data.bmp_altitude = bmp.readAltitude(SEALEVELPRESSURE_HPA);
        }
    
        sensors_event_t humidity, temp;
        aht.getEvent(&humidity, &temp);// populate temp and humidity objects with fresh data
        
        data.aht_temp = ((temp.temperature*1.8)+32);
        data.aht_humidity = humidity.relative_humidity;
    }

    return data;

}