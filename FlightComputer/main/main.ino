#include "main.h"
#include "Sensor.h"
#include "DOFSensor.h"
#include "altitudeSensor.h"
#include "temperatureSensor.h"
#include "RFManager.h"
#include <SPI.h>
#include <RH_RF95.h>
#include <RHReliableDatagram.h>



#define DEBUG 1
#define RelayPin 25

void setup() {
#ifdef DEBUG
  Serial.begin(115200);

  while (!Serial) delay(10);  // wait for serial port to open!
  
#endif
  pinMode(RelayPin, OUTPUT); // relay output pin
  digitalWrite(RelayPin, LOW); // set relay to off
  // put your setup code here, to run once:
}

void loop() {
  static rocket_states_t currRocketState = BOOTUP;
  static sensorStatus statusByte;
  static DOFSensor dofSensor;
  static AltitudeSensor altitude_sensor;
  static TemperatureSensor temperature_sensor;
  static Sensor gps;
  static RFManager rfManager;
  static bool armCommandReceived = false;
  static bool readyCommandReceived = false;
  static bool launchCommandReceived = false;
  static bool verifyDataCommandReceived = false;
  static bool reinitializeCommandReceived = false;
  #define DEBUG 1
  #ifdef DEBUG
    static bool firstEntry = true;
  #endif

  switch( currRocketState )
  {
      case POWERONFAILURE:
        #ifdef DEBUG
          Serial.println("Power On Failure...");
          Serial.println("Going back to BOOTUP");
        #endif
        currRocketState = BOOTUP;
        delay(1000);
        break;
      case BOOTUP:
        #ifdef DEBUG
          if( firstEntry )
          {
            firstEntry = false;
            Serial.println("ROCKET IN BOOTUP");
          }
        #endif
        // statusByte.bits.dof_sensor         = dofSensor.initialize();   // testing without DOF
        statusByte.bits.altitude_sensor    = altitude_sensor.initialize();
        statusByte.bits.temperature_sensor = temperature_sensor.initialize();
        statusByte.bits.gps                = gps.initialize(); 
        statusByte.bits.RFtransmitter      = rfManager.initialize();
        if(statusByte.byte == 0)
        {
        #ifdef DEBUG
          firstEntry = true;
        #endif
          currRocketState = SAFE;
          Serial.println("Going to SAFE mode...");
          rfManager.sendStatus( statusByte, currRocketState );
        }
        else
        {
          currRocketState = POWERONFAILURE;
          Serial.println("Power On Failure...");
          rfManager.sendStatus( statusByte, currRocketState );
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
        if( rfManager.receivedCommand( ARM_PACKET ) )
        {
          Serial.println("ARM Packet Received from Ground Station");
          armCommandReceived = true;
        }
        if( armCommandReceived )
        {
          delay(1000);
        #ifdef DEBUG
          firstEntry = true;
        #endif
          currRocketState = ARM;
          Serial.println("Sending ARM ACK to Ground Station");
          rfManager.sendStatus( statusByte, currRocketState );
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
        // receive cmd to put rocket in ready to launch mode:
        if( rfManager.receivedCommand( RTL_PACKET ) )
        {
          Serial.println("--Ready to Launch-- Packet Received from Ground Station");
          readyCommandReceived = true;
        }
        if ( readyCommandReceived ) {
          #ifdef DEBUG
            firstEntry = true;
          #endif
          currRocketState = READY_FOR_LAUNCH;
          Serial.println("Sending READY_TO_LAUNCH status to Ground Station");
          rfManager.sendStatus( statusByte, currRocketState );
        } 
        break;
      case READY_FOR_LAUNCH:
        #ifdef DEBUG
            if( firstEntry )
            {
              firstEntry = false;
              Serial.println("Ready for launch!");
            }
        #endif
        // receive cmd to put rocket in ready to launch mode:
        if( rfManager.receivedCommand( LAUNCH_PACKET ) )
          {
            Serial.println("--Launch-- Packet Received from Ground Station");
            launchCommandReceived = true;
          }
        if ( launchCommandReceived ) {
          #ifdef DEBUG
            firstEntry = true;
          #endif
          currRocketState = LAUNCH;
          Serial.println("Sending LAUNCH status to Ground Station");
          rfManager.sendStatus( statusByte, currRocketState );
        } 
        break;
      case LAUNCH:
        #ifdef DEBUG
          if ( firstEntry )
          {
            firstEntry = false;
            Serial.println("Launching...");
          }
        #endif
        break;
      default:
        Serial.println("ERROR");
        break;
        
      // #ifdef DEBUG
      //   if( firstEntry )
      //   {
      //     firstEntry = false;
      //     Serial.println("ROCKET IN ARM");
      //   }
      // #endif
      //   statusByte.bits.dof_sensor         = dofSensor.setInitialDataValues();
      //   statusByte.bits.altitude_sensor    = altitude_sensor.setInitialDataValues();
      //   statusByte.bits.temperature_sensor = temperature_sensor.setInitialDataValues();
      //   statusByte.bits.gps                = gps.setInitialDataValues();
      //   if(statusByte.byte == 0)
      //   {
      //   #ifdef DEBUG
      //     firstEntry = true;
      //   #endif
      //     currRocketState = READY_FOR_LAUNCH;
      //     rfManager.sendStatus( statusByte, currRocketState );
      //   }
      //   break;

      // case READY_FOR_LAUNCH:
      // #ifdef DEBUG
      //   if( firstEntry )
      //   {
      //     firstEntry = false;
      //     Serial.println("ROCKET IN READY_FOR_LAUNCH");
      //   }

      //   if( rfManager.receivedCommand( LAUNCH_PACKET ) )
      //   {
      //     launchCommandReceived = true;
      //   }
      // #endif
      //   if( launchCommandReceived )
      //   {
      //   #ifdef DEBUG
      //     firstEntry = true;
      //   #endif
      //     currRocketState = LAUNCH;
      //     rfManager.sendStatus( statusByte, currRocketState );
      //   }
      //   break;

      // case LAUNCH:
      // #ifdef DEBUG
      //   if( firstEntry )
      //   {
      //     firstEntry = false;
      //     Serial.println("ROCKET IN LAUNCH");
      //   }
      // #endif

      //   // digitalWrite(RelayPin, HIGH); fires relay
      //   // need to set this back to low, perhaps when flying?
      //   if( dofSensor.isFlying() )
      //   {
      //   #ifdef DEBUG
      //     firstEntry = true;
      //   #endif
      //     currRocketState = FLIGHT;
      //     rfManager.sendStatus( statusByte, currRocketState );
      //   }
      //   break;

      // case FLIGHT:
      // #ifdef DEBUG
      //   if( firstEntry )
      //   {
      //     firstEntry = false;
      //     Serial.println("ROCKET IN FLIGHT");
      //   }
      // #endif
      //   static uint8_t simAlt = 0; // REPLACE WITH ACTUAL CURRENT ALTITUDE
      //   statusByte.bits.altitude_sensor    = altitude_sensor.collectData( );
      //   statusByte.bits.temperature_sensor = temperature_sensor.collectData( altitude_sensor.currAltitude );
      //   statusByte.bits.dof_sensor         = dofSensor.collectData( altitude_sensor.currAltitude );
      //   statusByte.bits.gps                = gps.collectData(); // probably just velocity
        
      //   statusByte.bits.RFtransmitter          = rfManager.transmitData( dofSensor, altitude_sensor, temperature_sensor, gps );

      //   //COMMENT THIS NEXT SECTION OUT IF YOU ARE RUNNING ON YOUR COMPUTER ON THE GROUND OR IT WILL INSTANTLY TRANSITION
      //   if( dofSensor.doneFlying )
      //   {
      //   #ifdef DEBUG
      //      firstEntry = true;
      //   #endif
      //      currRocketState = RECOVERY;
      //      rfManager.sendStatus( statusByte, currRocketState );
      //   }
      //   break;
        
      // case RECOVERY:
      // #ifdef DEBUG
      //   if( firstEntry )
      //   {
      //     firstEntry = false;
      //     Serial.println("ROCKET IN RECOVERY");
      //   }
      // #endif
      //   //statusByte.bits.gps = gps.getCoordinates;
      //   if( rfManager.receivedCommand( VERIFY_DATA ) )
      //   {
      //     verifyDataCommandReceived = true;
      //   }
      //   if( verifyDataCommandReceived )
      //   {
      //   #ifdef DEBUG
      //     firstEntry = true;
      //   #endif
      //     currRocketState = POST_FLIGHT;
      //     rfManager.sendStatus( statusByte, currRocketState );
      //   }
      //   break;

      // case POST_FLIGHT:
      // #ifdef DEBUG
      //   if( firstEntry )
      //   {
      //     firstEntry = false;
      //     Serial.println("ROCKET IN POST_FLIGHT");
      //   }
      // #endif
      //   rfManager.retransmitData();
      //   if( rfManager.receivedCommand( REINITIALIZE ) )
      //   {
      //     reinitializeCommandReceived = true;
      //   }
      //   if( reinitializeCommandReceived )
      //   {
      //   #ifdef DEBUG
      //     firstEntry = true;
      //   #endif
      //     currRocketState = BOOTUP;
      //     rfManager.sendStatus( statusByte, currRocketState );
      //   }
      //   break;
  }



// #ifdef DEBUG
//   int incomingByte = 0; // for incoming serial data
//   if (Serial.available() > 0) {
//       incomingByte = Serial.read();
//   }
//   if( incomingByte == 49 )
//   {
//     firstEntry = true;
//     switch( currRocketState)
//     {
//       case BOOTUP:
//         currRocketState = SAFE;
//         break;

//       case SAFE:
//         currRocketState = ARM;
//         break;

//       case ARM:
//         currRocketState = READY_FOR_LAUNCH;
//         break;

//       case READY_FOR_LAUNCH:
//         currRocketState = LAUNCH;
//         break;

//       case LAUNCH:
//         currRocketState = FLIGHT;
//         break;

//       case FLIGHT:
//         currRocketState = RECOVERY;
//         break;
        
//       case RECOVERY:
//         currRocketState = POST_FLIGHT;
//         break;

//       case POST_FLIGHT:
//         currRocketState = BOOTUP;
//         break;
//     }
//   }

// #endif

  /* PACKAGE THE CURRENT DATA AND SHIP IT*/
}
