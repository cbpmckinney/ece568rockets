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
#include "AuxiliaryScreen.h"
#include "OLEDScreenTests.h"

// Launch Code
#define PIN_VALUE_1 2
#define PIN_VALUE_2 1
#define PIN_VALUE_3 3

#define ROTARY_PIN_B 27
#define ROTARY_PIN_A 28
#define ROTARY_BTN 11

// How much time needs to pass before registering another button press, in ms
#define BUTTON_PRESS_LIMIT 500

enum STATE
{
  INITIALIZE,
  CONN_WAIT,
  SAFE,
  ARM,
  PRIME,
  FIRE,
  COLLECT,
  RECOVERY,
  ERR
};

// Ground station
STATE state = ERR;

// Screens
MainScreen mainScreen = MainScreen();
AuxiliaryScreen auxScreen = AuxiliaryScreen();
LocalData groundStationData;
RocketData receivedRocketData;
uint8_t* pin = NULL;

unsigned long lastButtonPressTime = 0;

// Encoder
RotaryEncoder *encoder = nullptr;
int pos = 0;
int newPos = 0;
int previousButtonState = 0;

void setup() {
  state = INITIALIZE;
  Serial.begin(9600);
  delay(1000); // Wait for serial

  Serial.println("Serial started!");

  initializeScreens();

  encoder = new RotaryEncoder(ROTARY_PIN_B, ROTARY_PIN_A, RotaryEncoder::LatchMode::TWO03);
  attachInterrupt(digitalPinToInterrupt(ROTARY_PIN_A), checkEncoderPosition, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ROTARY_PIN_B), checkEncoderPosition, CHANGE);
}

void loop() {
  // Switch, only one state processed per main operating loop

  switch(state) {
    // INITIALIZE---------------------------------------
    case INITIALIZE:
      state = CONN_WAIT;
      break;

    case CONN_WAIT:
      // Verify connection to rocket
      state = SAFE;
      break;

    // SAFE---------------------------------------
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

      // Process user input
      processUserInput();

      break;

    // ARM---------------------------------------
    case ARM:
      if (keyInserted()) {
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
        // User (screen) gave go-ahead on priming rocket.
        // Send message to rocket signifying ground station ready to PRIME
        //  *requires a response back
        //  Update LED on ground station to signify rocket is primed
        // Make big red button glow
        state = PRIME;
      }

      // Process user input
      processUserInput();
      
    // PRIME---------------------------------------
    case PRIME:
      Serial.println("PRIME");

      processUserInput();

    case FIRE:
      Serial.println("FIRE");
      // processUserInput();?

    case COLLECT:
      Serial.println("COLLECT");
      // processUserInput( to show);?
      // Force data screen?
      // Receive radio information, make that top priority
      // Should Rocket send RECOVERY message to exit this state?

    case RECOVERY:
      Serial.println("RECOVERY");

      // processUserInput();?
      // Update AUX screen with coordinates and arrow?
      

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

  mainScreen.showMenu();

  //testFullLaunch(mainScreen);
  //testDataScreen(mainScreen, auxScreen);

  delay(2000);
}

void updateDataDisplay() {
  if (mainScreen.request_show_data == true && auxScreen.data_screen_enabled == false) {
      auxScreen.enableShowingData();
  } 
  else if (mainScreen.request_show_data == false) 
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
    auxScreen.updateLocalData(groundStationData);
    auxScreen.updateRocketData(receivedRocketData);
    if (auxScreen.currentScreen != mainScreen.data_screen_requested) {
      Serial.print("Aux: ");Serial.println(auxScreen.currentScreen);
      Serial.print("Main: ");Serial.println(mainScreen.data_screen_requested);
      auxScreen.requestScreen(mainScreen.data_screen_requested);
    }
  }
}

void processUserInput() {
  // Encoder input should be read in when read, but only
  // processed at set points through this function.

  // It will be stored in some class that has the last state read.
  // Input will be set to NONE after.
  // Read Encoder Value
  encoder->tick();

  int direction = 0;
  // Read button
  int buttonState = digitalRead(ROTARY_BTN);

  newPos = encoder->getPosition();
  if (pos != newPos && abs(abs(pos) - abs(newPos)) >= 2) {
    Serial.print("pos:");
    Serial.print(newPos);
    Serial.print(" dir:");
    direction = (int)(encoder->getDirection());
    Serial.println(direction);
    pos = newPos;
  }

  if (direction == -1 && buttonState == 0) {
    mainScreen.receiveScreenInput(ENC_LEFT);
  } else if (direction == 1 && buttonState == 0) {
    mainScreen.receiveScreenInput(ENC_RIGHT);
  } 
  else if (buttonState == 1 && previousButtonState == 0) 
  {
    // Require the button state to have changed (cannot hold)
    if (millis() - lastButtonPressTime > BUTTON_PRESS_LIMIT)
    {
      // Reduce effect of rapid pressing or spurious inputs.
      Serial.println("Received button press");
      mainScreen.receiveScreenInput(ENC_PRESS);
      lastButtonPressTime = millis();
    }
  }
 
  previousButtonState = buttonState;
  
}

bool validatePin(uint8_t* pin) {
    // Pin cannot be zeros, this is default
    if (pin[0] == 0 && pin[1] == 0 && pin[2] == 0) 
    {
      // 0 0 0 IS INVALID PIN VALUE
      return false;
    }

    if (pin[0] == PIN_VALUE_1 && pin[1] == PIN_VALUE_2 && pin[2] == PIN_VALUE_3)
    {
        return true;
    } else {
        return false;
    }
}

bool keyInserted() {
  // TODO
  return false;
}
