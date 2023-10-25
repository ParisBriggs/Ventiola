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

// Paris' notes:
// ESP32 is not compatible with Tone library
// need to change Tools/Upload speed -> 115200 
// libraries to download: BLE-MIDI, ToneESP32 by larry  

// ***************
// TO DO: 
// volume control with soft pot 
// breath sensor for volume control 
// BLE connection stabilised??? 
// BLE MIDI connection & setup 
// BLE MIDI working with phone app 
// Consolidate circuit onto board 
// rubber clarke habberfield 
// ***************

#include <ToneESP32.h>
#include <Audio.h>

// Emulated breath sensor using potentiometer 
// ESP32 analog input (WARNING: limit voltage to 3.3v maximum)
// const int breathPotPin = 34; // (GPIO 34) input pin used to control the audio volume
// const int volumeControlSteps = 100; //100 steps -- the potentiometer (on GPIO34) controls audio volume between zero and 100%
// int currentChannelNumber = 1;

Audio audio;  //class from the ESP32-audioI2S library

// turns on alternative pin for speaker 
bool alternativePin = true; 

// BLE libraries 
// #include <BLEMIDI_Transport.h>
// #include <hardware/BLEMIDI_ESP32.h> 

// create instance of BLE midi object 
// BLEMIDI_CREATE_INSTANCE("Ventiola", MIDI) 

// Pins 
const int softPotPin1 = 15;
const int softPotPin2 = 36; 
const int pressureSensor = 27; // ADC17

// Soft potentiometer values
int softPotValue_A;
int softPotValue_D;

// Instantiating speaker
const int SpeakerPin1 = 2; // GPIO02 (ADC12)
const int SpeakerChannel1 = 1;

const int SpeakerPin2 = 4; // GPIO02 (ADC12)
const int SpeakerChannel2 = 1;

ToneESP32 speaker1(SpeakerPin1, SpeakerChannel1); // sets pin and channel 
ToneESP32 speaker2(SpeakerPin2, SpeakerChannel2); // sets pin and channel 

const int GRAPH_LENGTH = 40; // Length of line graph

void beep( int note, int duree ) {                   
    speaker1.tone(note, duree);       
    speaker1.tone(0, 5); 
    delay(duree*0.25);
}

