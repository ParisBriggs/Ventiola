// Vertiola firmware 
// need to change Tools/Upload speed -> 115200 
// libraries to download: BLE-MIDI, MultiMap

// android apps to download: synprez fm, MIDI ble connect  
// sound built in 2, 6 for violin on synprez fm 

#include <BLEMIDI_Transport.h>
#include <hardware/BLEMIDI_ESP32.h> 
#include "MultiMap.h"

bool debug = false; // set debug prints on or off 

#define GD -1
#define DA 0
#define AE 1

// default state is DA
int state = DA;
int prev_state = GD;

// Names the Ventoila to be found via bluetooth 
BLEMIDI_CREATE_INSTANCE("Ventiola", MIDI); 

// unsigned long t0 = millis();
// unsigned long t1 = millis();

int currNote_RHS; 
int currNote_LHS; 

int prevNote_RHS = 0; 
int prevNote_LHS = 0; 

// Pins 
const int softPotPin1 = 15;
const int softPotPin2 = 34; 
// const int pressureSensorPin = 14; 

// Soft potentiometer values
int softPotValue_A;
int softPotValue_D;

int softPotADC_RHS_Prev = -100;
int softPotADC_LHS_Prev = -100;

const int NOTE_ARRAY_SIZE = 13; // Adjust this based on the number of notes in your arrays

long outputArray_LHS[NOTE_ARRAY_SIZE];
long outputArray_RHS[NOTE_ARRAY_SIZE];

void copyArray(long* source, long* destination, int size) {
  for (int i = 0; i < size; i++) {
    destination[i] = source[i];
  }
}

 // Looking at Ventiola front on LHS and RHS
  long inputArrayLHS[] = { 110, 185, 231, 272, 320, 352, 389, 463, 528, 586, 640, 705, 805, 950}; 
  long inputArrayRHS[] = { 50, 120, 165, 207, 250, 290, 325, 386, 452, 511, 575,645, 730, 920};

  long outputArray_A[] = {69,70,71,72,73,74,75,76,77,78,79,80,81}; 
  long outputArray_D[] = {62,63,64,65,66,67,68,69,70,71,72,73,74}; 

  long outputArray_G[] = {55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68}; 
  long outputArray_E[] = {76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88}; 

// int velocityPrev = -2; 
// int velocityToSend = -2;
// int velocityMapped; 

void setup() {
  Serial.begin(115200); 
  //pinMode(pressureSensorPin, INPUT_PULLUP);
  pinMode(softPotPin1, INPUT_PULLUP);  
  pinMode(softPotPin2, INPUT_PULLUP); 
  MIDI.begin();
}

