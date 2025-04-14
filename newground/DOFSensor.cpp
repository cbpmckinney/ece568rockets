#include "DOFSensor.h"
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <EEPROM.h>

#define AVERAGE_DATA_BUFFER_SIZE 1000
//#define DEBUGDOF 1

// LIBRARY CODE:
double xPos = 0, yPos = 0, headingVel = 0;
uint16_t BNO055_SAMPLERATE_DELAY_MS = 10; //how often to read data from the board
static uint16_t PRINT_DELAY_MS = 1000; // how often to print the data
static uint16_t printCount = 0; //counter to avoid printing every 10MS sample
//velocity = accel*dt (dt in seconds)
//position = 0.5*accel*dt^2
double ACCEL_VEL_TRANSITION =  (double)(BNO055_SAMPLERATE_DELAY_MS) / 1000.0;
double ACCEL_POS_TRANSITION = 0.5 * ACCEL_VEL_TRANSITION * ACCEL_VEL_TRANSITION;
double DEG_2_RAD = 0.01745329251; //trig functions require radians, BNO055 outputs degrees

// Check I2C device address and correct line below (by default address is 0x29 or 0x28)
//                                   id, address
Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x28);

static unsigned long timeOfLastPoll;
static bool isInitialized = false;
static bool firstPoll = false;

static float sum = 0;
static unsigned int count = 0;
void DOFSensor::updateAverage(float currVal)
{
    static int currIndex = 0;
    if (count < AVERAGE_DATA_BUFFER_SIZE) {
        sum += currVal;
        averageDataArray[currIndex] = currVal; 
        count++;// Increase the count
    }
    else {
        sum = sum - averageDataArray[currIndex] + currVal;
        averageDataArray[currIndex] = currVal;
        count--;
    }
    currIndex = (currIndex + 1) % AVERAGE_DATA_BUFFER_SIZE;
    this->average = sum/count;
}

void DOFSensor::updatePeak(float currVal)
{
  if( currVal > this->peak)
    this->peak = currVal;
}

static int currSaved10Altitude = 0;

void DOFSensor::updatePer1mDataArray( float currVal, int altitude)
{
    if( altitude > currSaved10Altitude )
    {
      if( currSaved10Altitude >= 100 )
          return;
      isCollectedArray[ currSaved10Altitude ] = SET_DATA;
      per1mDataArray[ currSaved10Altitude++ ] = currVal;
    }
}
void DOFSensor::printPer1mData()
{
  for( int i = 0; i < currSaved10Altitude; i++ )
  {
      Serial.print("VAL AT: ");
      Serial.print(i);
      Serial.print(" ");
      Serial.println( per1mDataArray[i] );
  }
}

void displaySensorOffsets(const adafruit_bno055_offsets_t &calibData)
{
    Serial.print("Accelerometer: ");
    Serial.print(calibData.accel_offset_x); Serial.print(" ");
    Serial.print(calibData.accel_offset_y); Serial.print(" ");
    Serial.print(calibData.accel_offset_z); Serial.print(" ");

    Serial.print("\nGyro: ");
    Serial.print(calibData.gyro_offset_x); Serial.print(" ");
    Serial.print(calibData.gyro_offset_y); Serial.print(" ");
    Serial.print(calibData.gyro_offset_z); Serial.print(" ");

    Serial.print("\nMag: ");
    Serial.print(calibData.mag_offset_x); Serial.print(" ");
    Serial.print(calibData.mag_offset_y); Serial.print(" ");
    Serial.print(calibData.mag_offset_z); Serial.print(" ");

    Serial.print("\nAccel Radius: ");
    Serial.print(calibData.accel_radius);

    Serial.print("\nMag Radius: ");
    Serial.print(calibData.mag_radius);
}

void displaySensorDetails(void)
{
  sensor_t sensor;
  bno.getSensor(&sensor);
  Serial.println("------------------------------------");
  Serial.print  ("Sensor:       "); Serial.println(sensor.name);
  Serial.print  ("Driver Ver:   "); Serial.println(sensor.version);
  Serial.print  ("Unique ID:    "); Serial.println(sensor.sensor_id);
  Serial.print  ("Max Value:    "); Serial.print(sensor.max_value); Serial.println(" xxx");
  Serial.print  ("Min Value:    "); Serial.print(sensor.min_value); Serial.println(" xxx");
  Serial.print  ("Resolution:   "); Serial.print(sensor.resolution); Serial.println(" xxx");
  Serial.println("------------------------------------");
  Serial.println("");
  delay(500);
}

/**************************************************************************/
/*
    Display some basic info about the sensor status
*/
/**************************************************************************/
void displaySensorStatus(void)
{
  /* Get the system status values (mostly for debugging purposes) */
  uint8_t system_status, self_test_results, system_error;
  system_status = self_test_results = system_error = 0;
  bno.getSystemStatus(&system_status, &self_test_results, &system_error);

  /* Display the results in the Serial Monitor */
  Serial.println("");
  Serial.print("System Status: 0x");
  Serial.println(system_status, HEX);
  Serial.print("Self Test:     0x");
  Serial.println(self_test_results, HEX);
  Serial.print("System Error:  0x");
  Serial.println(system_error, HEX);
  Serial.println("");
  delay(500);
}

