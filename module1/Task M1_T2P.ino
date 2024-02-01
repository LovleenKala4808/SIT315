int pir_pin = 2;
int led_pin = 5;
int buzzer_pin = 4;
int button_pin = 3;

volatile bool interruptFlag = false;

void setup()
{
  pinMode(pir_pin, INPUT);
  pinMode(led_pin, OUTPUT);
  pinMode(buzzer_pin, OUTPUT);
  pinMode(button_pin, INPUT_PULLUP); // Internal pull-up resistor

  attachInterrupt(digitalPinToInterrupt(button_pin), buttonInterrupt, FALLING); // Attach interrupt on falling edge
  Serial.begin(9600);
}

void loop()
{
  if (interruptFlag) {
    // Higher priority task
    Serial.println("Code interrupted");

    // Blink the LED for visual indication
    for (int i = 0; i < 3; i++) {
      digitalWrite(led_pin, HIGH);
      delay(200);
      digitalWrite(led_pin, LOW);
      delay(200);
    }

    // Reset the flag
    interruptFlag = false;
  }

  if (digitalRead(pir_pin) == HIGH)
  {
    digitalWrite(led_pin, HIGH);
    digitalWrite(buzzer_pin, HIGH);
    Serial.println("Motion Detectedion");
  }
  else
  {
    digitalWrite(led_pin, LOW);
    digitalWrite(buzzer_pin, LOW);
    Serial.println("No Detection");
  }
}

void buttonInterrupt()
{
  // Set the flag in the interrupt service routine
  interruptFlag = true;
}
