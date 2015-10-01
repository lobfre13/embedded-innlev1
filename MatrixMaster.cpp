#include "MatrixMaster.h"
#include "ASCII8x8.h"
#include "MAX7219.h"

MatrixMaster::MatrixMaster(int dataPin, int clockPin, int latchPin, bool commonAnode) {
  this->dataPin = dataPin;
  this->clockPin = clockPin;
  this->latchPin = latchPin;
  this->commonAnode = commonAnode;
  pinMode(dataPin, OUTPUT);
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  digitalWrite(latchPin, HIGH);

  displayTest(false);
  decodeMode(false);
  setScanLimit(7);
  isShutdown(false);
  clear();
}

void MatrixMaster::displayTest(bool on) {
  writeCommand(DISPLAYTEST, on);
}

void MatrixMaster::decodeMode(bool on) {
  writeCommand(DECODEMODE, on);
}

void MatrixMaster::setScanLimit(int limit) {
  if (limit < 0 || limit > 7) return;
  writeCommand(SCANLIMIT, limit);
}

void MatrixMaster::setBrightness(int brightness) {
  if (brightness < 1 || brightness > 15) return;
  writeCommand(INTENSITY, brightness);
}

void MatrixMaster::isShutdown(bool status) {
  writeCommand(SHUTDOWN, !status);
}

void MatrixMaster::clear() {
  for (int i = 1; i < 9; i++) {
    status[i - 1] = 0x00;
    writeCommand(i, status[i - 1]);
  }
}

void MatrixMaster::setRow(int row, bool on) {
  if (row < 1 || row > 8) return;
  updateStatus(row, 255, on);
  writeCommand(row, status[row]);
}

void MatrixMaster::setRow(int row, byte data) {
  if (row < 1 || row > 8) return;
  if (commonAnode) data = reverseBitOrder(data);
  status[row - 1] = data;
  writeCommand(row, status[row - 1]);
}

void MatrixMaster::setColoumn(int col, bool on) {
  if (col < 1 || col > 8) return;
  if (on) setColoumn(col, (byte)255);
  else setColoumn(col, (byte)0);
}

void MatrixMaster::setColoumn(int col, byte data) {
  for (int i = 0; i < 8; i++) {
    bool on = (byte)((byte)(data << i) >> 7) == 1;
    led(i + 1, col, on);
  }
}

void MatrixMaster::led(byte row, int col, bool on) {
  if (row < 1 || row > 8) return;
  if (col < 1 || col > 8) return;
  byte data = getColData(col);

  updateStatus(row, data, on);
  writeCommand(row, status[row - 1]);
}

void MatrixMaster::scrollText(char text[]) {
  byte tmp[7];
  byte tmp2[7];
  int chrlength = sizeof(text) / sizeof(text[0]);
  for(int j = 0; j < 500; j++){
    int index = text[j] - 65;
    int nextI = text[j+1] - 65;
    memcpy(tmp, ascii[index], 7);
    memcpy(tmp2, ascii[nextI], 7);
    for (int k = 0; k < 8; k++) {
      for (int i = 0; i < 7; i++) {
        setRow(8 - i, tmp[i]);
      }
      for(int i = 0; i < 7; i++){
        tmp[i] = tmp[i] << 1;
         byte b = tmp2[i];
           b = b << k;
           b = b >> 7;
           if(b == 1)
           tmp[i] = tmp[i] | B00000001;
      }
      delay(75);
  }
  }
    
}


void MatrixMaster::writeCommand(byte registerr, byte data) {
  digitalWrite(latchPin, LOW);

  shiftOut(dataPin, clockPin, MSBFIRST, registerr);
  shiftOut(dataPin, clockPin, MSBFIRST, data);

  digitalWrite(latchPin, HIGH);
}

byte MatrixMaster::reverseBitOrder(byte b) {
  int bits = 8;
  byte tmp = 0x00;
  for (int i = 0; i < bits; i++) {
    tmp = tmp | ((byte)((b >> i) << bits - 1 ) >> i);
  }
  return (tmp | (b >> bits - 1));
}

void MatrixMaster::updateStatus(int row, byte data, bool on) {
  if (on) status[row - 1] = status[row - 1] | data; //setting all 1's in data to 1's in status row
  else {
    status[row - 1] = status[row - 1] & (~data); //setting all 1's in data to 0's in status row
  }
}

byte MatrixMaster::getColData(int col) {
  col--;
  byte data = B10000000 >> col;
  if (commonAnode) data = reverseBitOrder(data);
  return data;
}

