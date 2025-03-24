/*********************************************************************
OLED Screen Implementation File
    Here is where you define all the methods that were declares in the
    header file.

    Since the header file contains two difference classes, the scope
    operator (::) distinguishes which methods belong to which class.

Written by:
Chris Silman
*********************************************************************/

#include "OLEDManager.h"
#include "Arduino.h"

/************************************
* Main Screen
*************************************/
void MainScreen::initialize(uint8_t i2caddr) {
    display.begin(i2caddr, true);
    clearDisplay();
}

void MainScreen::clearDisplay() {
    display.clearDisplay();
    display.display();
}

void MainScreen::showMenu() {
    clearDisplay();
    display.setTextSize(2);
    display.setTextColor(SH110X_WHITE);
    display.setCursor(0,0);
    display.println(F("DATA"));
    display.setCursor(0,16);
    display.println(F("LAUNCH"));
    display.setCursor(0,32);
    display.println(F("SLEEP"));
    display.setCursor(0,48);
    display.println(F("SETTINGS"));
    display.display();

    selectedScreen = MENU;
}

void MainScreen::showLaunch() {
    clearDisplay();
    display.setTextSize(1);

    selectedScreen = LAUNCH;
}

void MainScreen::resetDisplay() {
    clearDisplay();
    showMenu();
}

void MainScreen::updateLocalData(LocalData data) {
    if (data_screen_enabled) {
        Serial.println("Showing local data");
    }
}

void MainScreen::updateRocketData(RocketData data) {
    if (data_screen_enabled) {
        Serial.println("Showing rocket data");
    }
}

void MainScreen::receiveScreenInput(UserInput input) {
    uint8_t* selectedIndex = NULL;

    // Grab correct index
    switch (selectedScreen) {
        case MENU:
            selectedIndex = &screenSelectionIndexes.menuIndex;
            break;
        case DATA:
            selectedIndex = &screenSelectionIndexes.dataIndex;
            break;
        case LAUNCH:
            selectedIndex = &screenSelectionIndexes.launchIndex;
            break;
        case SLEEP:
            selectedIndex = &screenSelectionIndexes.sleepIndex;
            break;
        case SETTINGS:
            selectedIndex = &screenSelectionIndexes.settingsIndex;
            break;
        default:
            // Invalid screen
            return;
      }

      Serial.print("Index before: "); Serial.println(*selectedIndex);

    // Command handling
    switch (input) {
        case ENC_LEFT:
            // if encoder turned left, move up one menu option
            if (*selectedIndex > 0) {
                *selectedIndex = *selectedIndex-1;
            }
            break;
        case ENC_RIGHT:
            // if encoder turned right, move down one menu option
            if (*selectedIndex < MAX_MENU_OPTIONS) {
                *selectedIndex = *selectedIndex+1;
            }
            break;
        case ENC_PRESS:
            // statements
            break;
        case BIG_RED:
            // statements
            break;
        case KEY_ON:
            // statements
            break;
        case KEY_OFF:
            // statements
            break;
        default:
            // Invalid input
            return;
      }

      Serial.print("Index after: "); Serial.println(*selectedIndex);

    Serial.println("Called receiveScreenInput");
}


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