void stopMelody() {
  if (melodyPin > 0) {
    noTone(melodyPin);
    digitalWrite(melodyPin, LOW);
  }
  melodyLength = 0;
  melodyPointer = 0;
  melodyPin = 0;
  ISR_Timer2.enable(timer_melody);
}

void playMelody(int melody_length, unsigned short *melody, lineID line) {
  melodyLength = 0;
  melodyPointer = 0;
  for (int i = 0; i < melody_length; ++i) {
    int t = melody[2*i];
    int n = melody[2*i+1];
    for (int j = 0; j < n; ++j) {
      melodyBuffer[melodyLength] = t;
      melodyLength++;
    }
  } 
  Serial.print("Play melody on line ");
  Serial.println(line);
  if (line == LINE_A) {
    melodyPin = PIN_TONE_A;    
  } else {
    melodyPin = PIN_TONE_B;
  } 
  ISR_Timer2.enable(timer_melody);
}

void MelodyHandler() {

  if (melodyLength > 0 && melodyPointer < melodyLength && melodyPin > 0) {
    if (melodyBuffer[melodyPointer] == 1) 
      tone(melodyPin, 440);
    else if (melodyBuffer[melodyPointer] == 2) 
      tone(melodyPin, 523);
    else if (melodyBuffer[melodyPointer] == 3) 
      tone(melodyPin, 783);
    else if (melodyBuffer[melodyPointer] == 4) 
      tone(melodyPin, 1046);
    else {
      noTone(melodyPin);
      digitalWrite(melodyPin, LOW);
    }
    melodyPointer++;
    // wrap around
    if (melodyPointer == melodyLength)
      melodyPointer = 0;
  }
}