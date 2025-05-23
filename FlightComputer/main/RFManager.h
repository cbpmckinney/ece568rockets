#pragma once
#include "main.h"
#include "Sensor.h"
#include "DOFSensor.h"
#include "altitudeSensor.h"
#include "temperatureSensor.h"
#include "RFManager.h"
#include <Adafruit_GPS.h>

typedef enum
{
    STATUS,
    FAIL,
    ARM_PACKET,
    RTL_PACKET,
    M_DATA_PACKET, //1 m data
    LAUNCH_PACKET,
    PEAK_DATA_PACKET,
    AVERAGE_DATA_PACKET,
    ALTITUDE_PACKET,
    VERIFY_DATA,
    REINITIALIZE,
    GPS_PACKET

} packet_type_t;

typedef enum
{
    VELOCITY,
    ALTITUDE_TEMPERATURE,
    ALTITUDE_PRESSURE,
    TEMPERATURE_TEMPERATURE,
    TEMPERATURE_HUMIDITY,
    ALTITUDE,
    GPS_DATA
} data_type_t;

class RFManager 
{
    public:
      sensor_status_t initialize();
      void sendStatus( sensorStatus currStatus, rocket_states_t currState  );
      bool receivedCommand( packet_type_t commandToReceive );
      sensor_status_t send10m( );
      void transmitGPS(GPSDataStorage &RocketGPSData);
      sensor_status_t transmitData(DOFSensor& dofSensor, AltitudeSensor& altitude_sensor, TemperatureSensor& temperature_sensor);
      void transmit1m( float data[], dataPointStatus_t isCollected[], data_type_t dataType  );
      void transmitAltitude( float data, data_type_t dataType );
      void transmitAverage( float data, data_type_t dataType );
      void transmitPeak( float data, data_type_t dataType );
      void retransmitData( );
      void printDataType( char toprint);
      int8_t sentData[ 200000 ];
      uint32_t sentDataIndex;
    private:
      void tx(char *str_num, int size);
      void txNoSave(char *str_num, int size);
      char* rx();
};

// STATUS: 3 BYTES BYTE 0 = 0 BYTE 1 = sensorStatus.BYTE BYTE 2 = CURR ROCKET STATE