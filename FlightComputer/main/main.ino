#include "main.h"
#include "Sensor.h"
#include "DOFSensor.h"
#include "altitudeSensor.h"
#include "temperatureSensor.h"
#include <SPI.h>
#include <RH_RF95.h> // Driver to send and receive datagrams via LoRa capable radio transceiver
#include <RHReliableDatagram.h> // Manager to send addressed, acknowledged, retransmitted datagrams

#define DEBUG 1
#define RelayPin 25

#define CLIENT_ADDRESS 1
#define SERVER_ADDRESS 2

#define RFM95_CS   16 // Chip select pin
#define RFM95_INT  21 // Interrupt pin
#define RFM95_RST  17 // Reset pin

#define RF95_FREQ 915.0 // Change to 915.0 MHz

RH_RF95 rf95(RFM95_CS, RFM95_INT); // radio driver instance
RHReliableDatagram manager(rf95, CLIENT_ADDRESS); // manager instance using above driver

void setup() {
#ifdef DEBUG
  Serial.begin(115200);

  while (!Serial) delay(10);  // wait for serial port to open!
  
#endif
  pinMode(RelayPin, OUTPUT); // relay output pin
  digitalWrite(RelayPin, LOW); // set relay to off
  
  // radio initialization
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);
  if (!manager.init())
    Serial.println("RADIO INIT FAIL: MANAGER");
  delay(100);
  // radio manual reset
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);
  while (!rf95.init()) {
    Serial.println("RADIO INIT FAIL: DRIVER");
    while (1);
  }
  Serial.println("RADIO INIT OK");
  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println("SET FREQ FAIL");
    while (1);
  }
  rf95.setTxPower(23, false); // set tx power to 23 dBm
}

uint8_t buf[RH_RF95_MAX_MESSAGE_LEN]; // don't put this on the stack

void loop() {
  static rocket_states_t currRocketState = BOOTUP;
  static sensorStatus statusByte;
  static DOFSensor dofSensor;
  static AltitudeSensor altitude_sensor;
  static TemperatureSensor temperature_sensor;
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
        // else send error message
        uint8_t data[] = "ERROR: BOOTUP";
        manager.sendtoWait(data, sizeof(data), SERVER_ADDRESS)
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

        // listen to receive ARM command from ground station
        uint8_t len = sizeof(buf);
        uint8_t from;
        if (manager.recvfromAck(buf, &len, &from))
        {
          String command = (char*)buf;
          if (command = "ARM")
          {
            armCommandReceived = true;
          }
        }
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
        // listen to receive LAUNCH command from ground station
        uint8_t len = sizeof(buf);
        uint8_t from;
        if (manager.recvfromAck(buf, &len, &from))
        {
          String command = (char*)buf;
          if (command = "LAUNCH")
          {
            launchCommandReceived = true;
          }
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

        // digitalWrite(RelayPin, HIGH); fires relay
        // need to set this back to low, perhaps when flying?

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
        static uint8_t simAlt = 0; // REPLACE WITH ACTUAL CURRENT ALTITUDE
        statusByte.bits.altitude_sensor    = altitude_sensor.collectData( );
        statusByte.bits.temperature_sensor = temperature_sensor.collectData( altitude_sensor.currAltitude );
        statusByte.bits.dof_sensor         = dofSensor.collectData( altitude_sensor.currAltitude );
        statusByte.bits.gps                = gps.collectData(); // probably just velocity

        
        //COMMENT THIS NEXT SECTION OUT IF YOU ARE RUNNING ON YOUR COMPUTER ON THE GROUND OR IT WILL INSTANTLY TRANSITION
        if( dofSensor.doneFlying )
        {
        #ifdef DEBUG
           firstEntry = true;
        #endif
           currRocketState = RECOVERY;
        }
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
