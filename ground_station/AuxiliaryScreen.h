/*********************************************************************
Auxiliary Screen Header File
    Declares the Auxiliary Screen class, its members, and methods.

Written by:
Chris Silman
*********************************************************************/

#ifndef _AuxiliaryScreen_
#define _AuxiliaryScreen_ 

#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 128 // OLED display height, in pixels
#define OLED_RESET -1     // can set an oled reset pin if desired
#define MAX_MENU_OPTIONS 3 // 0 included

#include "Screen.h"
#include "LocalDataSensors.h"

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

#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <Arduino.h>
#include <memory>

class AuxiliaryScreen {
  public:
    
    // Members
    bool data_screen_enabled = false;
    ScreenEnums::Screen currentScreen = ScreenEnums::Screen::NONE;
    LocalSensorData storedLocalData;
    RocketData storedRocketData;
    uint32_t data_update_timer = millis();

    // Methods
    Adafruit_SH1107 display = Adafruit_SH1107(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET, 1000000, 100000);
    void initialize(uint8_t i2caddr);
    void clearDisplay();
    void jumpToScreen(ScreenEnums::Screen screen);
    void showRocketData();
    void showLocalData();
    void enableShowingData();
    void disableShowingData();
    void requestScreen(ScreenEnums::Screen targetScreen);
    void refreshDataPoint(float old_data, float new_data, uint8_t index_x, uint8_t index_y, const char* prefix, const char* suffix="", bool forceInt = false);
    void refreshDataPoint(int old_data1, int old_data2, int old_data3, int new_data1, int new_data2, int new_data3, uint8_t index_x, uint8_t index_y, const char* separator, const char* prefix, bool padZeros=false);
    void updateLocalData(LocalSensorData data);
    void updateRocketData(RocketData data);
};


#endif // _AuxiliaryScreen_