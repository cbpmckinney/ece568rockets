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
    menuOptions[0] = {ScreenEnums::Screen::DATA, 7, 0};
    menuOptions[1] = {ScreenEnums::Screen::LAUNCH, 7, 1};
    menuOptions[2] = {ScreenEnums::Screen::SLEEP, 7, 2};
    menuOptions[3] = {ScreenEnums::Screen::SETTINGS, 7, 3};

    // Set up dataOptions
    dataOptions[0] = {ScreenEnums::Screen::LOCAL, 5, 4}; // LOCAL
    dataOptions[1] = {ScreenEnums::Screen::ROCKET, 6, 5}; // ROCKET
    dataOptions[2] = {ScreenEnums::Screen::NONE, 5, 6};

    // Set up launchOptions
    launchOptions[0] = {ScreenEnums::Screen::LAUNCH_WAIT, 3, 4}; // Y
    launchOptions[1] = {ScreenEnums::Screen::MENU, 6, 4}; // N

    // Set up launchWaitOptions
    launchWaitOptions[0] = {ScreenEnums::Screen::MENU, 5, 6}; // MENU

    // Set up launchSeqOptions
    launchSeqOptions[0] = {ScreenEnums::Screen::NONE, 2, 5, 3, 10}; // NUM
    launchSeqOptions[1] = {ScreenEnums::Screen::NONE, 4, 5, 7, 10}; // NUM
    launchSeqOptions[2] = {ScreenEnums::Screen::NONE, 6, 5, 11, 10}; // NUM
    launchSeqOptions[3] = {ScreenEnums::Screen::LAUNCH_BIG_RED, 5, 7}; // Submit

    // Set up launchWrongPinOptions
    launchBigRedOptions[0] = {ScreenEnums::Screen::MENU, 5, 6}; // MENU

    // Set up launchWrongPinOptions
    launchWrongPinOptions[0] = {ScreenEnums::Screen::MENU, 5, 6}; // MENU
}

void MainScreen::clearDisplay() {
    display.clearDisplay();
    display.display();
}

