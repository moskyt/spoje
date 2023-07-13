void set_bus(bool state) {
  if (state) {
    connect_bus();
  } else {
    disconnect_bus();
  }
}

void set_ringer(lineID line, bool state) {
  if (state) {
    if (line == LINE_A) {
      digitalWrite(PIN_RING_A, LOW);
    }
    if (line == LINE_B) {
      digitalWrite(PIN_RING_B, LOW);
    }
  } else {
    if (line == LINE_A) {
      digitalWrite(PIN_RING_A, HIGH);
    }
    if (line == LINE_B) {
      digitalWrite(PIN_RING_B, HIGH);
    }
  }
}

void connect_bus() {
  digitalWrite(PIN_BUS, LOW);
}

void disconnect_bus() {
  digitalWrite(PIN_BUS, HIGH);
}
