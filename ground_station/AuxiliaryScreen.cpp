/*********************************************************************
Auxiliary Screen Implementation File
    Here is where you define all the methods that were declared in the
    header file.

    The scope operator (::) distinguishes the class the method is for.

Written by:
Chris Silman
*********************************************************************/

#include "AuxiliaryScreen.h"
#include "Arduino.h"

/************************************
* Auxiliary Screen
*************************************/

void AuxiliaryScreen::initialize(uint8_t i2caddr) {
    display.begin(i2caddr, true);
    clearDisplay();
}

void AuxiliaryScreen::clearDisplay() {
    display.clearDisplay();
    display.display();
}

void AuxiliaryScreen::showLocalData() {
    currentScreen = ScreenEnums::Screen::LOCAL;

    int y_scale = 8;

    clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SH110X_WHITE);
    display.setCursor(0,0);
    display.println(F("LOCAL DATA"));

    // AHT20
    display.setCursor(0,y_scale*3);
    display.print(F("Temp (F): ")); display.println(storedLocalData.aht_temp);
    display.setCursor(0,y_scale*4);
    display.print(F("Humid. (% rH): ")); display.println(storedLocalData.aht_humidity);

    // BMP390
    display.setCursor(0,y_scale*5);
    display.print(F("Temp (F): ")); display.println(storedLocalData.bmp_temperature);
    display.setCursor(0,y_scale*6);
    display.print(F("Press. (hPa): ")); display.println(storedLocalData.bmp_pressure);
    display.setCursor(0,y_scale*7);
    display.print(F("Alt. (m): ")); display.println(storedLocalData.bmp_altitude);

    // GPS
    // GPS Date
    display.setCursor(0,y_scale*1);
    display.print(F("Date: ")); 
        display.print(storedLocalData.gps_month); 
        display.print("/");
        display.print(storedLocalData.gps_day);
        display.print("/");
        display.println(storedLocalData.gps_year);

    // GPS Time
    display.setCursor(0,y_scale*2);
    if (storedLocalData.gps_hour < 10) { display.print('0'); }
        display.print(storedLocalData.gps_hour); display.print(':');
        if (storedLocalData.gps_minute < 10) { display.print('0'); }
        display.print(storedLocalData.gps_minute); display.print(':');
        if (storedLocalData.gps_seconds < 10) { display.print('0'); }
        display.print(storedLocalData.gps_seconds);

    display.display();
}

void AuxiliaryScreen::showRocketData() {
    currentScreen = ScreenEnums::Screen::ROCKET;

    clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SH110X_WHITE);
    display.setCursor(0,0);
    display.println(F("ROCKET DATA"));
    display.setCursor(0,8);
    display.print(F("Temp (F): ")); display.println(storedRocketData.temp);
    display.setCursor(0,16);
    display.print(F("Humidity: ")); display.println(storedRocketData.humidity);
    display.setCursor(0,24);
    display.print(F("Alt.: ")); display.println(storedRocketData.altitude);
    display.setCursor(0,32);
    display.print(F("Press.: ")); display.println(storedRocketData.pressure);
    display.setCursor(0,40);
    display.print(F("GPS: ")); display.println(storedRocketData.gps);
    display.display();
}

void AuxiliaryScreen::jumpToScreen(ScreenEnums::Screen screen) {
    Serial.print("In jump to screen, going to: "); Serial.println(screen);
    switch (screen) {
        case ScreenEnums::Screen::LOCAL:
            showLocalData();
            break;
        case ScreenEnums::Screen::ROCKET:
            showRocketData();
            break;
        case ScreenEnums::Screen::NONE:
            currentScreen = ScreenEnums::Screen::NONE;
            clearDisplay();
            break;
        default:
            return;

    }
}

void AuxiliaryScreen::enableShowingData() {
    data_screen_enabled = true;
}

void AuxiliaryScreen::disableShowingData() {
    data_screen_enabled = false;
    jumpToScreen(ScreenEnums::Screen::NONE);
}

void AuxiliaryScreen::requestScreen(ScreenEnums::Screen targetScreen) {
    if (targetScreen == ScreenEnums::Screen::LOCAL or targetScreen == ScreenEnums::Screen::ROCKET) {
        jumpToScreen(targetScreen);
    }
}

void AuxiliaryScreen::refreshDataPoint(float old_data, float new_data, uint8_t index_x, uint8_t index_y, const char* message) {
    display.setTextColor(SH110X_BLACK);
    display.setCursor(index_x,index_y);
    display.print(F(message)); display.println(old_data);
    display.setTextColor(SH110X_WHITE);
    display.setCursor(index_x,index_y);
    display.print(F(message)); display.println(new_data);
}

void AuxiliaryScreen::refreshDataPoint(int old_data1, int old_data2, int old_data3, int new_data1, int new_data2, int new_data3, uint8_t index_x, uint8_t index_y, const char* separator, const char* message) {
    display.setTextColor(SH110X_BLACK);
    display.setCursor(index_x,index_y);
    display.print(F(message));
    if (old_data1 < 10) { display.print('0'); }
    display.print(old_data1); display.print(separator);
    if (old_data2 < 10) { display.print('0'); }
    display.print(old_data2); display.print(separator);
    if (old_data3 < 10) { display.print('0'); }
    display.print(old_data3);

    display.setTextColor(SH110X_WHITE);
    display.setCursor(index_x,index_y);
    display.print(F(message));
    if (new_data1 < 10) { display.print('0'); }
    display.print(new_data1); display.print(separator);
    if (new_data2 < 10) { display.print('0'); }
    display.print(new_data2); display.print(separator);
    if (new_data3 < 10) { display.print('0'); }
    display.print(new_data3);
}

