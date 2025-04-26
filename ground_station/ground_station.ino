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
#include "LocalDataSensors.h"
#include <Adafruit_GPS.h>

#include <hardware/regs/m0plus.h>
#include <hardware/regs/addressmap.h>
#include <hardware/resets.h>

inline void reboot() {
    //reset USB controller
    reset_block(RESETS_WDSEL_USBCTRL_BITS);
    //you might want to add other perpherials here if you use them

    //reset the CPU
    auto & AIRCR_register = *(volatile uint32_t*)(PPB_BASE + M0PLUS_AIRCR_OFFSET);
    //From datasheet:
    //31:16 VECTKEY: On writes, write 0x05FA to VECTKEY, otherwise the write is ignored.
    //15 ENDIANESS: 0 = Little-endian.
    //14:3 Reserved
    //2 SYSRESETREQ: Writing 1 to this bit causes the SYSRESETREQ signal to the outer system to be asserted to request a reset.
    //1 VECTCLRACTIVE: not relevant here
    AIRCR_register = (0x05FA << M0PLUS_AIRCR_VECTKEY_LSB) | M0PLUS_AIRCR_SYSRESETREQ_BITS;
}

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
#define BUTTON_PRESS_LIMIT 250

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
    VERIFY_DATA,
    ERR,
    RESET
  };
};











// Ground station
GroundStation::STATE state = GroundStation::STATE::ERR;

// Screens
MainScreen mainScreen = MainScreen();
AuxiliaryScreen auxScreen = AuxiliaryScreen();
LocalSensorData groundStationData;
RocketData receivedRocketData;
uint8_t* pin = NULL;


DataStorage D_VelocityData;
DataStorage T_TempData;
DataStorage T_HumidityData;
DataStorage A_PressureData;
DataStorage A_TempData;
float A_Altitude;

GPSDataStorage RocketGPSData;

bool LaunchCommandSent = false;
unsigned long lastButtonPressTime = 0;

// Sensors
LocalDataSensors localDataSensors = LocalDataSensors();

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

// Radio
bool RFInit = false;
rocket_states_t currRocketState = BOOTUP;
sensorStatus statusByte;
DOFSensor dofSensor;
AltitudeSensor altitude_sensor;
TemperatureSensor temperature_sensor;
Sensor gps;
RFManager rfManager;
bool armCommandReceived = false;
bool launchCommandReceived = false;
bool verifyDataCommandReceived = false;
bool reinitializeCommandReceived = false;

bool keySwitched = false;
bool pinCorrect = false;
bool buttonPressed = false;
bool sendLaunchCommand = false;

int incomingByte;  // for serial keyboard control

void setup() {
  state = GroundStation::STATE::BOOTUP;
  Serial.begin(115200);
  delay(1000); // Wait for serial

  Serial.println("Serial started!");

  initializeLED();
  initializePeripherals();
  initializeScreens();
  localDataSensors.initializeSensors();

  encoder = new RotaryEncoder(ROTARY_PIN_B, ROTARY_PIN_A, RotaryEncoder::LatchMode::TWO03);
  attachInterrupt(digitalPinToInterrupt(ROTARY_PIN_A), checkEncoderPosition, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ROTARY_PIN_B), checkEncoderPosition, CHANGE);

  //Pin configuration
  pinMode(KEY_SW_PIN, INPUT);


}

