int snakeBoard[20][8];
int snakeLength, 
    snakeX, 
    snakeY,
    snakeLoopCounter;

enum direction {
  UP,
  DOWN,
  LEFT,
  RIGHT
};

direction snakeDirection;
 

void startSnake() {
  // Clear the board
  for (int y = 0; y < 8; y++) {
    for (int x = 0; x < 20; x++) {
      snakeBoard[x][y] = 0;
    }
  }

  // Snake starts heading right, appox middle of the screen, and is 3 blocks long
  snakeBoard[5][4] = 3;
  snakeBoard[6][4] = 2;
  snakeBoard[7][4] = 1;

  snakeLength = 3;
  snakeX = 7;
  snakeY = 4;
  snakeLoopCounter = 0;

  snakeDirection = RIGHT;

  dropFood();
}

void dropFood() {
  // Drop some random food
  int foodX, foodY;
  bool safeToDropFood = false;
  while ( !safeToDropFood ) {
    foodX = random(20);
    foodY = random(8);

    if ( snakeBoard[foodX][foodY] == 0 ) {
      safeToDropFood = true;
      if ( random(100) > 50 ) { // set random colors for food
        snakeBoard[foodX][foodY] = -1;
      } else {
        snakeBoard[foodX][foodY] = -2;
      }
    }
  }
}

void gameOver() {
  // Show the score, wait and then new game
  analogWrite(PIEZO, 127);
  delay(200);
  analogWrite(PIEZO, 0);
  clearBuffer();
  displayNumber(snakeLength - 3);
  sendBufferToDisplay();
  delay(2000);
  startSnake();
}

void snakeLoop() {
  clearBuffer();

  // Draw the snake
  currentColor = WHITE;
  for (int y = 0; y < 8; y++) {
    for (int x = 0; x < 20; x++) {
      if (snakeBoard[x][y] > 0) {
        // Positive numbers are the snake
        plot(x,y);
      } else if (snakeBoard[x][y] == -1) {
        // -1 is blue food 
        currentColor = BLUE;
        plot(x,y);
        currentColor = WHITE;
      } else if (snakeBoard[x][y] == -2) {
        // -2 is red food
        currentColor = RED;
        plot(x,y);
        currentColor = WHITE;
      } 
    }
  }

  sendBufferToDisplay();

  // Update direction but snake can't go back over itself
  if ((buttonUp.wasPressed()) && (snakeDirection != DOWN)) snakeDirection = UP;
  if ((buttonDown.wasPressed()) && (snakeDirection != UP)) snakeDirection = DOWN;
  if ((buttonLeft.wasPressed()) && (snakeDirection != RIGHT)) snakeDirection = LEFT;
  if ((buttonRight.wasPressed()) && (snakeDirection != LEFT)) snakeDirection = RIGHT;

  // Now update the snakes location every `tick`
  // 1 tick = 40 loops/frames
  snakeLoopCounter++;
  if (snakeLoopCounter == 40) {
    snakeLoopCounter = 0;

    analogWrite(PIEZO, 127);
    delay(10);
    analogWrite(PIEZO, 0);


    if (snakeDirection == UP) {
      snakeY--;
      if (snakeY < 0) {
        gameOver(); // boundary checks x_x
        return;
      }
    } else if (snakeDirection == DOWN) {
      snakeY++;
      if (snakeY > 7) {
        gameOver();
        return;
      }
    } else if (snakeDirection == LEFT) {
      snakeX--;
      if (snakeX < 0) {
        gameOver();
        return;
      }
    } else if (snakeDirection == RIGHT) {
      snakeX++;
      if (snakeX > 19) {
        gameOver();
        return;
      }
    }

    // Snake eats itself so game over
    if (snakeBoard[snakeX][snakeY] > 0) {
      gameOver();
      return;
    }

    if (snakeBoard[snakeX][snakeY] < 0) {
      // Found food
      analogWrite(PIEZO, 127);
      delay(100);
      analogWrite(PIEZO, 0);
      snakeLength++;
      dropFood();
    }

    for (int y = 0; y < 8; y++) {
      for (int x = 0; x < 20; x++) {
        if (snakeBoard[x][y] > 0) {
          snakeBoard[x][y] = snakeBoard[x][y] + 1;
        }

        // Erase the end of the snake
        if (snakeBoard[x][y] > snakeLength) {
          snakeBoard[x][y] = 0;
        }
      }
    }

    // New nose of the snake
    snakeBoard[snakeX][snakeY] = 1;
  }
}