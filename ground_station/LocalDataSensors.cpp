#include "LocalDataSensors.h"
#include <Adafruit_GPS.h>

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

    if (millis() - data_read_timer > 2000) {
        data_read_timer = millis(); // reset the timer
        if (!skipGPS) {
            Serial.print("\nTime: ");
            if (GPS.hour < 10) { Serial.print('0'); }
            Serial.print(GPS.hour, DEC); Serial.print(':');
            if (GPS.minute < 10) { Serial.print('0'); }
            Serial.print(GPS.minute, DEC); Serial.print(':');
            if (GPS.seconds < 10) { Serial.print('0'); }
            Serial.print(GPS.seconds, DEC); Serial.print('.');
            if (GPS.milliseconds < 10) {
                Serial.print("00");
            } else if (GPS.milliseconds > 9 && GPS.milliseconds < 100) {
                Serial.print("0");
            }
            Serial.println(GPS.milliseconds);
            Serial.print("Date: ");
            Serial.print(GPS.day, DEC); Serial.print('/');
            Serial.print(GPS.month, DEC); Serial.print("/20");
            Serial.println(GPS.year, DEC);
            Serial.print("Fix: "); Serial.print((int)GPS.fix);
            Serial.print(" quality: "); Serial.println((int)GPS.fixquality);
            if (GPS.fix) {
                Serial.print("Location: ");
                Serial.print(GPS.latitude, 4); Serial.print(GPS.lat);
                Serial.print(", ");
                Serial.print(GPS.longitude, 4); Serial.println(GPS.lon);
                Serial.print("Speed (knots): "); Serial.println(GPS.speed);
                Serial.print("Angle: "); Serial.println(GPS.angle);
                Serial.print("Altitude: "); Serial.println(GPS.altitude);
                Serial.print("Satellites: "); Serial.println((int)GPS.satellites);
            }
        }
        
        
        if (! bmp.performReading()) {
            Serial.println("Failed to perform BMP reading");
        } else {
            Serial.print("BMP Temperature = ");
            Serial.print(bmp.temperature);
            Serial.println(" *C");
    
            Serial.print("BMP Pressure = ");
            Serial.print(bmp.pressure / 100.0);
            Serial.println(" hPa");
    
            Serial.print("BMP Approx. Altitude = ");
            Serial.print(bmp.readAltitude(SEALEVELPRESSURE_HPA));
            Serial.println(" m");
        }
    
        sensors_event_t humidity, temp;
        aht.getEvent(&humidity, &temp);// populate temp and humidity objects with fresh data
        Serial.print("Temperature: "); Serial.print(temp.temperature); Serial.println(" degrees C");
        Serial.print("Humidity: "); Serial.print(humidity.relative_humidity); Serial.println("% rH");
    }

    return data;

}