void loop() {

  switch(state) {
    case GroundStation::STATE::BOOTUP:
      state = GroundStation::STATE::CONN_WAIT;
      break;

    case GroundStation::STATE::CONN_WAIT:
      // Verify connection to rocket
      if (!RFInit)
      {
        statusByte.bits.RFtransmitter = rfManager.initialize();
        RFInit = !RFInit;
      } else {

        if (rfManager.receiveStatus2(SAFE))
        {
          Serial.println("Rocket in safe mode, message received!!");
          updateLEDColor(0, 255, 0);
          state = GroundStation::STATE::SAFE;
        }
      }
      
      // Collect data from the local data sensors
      // Internally only collects every 1 second
      groundStationData = localDataSensors.collectData();

      // If data screen enabled
      updateDataDisplay();

      // Process user input
      processUserInput();

      // Allow serial commands to change state
      processStateBypassSerialCommands();

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

      static uint32_t timer_safe = millis();

      incomingByte = Serial.read();
      if (incomingByte == 75)
      {
        incomingByte = Serial.read();
        Serial.println("Key override, ARMING");
        rfManager.sendCommand(ARM_PACKET);
        //state = GroundStation::STATE::FIRE;
      }

      if (keyInserted() and (millis() - timer_safe > 1000)) {
        Serial.println("Key switched, ARMING!");
        mainScreen.key_inserted = true;
        rfManager.sendCommand(ARM_PACKET);
        timer_safe = millis();
      } 
      else
      {
        mainScreen.key_inserted = false;
      }

      if (rfManager.receiveStatus2(ARM))
      {
        Serial.println("Rocket in ARM mode, message received!!");
        mainScreen.rocket_armed = true;
        updateLEDColor(255,45,11);
        state = GroundStation::STATE::ARM;
      }

      // Collect data from the local data sensors
      // Internally only collects every 1 second
      groundStationData = localDataSensors.collectData();

      // If data screen enabled
      updateDataDisplay();

      // Process user input
      processUserInput();

      // Allow serial commands to change state
      processStateBypassSerialCommands();

      break;

    // ARM---------------------------------------
    case GroundStation::STATE::ARM:


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
        // This continuously sends, maybe not?
        rfManager.sendCommand(RTL_PACKET);
      }

      incomingByte = Serial.read();
      if (incomingByte == 80)
      {
        incomingByte = Serial.read();
        Serial.println("Pin override, PRIMING");
        rfManager.sendCommand(RTL_PACKET);
        //state = GroundStation::STATE::FIRE;
      }

      if (rfManager.receiveStatus2(READY_FOR_LAUNCH))
      {
        Serial.println("Rocket in RTL mode, message received!!");
        updateLEDColor(255, 0, 0);
        state = GroundStation::STATE::PRIME;
      }

      // Collect data from the local data sensors
      // Internally only collects every 1 second
      groundStationData = localDataSensors.collectData();

      // If data screen enabled
      updateDataDisplay();

      // Process user input
      processUserInput();

      // Allow serial commands to change state
      processStateBypassSerialCommands();

      break;
      
    // PRIME---------------------------------------
    case GroundStation::STATE::PRIME:
      digitalWrite(RED_BTN_LED_PIN, 1);
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
        state = GroundStation::STATE::FIRE;
      }


      incomingByte = Serial.read();
      if (incomingByte == 66)
      {
         incomingByte = Serial.read();
         Serial.println("Button override: launching!");
         sendLaunchCommand = true;
         //rfManager.sendCommand(LAUNCH_PACKET);
         //state = GroundStation::STATE::FIRE;
      }

      

      last_button_state = read_button_value;

      // Collect data from the local data sensors
      // Internally only collects every 1 second
      groundStationData = localDataSensors.collectData();

      // If data screen enabled
      updateDataDisplay();

      // Process user input
      processUserInput();

      // Allow serial commands to change state
      processStateBypassSerialCommands();

      break;

    case GroundStation::STATE::FIRE:
      if (!LaunchCommandSent)
      {
        Serial.println("Sending launch command to rocket");
        rfManager.sendCommand(LAUNCH_PACKET);
        LaunchCommandSent = true;
      }

      if (rfManager.receiveStatus2(LAUNCH))
      {
        Serial.println("Rocket acks launch command");
        mainScreen.rocket_armed = true;
        updateLEDColor(255,45,11);
        state = GroundStation::STATE::COLLECT;
      }

      if (rfManager.receiveStatus2(FLIGHT))
      {
        Serial.println("Rocket in flight!");
        state = GroundStation::STATE::COLLECT;
      }

      if (rfManager.receiveStatus2(IGNITIONFAILURE))
      {
        Serial.println("Ignition failure, switching to ERROR state");
        state = GroundStation::STATE::ERR;
      }


      
      // processUserInput();?
        

      // Allow serial commands to change state
      processStateBypassSerialCommands();

      break;

    case GroundStation::STATE::COLLECT:

      static bool recoveryStarted = false;
      //Serial.println("COLLECT");
      rfManager.receiveData(D_VelocityData, T_TempData, T_HumidityData, A_PressureData, A_TempData, &A_Altitude);
      
      //Serial.print("Peak Velocity: ");
      //Serial.println(D_VelocityData.peak);
      //Serial.print("Peak Temperature: ");
      //Serial.println(T_TempData.peak);
      //Serial.print("Peak Humidity: ");
      //Serial.println(T_HumidityData.peak);
      //Serial.print("Peak Pressure: ");
      //Serial.println(A_PressureData.peak);


      //Serial.print("Average Velocity: ");
      //Serial.println(D_VelocityData.average);
      //Serial.print("Average Temperature: ");
      //Serial.println(T_TempData.average);
      //Serial.print("Average Humidity: ");
      //Serial.println(T_HumidityData.average);
      //Serial.print("Average Pressure: ");
      //Serial.println(A_PressureData.average);


      // processUserInput( to show);?
      // Force data screen?
      // Receive radio information, make that top priority
      // Should Rocket send RECOVERY message to exit this state?

      // Allow serial commands to change state
      processStateBypassSerialCommands();


      if (rfManager.receiveStatus2(RECOVERY))
      {
        Serial.println("Entering Recovery");
        state = GroundStation::STATE::RECOVERY;
      }

      if (rfManager.receiveStatus2(IGNITIONFAILURE))
      {
        Serial.println("Entering Recovery");
        state = GroundStation::STATE::ERR;
      }


      break;

    case GroundStation::STATE::RECOVERY:
      // processUserInput();?
      // Update AUX screen with coordinates and arrow?

      rfManager.receiveGPSData(RocketGPSData);
      if (RocketGPSData.latitude != 0)
      {
        Serial.print("Landing LAT: ");
        Serial.print(RocketGPSData.latitude);
        Serial.print(" ");
        Serial.println(RocketGPSData.lat);

        Serial.print("Landing LON: ");
        Serial.print(RocketGPSData.longitude);
        Serial.print(" ");
        Serial.println(RocketGPSData.lon);
      }

      if (!recoveryStarted)
      {
      Serial.print("Peak Velocity: ");
      Serial.println(D_VelocityData.peak);
      Serial.print("Peak Temperature: ");
      Serial.println(T_TempData.peak);
      Serial.print("Peak Humidity: ");
      Serial.println(T_HumidityData.peak);
      Serial.print("Peak Pressure: ");
      Serial.println(A_PressureData.peak);


      Serial.print("Average Velocity: ");
      Serial.println(D_VelocityData.average);
      Serial.print("Average Temperature: ");
      Serial.println(T_TempData.average);
      Serial.print("Average Humidity: ");
      Serial.println(T_HumidityData.average);
      Serial.print("Average Pressure: ");
      Serial.println(A_PressureData.average);

      recoveryStarted = true;
      }


      // Allow serial commands to change state
      processStateBypassSerialCommands();
      break;

    case GroundStation::STATE::VERIFY_DATA:

      break;

    case GroundStation::STATE::ERR:
      static uint32_t timer_err = millis();
      
      last_button_state = read_button_value;
      groundStationData = localDataSensors.collectData();
      updateDataDisplay();
      processUserInput();

      if (millis() - timer_err > 1000)
      {
        rfManager.sendCommand(REINITIALIZE);
        timer_err = millis();
      }

      if (rfManager.receiveStatus2(RESET))
      {
        state = GroundStation::STATE::RESET;
      }

      processStateBypassSerialCommands();

      break;

    case GroundStation::STATE::RESET:

      Serial.println("RESETTING IN 1 SECOND");
      delay(1000);
      reboot();
      break;

  }
}

