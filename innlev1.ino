#include "MatrixMaster.h"


int dataPin = 12;
int latchPin = 10;
int clockPin = 11;

void setup() {
  MatrixMaster mm = MatrixMaster(dataPin, clockPin, latchPin);

  //mm.setRow(5, true);
  //mm.scrollText("FREDRIKI");
  // mm.setColoumn(5, (byte)B11101111);
//  mm.setBrightness(8);

}

void loop() {

}
