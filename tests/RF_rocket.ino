// ROCKET RECEIVER CODE
// Brandon Crudele - ECE568

// functions:
// void tx() -> sends packet
// char* rx() -> receives packet in the form of a string
// [Ground Station specific] ... bool ready_to_launch() -> returns true if the rocket sent back an ACK signal
// [Rocket specific]         ... bool rocket_process()  -> sends ACK back to ground station  when asked if it is ready for launch

// important variables:
// char ack[8] -> string sent from the ground station to the rocket to request ACK 
// char ready[8] -> string sent from the rocket to the ground station as an ACK

#include <SPI.h>
#include <RH_RF95.h>

#define RFM95_CS   16
#define RFM95_INT  21
#define RFM95_RST  17

// frequency
#define RF95_FREQ 915.0

// create radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);

char ack[8] = "ARM?";
char ready[8] = "READY!";
int len;

void setup_rf() {
  // init rf95,
  if (!rf95.init()) {
    Serial.println("LoRa init FAILURE");
  }
  Serial.println("LoRa init SUCCESS");

  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println("setFrequency failed");
  }
  Serial.print("Communication Frequency: "); Serial.println(RF95_FREQ);

  rf95.setTxPower(23, false);
}

// for rocket,
void setup() {
  // wait for serial,
  Serial.begin(115200);
  while (!Serial) delay(1);
  Serial.println("ROCKET TEST DEMO");

  setup_rf(); // initialize, set freq, set power (only needs to be called at the start)
  delay(1000);
}


// for both rocket and ground station,
void tx(char *str_num) {
  // use itoa() if we decide to work with integers to strings (we have floats right now)
  Serial.print("Sending "); Serial.print(str_num); Serial.println(" as a string");
  rf95.send((uint8_t *)str_num, 20); // might have to adjust packet size
}

// for both rocket and ground station,
char* rx() {
  if (rf95.available()) {
    uint8_t buf[20]; // for now im using size 20
    uint8_t len = sizeof(buf);

    if (rf95.recv(buf, &len)) {
      buf[len] = '\0'; // null-terminator

      // allocate string memory (gross)
      char* result = (char*)malloc(len + 1); // +1 for null terminator
      if (result != NULL) {
        memcpy(result, buf, len + 1);
        return result;
      } 
      else { 
        Serial.println("Memory allocation failed"); // memory fail
        return NULL; 
      }

    } 
    else {
      Serial.println("Receive failed");
    }
  }
  return NULL; // Return NULL if no message or receive failed
}

// for rocket,
bool rocket_process() {
  Serial.println("Waiting for Ground Station request...");
  while(1) { // wait for ground station ack
    char* received = rx();
    if (received != NULL) {
      Serial.print("Received: ");
      Serial.print(received);
      Serial.println(" from the ground station");
    }
    if (strcmp(received, ack) == 0) { // check if we got an ACK request,
      Serial.println("Sending ACK...");
      tx(ready); // send ACK
      return true;
    }
    free(received);
  }
}
// ROCKET PROCESS
void loop() {
  if (rocket_process()) {
    Serial.println("Sent ACK to Ground Station"); Serial.println("");
  }
  delay(1000);
}
