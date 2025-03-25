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

    // Set up menuOptions
    menuOptions[0] = DATA;
    menuOptions[1] = LAUNCH;
    menuOptions[2] = SLEEP;
    menuOptions[3] = SETTINGS;

    Serial.print("MENU"); Serial.println(MENU);
    Serial.print("DATA"); Serial.println(DATA);
    Serial.print("LAUNCH"); Serial.println(LAUNCH);
    Serial.print("SLEEP"); Serial.println(SLEEP);
    Serial.print("SETTINGS"); Serial.println(SETTINGS);
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

    currentScreen = MENU;
}

void MainScreen::showLaunch() {
    clearDisplay();
    display.setTextSize(2);
    display.setTextColor(SH110X_WHITE);
    display.println(F("ARM ROCKET?"));
    display.display();

    currentScreen = LAUNCH;
}

void MainScreen::returnToMenu() {
    clearDisplay();
    showMenu();
}

void MainScreen::refreshCurrentScreen() {
    clearDisplay();
    switch (currentScreen) {
        case MENU:
            showMenu();
            updateScreenPointer(screenSelectionIndexes.menuIndex, 0);
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

void MainScreen::jumpToScreen(Screen screen) {
    Serial.print("In jump to screen, going to: "); Serial.println(screen);
    switch (screen) {
        case MENU:
            Serial.println("MENU");
            showMenu();
            break;
        case DATA:
            Serial.println("DATA");
            break;
        case LAUNCH:
            Serial.println("LAUNCH");
            showLaunch();
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
    if (input == ENC_LEFT or input == ENC_RIGHT or input == ENC_PRESS) {
        switch (currentScreen) {
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
    }

      

    // Command handling
    switch (input) {
        case ENC_LEFT:
            // if encoder turned left, move up one menu option
            if (*selectedIndex > 0) {
                prev_index = *selectedIndex;
                *selectedIndex = *selectedIndex-1;
                updateScreenPointer(*selectedIndex, prev_index);
                Serial.print("Index after: "); Serial.println(*selectedIndex);
            }
            break;
        case ENC_RIGHT:
            // if encoder turned right, move down one menu option
            if (*selectedIndex < *maxIndex) {
                prev_index = *selectedIndex;
                *selectedIndex = *selectedIndex+1;
                updateScreenPointer(*selectedIndex, prev_index);
                Serial.print("Index after: "); Serial.println(*selectedIndex);
            }
            break;
        case ENC_PRESS:
            if (currentScreen == MENU) {
                Serial.print("Jumping to: "); Serial.println(menuOptions[*selectedIndex]);
                jumpToScreen(menuOptions[*selectedIndex]);
            }
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