#include "MatrixMaster.h"

bool interrupt;
int dataPin = 12;
int latchPin = 10;
int clockPin = 11;
MatrixMaster mm = MatrixMaster(dataPin, clockPin, latchPin);

void setup() {

}

void loop() {
  mm.scrollText("lasdsaddasdsdol");

}
