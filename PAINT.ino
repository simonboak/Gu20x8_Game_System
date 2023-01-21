int brushX, brushY;
int brushFlasher = 0;

byte paintBluePixels[20];
byte paintRedPixels[20];

color tempColor;

enum paintMode {
  PAINTING,
  PAINT_MENU,
  CHOOSE_COLOR,
  SAVING_MENU,
  LOADING_MENU
};

paintMode currentPaintMode = PAINTING;


int saveSlot = 0;
int storeAddress = 0;

enum saveMode {
  SAVING,
  LOADING
};

saveMode savingOrLoading = SAVING;

const byte saveScreen[20] = {0x40, 0xE0, 0x12, 0x15, 0xD, 0x0, 0x1E, 0x5, 0x1E, 0x0, 0xF, 0x10, 0xF, 0x0, 0x1F, 0x15, 0x11, 0x0, 0xE0, 0x40};
const byte loadScreen[20] = {0x40, 0xE0, 0x1F, 0x10, 0x10, 0x0, 0xE, 0x11, 0xE, 0x0, 0x1E, 0x5, 0x1E, 0x0, 0x1F, 0x11, 0xE, 0x0, 0xE0, 0x40};

void startPaint() {
  brushX = 9;
  brushY = 3;
  clearBuffer();
}