void MainScreen::showMenu() {
    currentScreen = ScreenEnums::Screen::MENU;

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

void MainScreen::showDataScreen() {
    currentScreen = ScreenEnums::Screen::DATA;

    clearDisplay();
    display.setTextSize(2);
    display.setTextColor(SH110X_WHITE);
    drawCentreString("SHOW", 64, 0);
    drawCentreString("DATA?", 88, 16);
    display.setCursor(0,64);
    display.println(F("LOCAL"));
    display.setCursor(0,80);
    display.println(F("ROCKET"));
    display.setCursor(0,96);
    display.println(F("NONE"));
    updateScreenCursor(dataOptions[screenCursorIndexes.dataIndex].cursor_x_index, 
                        dataOptions[screenCursorIndexes.dataIndex].cursor_y_index);
    display.display();
}

void MainScreen::showLaunch() {
    currentScreen = ScreenEnums::Screen::LAUNCH;

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
    if (rocket_armed) {
        // Skip wait screen, override to LAUNCH_SEQ
        jumpToScreen(ScreenEnums::Screen::LAUNCH_SEQ);
    } else
    {
        currentScreen = ScreenEnums::Screen::LAUNCH_WAIT;

        clearDisplay();
        display.setTextSize(2);
        display.setTextColor(SH110X_WHITE);
        display.setCursor(0,0);
        display.println(F("Rckt must be ARMed  to initiate a launch sequence."));
        display.setCursor(32,96);
        display.println(F("MENU"));
        updateScreenCursor(launchWrongPinOptions[screenCursorIndexes.launchWrongPinIndex].cursor_x_index, 
                            launchWrongPinOptions[screenCursorIndexes.launchWrongPinIndex].cursor_y_index);
        display.display();
    }
}

void MainScreen::showLaunchSeq() {
    currentScreen = ScreenEnums::Screen::LAUNCH_SEQ;

    clearDisplay();

    // Reset index to allow pin entering again
    screenCursorIndexes.launchSeqIndex = 0;

    display.setTextSize(2);
    display.setTextColor(SH110X_WHITE);
    display.setCursor(0,0);
    display.println(F("Insert key"));
    display.setCursor(0,32);
    display.println(F("Insert pin"));

    // Lines for pin input
    display.setCursor(24,84);
    display.println(F("_"));
    display.setCursor(56,84);
    display.println(F("_"));
    display.setCursor(88,84);
    display.println(F("_"));

    // Show current pin
    display.setCursor(24,80);
    display.println(pin[0]);
    display.setCursor(56,80);
    display.println(pin[1]);
    display.setCursor(88,80);
    display.println(pin[2]);

    // Submit
    display.setCursor(0,112);
    display.println(F("SUBMIT"));

    updateScreenCursor(launchSeqOptions[screenCursorIndexes.launchSeqIndex].cursor_x_index, 
                        launchSeqOptions[screenCursorIndexes.launchSeqIndex].cursor_y_index);
    display.display();
}

void MainScreen::showLaunchBigRed() {
    currentScreen = ScreenEnums::Screen::LAUNCH_BIG_RED;

    clearDisplay();
    display.setTextSize(2);
    display.setTextColor(SH110X_WHITE);
    display.setCursor(0,0);
    display.println(F("PRESS BTN TO LAUNCH!"));
    display.setCursor(32,96);
    display.println(F("MENU"));
    updateScreenCursor(launchBigRedOptions[screenCursorIndexes.launchBigRedIndex].cursor_x_index, 
                        launchBigRedOptions[screenCursorIndexes.launchBigRedIndex].cursor_y_index);
    display.display();
}

void MainScreen::showLaunchWrongPin() {
    currentScreen = ScreenEnums::Screen::LAUNCH_WRONG_PIN;

    clearDisplay();
    display.setTextSize(2);
    display.setTextColor(SH110X_WHITE);
    display.setCursor(0,0);
    display.println(F("Incorrect pin or key not inserted"));
    display.setCursor(32,96);
    display.println(F("MENU"));
    updateScreenCursor(launchWrongPinOptions[screenCursorIndexes.launchWrongPinIndex].cursor_x_index, 
                        launchWrongPinOptions[screenCursorIndexes.launchWrongPinIndex].cursor_y_index);
    display.display();
}

void MainScreen::jumpToScreen(ScreenEnums::Screen screen) {
    Serial.print("In jump to screen, going to: "); Serial.println(screen);
    switch (screen) {
        case ScreenEnums::Screen::MENU:
            Serial.println("Jumped to MENU");
            showMenu();
            break;
        case ScreenEnums::Screen::DATA:
            Serial.println("Jumped to DATA");
            showDataScreen();
            break;
        case ScreenEnums::Screen::LAUNCH:
            Serial.println("Jumped to LAUNCH");
            showLaunch();
            break;
        case ScreenEnums::Screen::LAUNCH_WAIT:
            Serial.println("Jumped to LAUNCH WAIT");
            showLaunchWait();
            break;
        case ScreenEnums::Screen::LAUNCH_SEQ:
            Serial.println("Jumped to LAUNCH SEQ");
            showLaunchSeq();
            break;
        case ScreenEnums::Screen::LAUNCH_BIG_RED:
            Serial.println("Jumped to LAUNCH BIG RED");
            showLaunchBigRed();
            break;
        case ScreenEnums::Screen::LAUNCH_WRONG_PIN:
            Serial.println("Jumped to LAUNCH WRONG PIN");
            showLaunchWrongPin();
            //TODO
            break;
        case ScreenEnums::Screen::SETTINGS:
            //TODO
            break;
        default:
            // Invalid screen
            Serial.println("No valid screen! Going to main");
            showMenu();
            return;
      }
}

void MainScreen::receiveScreenInput(UserInput input) {
    Serial.println("Called receiveScreenInput");
    uint8_t* cursorIndex = NULL;
    uint8_t* maxScreenIndex = NULL;
    ScreenNavInfo* screenNavInfo = NULL;
    uint8_t prev_x_index = 255;
    uint8_t prev_y_index = 255;
    ScreenEnums::Screen targetScreen;

    // Grab information related to current screen.
    if (input == ENC_LEFT or input == ENC_RIGHT or input == ENC_PRESS) {
        switch (currentScreen) {
            case ScreenEnums::Screen::MENU:
                cursorIndex = &screenCursorIndexes.menuIndex;
                maxScreenIndex = &screenCursorIndexes.menuMaxIndex;
                screenNavInfo = menuOptions;
                break;
            case ScreenEnums::Screen::DATA:
                cursorIndex = &screenCursorIndexes.dataIndex;
                maxScreenIndex = &screenCursorIndexes.dataMaxIndex;
                screenNavInfo = dataOptions;
                break;
            case ScreenEnums::Screen::LAUNCH:
                cursorIndex = &screenCursorIndexes.launchIndex;
                maxScreenIndex = &screenCursorIndexes.launchMaxIndex;
                screenNavInfo = launchOptions;
                break;
            case ScreenEnums::Screen::LAUNCH_WAIT:
                cursorIndex = &screenCursorIndexes.launchWaitIndex;
                maxScreenIndex = &screenCursorIndexes.launchWaitMaxIndex;
                screenNavInfo = launchWaitOptions;
                break;
            case ScreenEnums::Screen::LAUNCH_SEQ:
                cursorIndex = &screenCursorIndexes.launchSeqIndex;
                maxScreenIndex = &screenCursorIndexes.launchSeqMaxIndex;
                screenNavInfo = launchSeqOptions;
                break;
            case ScreenEnums::Screen::LAUNCH_BIG_RED:
                cursorIndex = &screenCursorIndexes.launchBigRedIndex;
                maxScreenIndex = &screenCursorIndexes.launchBigRedMaxIndex;
                screenNavInfo = launchBigRedOptions;
                break;
            case ScreenEnums::Screen::LAUNCH_WRONG_PIN:
                cursorIndex = &screenCursorIndexes.launchWrongPinIndex;
                maxScreenIndex = &screenCursorIndexes.launchWrongPinMaxIndex;
                screenNavInfo = launchWrongPinOptions;
                break;
            case ScreenEnums::Screen::SLEEP:
                cursorIndex = &screenCursorIndexes.sleepIndex;
                maxScreenIndex = &screenCursorIndexes.sleepMaxIndex;
                break;
            case ScreenEnums::Screen::SETTINGS:
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
            
                if (currentScreen != ScreenEnums::Screen::LAUNCH_SEQ) 
                {
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
                }
                else if (currentScreen == ScreenEnums::Screen::LAUNCH_SEQ)
                {
                    if (pin[*cursorIndex] > 0) {
                        uint8_t prev_pin_value = pin[*cursorIndex];
                        pin[*cursorIndex] = pin[*cursorIndex] - 1;

                        updatePinNumber(screenNavInfo[*cursorIndex].pin_x_index,
                                        screenNavInfo[*cursorIndex].pin_y_index,
                                        pin[*cursorIndex],
                                        prev_pin_value);
                    }
                }
            
            break;
        case ENC_RIGHT:
            // if encoder turned right, increment index by one and update cursor location
            
                if (currentScreen != ScreenEnums::Screen::LAUNCH_SEQ) 
                {
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
                } 
                else if (currentScreen == ScreenEnums::Screen::LAUNCH_SEQ)
                {
                    Serial.print("  Orig Pin value: "); Serial.println(pin[*cursorIndex]);
                    if (pin[*cursorIndex] < 9) {
                        uint8_t prev_pin_value = pin[*cursorIndex];
                        pin[*cursorIndex] = pin[*cursorIndex]+1;
                        
                        Serial.print("  New Pin value: "); Serial.println(pin[*cursorIndex]);

                        updatePinNumber(screenNavInfo[*cursorIndex].pin_x_index,
                                        screenNavInfo[*cursorIndex].pin_y_index,
                                        pin[*cursorIndex],
                                        prev_pin_value);
                    }
                }
            break;
        case ENC_PRESS:
            targetScreen = screenNavInfo[*cursorIndex].nextScreen;

            if (currentScreen != ScreenEnums::Screen::LAUNCH_SEQ and currentScreen != ScreenEnums::Screen::DATA) 
            {
                Serial.print("Jumping to: "); Serial.println(targetScreen);
                jumpToScreen(targetScreen);
            } 
            else if (currentScreen == ScreenEnums::Screen::LAUNCH_SEQ) 
            {
                if (targetScreen == ScreenEnums::Screen::NONE) {
                    // Currently looking at pin input. Need to update pin values
                    prev_x_index = screenNavInfo[*cursorIndex].cursor_x_index;
                    prev_y_index = screenNavInfo[*cursorIndex].cursor_y_index;
                    *cursorIndex = *cursorIndex+1;
                    updateScreenCursor(screenNavInfo[*cursorIndex].cursor_x_index, 
                                        screenNavInfo[*cursorIndex].cursor_y_index, 
                                        prev_x_index, 
                                        prev_y_index);

                    if (*cursorIndex == *maxScreenIndex) {
                        // Ready to submit
                        ready_to_submit_pin = true;
                    }

                } else {
                    // All pin inputs entered, now requesting we submit pin and move to another screen.
                    // The screen is NOT cognizant of any protection control, it just manages what is
                    // displayed and translates screen info into data.
                    //
                    // Enforcment of the pin MUST be managed outside of the MainScreen class via pin_correct
                    // and MainScreen::getPin().
                    //
                    // This can be done by checking if user is about to ENC_PRESS to submit pin, verifying pin,
                    // then setting pin_correct to true/false.
                    if (pin_correct && key_inserted) {
                        Serial.print("Jumping to: "); Serial.println(targetScreen); // RED BUTTON LAUNCH SCREEN
                        jumpToScreen(targetScreen);
                        prime_permissive = true;
                    } else {
                        Serial.print("Jumping to: "); Serial.println(ScreenEnums::Screen::LAUNCH_WRONG_PIN); // RED BUTTON LAUNCH SCREEN
                        jumpToScreen(ScreenEnums::Screen::LAUNCH_WRONG_PIN);
                    }

                    // Reset pin screen
                    pin_correct = false;
                    ready_to_submit_pin = false;
                }
            } else if (currentScreen == ScreenEnums::Screen::DATA) {
                if (targetScreen == ScreenEnums::Screen::NONE) {
                    request_show_data = false;
                } else {
                    request_show_data = true;
                }

                data_screen_requested = targetScreen;
                jumpToScreen(ScreenEnums::Screen::MENU);
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
      
    Serial.println("End receiveScreenInput\n");
}

void MainScreen::updateScreenCursor(uint8_t x_index, uint8_t y_index, uint8_t prev_x_index, uint8_t prev_y_index) {
    // If there were previous indices passed, use those to remove 
    // the old cursor
    Serial.print("Create at: ");Serial.print(x_index);Serial.print(", ");Serial.println(y_index);
    if (prev_x_index != 255 && prev_y_index != 255) {
        display.setCursor(16*prev_x_index,16*prev_y_index);
        display.setTextColor(SH110X_BLACK);
        display.println(F("<"));
    }

    // Redraws cursor
    display.setCursor(16*x_index,16*y_index);
    display.setTextColor(SH110X_WHITE);
    display.println(F("<"));
    display.display();
}

void MainScreen::updatePinNumber(uint8_t x_index, uint8_t y_index, uint8_t value, uint8_t prev_value) {
    Serial.print("Updating pin number: "); Serial.println(x_index); Serial.println(y_index); Serial.println(value); Serial.println(prev_value);
    if (prev_value != 255) {
        display.setCursor(8*x_index,8*y_index);
        display.setTextColor(SH110X_BLACK);
        display.println(prev_value);
    }

    // Redraws cursor
    display.setCursor(8*x_index,8*y_index);
    display.setTextColor(SH110X_WHITE);
    display.println(value);
    display.display();
}

uint8_t* MainScreen::getInputPin() {
    return pin;
}

void MainScreen::resetIndices() {
    screenCursorIndexes.menuIndex = 0;
    screenCursorIndexes.dataIndex = 0;
    screenCursorIndexes.launchIndex = 0;
    screenCursorIndexes.launchWaitIndex = 0;
    screenCursorIndexes.launchSeqIndex = 0;
    screenCursorIndexes.launchBigRedIndex = 0;
    screenCursorIndexes.launchWrongPinIndex = 0;
    screenCursorIndexes.sleepIndex = 0;
    screenCursorIndexes.settingsIndex = 0;
}