/**************************************************************************/
/*
    Display sensor calibration status
*/
/**************************************************************************/
void displayCalStatus(void)
{
  /* Get the four calibration values (0..3) */
  /* Any sensor data reporting 0 should be ignored, */
  /* 3 means 'fully calibrated" */
  uint8_t system, gyro, accel, mag;
  system = gyro = accel = mag = 0;
  bno.getCalibration(&system, &gyro, &accel, &mag);

  /* The data should be ignored until the system calibration is > 0 */
  Serial.print("\t");
  if (!system)
  {
    Serial.print("! ");
  }

  /* Display the individual values */
  Serial.print("Sys:");
  Serial.print(system, DEC);
  Serial.print(" G:");
  Serial.print(gyro, DEC);
  Serial.print(" A:");
  Serial.print(accel, DEC);
  Serial.print(" M:");
  Serial.print(mag, DEC);
}


sensor_status_t DOFSensor::initialize()
{
    if( isInitialized)
      return SENSOR_WORKING;

    if (!bno.begin())
    {
        Serial.println("No BNO055 detected");
        return SENSOR_NOT_WORKING;
    }
    delay(1000);

    Serial.begin(115200);
    delay(1000);
    Serial.println("Orientation Sensor Test"); Serial.println("");

    /* Initialise the sensor */
    if (!bno.begin())
    {
        /* There was a problem detecting the BNO055 ... check your connections */
        Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
        while (1);
    }
    #ifdef ALLOWCALIBRATION
    int eeAddress = 0;
    long bnoID;
    bool foundCalib = false;

    EEPROM.get(eeAddress, bnoID);

    adafruit_bno055_offsets_t calibrationData;
    sensor_t sensor;

    /*
    *  Look for the sensor's unique ID at the beginning oF EEPROM.
    *  This isn't foolproof, but it's better than nothing.
    */
    bno.getSensor(&sensor);
    if (bnoID != sensor.sensor_id)
    {
        Serial.println("\nNo Calibration Data for this sensor exists in EEPROM");
        delay(500);
    }
    else
    {
        Serial.println("\nFound Calibration for this sensor in EEPROM.");
        eeAddress += sizeof(long);
        EEPROM.get(eeAddress, calibrationData);

        displaySensorOffsets(calibrationData);

        Serial.println("\n\nRestoring Calibration data to the BNO055...");
        bno.setSensorOffsets(calibrationData);

        Serial.println("\n\nCalibration data loaded into BNO055");
        foundCalib = true;
    }

    delay(1000);

    /* Display some basic information on this sensor */
    displaySensorDetails();

    /* Optional: Display current status */
    displaySensorStatus();

   /* Crystal must be configured AFTER loading calibration data into BNO055. */
    bno.setExtCrystalUse(true);

    sensors_event_t event;
    bno.getEvent(&event);
    /* always recal the mag as It goes out of calibration very often */
    if (foundCalib){
        Serial.println("Move sensor slightly to calibrate magnetometers");
        while (!bno.isFullyCalibrated())
        {
            bno.getEvent(&event);
            delay(BNO055_SAMPLERATE_DELAY_MS);
        }
    }
    else
    {
        Serial.println("Please Calibrate Sensor: ");
        while (!bno.isFullyCalibrated())
        {
            bno.getEvent(&event);

            Serial.print("X: ");
            Serial.print(event.orientation.x, 4);
            Serial.print("\tY: ");
            Serial.print(event.orientation.y, 4);
            Serial.print("\tZ: ");
            Serial.print(event.orientation.z, 4);

            /* Optional: Display calibration status */
            displayCalStatus();

            /* New line for the next sample */
            Serial.println("");

            /* Wait the specified delay before requesting new data */
            delay(BNO055_SAMPLERATE_DELAY_MS);
        }
    }

    Serial.println("\nFully calibrated!");
    Serial.println("--------------------------------");
    Serial.println("Calibration Results: ");
    adafruit_bno055_offsets_t newCalib;
    bno.getSensorOffsets(newCalib);
    displaySensorOffsets(newCalib);

    Serial.println("\n\nStoring calibration data to EEPROM...");

    eeAddress = 0;
    bno.getSensor(&sensor);
    bnoID = sensor.sensor_id;

    EEPROM.put(eeAddress, bnoID);

    eeAddress += sizeof(long);
    EEPROM.put(eeAddress, newCalib);
    Serial.println("Data stored to EEPROM.");

    Serial.println("\n--------------------------------\n");
    delay(500);
    isInitialized = true;
    #endif
    firstPoll = true;
    return SENSOR_WORKING;
}

