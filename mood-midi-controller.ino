#include <SoftwareSerial.h>        // Ref: https://docs.arduino.cc/learn/built-in-libraries/software-serial
#include <MIDI.h>                  // Ref: https://github.com/FortySevenEffects/arduino_midi_library
#include <JC_Button.h>             // Ref: https://github.com/JChristensen/JC_Button
#include <ResponsiveAnalogRead.h>  // Ref: https://github.com/dxinteractive/ResponsiveAnalogRead
#include <jled.h>                  // Ref: https://github.com/jandelgado/jled

// Define MIDI Channel and control change numbers.
const int MIDI_CH = 2;  // MOOD MKII Default Channel

const int MIDI_CC_TOGGLE_BOUNCE = 66;
const int MIDI_CC_TOGGLE_SMOOTH = 78;
const int MIDI_CC_MIX = 15;

const int MIDI_CC_WET_TOGGLE = 103;
const int MIDI_CC_WET_HOLD = 105;
const int MIDI_CC_LOOP_TOGGLE = 102;
const int MIDI_CC_LOOP_HOLD = 106;

// Pin Definitions.
const int PIN_TOGGLE_BOUNCE = 4;
const int PIN_TOGGLE_SMOOTH = 5;
const int PIN_FOOTSWITCH_WET = 6;
const int PIN_FOOTSWITCH_LOOPER = 7;
const int PIN_POT = A0;
const int PIN_RX = 2;
const int PIN_TX = 3;
const int PIN_WET_LED = 9;
const int PIN_LOOPER_LED = 10;

// Setup switches.
Button toggleBounce(PIN_TOGGLE_BOUNCE);
Button toggleSmooth(PIN_TOGGLE_SMOOTH);
Button footSwitchWet(PIN_FOOTSWITCH_WET, 50);
Button footSwitchLooper(PIN_FOOTSWITCH_LOOPER, 50);

// Footswitch States.
const unsigned long holdThresholdMs = 500;

bool isWetOn = false;
bool isWetPressing = false;
unsigned long wetPressStartTime = 0;
bool isWetHoldTriggered = false;

bool isLooperOn = false;
bool isLooperPressing = false;
unsigned long looperPressStartTime = 0;
bool isLooperHoldTriggered = false;

// Setup potentiometer.
ResponsiveAnalogRead pot(PIN_POT, true);

// Setup LEDs.
JLed ledWet = JLed(PIN_WET_LED);
JLed ledLooper = JLed(PIN_LOOPER_LED);

// Setup MIDI.
SoftwareSerial softSerial(PIN_RX, PIN_TX);
MIDI_CREATE_INSTANCE(SoftwareSerial, softSerial, midiA);

void setup() {
  toggleBounce.begin();
  toggleSmooth.begin();
  footSwitchWet.begin();
  footSwitchLooper.begin();

  midiA.begin(MIDI_CH);
  Serial.begin(9600);
}

