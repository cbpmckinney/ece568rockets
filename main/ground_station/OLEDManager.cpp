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
            //invalid
            return;
      }

    //*selectedIndex = *selectedIndex+1;

    /*

    if (input == ENC_LEFT) {
        if (screenSelectionPointers.menuPointer > 0) {
            screenSelectionPointers.menuPointer--;
        }
        

        switch (var) {
            case label1:
              // statements
              break;
            case label2:
              // statements
              break;
            default:
              // statements
              break;
          }
        
    } else if (input == ENC_RIGHT) {
        if (screenSelectionPointers.menuPointer < MAX_MENU_OPTIONS) {
            screenSelectionPointers.menuPointer--;
        }
    } else if (input == ENC_PRESS) {
        
    } else if (input == BIG_RED) {

    } else if (input = KEY_ON) {

    } else if (input = KEY_OFF) {

    }
    else {
        Serial.println("Invalid Input")
    }*/
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