sensor_status_t DOFSensor::setInitialDataValues()
{
    this->average = 0;
    this->peak = 0;
    for (int i = 0; i < 1000; i++) {
        this->averageDataArray[i] = 0;
    }
    this->doneFlying = false;
    this->startedFlying = false;
    float zero = { 0xff };
    for (int i = 0; i < 100; i++) {
        this->per1mDataArray[i] = zero;
        this->isCollectedArray[i] = UNSET_DATA;
    }
    return SENSOR_WORKING;
}

sensor_status_t DOFSensor::collectData( int altitude )
{
  if( firstPoll )
  {
    timeOfLastPoll = micros();
    ACCEL_VEL_TRANSITION =  (double)(BNO055_SAMPLERATE_DELAY_MS) / 1000.0;
    firstPoll = false;
  }
  else if( (micros() - timeOfLastPoll) < (BNO055_SAMPLERATE_DELAY_MS * 1000) )
  {
    return SENSOR_WORKING; // skip this. It is too fast
  }
  else
  {
    ACCEL_VEL_TRANSITION =  (double)((micros() - timeOfLastPoll)/1000) / 1000.0;
    timeOfLastPoll = micros();
  }
  sensors_event_t orientationData , linearAccelData;
  bno.getEvent(&orientationData, Adafruit_BNO055::VECTOR_EULER);
  //  bno.getEvent(&angVelData, Adafruit_BNO055::VECTOR_GYROSCOPE);
  bno.getEvent(&linearAccelData, Adafruit_BNO055::VECTOR_LINEARACCEL);

  xPos = xPos + ACCEL_POS_TRANSITION * linearAccelData.acceleration.x;
  yPos = yPos + ACCEL_POS_TRANSITION * linearAccelData.acceleration.y;

  // velocity of sensor in the direction it's facing
  headingVel = ACCEL_VEL_TRANSITION * linearAccelData.acceleration.y / cos(DEG_2_RAD * orientationData.orientation.x);

  if( headingVel > 10000 || headingVel < -10000)
  {
     return SENSOR_WORKING; //THROW OUT NOISE
  }
  updateAverage(headingVel);
  updatePeak(headingVel);
  updatePer1mDataArray( headingVel, altitude );
  #ifdef DEBUGDOF
  if (printCount * BNO055_SAMPLERATE_DELAY_MS >= PRINT_DELAY_MS) {
    //enough iterations have passed that we can print the latest data
    Serial.println("--------------AVERAGE-----------------");
    Serial.print( this->average );
    Serial.println("--------------------------------");

    Serial.println("---------------PEAK-----------------");
    Serial.print( this->peak );
    Serial.println("--------------------------------");
    printPer1mData();
    printCount = 0;
  }
  else {
    printCount = printCount + 1;
  }
  #endif

  static int8_t lowCounter = 0;
  if( headingVel < .02 && headingVel > -.02 )
  {
    lowCounter++;
  }
  else
  {
    lowCounter = 0;
  }
  if( lowCounter >= 10 )
  {
    this->doneFlying = true;
  }

  return SENSOR_WORKING;
}

bool DOFSensor::isFlying()
{
   if( firstPoll )
  {
    timeOfLastPoll = micros();
    ACCEL_VEL_TRANSITION =  (double)(BNO055_SAMPLERATE_DELAY_MS) / 1000.0;
    firstPoll = false;
  }
  else if( (micros() - timeOfLastPoll) < (BNO055_SAMPLERATE_DELAY_MS * 1000) )
  {
    return SENSOR_WORKING; // skip this. It is too fast
  }
  else
  {
    ACCEL_VEL_TRANSITION =  (double)((micros() - timeOfLastPoll)/1000) / 1000.0;
    timeOfLastPoll = micros();
  }
  sensors_event_t orientationData , linearAccelData;
  bno.getEvent(&orientationData, Adafruit_BNO055::VECTOR_EULER);
  //  bno.getEvent(&angVelData, Adafruit_BNO055::VECTOR_GYROSCOPE);
  bno.getEvent(&linearAccelData, Adafruit_BNO055::VECTOR_LINEARACCEL);

  xPos = xPos + ACCEL_POS_TRANSITION * linearAccelData.acceleration.x;
  yPos = yPos + ACCEL_POS_TRANSITION * linearAccelData.acceleration.y;

  // velocity of sensor in the direction it's facing
  headingVel = ACCEL_VEL_TRANSITION * linearAccelData.acceleration.y / cos(DEG_2_RAD * orientationData.orientation.x);

  if( headingVel > 10000 || headingVel < -10000)
  {
     return false; //THROW OUT NOISE
  }

  static int8_t highCounter = 0;
  if( headingVel > .10 )
  {
    highCounter++;
  }
  else
  {
    highCounter = 0;
  }
  if( highCounter >= 2 )
  {
    return true;
  }

  return false;

}
