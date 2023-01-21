const byte digitFont[10][4] = {
  { // 0
    B01111110,
    B10000001,
    B01111110,
    0
  },
  { // 1
    B10000010,
    B11111111,
    B10000000,
    0
  },
  { // 2
    B11100010,
    B10010001,
    B10001110,
    0
  },
  { // 3
    B01000010,
    B10001001,
    B01110110,
    0
  },
  { // 4
    B00111111,
    B00100000,
    B11111100,
    0
  },
  { // 5
    B01001111,
    B10001001,
    B01110001,
    0
  },
  { // 6
    B01111110,
    B10001001,
    B01110010,
    0
  },
  { // 7
    B11110001,
    B00001101,
    B00000011,
    0
  },
  { // 8
    B01110110,
    B10001001,
    B01110110,
    0
  },
  { // 9
    B01001110,
    B10010001,
    B01111110,
    0
  }
};



void displayNumber(int inputNumber) {
  

  char charBuffer[5];

  // min 0, max 99999
  if ( (inputNumber > -1) && (inputNumber < 100000) ) {
    sprintf(charBuffer, "%i", inputNumber);
  } else {
    sprintf(charBuffer, "%i", 99999);
  }

  for (int i = 0; i < 5; i++) {
    int currentDigit = charBuffer[i] - '0';

    if ( (currentDigit > -1) && (currentDigit < 10) ) {
      int offset = i * 4;
      for (int pixelColumn = 0; pixelColumn < 4; pixelColumn++) {
        bluePixelData[pixelColumn + offset] = digitFont[currentDigit][pixelColumn];
      }
    }
  }
}


void plot(int x, int y) {
  // First clear the pixel in both blue and red
  byte blueByte = bluePixelData[x];
  byte redByte = redPixelData[x];
  blueByte = bitClear(blueByte, y);
  redByte = bitClear(redByte, y);

  // Plot blue if blue, but also if white
  if ((currentColor == BLUE) || (currentColor == WHITE)) {
    blueByte = bitSet(blueByte, y);
  }

  // Plot red if red, but also if white
  if ((currentColor == RED) || (currentColor == WHITE)) {
    redByte = bitSet(redByte, y);
  }

  // Save it back
  bluePixelData[x] = blueByte;
  redPixelData[x] = redByte;
}

color getColor(int x, int y) {
  if ( (bitSet(bluePixelData[x], y)) && (bitSet(redPixelData[x], y)) ) {
    return WHITE;
  } else if (bitSet(bluePixelData[x], y)) {
    return BLUE;
  } else if (bitSet(redPixelData[x], y)) {
    return RED;
  } else {
    return BLACK;
  }
}

void clearBuffer() {
  for (byte i = 0; i < 20; i++) {
    bluePixelData[i] = 0;
    redPixelData[i] = 0;
  }
}



void sendBufferToDisplay() {
  for (byte i = 0; i < 20; i++) {
    shiftOut(DATA, CLOCK, MSBFIRST, i);  // Shift out the blue address
    shiftOut(DATA, CLOCK, MSBFIRST, bluePixelData[int(i)]); // Now the blue data
    toggleWrite();

    shiftOut(DATA, CLOCK, MSBFIRST, int(i) + 32);  // Shift out the red address
    shiftOut(DATA, CLOCK, MSBFIRST, redPixelData[int(i)]); // Now the red data
    toggleWrite();
  }
}

void toggleWrite() {
  digitalWrite(WRITE_READ, LOW);
  digitalWrite(WRITE_READ, HIGH);
  while(digitalRead(BUSY));
}
