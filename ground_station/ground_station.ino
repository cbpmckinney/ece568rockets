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
#include "RFManager.h"


// Launch Code
#define PIN_VALUE_1 2
#define PIN_VALUE_2 1
#define PIN_VALUE_3 3

// Peripheral Pins
#define ROTARY_PIN_B 27
#define ROTARY_PIN_A 28
#define ROTARY_BTN 24
#define RED_BTN_LED_PIN 11
#define RED_BTN_PRESS_PIN 10
#define KEY_SW_PIN 12 // SW GREEN = CLOSED, SW RED = OPEN
#define LED_RED 5
#define LED_GREEN 6
#define LED_BLUE 9

// How much time needs to pass before registering another button press, in ms
#define BUTTON_PRESS_LIMIT 500

namespace GroundStation {
  enum STATE
  {
    BOOTUP,
    CONN_WAIT,
    SAFE,
    ARM,
    PRIME,
    FIRE,
    COLLECT,
    RECOVERY,
    ERR
  };
};


// Ground station
GroundStation::STATE state = GroundStation::STATE::ERR;

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

// Button debouncing
int read_button_value = 0;
int last_debounce_time = 0;
int last_button_state = 0;
int debounce_delay = 50; //ms


uint8_t statbuf[3];  // buffer for receiving status messages
int incomingByte;

void setup() {
  state = GroundStation::STATE::BOOTUP;
  Serial.begin(9600);
  delay(1000); // Wait for serial

  Serial.println("Serial started!");

  initializeLED();
  initializePeripherals();
  initializeScreens();

  encoder = new RotaryEncoder(ROTARY_PIN_B, ROTARY_PIN_A, RotaryEncoder::LatchMode::TWO03);
  attachInterrupt(digitalPinToInterrupt(ROTARY_PIN_A), checkEncoderPosition, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ROTARY_PIN_B), checkEncoderPosition, CHANGE);
}

void loop() {

  static uint8_t statbuf[3];  // buffer for receiving status messages

  // Switch, only one state processed per main operating loop

  switch(state) {
    // INITIALIZE---------------------------------------
    case GroundStation::STATE::BOOTUP:
    // Radio
      static bool RFInit = false;
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

      bool keySwitched = false;
      bool pinCorrect = false;
      bool buttonPressed = false;
      bool sendLaunchCommand = false;

      state = GroundStation::STATE::CONN_WAIT;
      break;

    case GroundStation::STATE::CONN_WAIT:
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
        updateLEDColor(0, 255, 0);
        state = GroundStation::STATE::SAFE;
        statbuf[2] = 0;
      }

      break;

    // SAFE---------------------------------------
    case GroundStation::STATE::SAFE:
      // Check if ARM message received
      /*
      if (ARMMessageReceived)
      {
        1. Respond with ARM Message CONFIRMED.
        2. Update main screen to have state rocket_armed = true (mainScreen.rocket_armed = true;)
        3. Update LED on ground station to signify rocket is armed (updateLEDColor(255,95,31);)
        4. Update state to ARM (state = ARM;)
      }
      */

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
        mainScreen.rocket_armed = true;
        updateLEDColor(255,95,31);
        state = GroundStation::STATE::ARM;
      }

      // If data screen enabled
      updateDataDisplay();

      // Process user input
      processUserInput();

      break;

    // ARM---------------------------------------
    case GroundStation::STATE::ARM:
      if (keyInserted()) {
        Serial.println("Key inserted");
        mainScreen.key_inserted = true;
      } 
      else
      {
        mainScreen.key_inserted = false;
      }

      if (mainScreen.ready_to_submit_pin) {
        //User is about to submit pin, check if correct as
        //changes cannot be made now
        pin = mainScreen.getInputPin();

       if (validatePin(pin)) {
        mainScreen.pin_correct = true;
       }
      }

      if (mainScreen.prime_permissive) {
        // User (screen) gave go-ahead on priming rocket.
        // Send message to rocket signifying ground station ready to PRIME
        //  *requires a response back
        // Make big red button glow
        state = GroundStation::STATE::PRIME;
        updateLEDColor(255, 0, 0);
      }

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
        state = GroundStation::STATE::PRIME;
      }

      // If data screen enabled
      updateDataDisplay();

      // Process user input
      processUserInput();

      break;
      
    // PRIME---------------------------------------
    case GroundStation::STATE::PRIME:
      Serial.println("PRIME");

      read_button_value = digitalRead(RED_BTN_PRESS_PIN);
      
      // Debounce button input
      if (read_button_value != last_button_state) {
        last_debounce_time = millis();
      }

      if ((millis() - last_debounce_time) > debounce_delay) {
        if (read_button_value == 1) {
          // BUTTON PRESSED
          Serial.println("Button pressed: Sending launch command!");
          sendLaunchCommand = true;
        }
      }

      if (sendLaunchCommand) {
        rfManager.sendCommand(LAUNCH_PACKET);

        rfManager.receiveStatus(statbuf);

        if (statbuf[2] == LAUNCH)
        {
          Serial.println("Rocket in LAUNCH mode, message received!!");
          updateLEDColor(255, 0, 0);
          delay(100);
          updateLEDColor(0, 0, 0);
          delay(100);
          updateLEDColor(255, 0, 0);
          delay(100);
          updateLEDColor(0, 0, 0);
          delay(100);
          updateLEDColor(255, 0, 0);
          delay(100);
          updateLEDColor(0, 0, 0);
          delay(100);
          state = GroundStation::STATE::FIRE;
        }
      }

      incomingByte = Serial.read();
      if (incomingByte == 98)
      {
        incomingByte = Serial.read();
        Serial.println("Button pressed: launching!");
        rfManager.sendCommand(LAUNCH_PACKET);
      }

      last_button_state = read_button_value;

      processUserInput();

      break;

    case GroundStation::STATE::FIRE:
      Serial.println("FIRE");
      // processUserInput();?
      break;

    case GroundStation::STATE::COLLECT:
      Serial.println("COLLECT");
      // processUserInput( to show);?
      // Force data screen?
      // Receive radio information, make that top priority
      // Should Rocket send RECOVERY message to exit this state?
      break;

    case GroundStation::STATE::RECOVERY:
      Serial.println("RECOVERY");

      // processUserInput();?
      // Update AUX screen with coordinates and arrow?
      break;
      

  }

}

void checkEncoderPosition()
{
  encoder->tick(); // just call tick() to check the state.
}

void updateLEDColor(int red, int green, int blue) {
  analogWrite(LED_RED, red);
  analogWrite(LED_GREEN, green);
  analogWrite(LED_BLUE, blue);
}

void initializeLED() {
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
  updateLEDColor(255, 255, 0);
}

void initializePeripherals() {
  pinMode(KEY_SW_PIN, INPUT);
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
    if (auxScreen.currentScreen != ScreenEnums::Screen::NONE) {
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
  // CURRENTLY NOT DEBOUNCED, SHOULD BE?
  if (digitalRead(KEY_SW_PIN)) {
    return true;
  } else {
    return false;
  }
  
}
