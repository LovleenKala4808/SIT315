int pir_pin = 2;
int led_pin = 3;
int buzzer_pin = 4;
void setup()
{
 pinMode(pir_pin, INPUT);
 pinMode(led_pin, OUTPUT);
 pinMode(buzzer_pin, OUTPUT);
 Serial.begin(9600);
}

void loop()
{
  if(digitalRead(pir_pin) == HIGH)
   {
  	digitalWrite(led_pin, HIGH);
    digitalWrite(buzzer_pin, HIGH);
    Serial.println("detected");
   }
   else{
    digitalWrite(led_pin, LOW);
    digitalWrite(buzzer_pin, LOW);
    Serial.println("Not detected");
   }
}
