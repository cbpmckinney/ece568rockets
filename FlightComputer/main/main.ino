#include "main.h"
#include "Sensor.h"
#include "DOFSensor.h"
#include "altitudeSensor.h"
#include "temperatureSensor.h"
#include "RFManager.h"
#include <SPI.h>
#include <RH_RF95.h>
#include <RHReliableDatagram.h>

#ifdef TEST_MODE_ON_GROUND
  uint8_t simAltitude = 0;
#endif

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

  #ifdef TEST_MODE_ON_GROUND
  static bool isFlying = false;
  static bool doneFlying = false;
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
        statusByte.bits.dof_sensor         = dofSensor.initialize();   // testing without DOF
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
              statusByte.bits.dof_sensor         = dofSensor.setInitialDataValues();
              statusByte.bits.altitude_sensor    = altitude_sensor.setInitialDataValues();
              statusByte.bits.temperature_sensor = temperature_sensor.setInitialDataValues();
              statusByte.bits.gps                = gps.setInitialDataValues();
            }
        #endif
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
            Serial.println("ROCKET IN LAUNCH");
          }
        #endif
  //    digitalWrite(RelayPin, HIGH); // fires relay
        // need to set this back to low, perhaps when flying?
        #ifdef TEST_MODE_ON_GROUND
        if( isFlying )
        {
          #ifdef DEBUG
          firstEntry = true;
          #endif
          currRocketState = FLIGHT;
          rfManager.sendStatus( statusByte, currRocketState );
        }
        #else
        if( dofSensor.isFlying() )
        {
          #ifdef DEBUG
            firstEntry = true;
          #endif
          currRocketState = FLIGHT;
          rfManager.sendStatus( statusByte, currRocketState );
        }
        #endif
        break;

      case FLIGHT:
        #ifdef DEBUG
          if( firstEntry )
          {
            firstEntry = false;
            Serial.println("ROCKET IN FLIGHT");
          }
        #endif
          statusByte.bits.altitude_sensor    = altitude_sensor.collectData( );
          #ifdef TEST_MODE_ON_GROUND
          statusByte.bits.temperature_sensor = temperature_sensor.collectData( simAltitude );
          statusByte.bits.dof_sensor         = dofSensor.collectData( simAltitude );
          #else
          statusByte.bits.temperature_sensor = temperature_sensor.collectData( altitude_sensor.currAltitudeDifferenceSinceStart );
          statusByte.bits.dof_sensor         = dofSensor.collectData( altitude_sensor.currAltitudeDifferenceSinceStart );
          #endif

          statusByte.bits.gps                = gps.collectData(); // probably just velocity
        
          statusByte.bits.RFtransmitter          = rfManager.transmitData( dofSensor, altitude_sensor, temperature_sensor, gps );
        //   COMMENT THIS NEXT SECTION OUT IF YOU ARE RUNNING ON YOUR COMPUTER ON THE GROUND OR IT WILL INSTANTLY TRANSITION
          #ifdef TEST_MODE_ON_GROUND
          if( doneFlying )
          {
            #ifdef DEBUG
             firstEntry = true;
          #endif
             currRocketState = RECOVERY;
             rfManager.sendStatus( statusByte, currRocketState );
          }
          #else
          if( dofSensor.doneFlying )
          {
          #ifdef DEBUG
             firstEntry = true;
          #endif
             currRocketState = RECOVERY;
             rfManager.sendStatus( statusByte, currRocketState );
          }
          #endif
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
        if( rfManager.receivedCommand( VERIFY_DATA ) )
        {
          verifyDataCommandReceived = true;
        }
        if( verifyDataCommandReceived )
        {
        #ifdef DEBUG
          firstEntry = true;
        #endif
          currRocketState = POST_FLIGHT;
          rfManager.sendStatus( statusByte, currRocketState );
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
        rfManager.retransmitData();
        if( rfManager.receivedCommand( REINITIALIZE ) )
        {
          reinitializeCommandReceived = true;
        }
        if( reinitializeCommandReceived )
        {
        #ifdef DEBUG
          firstEntry = true;
        #endif
          currRocketState = BOOTUP;
          rfManager.sendStatus( statusByte, currRocketState );
        }
        break;
      default:
        Serial.println("ERROR");
        break;
  }



#ifdef DEBUG
  int incomingByte = 0; // for incoming serial data
  if (Serial.available() > 0) {
      incomingByte = Serial.read();
  }
  if( incomingByte == 49 ) //1
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
  #ifdef TEST_MODE_ON_GROUND
  else if( incomingByte == 50 ) //2
  {
    isFlying = true;
    Serial.print("IS FLYING: ");
    Serial.println( isFlying ? "true": "false");
  }
  else if( incomingByte == 51) //3
  {
    isFlying = false;
    Serial.print("IS FLYING: ");
    Serial.println( isFlying ? "true": "false");
  }
  else if( incomingByte == 52) //4
  {
    doneFlying = true;
    Serial.print("DONE FLYING: ");
    Serial.println( doneFlying ? "true": "false");
  }
  else if( incomingByte == 53) //5
  {
    doneFlying = false;
    Serial.print("DONE FLYING: ");
    Serial.println( doneFlying ? "true": "false");
  }
  else if( incomingByte == 54 )  //6
  {
     Serial.print("DONE FLYING: ");
     Serial.println( doneFlying ? "true": "false");
     Serial.print("IS FLYING: ");
     Serial.println( isFlying ? "true": "false");
  }
  else if( incomingByte == 55 ) //7
  {
    armCommandReceived = true;
    Serial.print("ARM COMMAND RECEIVED: ");
    Serial.println( armCommandReceived ? "true": "false");
  }
  else if( incomingByte == 56 ) //8
  {
    armCommandReceived = false;
    Serial.print("ARM COMMAND RECEIVED: ");
    Serial.println( armCommandReceived ? "true": "false");
  }
  #endif
#endif
}
