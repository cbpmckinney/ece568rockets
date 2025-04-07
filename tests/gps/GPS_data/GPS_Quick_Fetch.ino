// Quick Fetch Code for AdaFruit Stemma MiniGPS Module
// ECE 568 - Brandon Crudele
// Functions to call:
// GPS_setup -> needed before fetch
// fetch_gps() -> return coordinates struct with coords
#include <Adafruit_GPS.h>
#include <stdio.h>

Adafruit_GPS GPS(&Wire);

typedef struct { // struct to extract data from callable function
  float lon;
  float lat;
  bool fix = false; // true when data has been recovered
} Location;

#define DEBUG false // to view raw GPS data

uint32_t gpsAddr = 0x10;

// coordinate conversion vars:
int converted_lat_deg;
int converted_lon_deg;
float converted_lat; // holds data for RF
float converted_lon; // holds data for RF

void setup()
{
  GPS_setup();  // setup GPS
  Serial.println("GPS ENABLED");
  Location info = fetch_gps(); // fetch gps info (takes time)
  delay(1000);
  Serial.print("Lat:");
  Serial.println(info.lat, 6);
  Serial.print("Lon:");
  Serial.println(info.lon, 6);
}

void loop() 
{
  delay(1000);
}

Location request_gps() {
  // variables:
  Location coordinates;
  // Location coordinates;
  char c = GPS.read();

  // RAW data:
  if (GPS.newNMEAreceived()) { // if we got data,
    GPS.parse(GPS.lastNMEA()); // parse through the raw data for extraction
    if (DEBUG) { 
    Serial.println(GPS.lastNMEA()); // print raw data
    }
  }

  // Fix check:
  if (GPS.fix) {
    coordinates.fix = true;
  } else {
    coordinates.fix = false;
  }

  // lat.
  converted_lat_deg = (int)(GPS.latitude/100); // extract degrees
  converted_lat = GPS.latitude - (converted_lat_deg * 100); // extract mins
  converted_lat = converted_lat_deg + (converted_lat / 60); // degrees + mins
  if (GPS.lon == 'S') {
    converted_lat *= -1;
  }

  // lon.
  converted_lon_deg = (int)(GPS.longitude/100); // extract degrees
  converted_lon = GPS.longitude - (converted_lon_deg * 100); // extract mins
  converted_lon = converted_lon_deg + (converted_lon / 60); // degrees + mins
  if (GPS.lon == 'W') {
    converted_lon *= -1;
  }

  // set the struct.
  coordinates.lat = converted_lat;
  coordinates.lon = converted_lon;

  return coordinates;
}

void GPS_setup() {
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

  // turn on RMC and GGA
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ); // polling
  GPS.sendCommand(PGCMD_ANTENNA);
}

Location fetch_gps() {
  Location coordinates; 
  Serial.println("Finding coordinates...");
  while (coordinates.fix != true) {
    //Serial.print(".");
    coordinates = request_gps();
  }
  Serial.println("GPS info found!");
  return coordinates;
}