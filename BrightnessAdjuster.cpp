#include <Arduino.h>
#include "MatrixMaster.h"
#include "BrightnessAdjuster.h"

volatile int brightness;
volatile unsigned long lastInterruptTime = 0;
MatrixMaster *matrix;

void startBrightnessInterrupt(int upPin, int downPin, int currentBrightness, MatrixMaster *mm){
  matrix = mm;
  brightness = currentBrightness;
  pinMode(upPin, INPUT);
  pinMode(downPin, INPUT);
  digitalWrite(upPin, HIGH);
  digitalWrite(downPin, HIGH);
  attachInterrupt(digitalPinToInterrupt(upPin), upBrightness, FALLING);
  attachInterrupt(digitalPinToInterrupt(downPin), lowerBrightness, FALLING);
}

void lowerBrightness(){
  setBrightness(brightness-2);
}

void upBrightness(){
  setBrightness(brightness+2);
}

void setBrightness(int b){
  if(millis() - lastInterruptTime < 300) return;
  if(matrix == NULL) return;
  if(b < 1 || b > 15) return;
  
  matrix->setBrightness(b);
  matrix->showBrightnessBar(b);
  brightness = b;
  lastInterruptTime = millis();
}