void playEveryNote() {
  beep(NOTE_B0, 500);
  beep(NOTE_C1, 500);
  beep(NOTE_CS1, 500);
  beep(NOTE_D1, 500);
  beep(NOTE_DS1, 500);
  beep(NOTE_E1, 500);
  beep(NOTE_F1, 500);
  beep(NOTE_FS1, 500);
  beep(NOTE_G1, 500);
  beep(NOTE_GS1, 500); 
  beep(NOTE_A1, 500);
  beep(NOTE_AS1, 500);
  beep(NOTE_B1, 500);
  beep(NOTE_C2, 500);
  beep(NOTE_CS2, 500);
  beep(NOTE_D2, 500);
  beep(NOTE_DS2, 500);
  beep(NOTE_E2, 500);
  beep(NOTE_F2, 500);
  beep(NOTE_FS2, 500);
  beep(NOTE_G2, 500);
  beep(NOTE_GS2, 500);
  beep(NOTE_A2, 500);
  beep(NOTE_AS2, 500);
  beep(NOTE_B2, 500);
  beep(NOTE_C3, 500);
  beep(NOTE_CS3, 500);
  beep(NOTE_D3, 500);
  beep(NOTE_DS3, 500);
  beep(NOTE_E3, 500);
  beep(NOTE_F3, 500);
  beep(NOTE_FS3, 500);
  beep(NOTE_G3, 500);
  beep(NOTE_GS3, 500);
  beep(NOTE_A3, 500);
  beep(NOTE_AS3, 500);
  beep(NOTE_B3, 500);
  beep(NOTE_C4, 500);
  beep(NOTE_CS4, 500);
  beep(NOTE_D4, 500);
  beep(NOTE_DS4, 500);
  beep(NOTE_E4, 500);
  beep(NOTE_F4, 500);
  beep(NOTE_FS4, 500);
  beep(NOTE_G4, 500);
  beep(NOTE_GS4, 500);
  beep(NOTE_A4, 500);
  beep(NOTE_AS, 500);
  beep(NOTE_AS4, 500);
  beep(NOTE_B4, 500);
  beep(NOTE_C5, 500);
  beep(NOTE_CS5, 500);
  beep(NOTE_D5, 500);
  beep(NOTE_DS5, 500);
  beep(NOTE_E5, 500);
  beep(NOTE_F5, 500);
  beep(NOTE_FS5, 500);
  beep(NOTE_G5, 500);
  beep(NOTE_GSH, 500);
  beep(NOTE_GS5, 500);
  beep(NOTE_A5, 500);
  beep(NOTE_AS5, 500);
  beep(NOTE_B5, 500);
  beep(NOTE_C6, 500);
  beep(NOTE_CS6, 500);
  beep(NOTE_D6, 500);
  beep(NOTE_DS6, 500);
  beep(NOTE_E6, 500);
  beep(NOTE_F6, 500);
  beep(NOTE_FS6, 500);
  beep(NOTE_G6, 500);
  beep(NOTE_GS6, 500);
  beep(NOTE_A6, 500);
  beep(NOTE_AS6, 500);
  beep(NOTE_B6, 500);
  beep(NOTE_C7, 500);
  beep(NOTE_CS7, 500);
  beep(NOTE_D7, 500);
  beep(NOTE_DS7, 500);
  beep(NOTE_E7, 500);
  beep(NOTE_F7, 500);
  beep(NOTE_FS7, 500);
  beep(NOTE_G7, 500);
  beep(NOTE_GS7, 500);
  beep(NOTE_A7, 500);
  beep(NOTE_AS7, 500);
  beep(NOTE_B7, 500);
  beep(NOTE_C8, 500);
  beep(NOTE_CS8, 500);
  beep(NOTE_D8, 500);
  beep(NOTE_DS8, 500);

}

void printLineGraph(int softPotPosition, int softPotADC) {

  // Print a line graph for first string:
  Serial.print("<"); // Starting end
  for (int i = 0; i < GRAPH_LENGTH; i++)
  {
    if (i == softPotPosition) Serial.print("|");
    else Serial.print("-");
  }
  Serial.println("> (" + String(softPotADC) + ")");

  // Second string graph:
  //  Serial.print("<"); // Starting end
  // for (int i=0; i<GRAPH_LENGTH; i++)
  // {
  //   if (i == softPotPosition_2) Serial.print("|");
  //   else Serial.print("-");
  // }
  // Serial.println("> (" + String(softPotADC_2) + ")");

} 

void setup()
{
  // initialise serial communications at 115200 bps:
  Serial.begin(115200);
  // initalise pot pins as inputs
  pinMode(softPotPin1, INPUT);
  pinMode(softPotPin2, INPUT); 
  pinMode(pressureSensor, INPUT);

  pinMode(breathPotPin,INPUT);

  //set the initial volume level
  audio.setVolumeSteps(volumeControlSteps);
  // map potentiometer value to a volume percentage
  int volume = map(analogRead(breathPotPin), 0, 4095, 0, volumeControlSteps);  
  audio.setVolume(volume);
  Serial.print("Volume set at ");
  Serial.print(volume);
  Serial.println("%");

  // get initial value for the "breath" pot 
  // int BreathPotPosition = analogRead(breathPotPin);

  // MIDI.begin(); 
}

