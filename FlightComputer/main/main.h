#pragma once
#include <cstdint>
typedef enum
{
    BOOTUP,
    SAFE,
    ARM,
    READY_FOR_LAUNCH,
    LAUNCH,
    FLIGHT,
    RECOVERY,
    POST_FLIGHT,
    POWERONFAILURE
} rocket_states_t;

typedef enum
{
    SENSOR_WORKING = 0,
    SENSOR_NOT_WORKING = 1
} sensor_status_t;

typedef enum
{
   UNSET_DATA       = 0,
   SET_DATA         = 1,
   TRANSMITTED_DATA = 2
} dataPointStatus_t;

union sensorStatus {
    uint8_t byte;
    struct {
        sensor_status_t dof_sensor         : 1;
        sensor_status_t altitude_sensor    : 1;
        sensor_status_t temperature_sensor : 1;
        sensor_status_t gps                : 1;
        sensor_status_t RFtransmitter      : 1;
        sensor_status_t unused2            : 1;
        sensor_status_t unused3            : 1;
        sensor_status_t unused4            : 1;
    } bits;
};

typedef struct
{
    float latitude;
    float longitude;
    char lat;
    char lon;
    float gpsalt;
} GPSDataStorage;


#define TEST_MODE_ON_GROUND 1
#ifdef TEST_MODE_ON_GROUND
extern uint8_t simAltitude;
#endif