void loop() {
  // Toggle Switch - Bounce.
  toggleBounce.read();
  if (toggleBounce.wasPressed()) {
    midiA.sendControlChange(MIDI_CC_TOGGLE_BOUNCE, 127, MIDI_CH);
    Serial.println("Toggle Bounce: ON");
  }
  if (toggleBounce.wasReleased()) {
    midiA.sendControlChange(MIDI_CC_TOGGLE_BOUNCE, 0, MIDI_CH);
    Serial.println("Toggle Bounce: OFF");
  }

  // Toggle Switch - Smooth.
  toggleSmooth.read();
  if (toggleSmooth.wasPressed()) {
    midiA.sendControlChange(MIDI_CC_TOGGLE_SMOOTH, 127, MIDI_CH);
    Serial.println("Toggle Smooth: ON");
  }
  if (toggleSmooth.wasReleased()) {
    midiA.sendControlChange(MIDI_CC_TOGGLE_SMOOTH, 0, MIDI_CH);
    Serial.println("Toggle Smooth: OFF");
  }

  // Wet Channel - Pressed.
  footSwitchWet.read();
  if (footSwitchWet.wasPressed()) {
    isWetPressing = true;
    wetPressStartTime = millis();
    isWetHoldTriggered = false;
  }

  // Wet Channel - Hold.
  if (isWetPressing && footSwitchWet.isPressed()) {
    if (!isWetHoldTriggered && (millis() - wetPressStartTime > holdThresholdMs)) {
      if (isWetOn) {
        midiA.sendControlChange(MIDI_CC_WET_HOLD, 127, MIDI_CH);
        ledWet = JLed(PIN_WET_LED).Breathe(500).Forever();
        Serial.println("Wet Hold: ON");
      }
      isWetHoldTriggered = true;
    }
  }

  // Wet Channel - Released.
  if (footSwitchWet.wasReleased()) {
    if (isWetOn) {
      if (isWetHoldTriggered) {
        midiA.sendControlChange(MIDI_CC_WET_HOLD, 0, MIDI_CH);
        ledWet = JLed(PIN_WET_LED).On();
        Serial.println("Wet Hold: OFF");
      } else {
        midiA.sendControlChange(MIDI_CC_WET_TOGGLE, 0, MIDI_CH);
        ledWet = JLed(PIN_WET_LED).Off();
        isWetOn = false;
        Serial.println("Wet: OFF");
      }
    } else {
      midiA.sendControlChange(MIDI_CC_WET_TOGGLE, 127, MIDI_CH);
      ledWet = JLed(PIN_WET_LED).On();
      isWetOn = true;
      if (isWetHoldTriggered) {
        Serial.println("Wet: ON (from long press)");
      } else {
        Serial.println("Wet: ON (from short press)");
      }
    }
    isWetPressing = false;
  }

  // Micro Looper - Pressed.
  footSwitchLooper.read();
  if (footSwitchLooper.wasPressed()) {
    isLooperPressing = true;
    looperPressStartTime = millis();
    isLooperHoldTriggered = false;
  }

  // Micro Looper - Hold.
  if (isLooperPressing && footSwitchLooper.isPressed()) {
    if (!isLooperHoldTriggered && (millis() - looperPressStartTime > holdThresholdMs)) {
      midiA.sendControlChange(MIDI_CC_LOOP_HOLD, 127, MIDI_CH);
      ledLooper = JLed(PIN_LOOPER_LED).Breathe(500).Forever();
      Serial.println("Looper: OVERDUB ON");
      isLooperHoldTriggered = true;
    }
  }

  // Micro Looper - Released.
  if (footSwitchLooper.wasReleased()) {
    if (isLooperHoldTriggered && !isLooperOn) {
      midiA.sendControlChange(MIDI_CC_LOOP_TOGGLE, 0, MIDI_CH);
      ledLooper = JLed(PIN_LOOPER_LED).Off();
    } else if (isLooperHoldTriggered) {
      midiA.sendControlChange(MIDI_CC_LOOP_HOLD, 0, MIDI_CH);
      ledLooper = JLed(PIN_LOOPER_LED).On();
      Serial.println("Looper: OVERDUB OFF");
    } else {
      if (isLooperOn) {
        midiA.sendControlChange(MIDI_CC_LOOP_TOGGLE, 0, MIDI_CH);
        ledLooper = JLed(PIN_LOOPER_LED).Off();
        isLooperOn = false;
        Serial.println("Looper: OFF");
      } else {
        midiA.sendControlChange(MIDI_CC_LOOP_TOGGLE, 127, MIDI_CH);
        ledLooper = JLed(PIN_LOOPER_LED).On();
        isLooperOn = true;
        Serial.println("Looper: ON");
      }
    }
    isLooperPressing = false;
  }

  // Potentiometer.
  pot.update();
  if (pot.hasChanged()) {
    int value = pot.getValue();
    int midiValue = map(value, 0, 1023, 0, 127);
    midiA.sendControlChange(MIDI_CC_MIX, midiValue, MIDI_CH);
    // Serial.print("Pot Value: ");
    // Serial.println(midiValue);
  }

  // Update led.
  ledWet.Update();
  ledLooper.Update();
}
