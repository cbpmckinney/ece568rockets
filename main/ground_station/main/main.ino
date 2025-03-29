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
#include "OLEDScreenTests.h"

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

  testFullLaunch(mainScreen);

  delay(2000);
}

void loop() {
  // put your main code here, to run repeatedly:

}
