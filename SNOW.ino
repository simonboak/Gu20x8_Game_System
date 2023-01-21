bool snowScene[20][8];
int snowSpeed = 400;

void startSnow() {
  currentColor = WHITE;
}

void snowLoop() {
  clearBuffer();

  // Random chance of new snowflake
  bool foundFreePixel = false;

  // Find a free pixel on the top row to drop it
  while (!foundFreePixel) {
    int randomLocation = random(19);
    // Ensure there's no snow at the first or second rows already for this location
    if ( (!snowScene[randomLocation][0]) && (!snowScene[randomLocation][1]) ) {
      foundFreePixel = true;
      snowScene[randomLocation][0] = true;
    }
  }

  // Draw the snow scene
  for (int y = 0; y < 8; y++) {
    for (int x = 0; x < 20; x++) {
      if (snowScene[x][y]) plot(x,y);
    }
  }

  sendBufferToDisplay();
  delay(snowSpeed);

  // Up and down arrows change the speed
  if (buttonUp.wasPressed()) {
    snowSpeed = snowSpeed - 50;
    if (snowSpeed < 0) snowSpeed = 0;
  }

  if (buttonDown.wasPressed()) {
    snowSpeed = snowSpeed + 50;
    if (snowSpeed > 1000) snowSpeed = 1000;
  }

  // Press A to toggle between snow and rain
  if (buttonA.wasPressed()) {
    if (currentColor == WHITE) {
      currentColor = BLUE;
    } else if (currentColor == BLUE) {
      currentColor = WHITE;
    }
  }

  // Each tick move down by 1 row
  // Start from the bottom and work up
  for (int y = 6; y > -1; y--) {
    for (int x = 0; x < 20; x++) {
      if ( snowScene[x][y] ) {
        snowScene[x][y+1] = true; // Move the flake 1 position down
        snowScene[x][y] = false; // Erase its old location
      }
    }
  }

  // Randomly melt some bits
  if (random(7) == 1) {
    snowScene[random(19)][7] = false;
  }
}