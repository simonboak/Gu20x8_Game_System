int currentMenuItem = 0;

const byte menus[4][20]= {
  // 0 LIFE
  {0x0, 0x0, 0x1F, 0x10, 0x10, 0x0, 0x11, 0x1F, 0x11, 0x0, 0x1F, 0x5, 0x1, 0x0, 0x1F, 0x15, 0x11, 0x0, 0x0, 0x0},
  // 1 SNAKE
  {0x12, 0x15, 0xD, 0x0, 0x1F, 0x2, 0x4, 0x1F, 0x0, 0x1E, 0x5, 0x1E, 0x0, 0x1F, 0x4, 0x1B, 0x0, 0x1F, 0x15, 0x11},
  // 2 PAINT
  {0x1F, 0x5, 0x2, 0x0, 0x1E, 0x5, 0x1E, 0x0, 0x11, 0x1F, 0x11, 0x0, 0x1F, 0x2, 0x4, 0x1F, 0x0, 0x1, 0x1F, 0x1},
  // 3 SNOW
  {0x12, 0x15, 0xD, 0x0, 0x1F, 0x2, 0x4, 0x1F, 0x0, 0xE, 0x11, 0x11, 0xE, 0x0, 0x7, 0x18, 0x6, 0x6, 0x18, 0x7}
};


void startMenu() {
  // Boot logo animation
  const byte SB[20] = {0x0, 0x0, 0x0, 0x4E, 0xDF, 0xDF, 0xFB, 0xFB, 0x72, 0x0, 0xFF, 0xFF, 0xCB, 0xFF, 0xFF, 0x76, 0x0, 0x0, 0x0, 0x0};

  clearBuffer();

  for (int i = 0; i < 20; i++) {
    redPixelData[i] = SB[i];
    sendBufferToDisplay();
    delay(10);
  }

  for (int i = 0; i < 20; i++) {
    redPixelData[i] = 0;
    bluePixelData[i] = SB[i];
    sendBufferToDisplay();
    delay(10);
  }

  for (int i = 0; i < 20; i++) {
    redPixelData[i] = SB[i];
    sendBufferToDisplay();
    delay(10);
  }

  delay(1000);
}


void menuLoop() {
  // Now draw the data for that menu item
  clearBuffer();
  for (int i = 0; i < 20; i++) {
    bluePixelData[i] = menus[currentMenuItem][i];
  }

  // Draw left/right arrows
  currentColor = WHITE;
  plot(1,5);
  plot(0,6);
  plot(1,6);
  plot(1,7);

  plot(18,5);
  plot(19,6);
  plot(18,6);
  plot(18,7);

  sendBufferToDisplay();


  // Use left/right to cycle through the menus
  if (buttonLeft.wasPressed()) currentMenuItem--;
  if (buttonRight.wasPressed()) currentMenuItem++;
  if ( currentMenuItem < 0 ) currentMenuItem = 3;
  if ( currentMenuItem > 3 ) currentMenuItem = 0;

  // Start or A chooses that item
  if ((buttonStart.wasPressed()) || (buttonA.wasPressed())) {
    currentMode = currentMenuItem + 1;

    switch (currentMode) {
      case LIFE:
        startLife();
        break;
      case SNAKE:
        startSnake();
        break;
      case PAINT:
        startPaint();
        break;
      case SNOW:
        startSnow();
        break;
    }
  }
}