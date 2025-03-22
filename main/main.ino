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

  mainScreen.hello();

  mainScreen.display.begin(0x3D, true);
  mainScreen.display.clearDisplay();

  mainScreen.display.setTextSize(1);             // Normal 1:1 pixel scale
  mainScreen.display.setTextColor(SH110X_WHITE);        // Draw white text
  mainScreen.display.setCursor(0,0);             // Start at top-left corner
  mainScreen.display.println(F("MainScreen to main, do you read?"));

  mainScreen.display.display();

  auxScreen.display.begin(0x3C, true);
  auxScreen.display.clearDisplay();

  auxScreen.display.setTextSize(1);             // Normal 1:1 pixel scale
  auxScreen.display.setTextColor(SH110X_WHITE);        // Draw white text
  auxScreen.display.setCursor(0,0);             // Start at top-left corner
  auxScreen.display.println(F("AuxScreen to main, do you read?"));

  auxScreen.display.display();

  Serial.print("Wrote!");

  delay(2000);
}

void loop() {
  // put your main code here, to run repeatedly:

}