void AuxiliaryScreen::updateLocalData(LocalSensorData input_data) {
    if (data_screen_enabled && current == ScreenEnums::Screen::LOCAL) {
        
        // Update data display every 500 seconds, data reads every 2 seconds.
        if (millis() - data_update_timer > 500) {
            data_update_timer = millis(); // reset the timer
            int y_scale = 8;

            if (storedLocalData.aht_temp !=  input_data.aht_temp) {
                refreshDataPoint(storedLocalData.aht_temp, input_data.aht_temp, 0, y_scale*3, "Temp (F): ");
                storedLocalData.aht_temp = input_data.aht_temp;
            }

            if (storedLocalData.aht_humidity != input_data.aht_humidity) {
                refreshDataPoint(storedLocalData.aht_humidity, input_data.aht_humidity, 0, y_scale*4, "Humid. (% rH): ");
                storedLocalData.aht_humidity = input_data.aht_humidity;
            }

            if (storedLocalData.bmp_temperature !=input_data.bmp_temperature) {
                refreshDataPoint(storedLocalData.bmp_temperature, input_data.bmp_temperature, 0, y_scale*5, "Temp (F): ");
                storedLocalData.bmp_temperature =input_data.bmp_temperature;
            }

            if (storedLocalData.bmp_pressure != input_data.bmp_pressure) {
                refreshDataPoint(storedLocalData.bmp_pressure, input_data.bmp_pressure, 0, y_scale*6, "Press. (hPa): ");
                storedLocalData.bmp_pressure = input_data.bmp_pressure;
            }

            if (storedLocalData.bmp_altitude !=input_data.bmp_altitude) {
                refreshDataPoint(storedLocalData.bmp_altitude, input_data.bmp_altitude, 0, y_scale*7, "Alt. (m): ");
                storedLocalData.bmp_altitude =input_data.bmp_altitude;
            }

            // GPS
            // GPS Date
            if (storedLocalData.gps_month != input_data.gps_month || 
                storedLocalData.gps_day != input_data.gps_day || 
                storedLocalData.gps_year != input_data.gps_year)
            {
                refreshDataPoint(storedLocalData.gps_month, 
                    storedLocalData.gps_day, 
                    storedLocalData.gps_year,
                    input_data.gps_month, 
                    input_data.gps_day, 
                    input_data.gps_year, 
                    0, 
                    y_scale*1, 
                    "/",
                    "Date: ");

                storedLocalData.gps_month = input_data.gps_month;
                storedLocalData.gps_day = input_data.gps_day;
                storedLocalData.gps_year = input_data.gps_year;
            }
            

            // GPS Time
            if (storedLocalData.gps_hour != input_data.gps_hour || 
                storedLocalData.gps_minute != input_data.gps_minute || 
                storedLocalData.gps_seconds != input_data.gps_seconds)
            {
                refreshDataPoint(storedLocalData.gps_hour, 
                                storedLocalData.gps_minute, 
                                storedLocalData.gps_seconds,
                                input_data.gps_hour, 
                                input_data.gps_minute, 
                                input_data.gps_seconds, 
                                0, 
                                y_scale*2, 
                                ":",
                                "");
                
                storedLocalData.gps_hour = input_data.gps_hour;
                storedLocalData.gps_minute = input_data.gps_minute;
                storedLocalData.gps_seconds = input_data.gps_seconds;
            }

        }
    }

    display.display();
}

void AuxiliaryScreen::updateRocketData(RocketData input_data) {
    if (data_screen_enabled && current == ScreenEnums::Screen::ROCKET) {
        // Find what data has actually updated since last update
        /*if (storedLocalData.temp !=  input_data.temp) {
            refreshDataPoint(storedLocalData.temp, input_data.temp, 0, 8, "Temp (F): ");
            storedLocalData.temp = input_data.temp;
        }

        if (storedLocalData.humidity != input_data.humidity) {
            refreshDataPoint(storedLocalData.humidity, input_data.humidity, 0, 16, "Humidity: ");
            storedLocalData.humidity = input_data.humidity;
        }

        if (storedLocalData.altitude !=input_data.altitude) {
            refreshDataPoint(storedLocalData.altitude, input_data.altitude, 0, 24, "Altitude: ");
            storedLocalData.altitude =input_data.altitude;
        }

        if (storedLocalData.pressure != input_data.pressure) {
            refreshDataPoint(storedLocalData.pressure, input_data.pressure, 0, 32, "Pressure: ");
            storedLocalData.pressure = input_data.pressure;
        }

        if (storedLocalData.gps != input_data.gps) {
            refreshDataPoint(storedLocalData.gps, input_data.gps, 0, 40, "GPS: ");
            storedLocalData.gps = input_data.gps;
        }*/
    }
    display.display();
}