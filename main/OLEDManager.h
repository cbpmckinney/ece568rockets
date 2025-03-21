// Chris Silman
// OLED Class

#ifndef _OLEDManager_
#define _OLEDManager_ 

#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

class OLEDScreen {
    private:
        Adafruit_SH1107 display;

    public:
        OLEDScreen(Adafruit_SH1107 managed_display);
        Adafruit_SH1107 getDisplay();
};


class MainScreen : private OLEDScreen {

};

class AuxillaryScreen : private OLEDScreen {

};


#endif // _OLEDManager_