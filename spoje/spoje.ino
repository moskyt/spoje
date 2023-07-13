// #include "pcf8574.h" // pcf8574 library, 1.0.3
#include <Wire.h>
// #include <Adafruit_GFX.h> // Adafruit GFX library, 1.11.5
// #include <Adafruit_SH110X.h> // Adafruit SH110X library, 2.1.8
#include <Adafruit_NeoPixel.h> // Adafruit NeoPixel library, 1.11.0
#include <Toggle.h> // Toggle library, 3.1.8
#include <U8g2lib.h> // U8g2@2.33.15

const int PHONE_NUMBER_A = 111;
const int PHONE_NUMBER_B = 222;

//--- pins and adresses

const int PIN_HOOK_A = 8;
const int PIN_HOOK_B = 9;
const int PIN_NEOPIXEL = 2;
const int PIN_TONE_A = 6;
const int PIN_TONE_B = 7;
const int PIN_RING_A = 5;
const int PIN_RING_B = 4;
const int PIN_BUS = 3;
const int PIN_BUTTON_A = 20;
const int PIN_BUTTON_B = 21;

const int I2C_OLED = 0x3C;

//--- hardware

//Adafruit_SH1106G display(128, 64, &Wire, -1);
U8G2_SH1106_128X64_NONAME_1_HW_I2C display(U8G2_R0, U8X8_PIN_NONE);
U8G2LOG log_display;
#define U8LOG_WIDTH 25
#define U8LOG_HEIGHT 8
uint8_t log_buffer[U8LOG_WIDTH*U8LOG_HEIGHT];

Adafruit_NeoPixel pixels(2, PIN_NEOPIXEL, NEO_RGB + NEO_KHZ800);

// timers stuff -- taken from the library

// Select USING_16MHZ     == true for  16MHz to Timer TCBx => shorter timer, but better accuracy
// Select USING_8MHZ      == true for   8MHz to Timer TCBx => shorter timer, but better accuracy
// Select USING_250KHZ    == true for 250KHz to Timer TCBx => shorter timer, but better accuracy
// Not select for default 250KHz to Timer TCBx => longer timer,  but worse accuracy
#define USING_16MHZ     false
#define USING_8MHZ      false
#define USING_250KHZ    false

// Select the timers you're using
// Cannot use TIMER_1 as it conflicts with tone
// TIMER_3 also conflicts with something
#define USE_TIMER_0     false
#define USE_TIMER_1     false
#define USE_TIMER_2     true
#define USE_TIMER_3     false

#include "megaAVR_TimerInterrupt.h"
#include "megaAVR_ISR_Timer.h" 

ISR_Timer ISR_Timer2;
int timer_dial = -1;
int timer_ring = -1;
int timer_melody = -1;

// --- line modes
enum lineMode {
  MODE_NULL, // this is only for the 'new_mode', meaning no change
  MODE_IDLE, // no ringing, on hook
  MODE_WAIT, // off hook, but not dialing (play Dial melody)
  MODE_DIAL, // dialing in progress (no melody here)
  MODE_INVALID, // failed call, invalid number (play Invalid melody)
  MODE_BUSY, // failed call, other is busy (play Busy melody)
  MODE_CALL, // successful call, waiting for the other to pick up (play Ring melody)
  MODE_RING, // ringing
  MODE_CONNECT, // connected to the bus
  MODE_POKE, // quick ring notification
  MODE_MUTE // picked up but the other one is already active, so "play dead"
};

// --- lines
enum lineID { LINE_A, LINE_B };

volatile lineMode mode_A = MODE_IDLE;
volatile lineMode mode_B = MODE_IDLE;

volatile bool hw_ring_A = false;
volatile bool hw_ring_B = false;

// --- dial stuff --- this is ofcourse stupid, needs to be twice => class
// timer/counter
volatile int dialCount = 0;
// current number being read
volatile int dialBuffer = 0;
// number of digits in code
const int dialNMax = 3;
// current code read
volatile int dialNumber[dialNMax] = {0,0,0};
// current digit
volatile int dialPtr = 0;
//
volatile int dial_read = -1;
//
volatile int currentDialPin = -1;
//
volatile bool dialHangupFlag = false;
//
volatile bool dialStartedFlag = false;
//
volatile bool dialPrimerFlag = false;

