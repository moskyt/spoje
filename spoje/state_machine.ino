/*
  MODE_IDLE, // no ringing, on hook
  MODE_WAIT, // off hook, but not dialing (play Dial melody)
  MODE_DIAL, // dialing in progress (no melody here)
  MODE_INVALID, // failed call, invalid number (play Busy melody)
  MODE_BUSY, // failed call, other is busy (play Busy melody)
  MODE_CALL, // successful call, waiting for the other to pick up (play Ring melody)
  MODE_RING, // ringing
  MODE_CONNECT, // connected to the bus
  */

lineMode updateState(lineID line, lineMode mode, bool off_hook, char line_name, lineMode other_mode) {

  lineMode new_mode = MODE_NULL;

  // -- state machine
  if ((mode == MODE_IDLE) && off_hook && (other_mode == MODE_IDLE)) {
    Serial.print(line_name);
    Serial.println(" IDLE -> WAIT");
    //log_display.print(line_name);
    //log_display.println(" zvednul sluchatko");
    new_mode = MODE_WAIT;    
  }
  else if ((mode == MODE_IDLE) && off_hook && (other_mode != MODE_IDLE)) {
    Serial.print(line_name);
    Serial.println(" IDLE -> MUTE");
    //log_display.print(line_name);
    //log_display.println(" zvednul sluchatko");
    new_mode = MODE_MUTE;    
  }
  else if ((mode == MODE_RING) && off_hook) {
    Serial.print(line_name);
    Serial.println(" RING -> CONNECT");
    //log_display.print(line_name);
    //log_display.println(" prijal");
    new_mode = MODE_CONNECT;    
  }
  else if ((mode == MODE_WAIT) && dialStartedFlag) {
    Serial.print(line_name);
    Serial.println(" WAIT -> DIAL");
    //log_display.print(line_name);
    //log_display.println(" vytaci");
    dialPtr = 0;
    new_mode = MODE_DIAL;    
  }
  else if ((mode == MODE_DIAL) && !off_hook) {
    Serial.print(line_name);
    Serial.println(" DIAL -> IDLE");
    //log_display.print(line_name);
    //log_display.println(" zavesil");
    new_mode = MODE_IDLE;    
  }
  else if ((mode == MODE_WAIT) && !off_hook) {
    Serial.print(line_name);
    Serial.println(" WAIT -> IDLE");
    //log_display.print(line_name);
    //log_display.println(" zavesil");
    new_mode = MODE_IDLE;    
  }
  else if ((mode == MODE_MUTE) && !off_hook) {
    Serial.print(line_name);
    Serial.println(" MUTE -> IDLE");
    //log_display.print(line_name);
    //log_display.println(" zavesil");
    new_mode = MODE_IDLE;    
  }
  else if ((mode == MODE_CALL) && !off_hook) {
    Serial.print(line_name);
    Serial.println(" CALL -> IDLE");
    //log_display.print(line_name);
    //log_display.println(" zavesil");
    stopMelody();
    new_mode = MODE_IDLE;    
  }
  else if ((mode == MODE_CONNECT) && !off_hook) {
    Serial.print(line_name);
    Serial.println(" CONNECT -> IDLE");
    //log_display.print(line_name);
    //log_display.println(" zavesil");
    stopMelody();
    new_mode = MODE_IDLE;    
  }
  else if ((mode == MODE_INVALID) && !off_hook) {
    Serial.print(line_name);
    Serial.println(" INVALID -> IDLE");
    //log_display.print(line_name);
    //log_display.println(" zavesil");
    stopMelody();
    new_mode = MODE_IDLE;    
  }
  else if ((mode == MODE_BUSY) && !off_hook) {
    Serial.print(line_name);
    Serial.println(" BUSY -> IDLE");
    //log_display.print(line_name);
    //log_display.println(" zavesil");
    stopMelody();
    new_mode = MODE_IDLE;    
  }

  // -- handle conflicts!
  
  // 1) if both are to play a melody, do not allow that
  // TBD
  // 2) if one is connected and the other not, crash everything
  // TBD

  // -- scrap meaningless state change
  if (new_mode == mode) new_mode = MODE_NULL;

  //Serial.print("State update for ");
  //Serial.print(line_name);
  //Serial.println(" completed.");

  return new_mode;

}