void paintLoop() {
  if (currentPaintMode == PAINTING) {
    if (buttonUp.wasPressed()) brushY--;
    if (brushY < 0) brushY = 7;

    if (buttonDown.wasPressed()) brushY++;
    if (brushY > 7) brushY = 0;

    if (buttonLeft.wasPressed()) brushX--;
    if (brushX < 0) brushX = 19;

    if (buttonRight.wasPressed()) brushX++;
    if (brushX > 19) brushX = 0;


    // Paint the current color
    if (buttonA.wasPressed()) {
      paint(brushX, brushY);
    }

    // B changes to the colour picker menu
    if (buttonB.wasPressed()) {
      currentPaintMode = CHOOSE_COLOR;
    }

    // Start clears the screen with the current color
    if (buttonStart.wasPressed()) {
      for (int i=0; i<20; i++) {
        if (currentColor == BLACK) {
          paintBluePixels[i] = 0;
          paintRedPixels[i] = 0;
        } else if (currentColor == BLUE) {
          paintBluePixels[i] = 255;
          paintRedPixels[i] = 0;
        } else if (currentColor == RED) {
          paintBluePixels[i] = 0;
          paintRedPixels[i] = 255;
        } else if (currentColor == WHITE) {
          paintBluePixels[i] = 255;
          paintRedPixels[i] = 255;
        }
      }
    }

    // Select opens the paint menu for saving or loading
    if (buttonSelect.wasPressed()) {
      saveSlot = 0;
      savingOrLoading = SAVING;
      currentPaintMode = PAINT_MENU;
    }

    // Copy the paint buffer into the display buffer
    for (int i=0; i<20; i++) {
      bluePixelData[i] = paintBluePixels[i];
      redPixelData[i] = paintRedPixels[i];
    }

    // Draw the brush as a flashing cursor in the opposite color to where its positioned
    brushFlasher++;
    tempColor = currentColor;
    currentColor = BLACK;
    if (brushFlasher < 25) currentColor = WHITE;
    if (brushFlasher > 50) brushFlasher = 0;
    plot(brushX, brushY);
    currentColor = tempColor;

  } else if (currentPaintMode == CHOOSE_COLOR) {
    // Draw the color palette
    for (int i=0; i<20; i++) {
      if ( (i > 4) && (i<10) ) {
        // blue;
        bluePixelData[i] = 0xF;
        redPixelData[i] = 0;
      } else if ( (i > 9) && (i<15) ) {
        // red;
        bluePixelData[i] = 0;
        redPixelData[i] = 0xF;
      } else if (i > 14) {
        // white;
        bluePixelData[i] = 0xF;
        redPixelData[i] = 0xF;
      } else {
        // black
        bluePixelData[i] = 0;
        redPixelData[i] = 0;
      }
    }

    // Draw the arrow
    if (buttonLeft.wasPressed()) currentColor = currentColor - 1;
    if (currentColor < 0) currentColor = 0;

    if (buttonRight.wasPressed()) currentColor = currentColor + 1;
    if (currentColor > 3) currentColor = 3;

    tempColor = currentColor;
    currentColor = WHITE;
    int arrowOffset = tempColor * 5;
    plot(1+arrowOffset,6);
    plot(2+arrowOffset,5);
    plot(2+arrowOffset,6);
    plot(3+arrowOffset,6);
    currentColor = tempColor;

    if (buttonA.wasPressed()) {
      currentPaintMode = PAINTING;
    }
  } else if (currentPaintMode == PAINT_MENU) {
    // Show the menu for saving or loading
    clearBuffer();

    if (savingOrLoading == SAVING) {
      for (int i=0; i<20; i++) {
        bluePixelData[i] = saveScreen[i];
      }
    } else if (savingOrLoading == LOADING) {
      for (int i=0; i<20; i++) {
        bluePixelData[i] = loadScreen[i];
      }
    }

    if ((buttonLeft.wasPressed()) || (buttonRight.wasPressed())) {
      if (savingOrLoading == SAVING) {
        savingOrLoading = LOADING;
      } else if (savingOrLoading == LOADING) {
        savingOrLoading = SAVING;
      }
    }

    // A chooses either load or save
    if (buttonA.wasPressed()) {
      if (savingOrLoading == SAVING) {
        currentPaintMode = SAVING_MENU;
      } else if (savingOrLoading == LOADING) {
        currentPaintMode = LOADING_MENU;
      }
    }

    // B escapes back to painting
    if (buttonB.wasPressed()) {
      currentPaintMode = PAINTING;
    }
  } else if (currentPaintMode == SAVING_MENU) {
    // Choose a slot to save into 
    clearBuffer();

    // Right increments the slot
    if (buttonRight.wasPressed()) {
      saveSlot++;
      if (saveSlot > 9) saveSlot = 0;
    }

    // Left decrements the slot
    if (buttonLeft.wasPressed()) {
      saveSlot--;
      if (saveSlot < 0) saveSlot = 9;
    }

    // A chooses the current slot and saves before returning to painting
    if (buttonA.wasPressed()) {
      // Loop through the pixel data and store on the EEPROM in 40 bytes per save slot
      // It would also be more space efficient to combine the red and blue pixel data but alas laziness
      for (int i=0; i<20; i++) {
        storeAddress = (saveSlot * 40) + i;
        EEPROM.update(storeAddress, paintBluePixels[i]);
        EEPROM.update(storeAddress + 20, paintRedPixels[i]);
      }
      currentPaintMode = PAINTING;
    }

    // B escapes back to painting
    if (buttonB.wasPressed()) {
      currentPaintMode = PAINTING;
      return;
    }

    // Preview the contents of the selected save slot
    for (int i=0; i<20; i++) {
      if (i < 4) {
        // Keep the left side free for the slot number
        bluePixelData[i] = 0;
         redPixelData[i] = 0;
      } else {  
        storeAddress = (saveSlot * 40) + i;
        bluePixelData[i] = EEPROM.read(storeAddress);
        redPixelData[i] = EEPROM.read(storeAddress + 20);
      }
    }

    // Draw the slot number on top of the preview
    displayNumber(saveSlot);
  }  else if (currentPaintMode == LOADING_MENU) {
    // Choose a slot to load from
    clearBuffer();

    // Right increments the slot
    if (buttonRight.wasPressed()) {
      saveSlot++;
      if (saveSlot > 9) saveSlot = 0;
    }

    // Left decrements the slot
    if (buttonLeft.wasPressed()) {
      saveSlot--;
      if (saveSlot < 0) saveSlot = 9;
    }

    // A chooses the current slot and loads before returning to painting
    if (buttonA.wasPressed()) {
      // Loop through the pixel data and read 40 bytes from the EEPROM
      for (int i=0; i<20; i++) {
        storeAddress = (saveSlot * 40) + i;
        paintBluePixels[i] = EEPROM.read(storeAddress);
         paintRedPixels[i] = EEPROM.read(storeAddress + 20);
      }
      currentPaintMode = PAINTING;
    }

    // B escapes back to painting
    if (buttonB.wasPressed()) {
      currentPaintMode = PAINTING;
      return;
    }

    // Preview the contents of the selected save slot
    for (int i=0; i<20; i++) {
      if (i < 4) {
        // Keep the left side free for the slot number
        bluePixelData[i] = 0;
         redPixelData[i] = 0;
      } else {  
        storeAddress = (saveSlot * 40) + i;
        bluePixelData[i] = EEPROM.read(storeAddress);
        redPixelData[i] = EEPROM.read(storeAddress + 20);
      }
    }

    // Draw the slot number on top of the preview
    displayNumber(saveSlot);
  }



  
  sendBufferToDisplay();
}


void paint(int x, int y) {
  // First clear the pixel in both blue and red
  byte blueByte = paintBluePixels[x];
  byte redByte = paintRedPixels[x];
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
  paintBluePixels[x] = blueByte;
  paintRedPixels[x] = redByte;
}