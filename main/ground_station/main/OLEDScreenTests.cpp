#include "OLEDScreenTests.h"

bool pinCorrect(uint8_t* pin) {
    if (pin[0] == 2 && pin[1] == 1 && pin[2] == 3)
    {
        return true;
    } else {
        return false;
    }
}

/* Simulates screen inputs */
void testFullLaunch(MainScreen screen) {
    screen.showMenu();

    screen.receiveScreenInput(ENC_RIGHT);
    delay(1*DEBUG_TIME_SCALE);
    screen.receiveScreenInput(ENC_RIGHT);
    delay(1*DEBUG_TIME_SCALE);
    screen.receiveScreenInput(ENC_LEFT);
    delay(1*DEBUG_TIME_SCALE);
    screen.receiveScreenInput(ENC_RIGHT);
    delay(1*DEBUG_TIME_SCALE);
    screen.receiveScreenInput(ENC_RIGHT);
    delay(1*DEBUG_TIME_SCALE);
    screen.receiveScreenInput(ENC_RIGHT);
    delay(1*DEBUG_TIME_SCALE); 
    screen.receiveScreenInput(ENC_LEFT);
    delay(1*DEBUG_TIME_SCALE);
    screen.receiveScreenInput(ENC_LEFT);
    delay(1*DEBUG_TIME_SCALE);
    screen.receiveScreenInput(ENC_PRESS); // Launch Screen
    delay(4*DEBUG_TIME_SCALE);
    screen.receiveScreenInput(ENC_PRESS); // Rocket Must Be Armed Screen
    delay(10*DEBUG_TIME_SCALE);
    screen.receiveScreenInput(ENC_PRESS); // Back to Menu Screen
    delay(2*DEBUG_TIME_SCALE);
    screen.receiveScreenInput(ENC_PRESS); // Launch Screen
    delay(1*DEBUG_TIME_SCALE);
    screen.receiveScreenInput(ENC_RIGHT); // Launch - N
    delay(2*DEBUG_TIME_SCALE);
    screen.receiveScreenInput(ENC_PRESS); // Back to Menu Screen
    delay(1*DEBUG_TIME_SCALE);
    screen.receiveScreenInput(ENC_PRESS); // Launch Screen
    delay(1*DEBUG_TIME_SCALE);
    screen.receiveScreenInput(ENC_LEFT); // Launch - Y
    delay(1*DEBUG_TIME_SCALE);
    screen.rocket_armed = true; // Arm rocket for testing
    screen.receiveScreenInput(ENC_PRESS); // Launch Seq Screen
    delay(1*DEBUG_TIME_SCALE);
    screen.receiveScreenInput(ENC_RIGHT);
    delay(1*DEBUG_TIME_SCALE);
    screen.receiveScreenInput(ENC_RIGHT);
    delay(1*DEBUG_TIME_SCALE);
    screen.receiveScreenInput(ENC_LEFT);
    delay(1*DEBUG_TIME_SCALE);
    screen.receiveScreenInput(ENC_PRESS); // Pin value 1 submitted  (1)
    delay(1*DEBUG_TIME_SCALE);
    screen.receiveScreenInput(ENC_LEFT);
    delay(1*DEBUG_TIME_SCALE);
    screen.receiveScreenInput(ENC_RIGHT);
    delay(1*DEBUG_TIME_SCALE);
    screen.receiveScreenInput(ENC_PRESS); // Pin value 2 submitted (1)
    delay(1*DEBUG_TIME_SCALE);
    screen.receiveScreenInput(ENC_RIGHT);
    delay(1*DEBUG_TIME_SCALE);
    screen.receiveScreenInput(ENC_RIGHT);
    delay(1*DEBUG_TIME_SCALE);
    screen.receiveScreenInput(ENC_RIGHT);
    delay(1*DEBUG_TIME_SCALE);
    screen.receiveScreenInput(ENC_PRESS); // Pin value 3 submitted (3)
    delay(4*DEBUG_TIME_SCALE);

    // We are now about to submit pin TO verify validity
    // MEANT TO BE INCORRECT HERE
    screen.key_inserted = true;
    uint8_t* pin = screen.getInputPin();

    Serial.print("Checking pin...");

    if (pinCorrect(pin)) {
        Serial.println("Pin correct!");
        screen.pin_correct = true;
    } else {
        Serial.println("Pin incorrect!");
    }

    screen.receiveScreenInput(ENC_PRESS); // Submit pin, go to pin incorrect screen
    delay(4*DEBUG_TIME_SCALE);
    screen.receiveScreenInput(ENC_PRESS); // Go back to menu screen
    delay(2*DEBUG_TIME_SCALE);
    screen.receiveScreenInput(ENC_PRESS); // Launch Screen
    delay(1*DEBUG_TIME_SCALE);
    screen.receiveScreenInput(ENC_LEFT); // Launch - Y
    delay(1*DEBUG_TIME_SCALE);
    //screen.rocket_armed = true;         -- ALREADY ARMED
    screen.receiveScreenInput(ENC_PRESS); // Launch Seq Screen
    delay(1*DEBUG_TIME_SCALE);
    screen.receiveScreenInput(ENC_RIGHT);
    delay(1*DEBUG_TIME_SCALE);
    screen.receiveScreenInput(ENC_PRESS); // Pin value 1 submitted  (2)
    delay(1*DEBUG_TIME_SCALE);
    screen.receiveScreenInput(ENC_PRESS); // Pin value 2 submitted (1)
    delay(1*DEBUG_TIME_SCALE);
    screen.receiveScreenInput(ENC_PRESS); // Pin value 3 submitted (3)
    delay(4*DEBUG_TIME_SCALE);

    // We are now about to submit pin TO verify validity
    // MEANT TO BE CORRECT HERE
    screen.key_inserted = true;
    pin = screen.getInputPin();

    Serial.print("Checking pin...");

    if (pinCorrect(pin)) {
        Serial.println("Pin correct!");
        screen.pin_correct = true;
    } else {
        Serial.println("Pin incorrect!");
    }

    screen.receiveScreenInput(ENC_PRESS); // Submit pin, go to btn launch screen
    delay(4*DEBUG_TIME_SCALE);
    screen.receiveScreenInput(ENC_PRESS); // Go back to menu screen
    delay(2*DEBUG_TIME_SCALE);
    
    Serial.println("Wrote!");

    screen.clearDisplay();
}