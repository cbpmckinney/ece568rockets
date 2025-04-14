/*********************************************************************
ECE56800 Final Project - Model Rocket Telemetry and Launch Control

Written by:
Colin McKinney, Chase Engle, Chris Silman,
Collin Hoffman, Cole Thornton, Brandon Crudele,
William Li
*********************************************************************/

#include <Adafruit_SH110X.h>
//#include <RotaryEncoder.h>
//#include "MainScreen.h"
//#include "AuxiliaryScreen.h"
//#include "OLEDScreenTests.h"
#include "RFManager.h"


// Launch Code
#define PIN_VALUE_1 2
#define PIN_VALUE_2 1
#define PIN_VALUE_3 3

#define ROTARY_PIN_B 27
#define ROTARY_PIN_A 28
#define ROTARY_BTN 11

// How much time needs to pass before registering another button press, in ms
#define BUTTON_PRESS_LIMIT 500


// Ground station



// Screens
//MainScreen mainScreen = MainScreen();
//AuxiliaryScreen auxScreen = AuxiliaryScreen();
//LocalData groundStationData;
//RocketData receivedRocketData;
uint8_t* pin = NULL;

unsigned long lastButtonPressTime = 0;

// Encoder
//RotaryEncoder *encoder = nullptr;
int pos = 0;
int newPos = 0;
int previousButtonState = 0;


bool RFInit = false;

void setup() {

  Serial.begin(115200);
  delay(1000); // Wait for serial

  Serial.println("Serial started!");


  //initializeScreens();

  //encoder = new RotaryEncoder(ROTARY_PIN_B, ROTARY_PIN_A, RotaryEncoder::LatchMode::TWO03);
  //attachInterrupt(digitalPinToInterrupt(ROTARY_PIN_A), checkEncoderPosition, CHANGE);
  //attachInterrupt(digitalPinToInterrupt(ROTARY_PIN_B), checkEncoderPosition, CHANGE);
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
  static bool launchCommandReceived = false;
  static bool verifyDataCommandReceived = false;
  static bool reinitializeCommandReceived = false;

  static bool keySwitched = false;
  static bool pinCorrect = false;
  static bool buttonPressed = false;


  static uint8_t statbuf[3];  // buffer for receiving status messages
  static int incomingByte;
  

  // Switch, only one state processed per main operating loop

  switch(currRocketState) {
    // INITIALIZE---------------------------------------
    case BOOTUP:

      // Verify connection to rocket
      if (!RFInit)
      {
        statusByte.bits.RFtransmitter = rfManager.initialize();
        RFInit = !RFInit;
      }
      rfManager.receiveStatus(statbuf);

      if (statbuf[2] == SAFE)
        {
        Serial.println("Rocket in safe mode, message received!!");
          currRocketState = SAFE;
          statbuf[2] = 0;
        }



      break;

    // SAFE---------------------------------------
    case SAFE:
      
      // If data screen enabled
      //updateDataDisplay();

      // Process user input
      //processUserInput();
      //if (RFInit)
      //{
      //  statusByte.bits.RFtransmitter = rfManager.initialize();
      //  RFInit = !RFInit;
      //}


      incomingByte = Serial.read();
      if (incomingByte == 107)
      {
        incomingByte = Serial.read();
        Serial.println("ARMING!");
        rfManager.sendCommand(ARM_PACKET);
      }
      rfManager.receiveStatus(statbuf);
      if (statbuf[2] != 0)
      {
        Serial.println(statbuf[2]);
      }

      if (statbuf[2] == ARM)
        {
        Serial.println("Rocket in ARM mode, message received!!");
          currRocketState = ARM;
        }


      break;

    // ARM---------------------------------------
    case ARM:

      // Assume key is inserted in SAFE to go to ARM.
      // Once in ARM, we need to enter the pin.

      incomingByte = Serial.read();
      if (incomingByte == 112)
      {
        incomingByte = Serial.read();
        Serial.println("PIN verified, Ready to Launch!");
        rfManager.sendCommand(RTL_PACKET);
      }

      rfManager.receiveStatus(statbuf);

      if (statbuf[2] == READY_FOR_LAUNCH)
        {
        Serial.println("Rocket in RTL mode, message received!!");
          currRocketState = READY_FOR_LAUNCH;
        }

      break;

      //if (keyInserted()) {
      //  mainScreen.key_inserted = true;
      //} 
      //else
      //{
      //  mainScreen.key_inserted = false;
      //}

      //if (mainScreen.ready_to_submit_pin) {
        // User is about to submit pin, check if correct as
        // changes cannot be made now
      //  pin = mainScreen.getInputPin();

      //  if (validatePin(pin)) {
      //    mainScreen.pin_correct = true;
      //  }
      //}

      //if (mainScreen.prime_permissive) {
        // User (screen) gave go-ahead on priming rocket.
        // Send message to rocket signifying ground station ready to PRIME
        //  *requires a response back
        //  Update LED on ground station to signify rocket is primed
        // Make big red button glow
      //  state = PRIME;
      //}

      // Process user input
      //processUserInput();
      
    // PRIME---------------------------------------
    case READY_FOR_LAUNCH:
      //Serial.println("READY FOR LAUNCH");


    incomingByte = Serial.read();
      if (incomingByte == 98)
      {
        incomingByte = Serial.read();
        Serial.println("Button pressed: launching!");
        rfManager.sendCommand(LAUNCH_PACKET);
      }

      rfManager.receiveStatus(statbuf);

      if (statbuf[2] == LAUNCH)
        {
        Serial.println("Rocket in LAUNCH mode, message received!!");
          currRocketState = LAUNCH;
        }

      break;
      //processUserInput();

    case LAUNCH:
      //Serial.println("LAUNCH");
      // processUserInput();?

      break;

    case FLIGHT:
      //Serial.println("FLIGHT");
      // processUserInput( to show);?
      // Force data screen?
      // Receive radio information, make that top priority
      // Should Rocket send RECOVERY message to exit this state?
      break;

    case RECOVERY:
      //Serial.println("RECOVERY");
      break;

      // processUserInput();?
      // Update AUX screen with coordinates and arrow?

    case POST_FLIGHT:
      //Serial.println("POST FLIGHT");
      break;
      

  }

}