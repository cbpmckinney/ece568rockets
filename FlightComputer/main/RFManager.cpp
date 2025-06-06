#include "RFManager.h"

// ROCKET RECEIVER CODE
// Brandon Crudele - ECE568

// functions:
// void tx() -> sends packet
// char* rx() -> receives packet in the form of a string
// [Ground Station specific] ... bool ready_to_launch() -> returns true if the rocket sent back an ACK signal
// [Rocket specific]         ... bool rocket_process()  -> sends ACK back to ground station  when asked if it is ready for launch

// important variables:
// char ack[8] -> string sent from the ground station to the rocket to request ACK 
// char ready[8] -> string sent from the rocket to the ground station as an ACK

#include <SPI.h>
#include <RH_RF95.h>

#define RFM95_CS   16
#define RFM95_INT  21
#define RFM95_RST  17

// frequency
#define RF95_FREQ 915.0

// create radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);

char ack[8] = "ARM?";
char ready[8] = "READY!";
int len;

#define TRANSMIT_BREAK 0xff // use this to space out the messages for retransmit at the end

// for rocket,
sensor_status_t RFManager::initialize() {
  if (!rf95.init()) {
    Serial.println("LoRa init FAILURE");
    return SENSOR_NOT_WORKING;
  }
  Serial.println("LoRa init SUCCESS");

  if (!rf95.setFrequency(RF95_FREQ)) {
    return SENSOR_NOT_WORKING;
  }
  Serial.print("Communication Frequency: "); Serial.println(RF95_FREQ);

  rf95.setTxPower(23, false);
  sentDataIndex = 0;
  return SENSOR_WORKING;
}


// for both rocket and ground station,
void RFManager::tx(char *str_num, int size) {
  // use itoa() if we decide to work with integers to strings (we have floats right now)
  //Serial.print("Sending "); Serial.print(str_num); Serial.println(" as a string");
  for( int i = 0; i < size; i++ )
  {
    if( sentDataIndex < 200000)
      sentData[sentDataIndex++] = str_num[i];
  }
  if( sentDataIndex < 200000)
  {
    sentData[sentDataIndex] = TRANSMIT_BREAK;
  }

  #ifdef TEST_MODE_ON_GROUND
  Serial.println( "Transmitting: ");
  Serial.print( "PACKET TYPE: ");
  switch( str_num[0] )
  {
    case STATUS:
      Serial.println( "STATUS");
      Serial.print( "STATUS BYTE: ");
      Serial.println( (uint8_t)str_num[1] );
      Serial.print( "STATE BYTE: ");
      Serial.println( (uint8_t)str_num[2] );
      break;
    case FAIL:
      Serial.println( "FAIL");
      break;
    case M_DATA_PACKET:
      Serial.println( "M_DATA");
      printDataType(str_num[1]);
      Serial.print( "At Altitude: ");
      Serial.println( str_num[2]);
      float sentFloatMdata;
      memcpy(&sentFloatMdata, &str_num[3], sizeof(float));
      Serial.print( "WITH VALUE: ");
      Serial.println(  sentFloatMdata );
      break; //1 m data

    case PEAK_DATA_PACKET:
    case AVERAGE_DATA_PACKET:
    case ALTITUDE_PACKET:
      if( str_num[0] == PEAK_DATA_PACKET)
        Serial.println( "PEAK_DATA_PACKET");
      else if( str_num[0] == AVERAGE_DATA_PACKET)
        Serial.println( "AVERAGE_DATA_PACKET");
      else if( str_num[0] == ALTITUDE_PACKET )
        Serial.println( "ALTITUDE_PACKET");
      printDataType((uint8_t)str_num[1]);
      float sentFloat;
      memcpy(&sentFloat, &str_num[3], sizeof(float));
      Serial.print( "WITH VALUE: ");
      Serial.println(  sentFloat );
      break;

    case GPS_PACKET:

      Serial.println("Sent GPS packet");

      break;
  }
  #endif

  rf95.send((uint8_t *)str_num, size); // might have to adjust packet size
  rf95.waitPacketSent();
  //delay(1);
}


