// "Hello World!" Code for AdaFruit Stemma MiniGPS Module
// ECE 568 - Brandon Crudele

#include <Adafruit_GPS.h>

Adafruit_GPS GPS(&Wire);

#define DEBUG false // to view raw GPS data

uint32_t timer = millis();
int timer_time = 1000; // 1 Hz
uint32_t gpsAddr = 0x10;

// coordinate conversion vars:
int converted_lat_deg;
int converted_lon_deg;
float converted_lat; // holds data for RF
float converted_lon; // holds data for RF

void setup()
{
  while (!Serial);  // wait until Serial is ready

  Serial.begin(115200);
  Serial.println("MiniGPS Starting...");

  bool gps = false; // check if I2C successfully linked to GPS

  while (!gps) {
    gps = GPS.begin(gpsAddr);
    if (gps) {
      Serial.println("I2C Connection Success!");
    }  // The I2C address to use is 0x10
    else {
      Serial.println("I2C Connection Failed!");
      Serial.println("Re-testing I2C connection...");
      delay(1000); // wait a second...
    }
  }

  // RMC provides essential data like time, date, position, speed, and course
  // GGA includes altitude, fix quality, and the number of satellites used
  // the rocket module needs both,

  // turn on RMC and GGA
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  // Set the update rate
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ); // Change header to poll 1, 2, 5 or 10 Hz
  // Request updates on antenna status
  GPS.sendCommand(PGCMD_ANTENNA);

  Serial.println("MiniGPS ENABLED");
  delay(1000);
}

void loop() 
{
  char c = GPS.read();
  if (GPS.newNMEAreceived()) { // if we got data,
    GPS.parse(GPS.lastNMEA()); // parse through the raw data for extraction
    if (DEBUG) { 
    Serial.println(GPS.lastNMEA()); // print raw data
    }
  }

  // print to screen per timer tick,
  if (millis() - timer > timer_time) {
    timer = millis(); // reset the timer
    if (GPS.fix) {
      Serial.println("==============");
      // Serial.println("[FIX]: TRUE"); 
      Serial.print("[LATITUDE]: ");
      converted_lat_deg = (int)(GPS.latitude/100); // extract degrees
      converted_lat = GPS.latitude - (converted_lat_deg * 100); // extract mins
      converted_lat = converted_lat_deg + (converted_lat / 60); // degrees + mins
      if (GPS.lon == 'S') {
        converted_lat *= -1;
      }
      Serial.println(converted_lat, 5);
      // Serial.println(GPS.lat); // char type (prints N or S)
      Serial.print("[LONGITUDE]: ");
      converted_lon_deg = (int)(GPS.longitude/100); // extract degrees
      converted_lon = GPS.longitude - (converted_lon_deg * 100); // extract mins
      converted_lon = converted_lon_deg + (converted_lon / 60); // degrees + mins
      if (GPS.lon == 'W') {
        converted_lon *= -1;
      }
      Serial.println(converted_lon, 5);
      // Serial.println(GPS.lon); // char type (prints W or E)
      Serial.print("[SPEED]: "); Serial.print(GPS.speed); Serial.println(" Knots");
      Serial.print("[ANGLE]: "); Serial.print(GPS.angle); Serial.println(" Degrees");
      Serial.print("[ALTITUDE]: "); Serial.print(GPS.altitude); Serial.println(" Meters");
      // Serial.print("[CONNECTED SATELLITES]: "); Serial.print((int)GPS.satellites); Serial.println(" Satellites");
    }
    else {
      Serial.println("No fix...");
    }
  }
}