void setNewState(lineID line, lineMode new_mode, char line_name) {
  // -- handle actual state
  if (new_mode == MODE_NULL) {
    // ok, do nothing. why would you.
  } else
  if (new_mode == MODE_IDLE) {
    Serial.print(line_name);
    Serial.println(F(" ~ IDLE"));
    set_bus(false);
  } else
  if (new_mode == MODE_MUTE) {
    Serial.print(line_name);
    Serial.println(F(" ~ MUTE"));
    set_bus(false);
  } else
  if (new_mode == MODE_DIAL) {
    Serial.print(line_name);
    Serial.println(F(" ~ DIAL"));
    set_bus(false);
    stopMelody();
  } else
  if (new_mode == MODE_WAIT) {
    Serial.print(line_name);
    Serial.println(F(" ~ WAIT"));
    set_bus(false);
    playMelody(melodyDialLength_, melodyDial_, line);
} else
  if (new_mode == MODE_CONNECT) {
    Serial.print(line_name);
    Serial.println(F(" ~ CONNECT"));
    ringCounter = -1;
    delay(1);
    set_bus(true);
  } else
  if (new_mode == MODE_RING) {
    Serial.print(line_name);
    Serial.println(F(" ~ RING"));
    set_bus(false);
    delay(1);
    // reset the counter so it starts nicely
    ringCounter = 0;
  } else
  if (new_mode == MODE_CALL) {
    Serial.print(line_name);
    Serial.println(F(" ~ CALL"));
    set_bus(false);
    playMelody(melodyCallLength_, melodyCall_, line);
  } else 
  if (new_mode == MODE_INVALID) {
    Serial.print(line_name);
    Serial.println(F(" ~ INVALID"));
    set_bus(false);
    playMelody(melodyInvalidLength_, melodyInvalid_, line);
  } else 
  if (new_mode == MODE_BUSY) {
    Serial.print(line_name);
    Serial.println(F(" ~ BUSY"));
    set_bus(false);
    playMelody(melodyBusyLength_, melodyBusy_, line);
  } else 
  {
    Serial.print(line_name);
    Serial.println(F(" ~ NO VALID STATE"));
    // invalid state !!!
    set_bus(false);
  }
}

// -- transition from DIAL either to INVALID or CALL based on the number dialed
lineMode processDial(lineID line, char line_name, char other_line_name, int other_number) {
  log_display.print(line_name);
  log_display.print(" vytocil ");
  log_display.println(dial_read);

  Serial.print(line_name);
  Serial.print(" dialled ");
  Serial.println(dial_read);
  
  if (dial_read == other_number) {
    log_display.print("Volam ");
    log_display.println(other_line_name);
    
    Serial.print("Correct number, should call ");
    Serial.println(other_line_name);
    Serial.print(line_name);
    Serial.println(" DIAL -> CALL");
    
    setNewState(line, MODE_CALL, line_name);
    return MODE_CALL;
  } else {
    log_display.println("Volane cislo neex.");
    Serial.println("Incorrect number!");
    Serial.print(line_name);
    Serial.println(" DIAL -> INVALID");

    setNewState(line, MODE_INVALID, line_name);
    return MODE_INVALID;
  }
}

// -- start or stop ringing at the other phone
void executeRing(lineID line, lineID other_line, volatile lineMode &mode, volatile lineMode &other_mode, char line_name, char other_line_name) {
  
  if (mode == MODE_CALL && other_mode == MODE_IDLE) {
    log_display.print("Vyzvanim u ");
    log_display.println(other_line_name);

    Serial.print("Ringing at ");
    Serial.println(other_line_name);
    Serial.print(other_line_name);
    Serial.println(" IDLE -> RING");

    setNewState(other_line, MODE_RING, other_line_name);
    other_mode = MODE_RING;
  }
  if (mode == MODE_CALL && other_mode != MODE_IDLE && other_mode != MODE_RING && other_mode != MODE_CONNECT) {
    log_display.print("Obsazeno u ");
    log_display.println(other_line_name);

    Serial.print(other_line_name);
    Serial.println(" is busy");
    Serial.print(line_name);
    Serial.println(" CALL -> BUSY");

    setNewState(line, MODE_BUSY, line_name);
    mode = MODE_BUSY;
  }
  if (other_mode == MODE_RING && mode != MODE_CALL) {
    //log_display.print("Konec zvoneni u ");
    //log_display.println(other_line_name);

    Serial.print("Stop ringing at ");
    Serial.println(other_line_name);
    Serial.print(other_line_name);
    Serial.println(" RING -> IDLE");

    setNewState(other_line, MODE_IDLE, other_line_name);
    other_mode = MODE_IDLE;
  }
}