void RFManager::printDataType(char toPrint) {
  Serial.print( "Data type: ");
  switch (toPrint) {
    case VELOCITY:
      Serial.println("VELOCITY");
      break;
    case ALTITUDE_TEMPERATURE:
      Serial.println("ALTITUDE_TEMPERATURE");
      break;
    case ALTITUDE_PRESSURE:
      Serial.println("ALTITUDE_PRESSURE");
      break;
    case TEMPERATURE_TEMPERATURE:
      Serial.println("TEMPERATURE_TEMPERATURE");
      break;
    case TEMPERATURE_HUMIDITY:
      Serial.println("TEMPERATURE_HUMIDITY");
      break;
    case ALTITUDE:
      Serial.println("ALTITUDE");
      break;
    default:
      Serial.println("UNKNOWN");
      break;
  }
}

// for both rocket and ground station,
void RFManager::txNoSave(char *str_num, int size) {
  // use itoa() if we decide to work with integers to strings (we have floats right now)
  //Serial.print("Sending "); Serial.print(str_num); Serial.println(" as a string");
  #ifdef TEST_MODE_ON_GROUND
  Serial.println( "Transmitting: ");
  Serial.print( "PACKET TYPE: ");
  switch( str_num[0] )
  {
    case STATUS:
      Serial.println( "STATUS");
      Serial.print( "STATUS BYTE: ");
      Serial.println( str_num[1] );
      Serial.print( "STATE BYTE: ");
      Serial.println( str_num[2] );
      break;
    case FAIL:
      Serial.println( "FAIL");
      break;
    case M_DATA_PACKET:
      Serial.println( "M_DATA");
      printDataType(str_num[1]);
      Serial.print( "At Altitude: ");
      Serial.println( str_num[2]);
      float sentFloatMdata;
      memcpy(&sentFloatMdata, &str_num[3], sizeof(float));
      Serial.print( "WITH VALUE: ");
      Serial.println(  sentFloatMdata );
      break; //1 m data

    case PEAK_DATA_PACKET:
    case AVERAGE_DATA_PACKET:
    case ALTITUDE_PACKET:
      if( str_num[0] == PEAK_DATA_PACKET)
        Serial.println( "PEAK_DATA_PACKET");
      else if( str_num[0] == AVERAGE_DATA_PACKET)
        Serial.println( "AVERAGE_DATA_PACKET");
      else if( str_num[0] == ALTITUDE_PACKET )
        Serial.println( "ALTITUDE_PACKET");
      printDataType(str_num[1]);
      float sentFloat;
      memcpy(&sentFloat, &str_num[3], sizeof(float));
      Serial.print( "WITH VALUE: ");
      Serial.println(  sentFloat );
      break;
  }
  #endif
  rf95.send((uint8_t *)str_num, size); // might have to adjust packet size
  rf95.waitPacketSent();
}

void RFManager::sendStatus( sensorStatus currStatus, rocket_states_t currState  )
{ 
    uint8_t toSendArray[3];
    toSendArray[0] = STATUS;
    toSendArray[1] = currStatus.byte;
    toSendArray[2] = currState;
    // Serial.println(toSendArray[0]);
    // Serial.println(toSendArray[1]);
    // Serial.println(toSendArray[2]);

    this->tx((char *)toSendArray, 3);
 }

// for both rocket and ground station,
char* RFManager::rx() {
  if (rf95.available()) {
    uint8_t buf[20]; // for now im using size 20
    uint8_t len = sizeof(buf);

    if (rf95.recv(buf, &len)) {
      buf[len] = '\0'; // null-terminator

      // allocate string memory (gross)
      char* result = (char*)malloc(len + 1); // +1 for null terminator
      if (result != NULL) {
        memcpy(result, buf, len + 1);
        return result;
      } 
      else { 
        Serial.println("Memory allocation failed"); // memory fail
        return NULL; 
      }

    } 
    else {
      Serial.println("Receive failed");
    }
  }
  return NULL; // Return NULL if no message or receive failed
}

bool RFManager::receivedCommand( packet_type_t commandToReceive )
{
    if( rf95.available())
    {
      Serial.println("RX Cmd...");
      uint8_t buf[1];
      uint8_t len = sizeof(buf);
      if( rf95.recv(buf, &len) )
      {
        Serial.println(buf[0]);
        if( buf[0] == commandToReceive )
        {
            return true;
        }
      }
      else
      {
        return false;
      }
    }
    return false;
}

