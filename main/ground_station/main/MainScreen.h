/*********************************************************************
Main Screen Header File
    Declares the Main Screen class, its members, and methods.

Written by:
Chris Silman
*********************************************************************/

#ifndef _MainScreen_
#define _MainScreen_ 

#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 128 // OLED display height, in pixels
#define OLED_RESET -1     // can set an oled reset pin if desired
#define MAX_MENU_OPTIONS 3 // 0 included

#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <Arduino.h>
#include <memory>
#include "Screen.h"

// Enum that defines all possible user inputs
enum UserInput {
    ENC_LEFT,
    ENC_RIGHT,
    ENC_PRESS,
    BIG_RED,
    KEY_ON,
    KEY_OFF
};

// Represents the index at which the user was last on for screens that
// have the option to select items
struct ScreenCursorIndex {
    uint8_t menuIndex = 0;
    uint8_t dataIndex = 0;
    uint8_t launchIndex = 0;
    uint8_t launchWaitIndex = 0;
    uint8_t launchSeqIndex = 0;
    uint8_t launchBigRedIndex = 0;
    uint8_t launchWrongPinIndex = 0;
    uint8_t sleepIndex = 0;
    uint8_t settingsIndex = 0;

    // Includes 0; 3 = 0, 1, 2, 3
    uint8_t menuMaxIndex = 3;
    uint8_t dataMaxIndex = 2;
    uint8_t launchMaxIndex = 1;
    uint8_t launchWaitMaxIndex = 0;
    uint8_t launchSeqMaxIndex = 3;
    uint8_t launchBigRedMaxIndex = 0;
    uint8_t launchWrongPinMaxIndex = 0;
    uint8_t sleepMaxIndex = 0;
    uint8_t settingsMaxIndex = 0;
};

// Stores navigation information for moving the cursor and jumping
// to another screen
struct ScreenNavInfo {
    Screen nextScreen;
    uint8_t cursor_x_index;
    uint8_t cursor_y_index;
    uint8_t pin_x_index = 0;
    uint8_t pin_y_index = 0;
};

class MainScreen {
    private:
        // HELPER METHODS
        void drawCentreString(const char *buf, int x, int y);

    public:
        // CLASS DATA MEMBERS
        Adafruit_SH1107 display = Adafruit_SH1107(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET, 1000000, 100000);
        ScreenCursorIndex screenCursorIndexes;
        Screen currentScreen = MENU;
        bool data_screen_enabled = false;
        bool rocket_armed = false;
        bool key_inserted = false;
        bool pin_correct = false;
        bool request_show_data = false;
        Screen data_screen_requested = NONE;
        ScreenNavInfo menuOptions[4];
        ScreenNavInfo dataOptions[3];
        ScreenNavInfo launchOptions[2];
        ScreenNavInfo launchWaitOptions[1];
        ScreenNavInfo launchSeqOptions[4];
        ScreenNavInfo launchBigRedOptions[1];
        ScreenNavInfo launchWrongPinOptions[1];
        uint8_t pin[3] = {0, 0, 0};

        // CLASS METHODS
        void initialize(uint8_t i2caddr);
        void clearDisplay();
        void showMenu();
        void showDataScreen();
        void showLaunch();
        void showLaunchWait();
        void showLaunchSeq();
        void showLaunchBigRed();
        void showLaunchWrongPin();
        void jumpToScreen(Screen screen);
        void receiveScreenInput(UserInput input);
        void updateScreenCursor(uint8_t x_index, uint8_t y_index, uint8_t prev_x_index=255, uint8_t prev_y_index=255);
        void updatePinNumber(uint8_t x_index, uint8_t y_index, uint8_t value, uint8_t prev_value=255);
        uint8_t* getInputPin(); // PIN IS MUTABLE, BAD PRACTICE?
};

#endif // _MainScreen_