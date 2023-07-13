// this is called every 10 ms
void DialHandler() {

  if (currentDialPin <= 0) return;
  if (dial_read >= 0) return;
  if (!dialer) return;

  dialer->poll();

  const int max_between_pulses = 10;
  const int min_pulse_length = 2;
  const int max_pulse_length = 20; // this probably does not matter
  const int hangup_timeout = 25;

  int saveNumber = -1;
  
/*
  1 - press - hanged up
  2 - release - lifted off hook
*/

  int df = dialer->onChange();
  if (df) {
    //Serial.print(" -> ");
    //Serial.print(df);
    //Serial.print(" ");
    //Serial.print(dialer->getElapsedMs());
    //Serial.println();
    dialer->clearTimer();
  }

  if (dialer->onPress()) {
	  // Serial.print("PX ");
    // Serial.println(dialCount);
    if (dialCount > max_between_pulses && dialBuffer > 0) {
      saveNumber = dialBuffer;
      dialBuffer = 0;
    }
    dialCount = 0;
    dialPrimerFlag = true;
  } else
  if (dialer->onRelease() && dialPrimerFlag) {
	  //Serial.print("RX ");
    //Serial.println(dialCount);
    if (dialCount <= max_pulse_length && dialCount >= min_pulse_length) {
      dialBuffer++;   
      dial_read = -1;
    } else {
      dialBuffer = 0;
    }
    dialCount = 0;
  } else {
    dialCount++;
    dial_read = -1;
    if (dialer->isPressed() && dialCount > hangup_timeout) {
      Serial.print("HANGUP! ");
      Serial.println(dialCount);
      dialHangupFlag = true;
      dialCount = 0;
      dialPtr = 0;
      dialBuffer = 0;
    }
    if (dialer->isReleased() && dialCount > 15) {
      if (dialBuffer > 0) saveNumber = dialBuffer;
      dialBuffer = 0;
    }
  }

  if (dialBuffer > 0) dialStartedFlag = true; 

  if (saveNumber >= 0) {
    if (saveNumber == 10) saveNumber = 0;
    Serial.print("NUMBER ");
    Serial.println(saveNumber);
    dialNumber[dialPtr] = saveNumber;
    dialPtr++; 
    if (dialPtr == dialNMax) {
      dial_read = 100 * dialNumber[0] + 10 * dialNumber[1] + dialNumber[2];
      Serial.print("DIAL ");
      Serial.println(dial_read);
    } else {
      dial_read = -1;
    }
  }
}