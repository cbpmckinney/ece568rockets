/*********************************************************************
ECE56800 Final Project - Model Rocket Telemetry and Launch Control

Written by:
Colin McKinney, Chase Engle, Chris Silman,
Collin Hoffman, Cole Thornton, Brandon Crudele,
William Li
*********************************************************************/

#include <Adafruit_SH110X.h>
#include <RotaryEncoder.h>
#include "MainScreen.h"
#include "AuxillaryScreen.h"
#include "OLEDScreenTests.h"

// Launch Code
#define PIN_VALUE_1 2
#define PIN_VALUE_2 1
#define PIN_VALUE_3 3

#define ROTARY_PIN_A 27
#define ROTARY_PIN_B 28

enum STATE
{
  INITIALIZE,
  CONN_WAIT,
  SAFE,
  ARM,
  PRIME,
  FIRE,
  COLLECT,
  RECOERY,
  ERR
};

// Ground station
STATE state = ERR;

// Screens
MainScreen mainScreen = MainScreen();
AuxillaryScreen auxScreen = AuxillaryScreen();
LocalData groundStationData;
RocketData receivedRocketData;
uint8_t* pin = NULL;

// Encoder
RotaryEncoder *encoder = nullptr;
int pos = 0;
int newPos;

void setup() {
  state = INITIALIZE;
  Serial.begin(9600);
  delay(1000); // Wait for serial

  Serial.println("Serial started!");

  initializeScreens();

  encoder = new RotaryEncoder(ROTARY_PIN_A, ROTARY_PIN_B, RotaryEncoder::LatchMode::TWO03);
  attachInterrupt(digitalPinToInterrupt(ROTARY_PIN_A), checkEncoderPosition, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ROTARY_PIN_B), checkEncoderPosition, CHANGE);
}

void loop() {
  // Switch, only one state processed per main operating loop

  switch(state) {
    case INITIALIZE:
      state = CONN_WAIT;
      break;

    case CONN_WAIT:
      // Verify connection to rocket
      state = SAFE;
      break;

    case SAFE:
      // Check if ARM message received
      /*
      if (ARMMessageReceived)
      {
        1. Respond with ARM Message CONFIRMED.
        2. Update main screen to have state rocket_armed = true
        3. Update LED on ground station to signify rocket is armed
        4. Update state to ARM
      }
      */

      // If data screen enabled
      updateDataDisplay();

      // Read Encoder Value
      encoder->tick();

      newPos = encoder->getPosition();
      if (pos != newPos && abs(abs(pos) - abs(newPos)) >= 2) {
        Serial.print("pos:");
        Serial.print(newPos);
        Serial.print(" dir:");
        Serial.println((int)(encoder->getDirection()));
        pos = newPos;
      }

      // Process user input
      processUserInput();

      break;

    case ARM:
      if (isKeyInserted()) {
        mainScreen.key_inserted = true;
      } 
      else
      {
        mainScreen.key_inserted = false;
      }

      if (mainScreen.ready_to_submit_pin) {
        // User is about to submit pin, check if correct as
        // changes cannot be made now
        pin = mainScreen.getInputPin();

        if (validatePin(pin)) {
          mainScreen.pin_correct = true;
        }
      }

      if (mainScreen.prime_permissive) {
        state = PRIME;
        // Update LED on ground station to signify rocket is primed
      }

      // Process user input
      processUserInput();
      

    case PRIME:
      // Make big red button glow

      processUserInput();


  }

}

void checkEncoderPosition()
{
  encoder->tick(); // just call tick() to check the state.
}

void initializeScreens() {
  Serial.println("Initializing screens!");

  mainScreen.initialize(0x3D);
  auxScreen.initialize(0x3C);

  //testFullLaunch(mainScreen);
  //testDataScreen(mainScreen, auxScreen);

  delay(2000);
}

void updateDataDisplay() {
  if (mainScreen.request_show_data && !auxScreen.data_screen_enabled) {
      auxScreen.enableShowingData();
  } 
  else if (!mainScreen.request_show_data) 
  {
    if (auxScreen.currentScreen != NONE) {
      auxScreen.disableShowingData();
    }
  } 
  else 
  {
    // else maintain current state
  }

    if (auxScreen.data_screen_enabled) {
      auxScreen.storedLocalData = groundStationData;
      auxScreen.storedRocketData = receivedRocketData;
      auxScreen.requestScreen(mainScreen.data_screen_requested);
    }
}

void processUserInput() {
  // Encoder input should be read in when read, but only
  // processed at set points through this function.

  // It will be stored in some class that has the last state read.
  // Input will be set to NONE after.
}

bool validatePin(uint8_t* pin) {
    // Pin cannot be zeros, this is default
    if (pin[0] == 0 && pin[1] == 0 && pin[2] == 0) 
    {
      // INVALID PIN VALUE
      return false;
    }

    if (pin[0] == PIN_VALUE_1 && pin[1] == PIN_VALUE_2 && pin[2] == PIN_VALUE_3)
    {
        return true;
    } else {
        return false;
    }
}

bool isKeyInserted() {
  return false;
}