// --- melodies: tone-duration pairs. last tone has to be 0.
// unit in CTU specs is 165 ms, we have 150 
const int melodyDialLength_ = 4;
unsigned short int melodyDial_[2*melodyDialLength_] = {1,2,0,2,1,4,0,4};
const int melodyCallLength_ = 2;
unsigned short int melodyCall_[2*melodyCallLength_] = {1,6,0,24};
const int melodyInvalidLength_ = 4;
unsigned short int melodyInvalid_[2*melodyInvalidLength_] = {2,2,3,2,4,2,0,10};
const int melodyBusyLength_ = 2;
unsigned short int melodyBusy_[2*melodyBusyLength_] = {1,2,0,2};
const int melodyFailedLength_ = 2;
unsigned short int melodyFailed_[2*melodyFailedLength_] = {1,1,0,1};

// length of current melody (in melodyBuffer)
int melodyLength = 0;
// pointer to current note in melodyBuffer
int melodyPointer = 0;
// pin on which the melody is played
int melodyPin = 0;
// current melody buffer (expanded melody vector)
int melodyBuffer[100];

// count cycles where both lines are idle
int idleCounter = 0;
// at this count system goes to sleep
const int idleThreshold = 1000;
// sleep flag
bool sleep = false;

// handle for the slow ISR interrrupts (ring and melody)
void Timer2Handler() {
  ISR_Timer2.run();
}

// this is kinda melodyPonter
volatile int ringCounter = 0;

void RingHandler() {
  if ((mode_A == MODE_RING) || (mode_B == MODE_RING)) {
    // this looks like a good pattern:
    // ---.---.....
    if (
        (ringCounter >= 0 && ringCounter < 3 ) ||
        (ringCounter >= 4 && ringCounter < 7 )
      ) {
      if (mode_A == MODE_RING) hw_ring_A = true;
      if (mode_B == MODE_RING) hw_ring_B = true;
    } else {
      hw_ring_A = false;
      hw_ring_B = false;
    }
    ringCounter++;
    if (ringCounter >= 12) ringCounter = 0;
  } else if ((mode_A == MODE_POKE) || (mode_B == MODE_POKE)) {
    if (
        (ringCounter >=  0 && ringCounter <  2 ) ||
        (ringCounter >=  4 && ringCounter <  6 ) ||
        (ringCounter >=  8 && ringCounter < 10 ) ||
        (ringCounter >= 12 && ringCounter < 14 ) ||
        (ringCounter >= 16 && ringCounter < 18 )
      ) {
      if (mode_A == MODE_POKE) hw_ring_A = true;
      if (mode_B == MODE_POKE) hw_ring_B = true;
    } else {
      hw_ring_A = false;
      hw_ring_B = false;
    }
    ringCounter++;
    if (ringCounter >= 20) {
      ringCounter = 0;
      if (mode_A == MODE_POKE) {
        Serial.println("A => IDLE");
        mode_A = MODE_IDLE;
      }
      if (mode_B == MODE_POKE) {
        Serial.println("B => IDLE");
        mode_B = MODE_IDLE;
      }
    }
  } else {
    hw_ring_A = false;
    hw_ring_B = false;
    ringCounter = 0;
  }
}

// byte hook_A_ = LOW;
// byte hook_B_ = LOW;
volatile Toggle hook_A(PIN_HOOK_A);
volatile Toggle hook_B(PIN_HOOK_B);
Toggle *dialer = NULL;

Toggle ring_button_A(PIN_BUTTON_A);
Toggle ring_button_B(PIN_BUTTON_B);

const int hook_state_threshold = 100;
const int hook_pickup_threshold = 150;
//const int hook_state_threshold = 500;
//const int hook_pickup_threshold = 750;
bool off_hook_A = false;
bool off_hook_B = false;

