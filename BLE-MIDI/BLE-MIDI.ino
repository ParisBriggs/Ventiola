// Using ESP32 to create a BLE MIDI device 
// Paris' notes:
// ESP32 is not compatible with Tone library
// need to change Tools/Upload speed -> 115200 
// libraries to download: BLE-MIDI, ToneESP32 by larry  

// android apps to download: synprez fm, MIDI ble connect  
// sound 6 for violin on synprez fm 

//#include <ToneESP32.h>
#include <BLEMIDI_Transport.h>
#include <hardware/BLEMIDI_ESP32.h> 
#include "MultiMap.h"

bool debug = false; // set debug prints on or off 

BLEMIDI_CREATE_INSTANCE("Ventiola", MIDI); 

unsigned long t0 = millis();
unsigned long t1 = millis();

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


int velocityPrev = -2; 
int velocityToSend = -2;
int velocityMapped;
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
  // Read sensor values
  int softPotADC_A = analogRead(softPotPin1);
  int softPotADC_D = analogRead(softPotPin2);
  int pressureSensorADC = analogRead(pressureSensorPin); 
  
  // Map pressure sensor to velocity values 
  int VelArraySize = 5; 
  long PressureInputArray[] = {600, 1000, 1700, 2000, 2600}; 

  long VelOutputArray[] = {31,63,79,94,127}; 
  velocityMapped = multiMap<long>(pressureSensorADC, PressureInputArray, VelOutputArray, VelArraySize); 

  // Smooths out sound of velocityMapped 
  if (abs(velocityMapped - velocityPrev) > 5) {
    velocityToSend = velocityMapped;
  } else if (velocityMapped < 40) {
    velocityToSend = 0;
  }


  // Map soft pot sensors to midi note values 
  int arraySize = 12; 
  long inputArray[] = {1, 75, 150, 225, 300, 375, 450, 640, 915, 1200, 1845, 2310, 4000}; 

  // A string
  long outputArray_A[] = {69,70,71,72,73,74,75,76,77,78,79,80,81}; 
  int midiNote_A = multiMap<long>(softPotADC_A, inputArray, outputArray_A, arraySize);

  // D string
  long outputArray_D[] = {62,63,64,65,66,66,67,68,69,70,71,72,73,74}; 
  int midiNote_D = multiMap<long>(softPotADC_D, inputArray, outputArray_D, arraySize);
  
  // Set curr notes 
  currNote_A = midiNote_A; 
  currNote_D = midiNote_D; 
  
  // if there is a valid velocity value
  if (velocityToSend >= 40 && velocityToSend <= 127 ) 
  {
    // if the note has changed for A string 
    if (currNote_A != prevNote_A) {
      // if a note is being pressed
      if (softPotADC_A > 3) {
        // turn on the note
        MIDI.sendNoteOn(currNote_A, velocityToSend, 1);
      }
      // if the velocity has changed
    } else if (velocityToSend != velocityPrev) {
      // a note is being pressed
        if (softPotADC_A > 3) {
          // turn on the note with new velocity on A string
          MIDI.sendNoteOn(currNote_A, velocityToSend, 1);
        }
    }
    // if the note has changed for D string 
    if (currNote_D != prevNote_D) {
      // if a note is being played on D string
      if (softPotADC_D > 3) {
        // turn on note on D string
          MIDI.sendNoteOn(currNote_D, velocityToSend, 1);
        }
        // if the velocity has changed
      } else if (velocityToSend != velocityPrev) {
        // if a note is being played on D string
          if (softPotADC_D > 3) {
            // turn on the note on the D string with new velocity 
            MIDI.sendNoteOn(currNote_D, velocityToSend, 1);
          }
      }
  }
  // no velocity for 100 ms 
  else if ( velocityToSend < 40 && velocityToSend > 127 && (millis() - t0) > 100)
  {
    t0 = millis();
    // turn currNote OFF for both strings
    MIDI.sendNoteOff(currNote_A, 0, 1);
    MIDI.sendNoteOff(currNote_D, 0, 1);
  }

  // note has changed for A string
  if (prevNote_A != currNote_A) {
    // turn OFF prevNote A string
    MIDI.sendNoteOff(prevNote_A, 0, 1);
  }

  // note has changed for D string 
  if (prevNote_D != currNote_D) {
    // turn OFF prevNote D string
    MIDI.sendNoteOff(prevNote_D, 0, 1);
  }
    // debug prints 
    if (debug) {
    Serial.print("pressure: ");
    Serial.println(pressureSensorADC); 
    Serial.print("velocity To Send: ");
    Serial.println(velocityToSend); 
    Serial.println();

    Serial.print("velocity Prev: ");
    Serial.println(velocityPrev); 
    Serial.println(); 

    Serial.print("midi A: "); 
    Serial.println(midiNote_A); 

    Serial.print("midi D: "); 
    Serial.println(midiNote_D);

  }

  // Update previous values before looping
  prevNote_A = currNote_A; 
  prevNote_D = currNote_D; 
  velocityPrev = velocityToSend;


}

