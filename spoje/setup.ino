void setup() {

  // --- neopixels
  pixels.begin();
  pixels.show();
  pixels.setPixelColor(0, 50, 0,50);
  pixels.setPixelColor(1, 50, 0,50);
  pixels.show();

  // --- serial
  Serial.begin(115200);
  while (! Serial) delay(100);
  Serial.println(F("Spoje HELO\n"));  

  delay(1000);

  pixels.setPixelColor(0, 0, 50,  0);
  pixels.setPixelColor(1, 50, 0, 50);
  pixels.show();

  // --- OLED display
  Serial.println(F("Init OLED..."));  
  //if(!display.begin(I2C_OLED, true)) {
  //  Serial.println(F("SSD1306 allocation failed"));
  //  for(;;); // Don't proceed, loop forever
  //}
  //display.clearDisplay();
  //display.display();
  
  //display.setI2CAddress(I2C_OLED);
  if(!display.begin()) {
   Serial.println(F("OLED init failed"));
   for(;;); // Don't proceed, loop forever
  }
  display.setFont(u8g2_font_5x8_mf);
  //display.setFlipMode(1);
  if (!log_display.begin(display, U8LOG_WIDTH, U8LOG_HEIGHT, log_buffer)) {
    Serial.println(F("Logger init failed"));
    for(;;); // Don't proceed, loop forever
  }
  log_display.setRedrawMode(0); // after newline
  log_display.print("\f");
  
  Serial.println(F("OLED OK."));  
  log_display.println("Spoje HVOZDY");
  log_display.println("");
  log_display.println("Inicializace...");

  pixels.setPixelColor(0, 50, 0,50);
  pixels.setPixelColor(1,  0,50, 0);
  pixels.show();

  delay(1000);

  // --- pins

  Serial.println(F("Init pins ..."));  

  pinMode(PIN_RING_A, OUTPUT);
  pinMode(PIN_RING_B, OUTPUT);
  pinMode(PIN_BUS, OUTPUT);
  
  digitalWrite(PIN_RING_A, HIGH);
  digitalWrite(PIN_RING_B, HIGH);
  digitalWrite(PIN_BUS, HIGH);

  delay(100);

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(PIN_TONE_A, OUTPUT);
  pinMode(PIN_TONE_B, OUTPUT);
  pinMode(PIN_HOOK_A, INPUT_PULLUP);
  pinMode(PIN_HOOK_B, INPUT_PULLUP);
  
  digitalWrite(LED_BUILTIN, LOW);
  digitalWrite(PIN_TONE_A, LOW);
  digitalWrite(PIN_TONE_B, LOW);

  delay(500);

  hook_A.begin(PIN_HOOK_A);
  hook_B.begin(PIN_HOOK_B);
  hook_A.setInputMode(Toggle::inputMode::input_pullup);
  hook_B.setInputMode(Toggle::inputMode::input_pullup);
  hook_A.setInputInvert(true);
  hook_B.setInputInvert(true);

  hook_A.setSamplePeriodUs(10000);
  hook_B.setSamplePeriodUs(10000);

  ring_button_A.begin(PIN_BUTTON_A);
  ring_button_B.begin(PIN_BUTTON_B);
  ring_button_A.setInputMode(Toggle::inputMode::input_pullup);
  ring_button_B.setInputMode(Toggle::inputMode::input_pullup);
  // hook_A.setInputInvert(true);
  // hook_B.setInputInvert(true);

  // --- timers
  Serial.println(F("Init timers..."));  


  // --- ISR timer for dial,
  //if (ITimer0.attachInterruptInterval(10, DialHandler)) {
  //  Serial.println(F("Melody/ring timer init OK."));
  //} else {
  //  Serial.println(F("Can't set melody/ring timer."));
  //  while(1){};
  //}
  // --- ISR timer for , tone generator and ringing
  if (ITimer2.attachInterruptInterval(10, Timer2Handler)) {
    Serial.println(F("Melody/ring timer init OK."));
  } else {
    Serial.println(F("Can't set melody/ring timer."));
    while(1){};
  }
  timer_dial = ISR_Timer2.setInterval(10, DialHandler);
  timer_melody = ISR_Timer2.setInterval(160, MelodyHandler);
  timer_ring = ISR_Timer2.setInterval(200, RingHandler);
  ISR_Timer2.disable(timer_dial);

  Serial.println(F("Init OK."));  
  log_display.println(F("Vse pripraveno."));
  pixels.setPixelColor(0, 0,50, 0);
  pixels.setPixelColor(1, 0,50, 0);
  pixels.show();

  delay(2000);

  //display.clearDisplay();
  //display.display();
  //display.setCursor(0,0);

  Serial.flush();
}