void RFManager::transmit1m( float data[], dataPointStatus_t isCollected[], data_type_t dataType ) // MAY REQUIRE TRACKING SPOT OTHER WISE 300 COMPARISONS
{
    for( int i = 0; i < 100; i++ )
    {
       switch( isCollected[i])
       {
          case UNSET_DATA:
              return; //ALREADY TRANSMITTED NEWEST DATA
              break;

          case SET_DATA:
          {
              Serial.println("SETTING DATA:");
              uint8_t toSendArray[7];
              toSendArray[0] = M_DATA_PACKET;
              toSendArray[1] = dataType;
              toSendArray[2] = i; //METER SENSOR CAUGHT VALUE AT
              float value = data[i];
              memcpy(&toSendArray[3], &value, sizeof(float));
              this->tx((char *)toSendArray, sizeof(toSendArray));
              isCollected[i] = TRANSMITTED_DATA;
              break;
          }
          case TRANSMITTED_DATA:
              break;

       }
    }
}

void RFManager::transmitPeak( float data, data_type_t dataType )
{
    uint8_t toSendArray[6];
    toSendArray[0] = PEAK_DATA_PACKET;
    toSendArray[1] = dataType;
    float value = data;
    memcpy(&toSendArray[2], &value, sizeof(float));
    this->tx((char *)toSendArray, sizeof(toSendArray)); 
    return;
}

void RFManager::transmitAverage( float data, data_type_t dataType )
{
    uint8_t toSendArray[6];
    toSendArray[0] = AVERAGE_DATA_PACKET;
    toSendArray[1] = dataType;
    float value = data;
    memcpy(&toSendArray[2], &value, sizeof(float));
    this->tx((char *)toSendArray, sizeof(toSendArray)); 
    return;
}

void RFManager::transmitAltitude( float data, data_type_t dataType )
{
    uint8_t toSendArray[6];
    toSendArray[0] = ALTITUDE_PACKET;
    toSendArray[1] = dataType;
    float value = data;
    memcpy(&toSendArray[2], &value, sizeof(float));
    this->tx((char *)toSendArray, sizeof(toSendArray)); 
    return;
}

void RFManager::transmitGPS(GPSDataStorage& RocketGPSData)
{
  uint8_t toSendArray[16];
  toSendArray[0] = GPS_PACKET;
  toSendArray[1] = GPS_DATA;
  
  float latitude;
  float longitude;
  char lat;
  char lon;
  float gpsalt;

  latitude = RocketGPSData.latitude;
  longitude = RocketGPSData.longitude;
  lat = RocketGPSData.lat;
  lon = RocketGPSData.lon;
  gpsalt = RocketGPSData.gpsalt;

  memcpy(&toSendArray[2], &latitude, sizeof(float));
  memcpy(&toSendArray[6], &longitude, sizeof(float));
  memcpy(&toSendArray[10], &lat, sizeof(char));
  memcpy(&toSendArray[11], &lon, sizeof(char));
  memcpy(&toSendArray[12], &gpsalt, sizeof(float));

  this->tx((char *)toSendArray, sizeof(toSendArray));


  return;
}




