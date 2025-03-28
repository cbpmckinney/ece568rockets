/*********************************************************************
Main Screen Implementation File
    Here is where you define all the methods that were declared in the
    header file.

    The scope operator (::) distinguishes the class the method is for.

Written by:
Chris Silman
*********************************************************************/

#include "MainScreen.h"
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
    menuOptions[0] = {DATA, 7, 0};
    menuOptions[1] = {LAUNCH, 7, 1};
    menuOptions[2] = {SLEEP, 7, 2};
    menuOptions[3] = {SETTINGS, 7, 3};

    // Set up launchOptions
    launchOptions[0] = {LAUNCH_WAIT, 3, 4}; // Y
    launchOptions[1] = {MENU, 6, 4}; // N

    // Set up launchWaitOptions
    launchWaitOptions[0] = {MENU, 5, 6}; // MENU
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
    updateScreenCursor(menuOptions[screenCursorIndexes.menuIndex].cursor_x_index, 
                        menuOptions[screenCursorIndexes.menuIndex].cursor_y_index);
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
    updateScreenCursor(launchOptions[screenCursorIndexes.launchIndex].cursor_x_index, 
                        launchOptions[screenCursorIndexes.launchIndex].cursor_y_index);
    display.display();
}

void MainScreen::showLaunchWait() {
    currentScreen = LAUNCH_WAIT;

    clearDisplay();
    display.setTextSize(2);
    display.setTextColor(SH110X_WHITE);
    display.setCursor(0,0);
    display.println(F("Rckt must be ARMed  to initiate a launch sequence."));
    display.setCursor(32,96);
    display.println(F("MENU"));
    updateScreenCursor(launchWaitOptions[screenCursorIndexes.launchWaitIndex].cursor_x_index, 
                        launchWaitOptions[screenCursorIndexes.launchWaitIndex].cursor_y_index);
    display.display();
}

void MainScreen::jumpToScreen(Screen screen) {
    Serial.print("In jump to screen, going to: "); Serial.println(screen);
    switch (screen) {
        case MENU:
            Serial.println("Jumped to MENU");
            showMenu();
            break;
        case DATA:
            Serial.println("Jumped to DATA");
            break;
        case LAUNCH:
            Serial.println("Jumped to LAUNCH");
            showLaunch();
            break;
        case LAUNCH_WAIT:
            Serial.println("Jumped to LAUNCH WAIT");
            showLaunchWait();
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
    uint8_t* cursorIndex = NULL;
    uint8_t* maxScreenIndex = NULL;
    ScreenNavInfo* screenNavInfo = NULL;
    uint8_t prev_x_index = 255;
    uint8_t prev_y_index = 255;

    // Grab information related to current screen.
    if (input == ENC_LEFT or input == ENC_RIGHT or input == ENC_PRESS) {
        switch (currentScreen) {
            case MENU:
                cursorIndex = &screenCursorIndexes.menuIndex;
                maxScreenIndex = &screenCursorIndexes.menuMaxIndex;
                screenNavInfo = menuOptions;
                break;
            case DATA:
                cursorIndex = &screenCursorIndexes.dataIndex;
                maxScreenIndex = &screenCursorIndexes.dataMaxIndex;
                break;
            case LAUNCH:
                cursorIndex = &screenCursorIndexes.launchIndex;
                maxScreenIndex = &screenCursorIndexes.launchMaxIndex;
                screenNavInfo = launchOptions;
                break;
            case LAUNCH_WAIT:
                cursorIndex = &screenCursorIndexes.launchWaitIndex;
                maxScreenIndex = &screenCursorIndexes.launchWaitMaxIndex;
                screenNavInfo = launchWaitOptions;
                break;
            case SLEEP:
                cursorIndex = &screenCursorIndexes.sleepIndex;
                maxScreenIndex = &screenCursorIndexes.sleepMaxIndex;
                break;
            case SETTINGS:
                cursorIndex = &screenCursorIndexes.settingsIndex;
                maxScreenIndex = &screenCursorIndexes.settingsMaxIndex;
                break;
            default:
                // Invalid screen
                return;
          }
          Serial.print("Index before: "); Serial.println(*cursorIndex);
    }

    // Command handling
    switch (input) {
        case ENC_LEFT:
            // if encoder turned left, decrement index by one and update cursor location
            if (*cursorIndex > 0) {
                prev_x_index = screenNavInfo[*cursorIndex].cursor_x_index;
                prev_y_index = screenNavInfo[*cursorIndex].cursor_y_index;
                *cursorIndex = *cursorIndex-1;
                Serial.print("Index after: "); Serial.println(*cursorIndex);
                updateScreenCursor(screenNavInfo[*cursorIndex].cursor_x_index, 
                                        screenNavInfo[*cursorIndex].cursor_y_index, 
                                        prev_x_index, 
                                        prev_y_index);
            }
            break;
        case ENC_RIGHT:
            // if encoder turned right, increment index by one and update cursor location
            if (*cursorIndex < *maxScreenIndex) {
                prev_x_index = screenNavInfo[*cursorIndex].cursor_x_index;
                prev_y_index = screenNavInfo[*cursorIndex].cursor_y_index;
                *cursorIndex = *cursorIndex+1;
                Serial.print("Index after: "); Serial.println(*cursorIndex);
                updateScreenCursor(screenNavInfo[*cursorIndex].cursor_x_index, 
                                        screenNavInfo[*cursorIndex].cursor_y_index, 
                                        prev_x_index, 
                                        prev_y_index);
            }
            break;
        case ENC_PRESS:
            if (currentScreen == MENU || 
                currentScreen == LAUNCH || 
                currentScreen == LAUNCH_WAIT) {
                Serial.print("Jumping to: "); Serial.println(screenNavInfo[*cursorIndex].nextScreen);
                jumpToScreen(screenNavInfo[*cursorIndex].nextScreen);
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

void MainScreen::updateScreenCursor(uint8_t x_index, uint8_t y_index, uint8_t prev_x_index, uint8_t prev_y_index) {
    // If there were previous indices passed, use those to remove 
    // the old cursor
    Serial.print("Create at: ");Serial.print(x_index);Serial.print(", ");Serial.println(y_index);
    if (prev_x_index != 255 && prev_y_index != 255) {
        display.setCursor(16*prev_x_index,16*prev_y_index);
        display.setTextColor(SH110X_BLACK);
        display.println(F("<"));
        display.display();
    }

    // Redraws cursor
    display.setCursor(16*x_index,16*y_index);
    display.setTextColor(SH110X_WHITE);
    display.println(F("<"));
    display.display();
}