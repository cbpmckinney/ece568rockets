/*********************************************************************
Auxiliary Screen Implementation File
    Here is where you define all the methods that were declared in the
    header file.

    The scope operator (::) distinguishes the class the method is for.

Written by:
Chris Silman
*********************************************************************/

#include "AuxiliaryScreen.h"
#include "Arduino.h"

/************************************
* Auxiliary Screen
*************************************/

void AuxiliaryScreen::initialize(uint8_t i2caddr) {
    display.begin(i2caddr, true);
    clearDisplay();
}

void AuxiliaryScreen::clearDisplay() {
    display.clearDisplay();
    display.display();
}

void AuxiliaryScreen::showLocalData() {
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

void AuxiliaryScreen::showRocketData() {
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

void AuxiliaryScreen::jumpToScreen(Screen screen) {
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

void AuxiliaryScreen::enableShowingData() {
    data_screen_enabled = true;
}

void AuxiliaryScreen::disableShowingData() {
    data_screen_enabled = false;
    jumpToScreen(NONE);
}

void AuxiliaryScreen::requestScreen(Screen targetScreen) {
    if (targetScreen == LOCAL or targetScreen == ROCKET) {
        jumpToScreen(targetScreen);
    }
}

void AuxiliaryScreen::refreshDataPoint(int old_data, int new_data, uint8_t index_x, uint8_t index_y, const char* message) {
    display.setTextColor(SH110X_BLACK);
    display.setCursor(index_x,index_y);
    display.print(F(message)); display.println(old_data);
    display.setTextColor(SH110X_WHITE);
    display.setCursor(index_x,index_y);
    display.print(F(message)); display.println(new_data);
}

void AuxiliaryScreen::updateLocalData(LocalData input_data) {
    if (data_screen_enabled) {
        Serial.println("Showing local data");
        // Find what data has actually updated since last update
        if (storedLocalData.temp !=  input_data.temp) {
            refreshDataPoint(storedLocalData.temp, input_data.temp, 0, 8, "Temp (F): ");
            storedLocalData.temp = input_data.temp;
        }

        if (storedLocalData.humidity != input_data.humidity) {
            refreshDataPoint(storedLocalData.humidity, input_data.humidity, 0, 16, "Humidity: ");
            storedLocalData.humidity = input_data.humidity;
        }

        if (storedLocalData.altitude !=input_data.altitude) {
            refreshDataPoint(storedLocalData.altitude, input_data.altitude, 0, 24, "Altitude: ");
            storedLocalData.altitude =input_data.altitude;
        }

        if (storedLocalData.pressure != input_data.pressure) {
            refreshDataPoint(storedLocalData.pressure, input_data.pressure, 0, 32, "Pressure: ");
            storedLocalData.pressure = input_data.pressure;
        }

        if (storedLocalData.gps != input_data.gps) {
            refreshDataPoint(storedLocalData.gps, input_data.gps, 0, 40, "GPS: ");
            storedLocalData.gps = input_data.gps;
        }
    }
    display.display();
}

void AuxiliaryScreen::updateRocketData(RocketData input_data) {
    if (data_screen_enabled) {
        Serial.println("Showing local data");
        // Find what data has actually updated since last update
        if (storedLocalData.temp !=  input_data.temp) {
            refreshDataPoint(storedLocalData.temp, input_data.temp, 0, 8, "Temp (F): ");
            storedLocalData.temp = input_data.temp;
        }

        if (storedLocalData.humidity != input_data.humidity) {
            refreshDataPoint(storedLocalData.humidity, input_data.humidity, 0, 16, "Humidity: ");
            storedLocalData.humidity = input_data.humidity;
        }

        if (storedLocalData.altitude !=input_data.altitude) {
            refreshDataPoint(storedLocalData.altitude, input_data.altitude, 0, 24, "Altitude: ");
            storedLocalData.altitude =input_data.altitude;
        }

        if (storedLocalData.pressure != input_data.pressure) {
            refreshDataPoint(storedLocalData.pressure, input_data.pressure, 0, 32, "Pressure: ");
            storedLocalData.pressure = input_data.pressure;
        }

        if (storedLocalData.gps != input_data.gps) {
            refreshDataPoint(storedLocalData.gps, input_data.gps, 0, 40, "GPS: ");
            storedLocalData.gps = input_data.gps;
        }
    }
    display.display();
}