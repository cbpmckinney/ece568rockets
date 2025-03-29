#include "main.h"
#include "Sensor.h"

#define DEBUG 1

void setup() {
#ifdef DEBUG
  Serial.begin(115200);

  while (!Serial) delay(10);  // wait for serial port to open!
  
#endif
  // put your setup code here, to run once:
}

void loop() {
  static rocket_states_t currRocketState = BOOTUP;
  static sensorStatus statusByte;
  static Sensor dofSensor;
  static Sensor altitude_sensor;
  static Sensor temperature_sensor;
  static Sensor gps;

#define DEBUG 1
#ifdef DEBUG
  static bool firstEntry = true;
#endif
  /* DECLARE A STATIC OF YOUR CLASS FOR SENSOR HERE */

  switch( currRocketState )
  {
      case BOOTUP:
      #ifdef DEBUG
        if( firstEntry )
        {
          firstEntry = false;
          Serial.println("ROCKET IN BOOTUP");
        }
      #endif
        statusByte.bits.dof_sensor         = dofSensor.initialize();
        statusByte.bits.altitude_sensor    = altitude_sensor.initialize();
        statusByte.bits.temperature_sensor = temperature_sensor.initialize();
        statusByte.bits.gps                = gps.initialize(); //use to initialize altitude sensor? IDK
        if(statusByte.byte == 0)
        {
        #ifdef DEBUG
          firstEntry = true;
        #endif
          currRocketState = SAFE;
        }
        // THE ELSE IS ASSUMING WE WILL SEND A RADIO SIGNAL THAT SOMETHING IS WRONG
        break;

      case SAFE:
      #ifdef DEBUG
        if( firstEntry )
        {
          firstEntry = false;
          Serial.println("ROCKET IN SAFE");
        }
      #endif
        //DO NOTHING
        static bool armCommandReceived = false;
        if( armCommandReceived )
        {
        #ifdef DEBUG
          firstEntry = true;
        #endif
          currRocketState = ARM;
        }
        break;

      case ARM:
      #ifdef DEBUG
        if( firstEntry )
        {
          firstEntry = false;
          Serial.println("ROCKET IN ARM");
        }
      #endif
        statusByte.bits.dof_sensor         = dofSensor.setInitialDataValues();
        statusByte.bits.altitude_sensor    = altitude_sensor.setInitialDataValues();
        statusByte.bits.temperature_sensor = temperature_sensor.setInitialDataValues();
        statusByte.bits.gps                = gps.setInitialDataValues();
        if(statusByte.byte == 0)
        {
        #ifdef DEBUG
          firstEntry = true;
        #endif
          currRocketState = READY_FOR_LAUNCH;
        }
        break;

      case READY_FOR_LAUNCH:
      #ifdef DEBUG
        if( firstEntry )
        {
          firstEntry = false;
          Serial.println("ROCKET IN READY_FOR_LAUNCH");
        }
      #endif
        static bool launchCommandReceived = false;
        if( launchCommandReceived )
        {
        #ifdef DEBUG
          firstEntry = true;
        #endif
          currRocketState = LAUNCH;
        }
        break;

      case LAUNCH:
      #ifdef DEBUG
        if( firstEntry )
        {
          firstEntry = false;
          Serial.println("ROCKET IN LAUNCH");
        }
      #endif
        static bool isFlying = false;
        if( isFlying )
        {
        #ifdef DEBUG
          firstEntry = true;
        #endif
          currRocketState = FLIGHT;
        }
        break;

      case FLIGHT:
      #ifdef DEBUG
        if( firstEntry )
        {
          firstEntry = false;
          Serial.println("ROCKET IN FLIGHT");
        }
      #endif
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
      #ifdef DEBUG
        if( firstEntry )
        {
          firstEntry = false;
          Serial.println("ROCKET IN RECOVERY");
        }
      #endif
        //statusByte.bits.gps = gps.getCoordinates;
        static bool verifyDataCommandReceived = false;
        if( verifyDataCommandReceived )
        {
        #ifdef DEBUG
          firstEntry = true;
        #endif
          currRocketState = POST_FLIGHT;
        }
        break;

      case POST_FLIGHT:
      #ifdef DEBUG
        if( firstEntry )
        {
          firstEntry = false;
          Serial.println("ROCKET IN POST_FLIGHT");
        }
      #endif
        //RADIO RESEND DATA
        static bool reinitializeCommandReceived = false;
        if( reinitializeCommandReceived )
        {
        #ifdef DEBUG
          firstEntry = true;
        #endif
          currRocketState = BOOTUP;
        }
        break;
  }



#ifdef DEBUG
  int incomingByte = 0; // for incoming serial data
  if (Serial.available() > 0) {
      incomingByte = Serial.read();
  }
  if( incomingByte == 49 )
  {
    firstEntry = true;
    switch( currRocketState)
    {
      case BOOTUP:
        currRocketState = SAFE;
        break;

      case SAFE:
        currRocketState = ARM;
        break;

      case ARM:
        currRocketState = READY_FOR_LAUNCH;
        break;

      case READY_FOR_LAUNCH:
        currRocketState = LAUNCH;
        break;

      case LAUNCH:
        currRocketState = FLIGHT;
        break;

      case FLIGHT:
        currRocketState = RECOVERY;
        break;
        
      case RECOVERY:
        currRocketState = POST_FLIGHT;
        break;

      case POST_FLIGHT:
        currRocketState = BOOTUP;
        break;
    }
  }

#endif

  /* PACKAGE THE CURRENT DATA AND SHIP IT*/
}
