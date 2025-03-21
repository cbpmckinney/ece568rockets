#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 128 // OLED display height, in pixels
#define OLED_RESET -1     // can set an oled reset pin if desired
Adafruit_SH1107 display = Adafruit_SH1107(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET, 1000000, 100000);
Adafruit_SH1107 display2 = Adafruit_SH1107(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET, 1000000, 100000);

void setup() {
  // put your setup code here, to run once:

  delay(250); // wait for the OLED to power up

  // Screen one
  display.begin(0x3D, true); // Address 0x3D default
 
  // Screen two
  display2.begin(0x3C, true);

  // Clear the buffers.
  display.clearDisplay();
  display2.clearDisplay();

  DrawText(display, "Hello world 1!");
  DrawText(display2, "Hello world 2!");

}

void DrawText(Adafruit_SH1107 target_display, const char *word) {
  target_display.clearDisplay();
  target_display.setTextSize(1);
  target_display.setTextColor(SH110X_WHITE);
  target_display.setCursor(0, 0);
  target_display.cp437(true); // use full 256 char 'Code Page 437' font

  target_display.println(F(word));
  target_display.display();

  delay(2000);
}

void loop() {
  // put your main code here, to run repeatedly:

}
