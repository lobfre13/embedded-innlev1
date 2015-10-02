#include "MatrixMaster.h"

volatile int brightness = 8;
volatile byte brightnessBar = B11110000;
volatile unsigned long lastInterruptTime = 0;
int lowerBrightnessPin = 2;
int upBrightnessPin = 3;
int dataPin = 12;
int latchPin = 10;
int clockPin = 11;
MatrixMaster mm = MatrixMaster(dataPin, clockPin, latchPin, false);

void setup() {
  pinMode(lowerBrightnessPin, INPUT);
  pinMode(upBrightnessPin, INPUT);
  digitalWrite(lowerBrightnessPin, HIGH);
  digitalWrite(upBrightnessPin, HIGH);
  attachInterrupt(digitalPinToInterrupt(lowerBrightnessPin), lowerBrightness, FALLING);
  attachInterrupt(digitalPinToInterrupt(upBrightnessPin), upBrightness, FALLING);
  mm.setBrightness(brightness);
}

void loop() {
 // mm.setRow(0, true);
  mm.scrollText("arduino");
  mm.clear();
}

void lowerBrightness(){
  if(millis() - lastInterruptTime < 300) return; 
  if(brightness <= 1) return;
  mm.setBrightness(--brightness);
  brightnessBar <<=1;
  mm.setRow(0, (byte)brightnessBar);
  lastInterruptTime = millis();
}

void upBrightness(){
  if(millis() - lastInterruptTime < 300) return; 
  if(brightness >= 15) return;
  mm.setBrightness(++brightness);
  brightnessBar >>=1;
  brightnessBar |= 128;
  mm.setRow(0, (byte)brightnessBar);
  lastInterruptTime = millis();
}
