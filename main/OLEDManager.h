// Chris Silman
// OLED Class

#ifndef _OLEDManager_
#define _OLEDManager_ 

#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 128 // OLED display height, in pixels
#define OLED_RESET -1     // can set an oled reset pin if desired

#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <Arduino.h>

class MainScreen {
  public:
    Adafruit_SH1107 display = Adafruit_SH1107(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET, 1000000, 100000);
    uint8_t menu_pointer;
    void initializeDisplay();
};

class AuxillaryScreen : public OLEDScreen {
  public:
    Adafruit_SH1107 display = Adafruit_SH1107(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET, 1000000, 100000);
    uint8_t menu_pointer;
    void initializeDisplay();
};


#endif // _OLEDManager_