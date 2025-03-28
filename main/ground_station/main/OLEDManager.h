/*********************************************************************
OLED Screen Header File
    This file is a bit atypical of a standard header file, however, it
    declares all the classes, members, and methods of the OLED screens.

Written by:
Chris Silman
*********************************************************************/

#ifndef _OLEDManager_
#define _OLEDManager_ 

#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 128 // OLED display height, in pixels
#define OLED_RESET -1     // can set an oled reset pin if desired
#define MAX_MENU_OPTIONS 3 // 0 included

#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <Arduino.h>
#include <memory>

// Represents possible user inputs
enum UserInput {
    ENC_LEFT,
    ENC_RIGHT,
    ENC_PRESS,
    BIG_RED,
    KEY_ON,
    KEY_OFF
};

// Represents what screen the user is currently on
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
    uint8_t sleepIndex = 0;
    uint8_t settingsIndex = 0;

    // Includes 0; 3 = 0, 1, 2, 3
    uint8_t menuMaxIndex = 3;
    uint8_t dataMaxIndex = 0;
    uint8_t launchMaxIndex = 1;
    uint8_t sleepMaxIndex = 0;
    uint8_t settingsMaxIndex = 0;
};

struct ScreenNavInfo {
    Screen nextScreen;
    uint8_t cursor_x_index;
    uint8_t cursor_y_index;
};

class MainScreen {
  public:
    // CLASS DATA MEMBERS
    Adafruit_SH1107 display = Adafruit_SH1107(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET, 1000000, 100000);
    ScreenCursorIndex screenCursorIndexes;
    Screen currentScreen = MENU;
    bool data_screen_enabled;
    bool rocket_armed;
    ScreenNavInfo menuOptions[4];
    ScreenNavInfo launchOptions[2];

    // CLASS METHODS
    void drawCentreString(const char *buf, int x, int y);
    void initialize(uint8_t i2caddr);
    void clearDisplay();
    void showMenu();
    void showLaunch();
    void returnToMenu();
    void refreshCurrentScreen();
    void jumpToScreen(Screen screen);
    void updateLocalData(LocalData data);
    void updateRocketData(RocketData data);
    void receiveScreenInput(UserInput input);
    void updateScreenPointer(uint8_t x_index, uint8_t y_index, uint8_t prev_x_index=255, uint8_t prev_y_index=255);
};

class AuxillaryScreen {
  public:
    Adafruit_SH1107 display = Adafruit_SH1107(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET, 1000000, 100000);
    void initialize(uint8_t i2caddr);
    void clearDisplay();
};


#endif // _OLEDManager_