/*********************************************************************
ECE56800 Final Project - Model Rocket Telemetry and Launch Control

Written by:
Colin McKinney, Chase Engle, Chris Silman,
Collin Hoffman, Cole Thornton, Brandon Crudele,
William Li
*********************************************************************/

#include <Adafruit_SH110X.h>
#include "MainScreen.h"
#include "AuxillaryScreen.h"

#define DEBUG_TIME_SCALE 250

MainScreen mainScreen = MainScreen();
AuxillaryScreen auxScreen = AuxillaryScreen();

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  delay(1000); // Wait for serial
  Serial.println("Serial started! 2");

  initializeScreens();

}

void initializeScreens() {
  Serial.println("Initializing screens!");

  mainScreen.initialize(0x3D);
  auxScreen.initialize(0x3C);

  mainScreen.showMenu();

  mainScreen.receiveScreenInput(ENC_RIGHT);
  delay(1*DEBUG_TIME_SCALE);
  mainScreen.receiveScreenInput(ENC_RIGHT);
  delay(1*DEBUG_TIME_SCALE);
  mainScreen.receiveScreenInput(ENC_LEFT);
  delay(1*DEBUG_TIME_SCALE);
  mainScreen.receiveScreenInput(ENC_RIGHT);
  delay(1*DEBUG_TIME_SCALE);
  mainScreen.receiveScreenInput(ENC_RIGHT);
  delay(1*DEBUG_TIME_SCALE);
  mainScreen.receiveScreenInput(ENC_RIGHT);
  delay(1*DEBUG_TIME_SCALE); 
  mainScreen.receiveScreenInput(ENC_LEFT);
  delay(1*DEBUG_TIME_SCALE);
  mainScreen.receiveScreenInput(ENC_LEFT);
  delay(1*DEBUG_TIME_SCALE);
  mainScreen.receiveScreenInput(ENC_PRESS); // Launch Screen
  delay(4*DEBUG_TIME_SCALE);
  mainScreen.receiveScreenInput(ENC_PRESS); // Rocket Must Be Armed Screen
  delay(10*DEBUG_TIME_SCALE);
  mainScreen.receiveScreenInput(ENC_PRESS); // Back to Menu Screen
  delay(2*DEBUG_TIME_SCALE);
  mainScreen.receiveScreenInput(ENC_PRESS); // Launch Screen
  delay(1*DEBUG_TIME_SCALE);
  mainScreen.receiveScreenInput(ENC_RIGHT); // Launch - N
  delay(2*DEBUG_TIME_SCALE);
  mainScreen.receiveScreenInput(ENC_PRESS); // Back to Menu Screen
  delay(1*DEBUG_TIME_SCALE);
  mainScreen.receiveScreenInput(ENC_PRESS); // Launch Screen
  delay(1*DEBUG_TIME_SCALE);
  mainScreen.receiveScreenInput(ENC_LEFT); // Launch - Y
  delay(1*DEBUG_TIME_SCALE);
  mainScreen.rocket_armed = true; // Arm rocket for testing
  mainScreen.receiveScreenInput(ENC_PRESS); // Launch Seq Screen
  delay(1*DEBUG_TIME_SCALE);
  mainScreen.receiveScreenInput(ENC_RIGHT);
  delay(1*DEBUG_TIME_SCALE);
  mainScreen.receiveScreenInput(ENC_RIGHT);
  delay(1*DEBUG_TIME_SCALE);
  mainScreen.receiveScreenInput(ENC_LEFT);
  delay(1*DEBUG_TIME_SCALE);
  mainScreen.receiveScreenInput(ENC_PRESS); // Pin value 1 submitted
  delay(1*DEBUG_TIME_SCALE);
  mainScreen.receiveScreenInput(ENC_LEFT);
  delay(1*DEBUG_TIME_SCALE);
  mainScreen.receiveScreenInput(ENC_RIGHT);
  delay(1*DEBUG_TIME_SCALE);
  mainScreen.receiveScreenInput(ENC_PRESS); // Pin value 2 submitted
  delay(1*DEBUG_TIME_SCALE);
  mainScreen.receiveScreenInput(ENC_RIGHT);
  delay(1*DEBUG_TIME_SCALE);
  mainScreen.receiveScreenInput(ENC_RIGHT);
  delay(1*DEBUG_TIME_SCALE);
  mainScreen.receiveScreenInput(ENC_RIGHT);
  delay(1*DEBUG_TIME_SCALE);
  mainScreen.receiveScreenInput(ENC_PRESS); // Pin value 3 submitted
  delay(4*DEBUG_TIME_SCALE);

  // We are now about to submit pin, verify validity
  uint8_t* pin = mainScreen.getInputPin();
  Serial.print("Checking pin...");
  if (pin[0] == 1 && pin[1] == 1 && pin[2] == 3)
  {
    Serial.println("Pin correct!");
    mainScreen.pin_correct = true;
  } else {
    Serial.println("Pin incorrect!");
  }

  mainScreen.receiveScreenInput(ENC_PRESS); // Submit pin, go to btn launch screen
  delay(4*DEBUG_TIME_SCALE);
  mainScreen.receiveScreenInput(ENC_PRESS); // Go back to menu screen
  delay(2*DEBUG_TIME_SCALE);
 
  Serial.println("Wrote!");

  mainScreen.clearDisplay();

  delay(2000);
}

void loop() {
  // put your main code here, to run repeatedly:

}