void setStatus(lineMode mode, int pixel_index) {
  if (mode == MODE_IDLE)
    pixels.setPixelColor(pixel_index, 10, 10, 10);
  else if (mode == MODE_WAIT)
    pixels.setPixelColor(pixel_index,  0, 50,  0);
  else if (mode == MODE_DIAL)
    pixels.setPixelColor(pixel_index,  0, 90,  0);
  else if (mode == MODE_INVALID)
    pixels.setPixelColor(pixel_index, 50, 50,  0);
  else if (mode == MODE_BUSY)
    pixels.setPixelColor(pixel_index, 50, 20,  0);
  else if (mode == MODE_CALL)
    pixels.setPixelColor(pixel_index,  0, 50, 50);
  else if (mode == MODE_RING)
    pixels.setPixelColor(pixel_index, 50,  0, 50);
  else if (mode == MODE_POKE)
    pixels.setPixelColor(pixel_index,250,  0,250);
  else if (mode == MODE_CONNECT)
    pixels.setPixelColor(pixel_index,  0,  0, 50);
  else
    pixels.setPixelColor(pixel_index,  1,  1,  1);
}

void loop() {

  if (currentDialPin == PIN_HOOK_A) {
    if (dialHangupFlag) {
      stopMelody();
      off_hook_A = false; 
      currentDialPin = -1;
      ISR_Timer2.disable(timer_dial);
      hook_A.clearTimer();
      hook_A.poll();
    } else {
      off_hook_A = true;
    }
  } else {
    hook_A.poll();

    if (hook_A.releasedFor(hook_pickup_threshold)) off_hook_A = true;
    if (hook_A.pressedFor(hook_state_threshold))  off_hook_A = false;
  }

  if (currentDialPin == PIN_HOOK_B) {
    if (dialHangupFlag) {
      stopMelody();
      off_hook_B = false; 
      currentDialPin = -1;
      ISR_Timer2.disable(timer_dial);
      hook_B.clearTimer();
      hook_B.poll();
    } else {
      off_hook_B = true;
    }
  } else {
    hook_B.poll();

    if (hook_B.releasedFor(hook_pickup_threshold)) off_hook_B = true;
    if (hook_B.pressedFor(hook_state_threshold))  off_hook_B = false;
  }

  if (hw_ring_A) off_hook_A = false;
  if (hw_ring_B) off_hook_B = false;

  unsigned long now = millis();
  
  // Serial.print(off_hook_A);
  // Serial.print(off_hook_B);
  // Serial.print("-");
  // Serial.print(digitalRead(PIN_HOOK_A));
  // Serial.println(digitalRead(PIN_HOOK_B));

  {
    lineMode new_mode = updateState(LINE_A, mode_A, off_hook_A, 'A', mode_B);  
    if (new_mode != MODE_NULL) {
      mode_A = new_mode;
      if (new_mode == MODE_WAIT) {
        log_display.println("A zvednul sluchatko");
      }
      setNewState(LINE_A, new_mode, 'A');
    }
    if (new_mode == MODE_WAIT) {
      currentDialPin = PIN_HOOK_A;
      dialCount = 0;
      dialHangupFlag = false;
      dialStartedFlag = false;
      dialPrimerFlag = false;
      dialer = &hook_A;
      digitalWrite(PIN_TONE_A, LOW);
      dialer->poll();
      delay(1);
      ISR_Timer2.enable(timer_dial);
    }
    if (new_mode == MODE_DIAL) {
      stopMelody();
    }
  }
  {
    lineMode new_mode = updateState(LINE_B, mode_B, off_hook_B, 'B', mode_A);  
    if (new_mode != MODE_NULL) {
      mode_B = new_mode;
      if (new_mode == MODE_WAIT) {
        log_display.println("B zvednul sluchatko");
      }
      setNewState(LINE_B, new_mode, 'B');
    }
    if (new_mode == MODE_WAIT) {
      currentDialPin = PIN_HOOK_B;
      dialCount = 0;
      dialHangupFlag = false;
      dialStartedFlag = false;
      dialPrimerFlag = false;
      dialer = &hook_B;
      pinMode(currentDialPin, INPUT_PULLUP);
      digitalWrite(PIN_TONE_B, LOW);
      dialer->poll();
      delay(1);
      ISR_Timer2.enable(timer_dial);
    }
    if (new_mode == MODE_DIAL) {
      stopMelody();
    }
  }

  ring_button_A.poll();
  ring_button_B.poll();
  if ((mode_A == MODE_IDLE) && (mode_B == MODE_IDLE)) {
    if (ring_button_A.onPress()) {
      Serial.println(F("A ~ POKE"));
      mode_A = MODE_POKE;
    }
    if (ring_button_B.onPress()) {
      Serial.println(F("B ~ POKE"));
      mode_B = MODE_POKE;
    }
  }

  // -- did someone finish dialing?
  if (dial_read >= 0) {
    currentDialPin = 0;
    ISR_Timer2.disable(timer_dial);

    if (mode_A == MODE_DIAL) 
      mode_A = processDial(LINE_A, 'A', 'B', PHONE_NUMBER_B);
    if (mode_B == MODE_DIAL) 
      mode_B = processDial(LINE_B, 'B', 'A', PHONE_NUMBER_A);

    dial_read = -1;
  }

  // -- now either ring or switch to busy
  executeRing(LINE_A, LINE_B, mode_A, mode_B, 'A', 'B');
  executeRing(LINE_B, LINE_A, mode_B, mode_A, 'B', 'A');


  // -- AWFUL HACK! lets disable it for a while it might make things baad
  // if (mode_A == MODE_WAIT && mode_B == MODE_IDLE) melodyPin = PIN_TONE_A;
  //if (mode_B == MODE_WAIT && mode_A == MODE_IDLE) melodyPin = PIN_TONE_B;

  // -- connect
  if ((mode_A == MODE_CONNECT && mode_B == MODE_CALL) || (mode_A == MODE_CALL && mode_B == MODE_CONNECT)) {
    Serial.println("Connecting!");
    log_display.println("Spojuji hovor");

    stopMelody();
    currentDialPin = 0;
    ISR_Timer2.disable(timer_dial);
    setNewState(LINE_A, MODE_CONNECT, 'A');
    setNewState(LINE_B, MODE_CONNECT, 'B');
    mode_A = MODE_CONNECT;    
    mode_B = MODE_CONNECT;    
  }

  // -- disconnect
  if (mode_A == MODE_CONNECT && mode_B != MODE_CONNECT) {
    setNewState(LINE_A, MODE_BUSY, 'A');
    mode_A = MODE_BUSY;
    currentDialPin = 0;
    ISR_Timer2.disable(timer_dial);
    dialCount = 0;
    dialPrimerFlag = false;
    dialStartedFlag = false;
    dialHangupFlag = false;
    currentDialPin = 0;
    dialer = NULL;

    log_display.println("A odpojen");
  }
  if (mode_B == MODE_CONNECT && mode_A != MODE_CONNECT) {
    setNewState(LINE_B, MODE_BUSY, 'B');
    mode_B = MODE_BUSY;
    currentDialPin = 0;
    ISR_Timer2.disable(timer_dial);
    dialCount = 0;
    dialPrimerFlag = false;
    dialStartedFlag = false;
    dialHangupFlag = false;
    currentDialPin = 0;
    dialer = NULL;

    log_display.println("B odpojen");
  }

  // -- I feel that we should do some state based cleanup. Because it's safer.
  // - kill invalid combinations. allowed are RING-CALL, CONNECT-CONNECT, DIAL-IDLE
  // TBD
  // - turn off ringer unless in ring mode
  // TBD
  // - disconnect bus unless in connected mode
  // TBD
  // - disconnect tone unless in melody-attached mode
  // TBD

  set_ringer(LINE_A, hw_ring_A);
  set_ringer(LINE_B, hw_ring_B);

  if ((mode_A == MODE_IDLE) && (mode_B == MODE_IDLE)) {
    
    idleCounter++;
    if (idleCounter > idleThreshold) {
      sleep = true;

      currentDialPin = -1;
      ISR_Timer2.disable(timer_dial);
      stopMelody();
      set_bus(false);
      set_ringer(LINE_A, false);
      set_ringer(LINE_B, false);
      display.clearDisplay();
    }
  } else {
    if (sleep) {
      // wake up!
      log_display.print("\f");
    }
    idleCounter = 0;
    sleep = false;
  }

  //! ??? 
  if (melodyPin == PIN_HOOK_A && !off_hook_A) stopMelody();
  if (melodyPin == PIN_HOOK_B && !off_hook_B) stopMelody();

  if (sleep) {
    pixels.setPixelColor(0, 1, 1, 1);
    pixels.setPixelColor(1, 1, 1, 1);
  } else {
    setStatus(mode_A, 0);
    setStatus(mode_B, 1);
  }
  pixels.show();

  delay(50);

}

