// Include libraries 
#include <frequencyToNote.h>
#include <MIDIUSB.h>
#include <MIDIUSB_Defs.h>
#include <pitchToFrequency.h>
#include <pitchToNote.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
// #include <Fonts/FreeMonoBold9pt7b.h>

// Define variables 

// #define numberOfButtons 35

#define sensorPin 18
#define stripPin 19
#define TFT_CS 23 // ??
#define TFT_RST -1 // ?? 
#define TFT_DC 21 // ??

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

// #define R5 0
// #define R4 1
// #define R3 4
// #define R2 5
// #define R1 6

#define rowsLength 5
#define columnsLength 7

byte rows[rowsLength] = { R1, R2, R3, R4, R5 };
byte columns[columnsLength] = { C1, C2, C3, C4, C5, C6, C7 };

byte startingNote = 60;

NoteButton *noteButtons[rowsLength][columnsLength];

// MIDIFlute config

// Sensor modes:
// const byte SENSOR_VELOCITY = 0;
// const byte SENSOR_MOD_WHEEL = 1;
// const byte SENSOR_PITCH_BEND = 2;
// const byte SENSOR_OCTAVE_SHIFT = 3;
// const byte SENSOR_OFF = 4;
// const byte SENSOR_CUSTOM_CC = 5;

// byte sensorMode = SENSOR_MOD_WHEEL;

// Under button modes:
// const byte UNDER_BUTTON_8VA_UP = 0;
// const byte UNDER_BUTTON_8VA_DOWN = 1;
// const byte UNDER_BUTTON_SUSTAIN = 2;

// byte underButtonMode = 0;

// Strip pot modes:
const byte STRIP_VELOCITY = 0;
const byte STRIP_MOD_WHEEL = 1;
const byte STRIP_PITCH_BEND = 2;
const byte STRIP_OFF = 3;
const byte STRIP_CUSTOM_CC = 4;

byte stripSensorMode = 2;

byte currentScale = 0;
byte currentStartingNote = 0;
byte currentStartingOctave = 4;
byte currentSensitivity = 1;

int currentChannel = 0;
int sensorMIDICC = 0;
int stripMIDICC = 0;

byte settingsLength = 7;

byte sensorSensitivities[4] = {200, 175, 150, 125};
byte minimumSensitivity = 100;

byte nextVal;

byte nextStripVal;

byte increment = 4;
uint16_t colors[] = { textColor, 0x545d, textShadowColor };

void showSplashScreen() {}

void launchScreen() {}

void setup() {
  Serial.begin(9600);
  
  // setup pins 
  pinMode(stripPin, INPUT);
  pinMode(sensorPin, INPUT);
  digitalWrite(17, HIGH);

  // setup rows 
  for (byte i = 0; i < rowsLength; i++) {
    pinMode(rows[i], INPUT);
  }
  // setup cols 
  for (byte j = 0; j < columnsLength; j++) {
    pinMode(columns[j], INPUT_PULLUP);
    digitalWrite(columns[j], HIGH);
  }
  
}

void loop() {
  playMIDINotes();
  int sensorPinVal = analogRead(sensorPin);
  int constrainedSensorVal = constrain(analogRead(sensorPin), minimumSensitivity, sensorSensitivities[currentSensitivity]);

  // Avoid unwanted noise when idle.
  if (constrainedSensorVal <= 105) {
    constrainedSensorVal = 100;
  }

  byte sensorValue = map(constrainedSensorVal, minimumSensitivity, sensorSensitivities[currentSensitivity], 0, 127);

  if (sensorValue != nextVal) {
    handleSensorModes(sensorValue);
    nextVal = sensorValue;
  }

  int stripPinVal = analogRead(stripPin);
  int stripVal = map(stripPinVal, 0, 1000, 0, 127);
  stripVal = constrain(stripVal, 0, 127);
  
  if (stripVal != nextStripVal) {
    handleStripVal(stripVal);
    nextStripVal = stripVal;
  }
}

