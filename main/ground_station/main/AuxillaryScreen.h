/*********************************************************************
Auxillary Screen Header File
    Declares the Auxillary Screen class, its members, and methods.

Written by:
Chris Silman
*********************************************************************/

#ifndef _AuxillaryScreen_
#define _AuxillaryScreen_ 

#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 128 // OLED display height, in pixels
#define OLED_RESET -1     // can set an oled reset pin if desired
#define MAX_MENU_OPTIONS 3 // 0 included

#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <Arduino.h>
#include <memory>

class AuxillaryScreen {
  public:
    Adafruit_SH1107 display = Adafruit_SH1107(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET, 1000000, 100000);
    void initialize(uint8_t i2caddr);
    void clearDisplay();
};


#endif // _AuxillaryScreen_