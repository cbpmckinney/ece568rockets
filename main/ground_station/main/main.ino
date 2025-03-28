/*********************************************************************
ECE56800 Final Project - Model Rocket Telemetry and Launch Control

Written by:
Colin McKinney, Chase Engle, Chris Silman,
Collin Hoffman, Cole Thornton, Brandon Crudele,
William Li
*********************************************************************/

#include <Adafruit_SH110X.h>
#include "OLEDManager.h"

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

  //mainScreen.showMenu();
  mainScreen.receiveScreenInput(ENC_RIGHT);
  delay(500);
  mainScreen.receiveScreenInput(ENC_RIGHT);
  delay(500);
  mainScreen.receiveScreenInput(ENC_LEFT);
  delay(500);
  mainScreen.receiveScreenInput(ENC_RIGHT);
  delay(500);
  mainScreen.receiveScreenInput(ENC_RIGHT);
  delay(500);
  mainScreen.receiveScreenInput(ENC_RIGHT);
  delay(500); 
  mainScreen.receiveScreenInput(ENC_LEFT);
  delay(500);
  mainScreen.receiveScreenInput(ENC_LEFT);
  delay(500);
  mainScreen.receiveScreenInput(ENC_PRESS);
  delay(500);
  mainScreen.receiveScreenInput(ENC_PRESS);
  delay(500);
  mainScreen.receiveScreenInput(ENC_PRESS);
  delay(500);
  mainScreen.receiveScreenInput(ENC_RIGHT);
  delay(500);
  mainScreen.receiveScreenInput(ENC_PRESS);
  Serial.print("Wrote!");

  delay(2000);
}

void loop() {
  // put your main code here, to run repeatedly:

}