void loop() {
  // Read sensor values
   // Looking at Ventiola front on LHS and RHS
  int softPotADC_LHS = analogRead(softPotPin1);
  int softPotADC_RHS = analogRead(softPotPin2);
  //int pressureSensorADC =  analogRead(pressureSensorPin); 
  
  // Map pressure sensor to velocity values 
  //int VelArraySize = 14; 
  //long PressureInputArray[] = {600, 800, 850 ,900, 950, 1000, 1100, 1250, 1500, 1650, 1750, 2000, 2200, 2600}; 

  // long VelOutputArray[] = {0,50,55,65,75,85,95,100,105,110,115,120,125,127}; 
  // velocityMapped = multiMap<long>(pressureSensorADC, PressureInputArray, VelOutputArray, VelArraySize); 

  // // Smooths out sound of velocityMapped 
  // if (abs(velocityMapped - velocityPrev) > 5) {
  //   velocityToSend = velocityMapped;
  // } else if (velocityMapped < 40) {
  //   velocityToSend = 0;
  // }

  if (softPotADC_LHS > 1000 ) {
    if ( state >= 0 && abs(softPotADC_LHS - softPotADC_LHS_Prev) > 150) {
      state = state - 1;
    }
    softPotADC_LHS_Prev = softPotADC_LHS;
  
  } else if (softPotADC_RHS > 1000 ) {
    if (state <= 0 && abs(softPotADC_RHS - softPotADC_RHS_Prev) > 150) {
      state = state + 1;
    }
    softPotADC_RHS_Prev = softPotADC_RHS;
  } else {

    if (state != prev_state) {
      switch (state) {
        case GD:
          // Code for GD case
          copyArray(outputArray_G, outputArray_LHS, sizeof(outputArray_G) / sizeof(outputArray_G[0]));
          copyArray(outputArray_D, outputArray_RHS, sizeof(outputArray_D) / sizeof(outputArray_D[0]));
          break;

        case DA:
          // Code for DA case
          copyArray(outputArray_D, outputArray_LHS, sizeof(outputArray_D) / sizeof(outputArray_D[0]));
          copyArray(outputArray_A, outputArray_RHS, sizeof(outputArray_A) / sizeof(outputArray_A[0]));
          break;

        case AE:
          // Code for AE case
          copyArray(outputArray_A, outputArray_LHS, sizeof(outputArray_A) / sizeof(outputArray_A[0]));
          copyArray(outputArray_E, outputArray_RHS, sizeof(outputArray_E) / sizeof(outputArray_E[0]));
          break;

        default:
          // Optional default case if none of the cases match
          break;
      }
    }

      // RHS string
    // If there is pressure on the potentiometer for RHS string
    if (softPotADC_RHS != 0) {
      // map pot value to MIDI note for RHS string
      currNote_RHS = multiMap<long>(softPotADC_RHS, inputArrayRHS, outputArray_RHS, NOTE_ARRAY_SIZE);
    } else {
      // set RHS string midi note to 0
      currNote_RHS = 0;
    }

    // LHS string
    // If there is pressure on the potentiometer for LHS string
    if (softPotADC_LHS != 0) {
      // map pot value to MIDI note for LHS string
      currNote_LHS = multiMap<long>(softPotADC_LHS, inputArrayLHS, outputArray_LHS, NOTE_ARRAY_SIZE);
    } else {
      // set LHS string midi note to 0
      currNote_LHS = 0;
    }

    // if the note has changed for RHS string 
    if (currNote_RHS != prevNote_RHS) {
      // if a note is being pressed
      if (softPotADC_RHS > 80 && softPotADC_RHS < 900 ) {
        // turn on the note
        MIDI.sendNoteOn(currNote_RHS, 127, 1);
      }
    } 
    // if the note has changed for LHS string 
    if (currNote_LHS != prevNote_LHS) {
      // if a note is being played on LHS string
      if (softPotADC_LHS > 140 && softPotADC_LHS < 945) {
        // turn on note on LHS string
          MIDI.sendNoteOn(currNote_LHS, 127, 1);
        }
    }
    // note has changed for RHS string
    if (prevNote_RHS != currNote_RHS) {
      // turn OFF prevNote RHS string
      MIDI.sendNoteOff(prevNote_RHS, 0, 1);
    }

    // note has changed for LHS string 
    if (prevNote_LHS != currNote_LHS) {
      // turn OFF prevNote LHS string
      MIDI.sendNoteOff(prevNote_LHS, 0, 1);
    }

    // debug print
    if (debug) {
      Serial.print("Previous state: ");
      Serial.println(prev_state); 
    }
    // Set previous state to be the current state
    prev_state = state;
    softPotADC_LHS_Prev = softPotADC_LHS;
    softPotADC_RHS_Prev = softPotADC_RHS;
  }

    // debug prints 
    if (debug) {
    Serial.print("State: ");
    Serial.println(state); 

    Serial.print("soft pot LHS: ");
    Serial.println(softPotADC_LHS); 
    Serial.print("soft pot RHS: ");
    Serial.println(softPotADC_RHS); 
    
    // Serial.print("pressure: ");
    // Serial.println(pressureSensorADC); 

    // Serial.print("velocity Mapped: ");
    // Serial.println(velocityMapped);
    
    // Serial.print("velocity To Send: ");
    // Serial.println(velocityToSend); 
    

    // Serial.print("velocity Prev: ");
    // Serial.println(velocityPrev); 
    // Serial.println(); 

    Serial.print("midi RHS: "); 
    Serial.println(currNote_RHS); 

    Serial.print("prev note RHS: "); 
    Serial.println(prevNote_RHS); 

    Serial.print("midi LHS: "); 
    Serial.println(currNote_RHS);

    Serial.print("prev note LHS: "); 
    Serial.println(prevNote_LHS); 

  } else {
    delay(25);
  }

  // Update previous values before looping
  prevNote_RHS = currNote_RHS; 
  prevNote_LHS = currNote_LHS; 
  softPotADC_LHS_Prev = softPotADC_LHS;
  softPotADC_RHS_Prev = softPotADC_RHS;
  //velocityPrev = velocityToSend;
  //delay(200);

}

