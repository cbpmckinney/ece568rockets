/* 
Test file for ignition related components: key switch, big red button, relay
Relay will be the only one of these components aboard the rocket
Switch and button will be on the ground

Wiring: relay needs power, ground, and a digital pin.  When this pin is 
pulled high, it'll close the relay, which will close the ignition circuit.
LIFTOFF!
Key switch needs ground and one digital pin with a pullup resistor.  When this 
pin goes low, switch is closed and in the ARM position.
Big red button needs:
  -Button: ground and one digital pin with pull-up resistor for input
  -LED: ground and one digital pin for output.  The LED has an internal resistor, 
  so an extra resistor is not needed.

Arbitrary choices for pins, can change later:
Relay: ground, + 3.3V, and digital pin D24
Key switch:
Big red button:
*/

// Values for RP 2040 board
/*
#define relaypin 24
#define ledpin 13
#define buttonpin 12
#define switchpin 11
*/

// Values for ESP32 board for testing
#define relaypin 12
#define ledpin 15
#define buttonpin 13
#define switchpin 14



void setup() {
  Serial.begin(115200);
  Serial.println("Ignition systems test!");

  pinMode(relaypin, OUTPUT);
  digitalWrite(relaypin, LOW);

  pinMode(ledpin, OUTPUT);
  digitalWrite(ledpin, LOW);

  pinMode(buttonpin, INPUT_PULLUP);
  pinMode(switchpin, INPUT_PULLUP);


  attachInterrupt(digitalPinToInterrupt(switchpin), switchisr, CHANGE);


}

void buttonisr(void)
{
  detachInterrupt(digitalPinToInterrupt(buttonpin));
  Serial.println("Mr. Worf, FIRE!...");
  digitalWrite(relaypin, HIGH);
  attachInterrupt(digitalPinToInterrupt(buttonpin), buttonisr, FALLING);
}

void switchisr(void)
{
  if (digitalRead(switchpin) == LOW)
  {
    Serial.println("Switch closed: ARMED!");
    attachInterrupt(digitalPinToInterrupt(buttonpin), buttonisr, FALLING);
    digitalWrite(ledpin, HIGH);
  }
  if (digitalRead(switchpin) == HIGH)
  {
    Serial.println("Switch opened: SAFE!");
    detachInterrupt(digitalPinToInterrupt(buttonpin));
    digitalWrite(ledpin, LOW);
    digitalWrite(relaypin, LOW);
  }
}




void loop() {





}

