#include "MatrixMaster.h"
#include "ASCII5x7.h"
#include "MAX7219.h"

MatrixMaster::MatrixMaster(int dataPin, int clockPin, int latchPin, bool commonAnode, int numOfParents) {
  this->dataPin = dataPin;
  this->clockPin = clockPin;
  this->latchPin = latchPin;
  this->commonAnode = commonAnode;
  this->numOfParents = numOfParents;
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
  for (int i = 0; i < 8; i++) {
    status[i] = 0x00;
    writeCommand(i + 1, status[i]);
  }
}

void MatrixMaster::setRow(int row, bool on) {
  if (row < 0 || row > 7) return;
  updateStatus(row, 255, on);
  writeCommand(row + 1, status[row]);
}

void MatrixMaster::setRow(int row, byte data) {
  if (row < 0 || row > 7) return;
  if (commonAnode) data = reverseBitOrder(data);
  status[row] = data;
  writeCommand(row + 1, status[row]);
}

void MatrixMaster::setColoumn(int col, bool on) {
  if (col < 0 || col > 7) return;
  if (on) setColoumn(col, (byte)255);
  else setColoumn(col, (byte)0);
}

void MatrixMaster::setColoumn(int col, byte data) {
  for (int i = 0; i < 8; i++) {
    bool on = (byte)((byte)(data << i) >> 7) == 1;
    led(i, col, on);
  }
}

void MatrixMaster::led(byte row, int col, bool on) {
  if (row < 0 || row > 7) return;
  if (col < 0 || col > 7) return;
  byte data = getColData(col);

  updateStatus(row, data, on);
  writeCommand(row + 1, status[row]);
}

void MatrixMaster::scrollText(String text, int slowdownFactor) {
  for (int i = 0; i < text.length(); i++) text[i] = toupper(text[i]);

  int index = getIndexOfAscii(&text, 0);
  byte tmp[7];
  memcpy(tmp, ascii[index], 7);
  for (int i = 0; i < 7; i++) tmp[i] >>= 2; //first char 2 leds left

  for (int j = 1; j < text.length(); j++) {
    int next = getIndexOfAscii(&text, j);
    for (int k = 0; k < 6; k++) { //left shifts per char, 6 because first char inits 2 from left
      for (int i = 0; i < 7; i++) { //bytes per char
        setRow(7 - i, tmp[i]);
        appendByte(&tmp[i], ascii[next][i], k);
      }
      delay(slowdownFactor);
    }
  }
  centerLastChar(tmp);
}
void MatrixMaster::writeCommand(byte registerr, byte data) {
  digitalWrite(latchPin, LOW);

  shiftOut(dataPin, clockPin, MSBFIRST, registerr);
  shiftOut(dataPin, clockPin, MSBFIRST, data);
  for (int i = 0; i < numOfParents * 2; i++) shiftOut(dataPin, clockPin, MSBFIRST, NOOP); //two bytes of NOOP to each parent

  digitalWrite(latchPin, HIGH);
}

byte MatrixMaster::reverseBitOrder(byte b, int bits) {
  byte tmp = 0x00;
  for (int i = 0; i < bits; i++) {
    tmp = tmp | ((byte)((b >> i) << bits - 1 ) >> i);
  }
  return (tmp | (b >> bits - 1));
}

void MatrixMaster::updateStatus(int row, byte data, bool on) {
  if (on) status[row] = status[row] | data; //setting all 1's in data to 1's in status row
  else status[row] = status[row] & (~data); //setting all 1's in data to 0's in status row
}

byte MatrixMaster::getColData(int col) {
  byte data = B10000000 >> col;
  if (commonAnode) data = reverseBitOrder(data);
  return data;
}

void MatrixMaster::appendByte(byte *dest, byte src, int bitIndex) {
  *dest <<= 1;
  src <<= bitIndex; //which bit in src to append to end of dest
  src >>= 7;
  if (src == 1) *dest |= 1; //if the bit at binIndex was 1, set first bit of dest to 1
}

int MatrixMaster::getIndexOfAscii(String *text, int charIndex) {
  unsigned char c = (*text)[charIndex];
  if (c == 195) {
    unsigned char c2 = (*text)[charIndex + 1];
    text->remove(charIndex + 1, 1);
    if (c2 == 133) return 28; //Å
    if (c2 == 152) return 27; //Ø
    if (c2 == 134) return 26; //Æ
  }
  if (c == ' ') return 29; //space
  return c - 65;
}

void MatrixMaster::centerLastChar(byte ch[]){
  for (int k = 0; k < 2; k++) {
    for (int i = 0; i < 7; i++) {
      setRow(7 - i, ch[i]);
      ch[i] <<= 1;
    }
  }
}

