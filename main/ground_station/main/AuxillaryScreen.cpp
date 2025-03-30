/*********************************************************************
Auxillary Screen Implementation File
    Here is where you define all the methods that were declared in the
    header file.

    The scope operator (::) distinguishes the class the method is for.

Written by:
Chris Silman
*********************************************************************/

#include "AuxillaryScreen.h"
#include "Arduino.h"

/************************************
* Auxillary Screen
*************************************/

void AuxillaryScreen::initialize(uint8_t i2caddr) {
    display.begin(i2caddr, true);
    clearDisplay();
}

void AuxillaryScreen::clearDisplay() {
    display.clearDisplay();
    display.display();
}

void AuxillaryScreen::showLocalData() {
    currentScreen = LOCAL;

    clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SH110X_WHITE);
    display.setCursor(0,0);
    display.println(F("LOCAL DATA"));
    display.setCursor(0,8);
    display.print(F("Temp (F): ")); display.println(storedLocalData.temp);
    display.setCursor(0,16);
    display.print(F("Humidity: ")); display.println(storedLocalData.humidity);
    display.setCursor(0,24);
    display.print(F("Altitude: ")); display.println(storedLocalData.altitude);
    display.setCursor(0,32);
    display.print(F("Pressure: ")); display.println(storedLocalData.pressure);
    display.setCursor(0,40);
    display.print(F("GPS: ")); display.println(storedLocalData.gps);
    display.display();
}

void AuxillaryScreen::showRocketData() {
    currentScreen = ROCKET;

    clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SH110X_WHITE);
    display.setCursor(0,0);
    display.println(F("ROCKET DATA"));
    display.setCursor(0,8);
    display.print(F("Temp (F): ")); display.println(storedRocketData.temp);
    display.setCursor(0,16);
    display.print(F("Humidity: ")); display.println(storedRocketData.humidity);
    display.setCursor(0,24);
    display.print(F("Altitude: ")); display.println(storedRocketData.altitude);
    display.setCursor(0,32);
    display.print(F("Pressure: ")); display.println(storedRocketData.pressure);
    display.setCursor(0,40);
    display.print(F("GPS: ")); display.println(storedRocketData.gps);
    display.display();
}

void AuxillaryScreen::jumpToScreen(Screen screen) {
    Serial.print("In jump to screen, going to: "); Serial.println(screen);
    switch (screen) {
        case LOCAL:
            showLocalData();
            break;
        case ROCKET:
            showRocketData();
            break;
        case NONE:
            currentScreen = NONE;
            clearDisplay();
            break;
        default:
            return;

    }
}

void AuxillaryScreen::enableShowingData() {
    data_screen_enabled = true;
}

void AuxillaryScreen::disableShowingData() {
    data_screen_enabled = false;
    jumpToScreen(NONE);
}

void AuxillaryScreen::requestScreen(Screen targetScreen) {
    if (targetScreen == LOCAL or targetScreen == ROCKET) {
        jumpToScreen(targetScreen);
    }
}

void AuxillaryScreen::updateLocalData(LocalData data) {
    if (data_screen_enabled) {
        Serial.println("Showing local data");
    }
}

void AuxillaryScreen::updateRocketData(RocketData data) {
    if (data_screen_enabled) {
        Serial.println("Showing rocket data");
    }
}