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

void MainScreen::returnToMenu() {
    clearDisplay();
    showMenu();
}

void MainScreen::refreshCurrentScreen() {
    switch (selectedScreen) {
        case MENU:
            showMenu();
            break;
        case DATA:
            showLaunch();
            break;
        case LAUNCH:
            //TODO
            break;
        case SLEEP:
            //TODO
            break;
        case SETTINGS:
            //TODO
            break;
        default:
            // Invalid screen
            return;
      }
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
    uint8_t* maxIndex = NULL;
    uint8_t prev_index = 255;

    // Grab correct index to track
    switch (selectedScreen) {
        case MENU:
            selectedIndex = &screenSelectionIndexes.menuIndex;
            maxIndex = &screenSelectionIndexes.menuMaxIndex;
            break;
        case DATA:
            selectedIndex = &screenSelectionIndexes.dataIndex;
            maxIndex = &screenSelectionIndexes.dataMaxIndex;
            break;
        case LAUNCH:
            selectedIndex = &screenSelectionIndexes.launchIndex;
            maxIndex = &screenSelectionIndexes.launchMaxIndex;
            break;
        case SLEEP:
            selectedIndex = &screenSelectionIndexes.sleepIndex;
            maxIndex = &screenSelectionIndexes.sleepMaxIndex;
            break;
        case SETTINGS:
            selectedIndex = &screenSelectionIndexes.settingsIndex;
            maxIndex = &screenSelectionIndexes.settingsMaxIndex;
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
                prev_index = *selectedIndex;
                *selectedIndex = *selectedIndex-1;
                updateScreenPointer(*selectedIndex, prev_index);
            }
            break;
        case ENC_RIGHT:
            // if encoder turned right, move down one menu option
            if (*selectedIndex < *maxIndex) {
                prev_index = *selectedIndex;
                *selectedIndex = *selectedIndex+1;
                updateScreenPointer(*selectedIndex, prev_index);
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
      

    Serial.println("Called receiveScreenInput\n");
}

void MainScreen::updateScreenPointer(uint8_t index, uint8_t prev_index) {
    // If there was a prev_index passed, use that to remove 
    // pointer rather the refreshing whole screen
    if (prev_index != 255) {
        display.setCursor(112,16*prev_index);
        display.setTextColor(SH110X_BLACK);
        display.println(F("<"));
        display.display();
    } else {
        // Using this method causes the screen to flicker as it redraws
        refreshCurrentScreen();
    }

    // Redraws pointer
    display.setCursor(112,16*index);
    display.setTextColor(SH110X_WHITE);
    display.println(F("<"));
    display.display();
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