// Include libraries 
#include <MIDIUSB.h>
#include <MIDIUSB_Defs.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
// #include <frequencyToNote.h>
// #include <pitchToFrequency.h>
// #include <pitchToNote.h>

// Define variables 
#define sensorPin 18
#define stripPin 19
byte startNote = 60;
byte currentScale = 0;
byte currentStartingNote = 0;
byte currentStartingOctave = 4;
byte currentSensitivity = 1;

int currentChannel = 0;
int stripMIDICC = 0;

byte nextStripVal;

void setup() {
  Serial.begin(9600);
  // setup pins 
  pinMode(stripPin, INPUT);
  pinMode(sensorPin, INPUT);
  digitalWrite(17, HIGH);
}

void loop() {
  setup(); 

  // Soft pot determines note 
  // Read in sensor values 
  // Sensor values determine the pitch bend of the note 
  // Only play note if sensor values exist
  
  // Read soft pot output 
  int stripPinVal = analogRead(stripPin);
  int stripVal = map(stripPinVal, 0, 1000, 0, 127);
  stripVal = constrain(stripVal, 0, 127);

  // if soft pot strip is used, update value 
  if (stripVal != nextStripVal) { nextStripVal = stripVal; }

}

