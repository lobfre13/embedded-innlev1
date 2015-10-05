#include "MatrixMaster.h"
#include "BrightnessAdjuster.h"
#include "DHT.h"
#define DHTTYPE DHT11

int dhtPin = 8;
int dataPin = 12;
int latchPin = 10;
int clockPin = 11;
int aSpeedPin = 1;
int togglePin = 7;
int upBrightnessPin = 3;
int downBrightnessPin = 2;
MatrixMaster mm = MatrixMaster(dataPin, clockPin, latchPin);
DHT dht(dhtPin, DHTTYPE);

void setup() {
  pinMode(togglePin, INPUT);
  digitalWrite(togglePin, HIGH);
  mm.setBrightness(7);
  startBrightnessInterrupt(upBrightnessPin, downBrightnessPin, 7, &mm);
  dht.begin();
}

void loop() {
  int speed = analogRead(aSpeedPin)/ 3 + 20;
  if (digitalRead(togglePin) == HIGH) {
    mm.scrollText("arduino", speed);
  }
  else {
    int temp = dht.readTemperature();
    mm.scrollText((String)temp + (char)167 + "C", speed);
  }
  delay(1000);
  mm.clear();
}