void loop()
{
  int volume = map(analogRead(breathPotPin), 0, 4095, 0, volumeControlSteps);  
  audio.setVolume(volume);

  // Read in the soft pot's ADC value
  int softPotADC1 = analogRead(softPotPin1);
  int softPotADC2 = analogRead(softPotPin2);
  int BreathPotADC = analogRead(breathPotPin);
  int pressureSensorADC = analogRead(pressureSensor); 

  // If breath Pot has changed more than 6 values 
  // if (abs(BreathPotADC - BreathPotPosition) > 6) {
  //   BreathPotPosition = BreathPotADC; // you could map this to whatever range you wanted e.g.map(BreathPotADCValue, 0, 1023, 0, 255);
  // }

  // Map the 0-1023 value to 0-40
  int softPotPosition = map(softPotADC1, 0, 1023, 0, GRAPH_LENGTH);
  int softPotPosition_2 = map(softPotADC2, 0, 1023, 0, GRAPH_LENGTH);

  // Map the potentiometer values to the A and D string 
  softPotValue_A =  map(softPotADC1, 0, 1023, 440, 880);
  softPotValue_D =  map(softPotADC2, 0, 1023, 294, 587); // doesn't use full potentiometer?

  Serial.println("Pot1 value: ");
  Serial.println(String(softPotADC1));

  Serial.println("Pot2 value: ");
  Serial.println(String(softPotADC2));

  Serial.print("Breath Pot ADC");
  Serial.println(BreathPotADC);


  // playEveryNote(); 

  // soft pots work simultaneously on one speaker! 
  if (softPotADC1 > 10) {
    speaker1.tone(softPotValue_A, 50);
    // speaker2.tone(softPotADC1, 50);
  } 

  if (softPotADC2 > 10) {
    speaker1.tone(softPotValue_D, 200);
    // speaker2.tone(softPotADC2, 50); 
  } 

  // delay 
  // delay(100);

}


// NOTE_B0  31
// NOTE_C1  33
// NOTE_CS1 35
// NOTE_D1  37
// NOTE_DS1 39
// NOTE_E1  41
// NOTE_F1  44
// NOTE_FS1 46
// NOTE_G1  49
// NOTE_GS1 52
// NOTE_A1  55
// NOTE_AS1 58
// NOTE_B1  62
// NOTE_C2  65
// NOTE_CS2 69
// NOTE_D2  73
// NOTE_DS2 78
// NOTE_E2  82
// NOTE_F2  87
// NOTE_FS2 93
// NOTE_G2  98
// NOTE_GS2 104
// NOTE_A2  110
// NOTE_AS2 117
// NOTE_B2  123
// NOTE_C3  131
// NOTE_CS3 139
// NOTE_D3  147
// NOTE_DS3 156
// NOTE_E3  165
// NOTE_F3  175
// NOTE_FS3 185
// NOTE_G3  196
// NOTE_GS3 208
// NOTE_A3  220
// NOTE_AS3 233
// NOTE_B3  247
// NOTE_C4  262
// NOTE_CS4 277
// NOTE_D4  294
// NOTE_DS4 311
// NOTE_E4  330
// NOTE_F4  349
// NOTE_FS4 370
// NOTE_G4  392
// NOTE_GS4 415
// NOTE_A4  440
// NOTE_AS  455
// NOTE_AS4 466
// NOTE_B4  494
// NOTE_C5  523
// NOTE_CS5 554
// NOTE_D5  587
// NOTE_DS5 622
// NOTE_E5  659
// NOTE_F5  698
// NOTE_FS5 740
// NOTE_G5  784
// NOTE_GSH  830
// NOTE_GS5 831
// NOTE_A5  880
// NOTE_AS5 932
// NOTE_B5  988
// NOTE_C6  1047
// NOTE_CS6 1109
// NOTE_D6  1175
// NOTE_DS6 1245
// NOTE_E6  1319
// NOTE_F6  1397
// NOTE_FS6 1480
// NOTE_G6  1568
// NOTE_GS6 1661
// NOTE_A6  1760
// NOTE_AS6 1865
// NOTE_B6  1976
// NOTE_C7  2093
// NOTE_CS7 2217
// NOTE_D7  2349
// NOTE_DS7 2489
// NOTE_E7  2637
// NOTE_F7  2794
// NOTE_FS7 2960
// NOTE_G7  3136
// NOTE_GS7 3322
// NOTE_A7  3520
// NOTE_AS7 3729
// NOTE_B7  3951
// NOTE_C8  4186
// NOTE_CS8 4435
// NOTE_D8  4699
// NOTE_DS8 4978
