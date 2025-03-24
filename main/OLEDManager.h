// Chris Silman
// OLED Class

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

struct LocalData {
    int temp;
    int humidity;
    int altitude;
    int pressure;
    int gps; // temporary, likely will be an array
};

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

struct ScreenSelectionIndex {
    uint8_t menuIndex;
    uint8_t dataIndex;
    uint8_t launchIndex;
    uint8_t sleepIndex;
    uint8_t settingsIndex;
};

enum UserInput {
    ENC_LEFT,
    ENC_RIGHT,
    ENC_PRESS,
    BIG_RED,
    KEY_ON,
    KEY_OFF
};

enum SelectedScreen {
    MENU,
    DATA,
    LAUNCH,
    SLEEP,
    SETTINGS
};

class MainScreen {
  public:
    Adafruit_SH1107 display = Adafruit_SH1107(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET, 1000000, 100000);
    ScreenSelectionIndex screenSelectionIndexes = {0, 0};
    SelectedScreen selectedScreen = MENU;
    bool data_screen_enabled;
    bool rocket_armed;
    void initialize(uint8_t i2caddr);
    void clearDisplay();
    void showMenu();
    void showLaunch();
    void resetDisplay();
    void updateLocalData(LocalData data);
    void updateRocketData(RocketData data);
    void receiveScreenInput(UserInput input);
};

class AuxillaryScreen {
  public:
    Adafruit_SH1107 display = Adafruit_SH1107(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET, 1000000, 100000);
    void initialize(uint8_t i2caddr);
    void clearDisplay();
};


#endif // _OLEDManager_