void handleSensorModes(byte sensorVal) {
  // YES I KNOW but the switch here isn't working properly
  // for reasons I don't understand. So this will have to do.
  if (sensorMode == SENSOR_VELOCITY) {
    velocity = sensorVal;
  } else if (sensorMode == SENSOR_MOD_WHEEL) {
    midiEventPacket_t ccModWheel = {0x0B, 0xB0 | currentChannel, 1, sensorVal};
    MidiUSB.sendMIDI(ccModWheel);
  } else if (sensorMode == SENSOR_PITCH_BEND) {
    midiEventPacket_t sensorPitchBendChange = {0x0B, 0xE0 | currentChannel, 1, sensorVal};
    MidiUSB.sendMIDI(sensorPitchBendChange);
  } else if (sensorMode == SENSOR_OCTAVE_SHIFT) {
    handleSensorOctaveShift(sensorVal);
  } else if (sensorMode == SENSOR_OFF) {
    return;
  } else if (sensorMode == SENSOR_CUSTOM_CC) {
    midiEventPacket_t ccMessage = {0x0B, 0xB0 | currentChannel, sensorMIDICC, sensorVal};
    MidiUSB.sendMIDI(ccMessage);
  }

  MidiUSB.flush();
}

void handleStripVal(int stripVal) {
  // YES I KNOW but the switch here isn't working properly
  // for reasons I don't understand. So this will have to do.
  if (stripSensorMode == STRIP_VELOCITY) {
    velocity = stripVal;
  } else if (stripSensorMode == STRIP_MOD_WHEEL) {
    midiEventPacket_t ccChange = {0x0B, 0xB0 | currentChannel, 1, stripVal};
    MidiUSB.sendMIDI(ccChange);
  } else if (stripSensorMode == STRIP_PITCH_BEND) {
    midiEventPacket_t stripPitchBendChange = {0x0B, 0xE0 | currentChannel, 1, stripVal};
    Serial.println(stripVal);
    MidiUSB.sendMIDI(stripPitchBendChange);
  } else if (stripSensorMode == STRIP_OFF) {
    return;
  } else if (stripSensorMode == STRIP_CUSTOM_CC) {
    midiEventPacket_t ccMessage = {0x0B, 0xB0 | currentChannel, stripMIDICC, stripVal};
    MidiUSB.sendMIDI(ccMessage);
  }

  MidiUSB.flush();

}

byte lastOctave = currentStartingOctave;

void handleSensorOctaveShift(byte sensorVal) {
  byte sensorOctaveShift = map(sensorVal, 0, 127, 0, 3);
  byte newOctave = currentStartingOctave + sensorOctaveShift;

  if (lastOctave != newOctave) {
    assignNotesToButtons(currentStartingNote, newOctave, scales[currentScale], scaleLengths[currentScale]);
    lastOctave = newOctave;
  }

}

byte offset = 2;
void drawTextWithShadow(char* text, byte x, byte y) {
  tft.setTextColor(0x545d);
  tft.setCursor(x - offset, y + offset);
  tft.print(text);

  tft.setTextColor(textColor);
  tft.setCursor(x, y);
  tft.print(text);
}


void updateNumberSelectMenuScreen(char* menuName, byte menuNameLength, char value[4], byte selectedIndex, bool isMaximumThreeDigits) {
  tft.fillScreen(backgroundColor);
  tft.setTextSize(2);

  byte menuNameX = (tft.width() / 2) - (22 * menuNameLength / 2);
  drawTextWithShadow(menuName, menuNameX, 50);

  tft.setTextSize(4);

  byte indexOffset = 1;
  if (isMaximumThreeDigits) {
    indexOffset = 2;
  }

  byte textX;
  if (isMaximumThreeDigits) {
    textX = (tft.width() / 2) - (44 * 3 / 2);
    drawTextWithShadow(value, textX, 120);
  } else {
    textX = (tft.width() / 2) - (44 * 2 / 2);
    char shortenedValue[] = {value[1], value[2]};
    drawTextWithShadow(shortenedValue, textX, 120);
  }

  byte underscorePositionOffset = (44 * (indexOffset - selectedIndex));
  drawTextWithShadow("_", textX + underscorePositionOffset, 120);
}