/*
* !!!FOR TESTING PURPOSES ONLY!!!
*
* This function allows the user to bypass/change states via
* serial commands. It should not be left in on release, as the
* user will be able to negate security requirements that 
* prevent launch.
*/
void processStateBypassSerialCommands() {
  if (Serial.available() > 0) {
    Serial.println("Received serial input");

    char incomingSerialData[16];
    int index = 0;

    while (Serial.available() > 0) {
      incomingSerialData[index] = Serial.read();
      index++;
    }
    incomingSerialData[index] = '\0';

    if (strstr(incomingSerialData, "SAFE")) 
    {
      Serial.println("USER CMD: SAFE RECEIVED");
      Serial.println("Changing state.");

      updateLEDColor(0, 255, 0);
      mainScreen.rocket_armed = false;

      state = GroundStation::STATE::SAFE;
    } 
    else if (strstr(incomingSerialData, "ARM")) 
    {
      Serial.println("USER CMD: ARM RECEIVED");
      Serial.println("Changing state.");

      updateLEDColor(255,45,11);
      mainScreen.rocket_armed = true; // Act as if ARM command was received
      rfManager.sendCommand(ARM_PACKET);

      state = GroundStation::STATE::ARM;
    } 
    else if (strstr(incomingSerialData, "PRIME")) 
    {
      Serial.println("USER CMD: PRIME RECEIVED");
      Serial.println("Changing state.");
      
      updateLEDColor(255, 0, 0);
      mainScreen.rocket_armed = true; // Act as if ARM command was received
      rfManager.sendCommand(RTL_PACKET);

      state = GroundStation::STATE::PRIME;
    } 
    else if (strstr(incomingSerialData, "FIRE")) 
    {
      Serial.println("USER CMD: FIRE RECEIVED");
      Serial.println("Changing state.");

      mainScreen.rocket_armed = true; // Act as if ARM command was received
      rfManager.sendCommand(LAUNCH_PACKET);

      state = GroundStation::STATE::FIRE;
    } 
    else if (strstr(incomingSerialData, "COLLECT")) 
    {
      Serial.println("USER CMD: COLLECT RECEIVED");
      Serial.println("Changing state.");

      mainScreen.rocket_armed = false;

      state = GroundStation::STATE::COLLECT;
    }
    else if (strstr(incomingSerialData, "RECOVERY")) 
    {
      Serial.println("USER CMD: RECOVERY RECEIVED");
      Serial.println("Changing state.");

      mainScreen.rocket_armed = false;

      state = GroundStation::STATE::RECOVERY;
    }
    else if (strstr(incomingSerialData, "RESET"))
    {
      Serial.println("USER CMD: RESET");
      state = GroundStation::STATE::RESET;
    }  
    else if (strstr(incomingSerialData, "ERR"))
    {
      Serial.println("USER CMD: ERR");
      state = GroundStation::STATE::ERR;
    }  
    else if (strstr(incomingSerialData, "?")) 
    {
      Serial.print("    BOOTUP = ");    Serial.println("0");
      Serial.print("    CONN_WAIT = "); Serial.println("1");
      Serial.print("    SAFE = ");      Serial.println("2");
      Serial.print("    ARM = ");       Serial.println("3");
      Serial.print("    PRIME = ");     Serial.println("4");
      Serial.print("    FIRE = ");      Serial.println("5");
      Serial.print("    COLLECT = ");   Serial.println("6");
      Serial.print("    RECOVERY = ");  Serial.println("7");
      Serial.print("    ERR = ");       Serial.println("8");
      Serial.print("    RESET = ");     Serial.println("9");
      Serial.print("Current ground station state is: "); Serial.println(state);
        
    }
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
  updateLEDColor(255, 150, 0);
  Serial.println("LED Initialized");
}

void initializePeripherals() {
  pinMode(RED_BTN_LED_PIN, OUTPUT);
  pinMode(KEY_SW_PIN, INPUT);
  Serial.println("Peripheral Inputs Initialized");
}

void initializeScreens() {
  mainScreen.initialize(0x3C);
  auxScreen.initialize(0x3D);

  mainScreen.showMenu();

  Serial.println("Screens initialized!");

  //testFullLaunch(mainScreen);
  //testDataScreen(mainScreen, auxScreen);

  delay(500);
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
    direction = (int)(encoder->getDirection());
    pos = newPos;
  }

  if (direction == -1 && buttonState == 0) {
    mainScreen.receiveScreenInput(ENC_LEFT);
  } 
  else if (direction == 1 && buttonState == 0) 
  {
    mainScreen.receiveScreenInput(ENC_RIGHT);
  } 
  else if (buttonState == 1 && previousButtonState == 0) 
  {
    // Require the button state to have changed (cannot hold)
    if (millis() - lastButtonPressTime > BUTTON_PRESS_LIMIT)
    {
      // Reduce effect of rapid pressing or spurious inputs.
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
