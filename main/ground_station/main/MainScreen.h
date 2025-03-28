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

// Enum that defines all possible user inputs
enum UserInput {
    ENC_LEFT,
    ENC_RIGHT,
    ENC_PRESS,
    BIG_RED,
    KEY_ON,
    KEY_OFF
};

// Enum that defines all possible screens
enum Screen {
    MENU,
    DATA,
    LAUNCH,
    LAUNCH_WAIT,
    LAUNCH_SEQ,
    LAUNCH_BIG_RED,
    SLEEP,
    SETTINGS
};

// Expected structure of local data
struct LocalData {
    int temp;
    int humidity;
    int altitude;
    int pressure;
    int gps; // temporary, likely will be an array
};

// Expected structure of rocket data
struct RocketData {
    int temp;
    int humidity;
    int altitude;
    int pressure;
    int gps; //temporary, likely will be an array
    int absOrientation_EV;
    int absOreintation_Q;
    int angularVelocity;
    int accelerationVector;
    int magneticFieldStrengthVector;
    int linearAccelerationVector;
    int gravityVector;
};

// Represents the index at which the user was last on for screens that
// have the option to select items
struct ScreenCursorIndex {
    uint8_t menuIndex = 0;
    uint8_t dataIndex = 0;
    uint8_t launchIndex = 0;
    uint8_t launchWaitIndex = 0;
    uint8_t sleepIndex = 0;
    uint8_t settingsIndex = 0;

    // Includes 0; 3 = 0, 1, 2, 3
    uint8_t menuMaxIndex = 3;
    uint8_t dataMaxIndex = 0;
    uint8_t launchMaxIndex = 1;
    uint8_t launchWaitMaxIndex = 0;
    uint8_t sleepMaxIndex = 0;
    uint8_t settingsMaxIndex = 0;
};

// Stores navigation information for moving the cursor and jumping
// to another screen
struct ScreenNavInfo {
    Screen nextScreen;
    uint8_t cursor_x_index;
    uint8_t cursor_y_index;
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
        bool data_screen_enabled;
        bool rocket_armed = false;
        ScreenNavInfo menuOptions[4];
        ScreenNavInfo launchOptions[2];
        ScreenNavInfo launchWaitOptions[1];

        // CLASS METHODS
        void initialize(uint8_t i2caddr);
        void clearDisplay();
        void showMenu();
        void showLaunch();
        void showLaunchWait();
        void jumpToScreen(Screen screen);
        void updateLocalData(LocalData data);
        void updateRocketData(RocketData data);
        void receiveScreenInput(UserInput input);
        void updateScreenCursor(uint8_t x_index, uint8_t y_index, uint8_t prev_x_index=255, uint8_t prev_y_index=255);
};

#endif // _MainScreen_