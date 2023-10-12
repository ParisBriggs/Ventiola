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
// Code which uses poteniometer as strings with A and D tones, with volume control 
// Issue here ! Only 1 string plays at a time, sound is oscilatory due to library used 
// (the library uses PWM to volume control which may be part og the issue)
// Note: You could swap BreathPot for Breath Sensor (output values would need to be looked 
// at to see whether they match required volume input of vol.tone(speakerPin2, softPotValue_D, BreathPotPosition),


// Volume and tone creating library (oscilation problem with the output sound)
#include "Volume3.h"

// Digital output pins the speakers are connected to
#define speakerPin1 9
#define speakerPin2 10

// Emulated breath sensor using potentiometer 
int BreathPotPosition;

// Soft potentiometer values 
int softPotValue_A;
int softPotValue_D;

const int SOFT_POT_PIN = A0; // Pin connected to softpot wiper

const int SOFT_POT_PIN_2 = A1; // Pin connected to softpot wiper

const int BREATH_POT_PIN = A5; // Pin connected to pot wiper

const int GRAPH_LENGTH = 40; // Length of line graph

// turns on alternative pin for speaker 
bool alt_pin = true; 

void setup() 
{
  // initialize serial communications at 9600 bps:
  Serial.begin(9600);
  // Intalise pot as input
  pinMode(BREATH_POT_PIN,INPUT);
  // initalise soft pot pins as inputs
  pinMode(SOFT_POT_PIN, INPUT);
  pinMode(SOFT_POT_PIN_2, INPUT);
  // get a value for the "breath" pot initally 
  BreathPotPosition = analogRead(BREATH_POT_PIN);
  
}

void loop() 
{
  // Read in the soft pot's ADC value
  int softPotADCValue = analogRead(SOFT_POT_PIN);
  int softPotADCValue_2 = analogRead(SOFT_POT_PIN_2);
  int BreathPotADCValue = analogRead(BREATH_POT_PIN);

  // Map the 0-1023 value to 0-40
  int softPotPosition = map(softPotADCValue, 0, 1023, 0, GRAPH_LENGTH);
  int softPotPosition_2 = map(softPotADCValue_2, 0, 1023, 0, GRAPH_LENGTH);

  Serial.print("Breath Pot ADC");
  Serial.println(BreathPotADCValue);

  // If breath Pot has changed more than 6 values 
  if (abs(BreathPotADCValue- BreathPotPosition) > 6) {
    BreathPotPosition = BreathPotADCValue; // you could map this to whatever range you wanted e.g.map(BreathPotADCValue, 0, 1023, 0, 255);
  }
  
  // Debug print
  Serial.print("Pot Pos");
  Serial.println(BreathPotPosition);

  // Print a line graph for first string:
  Serial.print("<"); // Starting end
  for (int i=0; i<GRAPH_LENGTH; i++)
  {
    if (i == softPotPosition) Serial.print("|");
    else Serial.print("-");
  }
  Serial.println("> (" + String(softPotADCValue) + ")");

// Second string graph:
   Serial.print("<"); // Starting end
  for (int i=0; i<GRAPH_LENGTH; i++)
  {
    if (i == softPotPosition_2) Serial.print("|");
    else Serial.print("-");
  }
  Serial.println("> (" + String(softPotADCValue_2) + ")");

  // maps the potentiometer values to the A and D string 
  softPotValue_A =  map(softPotADCValue, 0, 1023, 440, 880);
  softPotValue_D =  map(softPotADCValue_2, 0, 1023, 293.7, 597.3);

   // If you aren't pressing the pot, no sound
  if (softPotADCValue < 10) {
    // Issue here ! Running either of these lines turn off both speakers ! Resolve
    //vol.noTone();
    //vol.tone(speakerPin1, softPotValue_A, 0);
  } 
  // If you are pressing the pot, play mapped note
  else {
    vol.tone(speakerPin1, softPotValue_A, BreathPotPosition);
  }

  // If you aren't pressing the pot, no sound
  if (softPotADCValue_2 < 10) {
    // Issue here ! Running either of these lines turn off both speakers ! Resolve
    //vol.tone(speakerPin2, softPotValue_A, 0);
    //vol.noTone();
  } 
  // If you are pressing the pot, play mapped note
  else {
    
    vol.tone(speakerPin2, softPotValue_D, BreathPotPosition);
  }
  // 1 millisecond
  delay(1);

}



