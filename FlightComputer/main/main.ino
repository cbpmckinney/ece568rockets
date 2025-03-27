#include "main.h"
#include "Sensor.h"

void setup() {
  // put your setup code here, to run once:
}

void loop() {
  static rocket_states_t currRocketState = BOOTUP;
  static sensorStatus statusByte;
  static Sensor dofSensor;
  static Sensor altitude_sensor;
  static Sensor temperature_sensor;
  static Sensor gps;
  /* DECLARE A STATIC OF YOUR CLASS FOR SENSOR HERE */

  switch( currRocketState )
  {
      case BOOTUP:
        statusByte.bits.dof_sensor         = dofSensor.initialize();
        statusByte.bits.altitude_sensor    = altitude_sensor.initialize();
        statusByte.bits.temperature_sensor = temperature_sensor.initialize();
        statusByte.bits.gps                = gps.initialize(); //use to initialize altitude sensor? IDK
        if(statusByte.byte == 0)
        {
          currRocketState = SAFE;
        }
        // THE ELSE IS ASSUMING WE WILL SEND A RADIO SIGNAL THAT SOMETHING IS WRONG
        break;

      case SAFE:
        //DO NOTHING
        static bool armCommandReceived = false;
        if( armCommandReceived )
        {
          currRocketState = ARM;
        }
        break;

      case ARM:
        statusByte.bits.dof_sensor         = dofSensor.setInitialDataValues();
        statusByte.bits.altitude_sensor    = altitude_sensor.setInitialDataValues();
        statusByte.bits.temperature_sensor = temperature_sensor.setInitialDataValues();
        statusByte.bits.gps                = gps.setInitialDataValues();
        if(statusByte.byte == 0)
        {
          currRocketState = READY_FOR_LAUNCH;
        }
        break;

      case READY_FOR_LAUNCH:
        static bool launchCommandReceived = false;
        if( launchCommandReceived )
        {
          currRocketState = LAUNCH;
        }
        break;

      case LAUNCH:
        static bool isFlying = false;
        if( isFlying )
        {
          currRocketState = FLIGHT;
        }
        break;

      case FLIGHT:
        static uint8_t debounceVelocityCount = 0;
        statusByte.bits.dof_sensor         = dofSensor.collectData();
        statusByte.bits.altitude_sensor    = altitude_sensor.collectData();
        statusByte.bits.temperature_sensor = temperature_sensor.collectData();
        statusByte.bits.gps                = gps.collectData(); // probably just velocity
        //if( dofSensor.velocity < 2 )
        //   debounceVeloctyCount++;
        //else
        //   debounceVeloctyCount = 0;
        //if( debounceVeloctyCount >= 10 )
        //   currRocketState = RECOVERY;
        break;
        
      case RECOVERY:
        //statusByte.bits.gps = gps.getCoordinates;
        static bool verifyDataCommandReceived = false;
        if( verifyDataCommandReceived )
        {
          currRocketState = POST_FLIGHT;
        }
        break;

      case POST_FLIGHT:
        //RADIO RESEND DATA
        static bool reinitializeCommandReceived = false;
        if( reinitializeCommandReceived )
        {
          currRocketState = BOOTUP;
        }
        break;
  }
  /* PACKAGE THE CURRENT DATA AND SHIP IT*/
}
