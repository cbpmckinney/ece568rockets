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
* Helper Functions
*************************************/

void MainScreen::drawCentreString(const char *buf, int x, int y)
{
    int16_t x1, y1;
    uint16_t w, h;
    display.getTextBounds(buf, x, y, &x1, &y1, &w, &h); //calc width of new string
    display.setCursor(x - w / 2, y);
    display.print(buf);
}

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

    // Set up launchOptions
    launchOptions[0] = LAUNCH_WAIT; // Y
    launchOptions[1] = MENU; // N

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
    currentScreen = MENU;

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
}

void MainScreen::showLaunch() {
    currentScreen = LAUNCH;

    clearDisplay();
    display.setTextSize(2);
    display.setTextColor(SH110X_WHITE);
    drawCentreString("PRIME", 64, 0);
    drawCentreString("ROCKET?", 88, 16);
    display.setCursor(32,64);
    display.println(F("Y"));
    display.setCursor(80,64);
    display.println(F("N"));
    display.display();
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
            updateScreenPointerVert(screenCursorIndexes.menuIndex, 0);
            break;
        case DATA:
            //TODO
            break;
        case LAUNCH:
            showLaunch();
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
            Serial.println("No valid screen! Going to main");
            showMenu();
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
                selectedIndex = &screenCursorIndexes.menuIndex;
                maxIndex = &screenCursorIndexes.menuMaxIndex;
                break;
            case DATA:
                selectedIndex = &screenCursorIndexes.dataIndex;
                maxIndex = &screenCursorIndexes.dataMaxIndex;
                break;
            case LAUNCH:
                selectedIndex = &screenCursorIndexes.launchIndex;
                maxIndex = &screenCursorIndexes.launchMaxIndex;
                break;
            case SLEEP:
                selectedIndex = &screenCursorIndexes.sleepIndex;
                maxIndex = &screenCursorIndexes.sleepMaxIndex;
                break;
            case SETTINGS:
                selectedIndex = &screenCursorIndexes.settingsIndex;
                maxIndex = &screenCursorIndexes.settingsMaxIndex;
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
                updateScreenPointerVert(*selectedIndex, prev_index);
                Serial.print("Index after: "); Serial.println(*selectedIndex);
            }
            break;
        case ENC_RIGHT:
            // if encoder turned right, move down one menu option
            if (*selectedIndex < *maxIndex) {
                prev_index = *selectedIndex;
                *selectedIndex = *selectedIndex+1;
                updateScreenPointerVert(*selectedIndex, prev_index);
                Serial.print("Index after: "); Serial.println(*selectedIndex);
            }
            break;
        case ENC_PRESS:
            if (currentScreen == MENU) {
                Serial.print("Jumping to: "); Serial.println(menuOptions[*selectedIndex]);
                jumpToScreen(menuOptions[*selectedIndex]);
            } else if (currentScreen == LAUNCH) {
                Serial.print("Jumping to: "); Serial.println(launchOptions[*selectedIndex]);
                jumpToScreen(launchOptions[*selectedIndex]);
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

void MainScreen::updateScreenPointerVert(uint8_t index, uint8_t prev_index) {
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

void MainScreen::updateScreenPointerHorz(uint8_t x_index, uint8_t y_index, uint8_t prev_x_index, uint8_t prev_y_index) {
    // If there was a prev_index passed, use that to remove 
    // pointer rather the refreshing whole screen
    if (prev_x_index != 255 && prev_y_index != 255) {
        display.setCursor(16*prev_x_index,16*prev_x_index);
        display.setTextColor(SH110X_BLACK);
        display.println(F("<"));
        display.display();
    } else {
        // Using this method causes the screen to flicker as it redraws
        refreshCurrentScreen();
    }

    // Redraws pointer
    display.setCursor(16*x_index,16*y_index);
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