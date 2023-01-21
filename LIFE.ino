bool lifeBoard[20][8];
bool nextLifeBoard[20][8];

int generationCount = 0;

int cursorX = 0;
int cursorY = 0;

bool lifeIsRunning = true;

int speedOfLife = 100;

color lifeColor = RED;


void startLife() {
  fillWithLife();
}


void lifeLoop() {
  clearBuffer();

  // Pressing START toggles run/edit state
  if (buttonStart.wasPressed()) {
    if (lifeIsRunning) {
      lifeIsRunning = false;
    } else {
      generationCount = 0;
      lifeIsRunning = true;
    }
  }

  // SELECT restarts a random board
  if (buttonSelect.wasPressed()) {
    lifeIsRunning = true;
    generationCount = 0;
    fillWithLife();
  }

  if (lifeIsRunning == false) {
    // Cursor controls
    if (buttonUp.wasPressed()) cursorY--;
    if (cursorY < 0) cursorY = 7;

    if (buttonDown.wasPressed()) cursorY++;
    if (cursorY > 7) cursorY = 0;

    if (buttonLeft.wasPressed()) cursorX--;
    if (cursorX < 0) cursorX = 19;

    if (buttonRight.wasPressed()) cursorX++;
    if (cursorX > 19) cursorX = 0;

    // Toggle the current cell
    if (buttonA.wasPressed()) {
      if (lifeBoard[cursorX][cursorY]) {
        lifeBoard[cursorX][cursorY] = false;
      } else {
        lifeBoard[cursorX][cursorY] = true;
      }
    }

    // B clears the board
    if (buttonB.wasPressed()){
      for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 20; x++) {
          lifeBoard[x][y] = false;
        }
      }
    }
  } else {
    // Press up while running and speed increases up to 2 seconds in .02 increments
    if (buttonUp.wasPressed()) {
      speedOfLife = speedOfLife - 20;
      if (speedOfLife < 0) speedOfLife = 0;
    }

    // Press down while running and speed decreases all the way to 0 in .02 increments
    if (buttonDown.wasPressed()) {
      speedOfLife = speedOfLife + 20;
      if (speedOfLife > 2000) speedOfLife = 2000;
    }

    // Press A while running to change the color
    if (buttonA.wasPressed()) {
      if (lifeColor == WHITE) {
        lifeColor = BLUE;
      } else {
        lifeColor = lifeColor + 1;
      }
    }
  }

  

  

  // Draw the current life board
  if (lifeIsRunning) {
    currentColor = lifeColor;
  } else {
    currentColor = RED; // Always RED during edit mode
  }
  for (int y = 0; y < 8; y++) {
    for (int x = 0; x < 20; x++) {
      if (lifeBoard[x][y]) {
        plot(x,y);
      }
    }
  }

  


  
  // End here if we're in edit mode
  if (lifeIsRunning == false) {
    currentColor = BLUE;
    plot(cursorX, cursorY);
    sendBufferToDisplay();
    return;
  }
  sendBufferToDisplay();
    


  // Now simulate the next generation
  int numberOfChangedLives = 0;
  generationCount++;

  for (int y = 0; y < 8; y++) {
    for (int x = 0; x < 20; x++) {
      int liveNeighbourCount = 0;
      int north = y - 1;
      int south = y + 1;
      int east = x + 1;
      int west = x - 1;
      
      if (north < 0) {
        north = 7;
      }
      
      if (west < 0) {
        west = 19;
      }
      
      if (south >= 8) {
        south = 0;
      }
      
      if (east >= 20) {
        east = 0;
      }

      if (lifeBoard[x][north]) liveNeighbourCount++;
      if (lifeBoard[east][north]) liveNeighbourCount++;
      if (lifeBoard[east][y])     liveNeighbourCount++;
      if (lifeBoard[east][south]) liveNeighbourCount++;
      if (lifeBoard[x][south]) liveNeighbourCount++;
      if (lifeBoard[west][south]) liveNeighbourCount++;
      if (lifeBoard[west][y])     liveNeighbourCount++;
      if (lifeBoard[west][north]) liveNeighbourCount++;

      if ( lifeBoard[x][y] ) { // Alive in this generation
        if ( (liveNeighbourCount == 2) || (liveNeighbourCount == 3) ) { 
          // Stays alive
          nextLifeBoard[x][y] = true;
        } else {
          // Dies
          nextLifeBoard[x][y] = false;
          numberOfChangedLives++;
        }

      } else { // Dead in this generation
        if (liveNeighbourCount == 3) {
          // Becomese alive
          nextLifeBoard[x][y] = true;
          numberOfChangedLives++;
        } else {
          // Stays dead
          nextLifeBoard[x][y] = false;
        }
      }
    }
  }

  if ((numberOfChangedLives == 0) || (generationCount == 1000)) {
    // No cells have changed in this generation, or we've likely reached still life (mostly guaranteed within ~1000 generations) so restart with random life
    delay(2000);
    generationCount = 0;

    // Draw a white line to clear the screen
    for (byte i = 0; i < 20; i++) {
      delay(40);

      // Blue...
      shiftOut(DATA, CLOCK, MSBFIRST, i);
      shiftOut(DATA, CLOCK, MSBFIRST, 255);
      toggleWrite();

      // ...and red makes white
      shiftOut(DATA, CLOCK, MSBFIRST, i + 32);
      shiftOut(DATA, CLOCK, MSBFIRST, 255);
      toggleWrite();
      
      if (i > 0) {
        // Clear the previous blue...
        shiftOut(DATA, CLOCK, MSBFIRST, i - 1);
        shiftOut(DATA, CLOCK, MSBFIRST, 0);
        toggleWrite();

        // .. and the previous red
        shiftOut(DATA, CLOCK, MSBFIRST, i + 31);
        shiftOut(DATA, CLOCK, MSBFIRST, 0);
        toggleWrite();
      }
    }

    // Clear the last blue...
    shiftOut(DATA, CLOCK, MSBFIRST, 19);
    shiftOut(DATA, CLOCK, MSBFIRST, 0);
    toggleWrite();

    // .. and the last red
    shiftOut(DATA, CLOCK, MSBFIRST, 19 + 32);
    shiftOut(DATA, CLOCK, MSBFIRST, 0);
    toggleWrite();

    fillWithLife();
  } else {
    // Apply the next generation
    for (int y = 0; y < 8; y++) {
      for (int x = 0; x < 20; x++) {
        lifeBoard[x][y] = nextLifeBoard[x][y];
      }
    }
  }

  delay(speedOfLife);
}

void fillWithLife() {
  for (int y = 0; y < 8; y++) {
    for (int x = 0; x < 20; x++) {
      if (random(255) > 128) {
        lifeBoard[x][y] = true;
      } else {
        lifeBoard[x][y] = false;
      }
    }
  }
}