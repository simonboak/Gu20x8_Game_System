#include "pushButton.h"
#include <EEPROM.h>


// Controller inputs
pushButton buttonUp(A5);
pushButton buttonDown(A1);
pushButton buttonLeft(A0);
pushButton buttonRight(A2);
pushButton buttonA(A3);
pushButton buttonB(A4);
pushButton buttonStart(3);
pushButton buttonSelect(2);


// Gu20x8 connections
#define CONTROL_DATA 12 // control = LOW, data = HIGH
#define   WRITE_READ 11 // write = LOW, read = HIGH
#define       ENABLE 10
#define        RESET 9  // active LOW
#define         BUSY 8

// Shift register outputs
#define CLOCK 7
#define  DATA 6

// Piezo connection
#define PIEZO 5

byte bluePixelData[20];
byte  redPixelData[20];

enum color {
  BLACK,
  BLUE,
  RED,  // orange?!
  WHITE
};

color currentColor = RED;





enum mode {
  MENU,
  LIFE,
  SNAKE,
  PAINT,
  SNOW
};

mode currentMode = MENU;



void setup() {
  // Display setup
  pinMode(CONTROL_DATA, OUTPUT);
  pinMode(WRITE_READ, OUTPUT);
  pinMode(ENABLE, OUTPUT);
  pinMode(RESET, OUTPUT);
  pinMode(BUSY, INPUT);

  pinMode(CLOCK, OUTPUT);
  pinMode(DATA, OUTPUT);

  // Piezo setup
  pinMode(PIEZO, OUTPUT);

  // Reset sequence
  digitalWrite(CONTROL_DATA, HIGH);
  digitalWrite(WRITE_READ, HIGH);
  digitalWrite(ENABLE, LOW);
  digitalWrite(RESET, LOW);
  digitalWrite(RESET, HIGH);
  while(digitalRead(BUSY));
  digitalWrite(ENABLE, HIGH);

  // Lower the brightness of the display
  digitalWrite(CONTROL_DATA, LOW);
  shiftOut(DATA, CLOCK, MSBFIRST, 0x3F);  // Address of the brightness register
  shiftOut(DATA, CLOCK, MSBFIRST, 0); // The value we want
  toggleWrite();
  digitalWrite(CONTROL_DATA, HIGH);

  // Set up random data for our starting game
  randomSeed(analogRead(0));

  // Boop
  analogWrite(PIEZO, 127);
  delay(100);
  analogWrite(PIEZO, 0);
  
  startMenu();
}

void loop() {
  switch (currentMode) {
    case LIFE:
      lifeLoop();
      break;
    case SNAKE:
      snakeLoop();
      break;
    case PAINT:
      paintLoop();
      break;
    case SNOW:
      snowLoop();
      break;
    default:
      menuLoop();
  }
}