sensor_status_t RFManager::transmitData(DOFSensor& dofSensor, AltitudeSensor& altitude_sensor, TemperatureSensor& temperature_sensor)
{
  transmit1m( dofSensor.per1mDataArray, dofSensor.isCollectedArray, VELOCITY );
  transmit1m( altitude_sensor.per1mPressureDataArray, altitude_sensor.isPressureCollectedArray, ALTITUDE_PRESSURE );
  transmit1m( altitude_sensor.per1mTemperatureDataArray, altitude_sensor.isTemperatureCollectedArray, ALTITUDE_TEMPERATURE );
  transmit1m( temperature_sensor.per1mTemperatureDataArray, temperature_sensor.isTemperatureCollectedArray, TEMPERATURE_TEMPERATURE );
  transmit1m( temperature_sensor.per1mHumidityDataArray, temperature_sensor.isHumidityCollectedArray, TEMPERATURE_HUMIDITY );

  static float dofSensorPeakReported = 0;
  if( dofSensorPeakReported != dofSensor.peak)
  {
    transmitPeak( dofSensor.peak, VELOCITY );
    dofSensorPeakReported = dofSensor.peak;
  }

  static float altitudePressurePeakReported = 0;
  if( altitudePressurePeakReported != altitude_sensor.peakPressure)
  {
    transmitPeak( altitude_sensor.peakPressure, ALTITUDE_PRESSURE );
    altitudePressurePeakReported = altitude_sensor.peakPressure;
  }

  static float altitudeTemperaturePeakReported = 0;
  if( altitudeTemperaturePeakReported != altitude_sensor.peakTemperature)
  {
    transmitPeak( altitude_sensor.peakTemperature, ALTITUDE_TEMPERATURE );
    altitudeTemperaturePeakReported = altitude_sensor.peakTemperature;
  }

  static float temperatureSensorHumidityPeakReported = 0;
  if( temperatureSensorHumidityPeakReported != temperature_sensor.peakHumidity)
  {
    transmitPeak( temperature_sensor.peakHumidity, TEMPERATURE_HUMIDITY );
    temperatureSensorHumidityPeakReported = temperature_sensor.peakHumidity;
  }

  static float temperatureSensorTemperaturePeakReported = 0;
  if( temperatureSensorTemperaturePeakReported != temperature_sensor.peakTemperature)
  {
    transmitPeak( temperature_sensor.peakTemperature, TEMPERATURE_TEMPERATURE );
    temperatureSensorTemperaturePeakReported = temperature_sensor.peakTemperature;
  }

  // AVERAGES

  static float dofSensorAverageReported = 0;
  if( dofSensorAverageReported != dofSensor.average)
  {
    transmitAverage( dofSensor.average, VELOCITY );
    dofSensorAverageReported = dofSensor.average;
  }

  static float altitudePressureAverageReported = 0;
  if( altitudePressureAverageReported != altitude_sensor.averagePressure)
  {
    transmitAverage( altitude_sensor.averagePressure, ALTITUDE_PRESSURE );
    altitudePressureAverageReported = altitude_sensor.averagePressure;
  }

  static float altitudeTemperatureAverageReported = 0;
  if( altitudeTemperatureAverageReported != altitude_sensor.averageTemperature)
  {
    transmitAverage( altitude_sensor.averageTemperature, ALTITUDE_TEMPERATURE );
    altitudeTemperatureAverageReported = altitude_sensor.averageTemperature;
  }

  static float temperatureSensorHumidityAverageReported = 0;
  if( temperatureSensorHumidityAverageReported != temperature_sensor.averageHumidity)
  {
    transmitAverage( temperature_sensor.averageHumidity, TEMPERATURE_HUMIDITY );
    temperatureSensorHumidityAverageReported = temperature_sensor.averageHumidity;
  }

  static float temperatureSensorTemperatureAverageReported = 0;
  if( temperatureSensorTemperatureAverageReported != temperature_sensor.averageTemperature)
  {
    transmitAverage( temperature_sensor.averageTemperature, TEMPERATURE_TEMPERATURE );
    temperatureSensorTemperatureAverageReported = temperature_sensor.averageTemperature;
  }

  static float altitudeReported = 0;
  if( altitudeReported != altitude_sensor.currAltitudeDifferenceSinceStart)
  {
    transmitAltitude( altitude_sensor.currAltitudeDifferenceSinceStart, ALTITUDE );
    altitudeReported = temperature_sensor.averageTemperature;
  }

  return SENSOR_WORKING;
}

void RFManager::retransmitData( )
{
   uint8_t toSendIndex =  0;
   for( int i = 0; i < sentDataIndex; i++ )
   {
      uint8_t toSendArray[7];
      if( toSendIndex >= 7 )
        return;
      toSendArray[ toSendIndex++ ] = sentData[i];
      if( sentData[i] == TRANSMIT_BREAK)
      {
        txNoSave((char *)toSendArray, toSendIndex - 1); // I don't think this is off by 1 but probably needs a check
        toSendIndex = 0;
      }
   }
}

// // for rocket,
// bool rocket_process() {
//   Serial.println("Waiting for Ground Station request...");
//   while(1) { // wait for ground station ack
//     char* received = rx();
//     if (received != NULL) {
//       Serial.print("Received: ");
//       Serial.print(received);
//       Serial.println(" from the ground station");
//       if (strcmp(received, ack) == 0) { // check if we got an ACK request,
//         Serial.println("Sending ACK...");
//         tx(ready); // send ACK
//         return true;
//       }
//       else {
//         return false; // weird ACK
//       }

//     }
//     free(received);
//   }
// }
// ROCKET PROCESS
// void loop() {
//   if (rocket_process()) {
//     Serial.println("Sent ACK to Ground Station"); Serial.println("");
//   }
//   delay(1000);
// }