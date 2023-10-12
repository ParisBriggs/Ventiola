/******************************************************************************
SoftPot_Example.ino
Example sketch for SparkFun's soft membrane potentiometer
  (https://www.sparkfun.com/products/8680)
Jim Lindblom @ SparkFun Electronics
April 28, 2016

- Connect the softpot's outside pins to 5V and GND (the outer pin with an arrow
indicator should be connected to GND). 
- Connect the middle pin to A0.

As the voltage output of the softpot changes, a line graph printed to the
serial monitor should match the wiper's position.

Development environment specifics:
Arduino 1.6.7
******************************************************************************/

/*
  Keyboard

  Plays a pitch that changes based on a changing analog input

  circuit:
  - three force-sensing resistors from +5V to analog in 0 through 5
  - three 10 kilohm resistors from analog in 0 through 5 to ground
  - 8 ohm speaker on digital pin 8

  created 21 Jan 2010
  modified 9 Apr 2012
  by Tom Igoe

  This example code is in the public domain.

  https://www.arduino.cc/en/Tutorial/BuiltInExamples/toneKeyboard
*/

///*****************************************************************//
// Clare's notes:
// Code which uses poteniometer as strings with A and D tones, no volume control 

// this library was experimented with previously
//#include "pitches.h"

// Tone library allows musical functionality 
#include <Tone.h>

// Soft potentiometer values 
int softPotValue_A;
int softPotValue_D;

// Digital output pins the speakers are connected to
const int SpeakerPin_1 = 10;
cost inst SpeakerPin_2 = 9;

// Instantiating two speakers 
Tone speaker[2];

const int SOFT_POT_PIN = A0; // Pin connected to softpot wiper

const int SOFT_POT_PIN_2 = A1; // Pin connected to softpot wiper

const int GRAPH_LENGTH = 40; // Length of line graph

void setup() 
{
  // initialize serial communications at 9600 bps:
  Serial.begin(9600);
  // initalise soft pot pins as inputs
  pinMode(SOFT_POT_PIN, INPUT);
  pinMode(SOFT_POT_PIN_2, INPUT);

  // intalise speakers to speaker pins
  speaker[0].begin(SpeakerPin_1);
  speaker[1].begin(SpeakerPin_2);
}

void loop() 
{
  // Read in the soft pot's ADC value
  int softPotADC = analogRead(SOFT_POT_PIN);
  int softPotADC_2 = analogRead(SOFT_POT_PIN_2);

  // Map the 0-1023 value to 0-40
  int softPotPosition = map(softPotADC, 0, 1023, 0, GRAPH_LENGTH);
  int softPotPosition_2 = map(softPotADC_2, 0, 1023, 0, GRAPH_LENGTH);

  // Print a line graph for first string:
  Serial.print("<"); // Starting end
  for (int i=0; i<GRAPH_LENGTH; i++)
  {
    if (i == softPotPosition) Serial.print("|");
    else Serial.print("-");
  }
  Serial.println("> (" + String(softPotADC) + ")");

// Second string graph:
   Serial.print("<"); // Starting end
  for (int i=0; i<GRAPH_LENGTH; i++)
  {
    if (i == softPotPosition_2) Serial.print("|");
    else Serial.print("-");
  }
  Serial.println("> (" + String(softPotADC_2) + ")");

  // maps the potentiometer values to the A and D string 
  softPotValue_A =  map(softPotADC, 0, 1023, 440, 880);
  softPotValue_D =  map(softPotADC_2, 0, 1023, 294, 587); 

  // Debugging prints
  Serial.print("pot Val D string");
  Serial.println(String(softPotValue_D));
  Serial.print("pot Val A string");
  Serial.println(String(softPotValue_A));

  // If you aren't pressing the pot, no sound
  if (softPotADC < 10) {
    speaker[0].stop();
    // If you are pressing the pot, play mapped note
  } else {
    speaker[0].play(softPotValue_A);
  }

  // If you aren't pressing the pot, no sound
  if (softPotADC_2 < 10) {
    speaker[1].stop();
    // If you are pressing the pot, play mapped note
  } else {
    speaker[1].play(softPotValue_D);
  }
  // 1 millisecond
  delay(1);

}



