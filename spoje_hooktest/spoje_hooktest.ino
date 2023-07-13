#include <Wire.h>

const int PIN_HOOK_A = 2;
const int PIN_HOOK_B = 3;
const int PIN_TONE_A = 5;
const int PIN_TONE_B = 6;

void setup() {
  Serial.begin(115200);
  while (! Serial) delay(100);
  Serial.println(F("Spoje test HELO\n"));  

  pinMode(PIN_HOOK_A, INPUT_PULLUP);
  pinMode(PIN_HOOK_B, INPUT_PULLUP);
  pinMode(PIN_TONE_A, OUTPUT);
  pinMode(PIN_TONE_B, OUTPUT);
  digitalWrite(PIN_TONE_A, LOW);
  digitalWrite(PIN_TONE_B, LOW);
  
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.print(digitalRead(PIN_HOOK_A));
  delay(5);
  Serial.println(digitalRead(PIN_HOOK_B));
  delay(50);
}
