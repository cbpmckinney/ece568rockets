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
    POST_FLIGHT
} rocket_states_t;

typedef enum
{
    SENSOR_WORKING = 0,
    SENSOR_NOT_WORKING = 1
} sensor_status_t;


union sensorStatus {
    uint8_t byte;
    struct {
        sensor_status_t dof_sensor         : 1;
        sensor_status_t altitude_sensor    : 1;
        sensor_status_t temperature_sensor : 1;
        sensor_status_t gps                : 1;
        sensor_status_t unused1            : 1;
        sensor_status_t unused2            : 1;
        sensor_status_t unused3            : 1;
        sensor_status_t unused4            : 1;
    } bits;
};

