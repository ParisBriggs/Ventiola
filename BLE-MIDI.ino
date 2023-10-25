// Using ESP32 to create a BLE MIDI device 
// Paris' notes:
// ESP32 is not compatible with Tone library
// need to change Tools/Upload speed -> 115200 
// libraries to download: BLE-MIDI, ToneESP32 by larry  

// android apps to download: synprez fm, MIDI ble connect  
// sound 6 for violin on synprez fm 

#include <ToneESP32.h>
#include <BLEMIDI_Transport.h>
#include <hardware/BLEMIDI_ESP32.h> 
#include "MultiMap.h"

BLEMIDI_CREATE_INSTANCE("Ventiola", MIDI); 

int currNote_A; 
int currNote_D; 

int prevNote_A = 0; 
int prevNote_D = 0; 

// Pins 
const int softPotPin1 = 15;
const int softPotPin2 = 36; 
const int pressureSensorPin = 27; // ADC17

// Soft potentiometer values
int softPotValue_A;
int softPotValue_D;

// Instantiating speaker
const int SpeakerPin1 = 2; // GPIO02 (ADC12)
const int SpeakerChannel1 = 1;

const int SpeakerPin2 = 4; // GPIO04 (ADC)
const int SpeakerChannel2 = 1;

ToneESP32 speaker1(SpeakerPin1, SpeakerChannel1); // sets pin and channel 
ToneESP32 speaker2(SpeakerPin2, SpeakerChannel2); // sets pin and channel 

int velocity;
int midiNote1; 
int midiNote2; 

void setup() {
  Serial.begin(115200); 
  pinMode(pressureSensorPin, INPUT_PULLUP);
  pinMode(softPotPin1, INPUT_PULLUP);  
  pinMode(softPotPin2, INPUT_PULLUP); 
  MIDI.begin();
}

void loop() {
  int softPotADC_A = analogRead(softPotPin1);
  int softPotADC_D = analogRead(softPotPin2);
  int pressureSensorADC = analogRead(pressureSensorPin); 

  Serial.print("pressure: ");
  Serial.println(pressureSensorADC); 

  velocity = map(pressureSensorADC, 600, 2000, 0, 127); // 127 max 

  Serial.print("velocity: ");
  Serial.println(velocity); 
  Serial.println(); 

  int arraySize = 12; 
  long inputArray[] = {1, 75, 150, 225, 300, 375, 450, 640, 915, 1200, 1845, 2310, 4000}; 

  long outputArray_A[] = {69,70,71,72,73,74,75,76,77,78,79,80,81}; 
  int midiNote_A = multiMap<long>(softPotADC_A, inputArray, outputArray_A, arraySize);
  Serial.print("midi A: "); 
  Serial.println(midiNote_A); 

  long outputArray_D[] = {62,63,64,65,66,66,67,68,69,70,71,72,73,74}; 
  int midiNote_D = multiMap<long>(softPotADC_D, inputArray, outputArray_D, arraySize);
  
  Serial.print("midi D: "); 
  Serial.println(midiNote_D);

  // Set curr notes 
  currNote_A = midiNote_A; 
  currNote_D = midiNote_D; 

  // note has changed 
  if (prevNote_A != currNote_A) {
    // turn OFF prevNote
    MIDI.sendNoteOff(prevNote_A, 0, 1);
  }
  if (prevNote_D != currNote_D) {
    // turn OFF prevNote
    MIDI.sendNoteOff(prevNote_D, 0, 1);
  }
  // velocity 
  if (velocity > 0 ) 
  {
    if (softPotADC_A > 3) {
      MIDI.sendNoteOn(currNote_A, velocity, 1);
    }

    if (softPotADC_D > 3) {
      MIDI.sendNoteOn(currNote_D, velocity, 1);
    }
  }
  // no velocity 
  else 
  {
    // turn currNote OFF 
    MIDI.sendNoteOff(currNote_A, 0, 1);
    MIDI.sendNoteOff(currNosste_D, 0, 1);
  }
  prevNote_A = currNote_A; 
  prevNote_D = currNote_D; 
}

