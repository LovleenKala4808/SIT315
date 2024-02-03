#include <IRremote.h>

// Pin assignments
const int PIR_PIN = 2;
const int RED_LED_PIN = 8;
const int GREEN_LED_PIN = 7;
const int IR_RECEIVER_PIN = 3;

// Initial states of LEDs
volatile int redState = LOW;
volatile int greenState = LOW;

// IR remote setup
IRrecv irReceiver(IR_RECEIVER_PIN);
decode_results irResults;

void setup() {
  // Pin mode setup
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(PIR_PIN, INPUT);

  // IR remote setup
  irReceiver.enableIRIn();

  // Attach interrupts
  attachInterrupt(digitalPinToInterrupt(PIR_PIN), PIRTrigger, CHANGE);
  attachInterrupt(digitalPinToInterrupt(IR_RECEIVER_PIN), IRTrigger, CHANGE);

  // Serial communication setup
  Serial.begin(9600);
}

void loop() {
  // Update LED states based on interrupts
  digitalWrite(RED_LED_PIN, redState);
  digitalWrite(GREEN_LED_PIN, greenState);
}

void PIRTrigger() {
  // Toggle the state of the red LED
  redState = !redState;
  Serial.println("PIR Interrupt.");
}

void IRTrigger() {
  // Check if IR signal is received
  if (irReceiver.decode(&irResults)) {
    Serial.println(irResults.value, DEC);

    // Check if the received IR signal matches a specific value
    if (irResults.value == 16580863) {
      // Toggle the state of the green LED
      greenState = !greenState;
      Serial.println("IR Interrupt.");
    }

    // Resume IR signal reception
    irReceiver.resume();
  }
}
