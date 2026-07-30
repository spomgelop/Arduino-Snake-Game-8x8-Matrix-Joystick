#include <LedControl.h>

const int matrixDin = 11; //Data in
const int matrixCs = 10;  //Chip select
const int matrixClk = 13; //Clock
const int joystickV = A0; //Joystick verticle axis (up/down)
const int joystickH = A1; //Joystick horizontal axis (left/right)
const int joystickSW = 2; //Joystick push button (start / reset)


LedControl lc = LedControl(matrixDin, matrixClk, matrixCs, 1);

const int GRID_SIZE = 8; //8x8 matrix
const int MAX_LENGTH = 64; //Snake can't be longer than total circles

unsigned long moveInterval = 400; //ms between snake moves (lower = faster)
unsigned long lastMoveTime = 0; //Tracks when the snake last moved

//Game states: waiting for start, actively playing, or waiting after game over
enum GameState {
  STATE_WAITING,
  STATE_PLAYING,
  STATE_GAMEOVER
};

GameState gameState = STATE_WAITING;

//Snake Body coordinates
//snakeX[0] / snakeY[0] is always the head
int snakeX[MAX_LENGTH];
int snakeY[MAX_LENGTH];
int snakeLength;

// Current direction of travel (one of these is always 0, the other is -1 or 1)
int dirX = 1;
int dirY = 0;

//Food position
int foodX, foodY;

void setup() {
  lc.shutdown(0, false); //Wake the display up (Starts in power saving mode)
  lc.setIntensity(0, 8); //Sets the brightness: 0 (dim) to 15 (max)
  lc.clearDisplay(0);    //Start with all LEDs off

  //INPUT_PULLUP means the pin reads HIGH normally
  //and LOW when the button is pressed (pressed = connects to GND)
  pinMode(joystickSW, INPUT_PULLUP);

  //Seed the random number generator using noise from an unused analog pin
  //Without this, "random" food positions would be identical everytime the game runs
  randomSeed(analogRead(A2));

  gameState = STATE_WAITING;
  drawWaitingScreen();
}

void loop() {
  switch (gameState) {

    case STATE_WAITING:
      //Sit here until the button is pressed, then start the game
      if (buttonPressed()) {
        startGame();
      }
      break;

    case STATE_PLAYING:
      //Check joystick every loop cycle so direction changes feel instant
      readJoystick();

      //Only move the snake every 'moveInterval' ms.
      //This is the "non-blocking timer" pattern
      //it allows to check the joystick constantly without using delay()
      //which would freeze input reading
      if (millis() - lastMoveTime >= moveInterval) {
        lastMoveTime = millis();
        moveSnake();
      }
      break;

    case STATE_GAMEOVER:
      //Sit here until the button is pressed, then go back to waiting screen
      if (buttonPressed()) {
        gameState = STATE_WAITING;
        drawWaitingScreen();
      }
      break;
  }
}

//Returns true once per physical press (debounced)
bool buttonPressed() {
  if (digitalRead(joystickSW) == LOW) {
    delay(200); //debounce: ignore rapid re-triggers from switch bounce
    return true;
  }
  return false;
}

void startGame() {
  //Reset snake to 3 segments, placed horizontally in the middle
  snakeLength = 3;
  snakeX[0] = 4; snakeY[0] = 4; //Head
  snakeX[1] = 3; snakeY[1] = 4; 
  snakeX[2] = 2; snakeY[2] = 4; //Tail

  //Start moving right
  dirX = 1;
  dirY = 0;

  moveInterval = 400; //reset speed in case it sped up last game

  placeFood();

  gameState = STATE_PLAYING;
  drawGame();
}

void readJoystick() {
  int vVal = analogRead(joystickV); //0-1023, ~512 = centered
  int hVal = analogRead(joystickH);

  int newDirX = dirX;
  int newDirY = dirY;

  //Check horizontal axis first, then vertical
  //Thresholds of 400/600 create a 'dead zone' around center (512)
  //so the snake doesn't jitter when the joystick isn't being touched
  if (hVal < 400) {
  newDirX = -1; newDirY = 0; //pushed left
  } else if (hVal > 600) {
  newDirX = 1; newDirY = 0;  //pushed right
  } else if (vVal < 400) {
  newDirX = 0; newDirY = 1;  //pushed down
  } else if (vVal > 600) {
  newDirX = 0; newDirY = -1; //pushed up
  }


  //prevent the snake from reversing directly into itself
  //e.g. can't go left if currently moving right
  bool isReversal = (newDirX == -dirX && newDirY == -dirY);
  if (!isReversal) {
    dirX = newDirX;
    dirY = newDirY;
  }
}

void moveSnake() {
  //calculate where the new head will be
  int newHeadX = snakeX[0] + dirX;
  int newHeadY = snakeY[0] + dirY;

  //Check wall collision
  if (newHeadX < 0 || newHeadX >= GRID_SIZE ||
      newHeadY < 0 || newHeadY >= GRID_SIZE){
    triggerGameOver();
    return;
  }

  //Check self collision
  for (int i = 0; i < snakeLength; i++) {
    if (snakeX[i] == newHeadX && snakeY[i] == newHeadY) {
      triggerGameOver();
      return;
    }
  }

  //Did snake eat food?
  bool ate = (newHeadX == foodX && newHeadY == foodY);

  //Each segment moves to where the segment in front of it used to be
  //Save 'prevX / prevY' as we go so we know where the old tail was
  //(needed below if the snake just ate and needs to grow)
  int prevX = snakeX[0];
  int prevY = snakeY[0];

  //Move the head to its new position
  //(this was missing before, which is why the snake never moved!)
  snakeX[0] = newHeadX;
  snakeY[0] = newHeadY;

  for (int i = 1; i < snakeLength; i++) {
    int tmpX = snakeX[i];
    int tmpY = snakeY[i];
    snakeX[i] = prevX;
    snakeY[i] = prevY;
    prevX = tmpX;
    prevY = tmpY;
  }

  if (ate) {
    if (snakeLength < MAX_LENGTH) {
      //Add a new tail segment at the old tail's last position
      snakeX[snakeLength] = prevX;
      snakeY[snakeLength] = prevY;
      snakeLength++;
    }

    placeFood(); //spawn new food somewhere else

    //Optional: speed up slightly each time food is eaten
    if (moveInterval > 150) {
      moveInterval -= 5;
    }
  }
  drawGame();
}

void placeFood() {
  bool validSpot;

  //keep picking random coordinates until we find one
  //that isn't currently being occupied by the snake's body
  do {
    validSpot = true;
    foodX = random(0, GRID_SIZE);
    foodY = random(0, GRID_SIZE);

    for (int i = 0; i < snakeLength; i++) {
      if (snakeX[i] == foodX && snakeY[i] == foodY) {
        validSpot = false;
        break;
      }
    }
  } while (!validSpot);
}

void drawGame() {
  lc.clearDisplay(0); //turn all LED's off first

  // Draw each snake segment
  for (int i = 0; i < snakeLength; i++) {
    // setLed(deviceIndex, row, column, state)
    lc.setLed(0, snakeY[i], snakeX[i], true);
  }

  //draw the food
  lc.setLed(0, foodY, foodX, true);
}

//Simple idle-screen: a small square in the center so you know
//the board is on and waiting for the button press to start
void drawWaitingScreen() {
  lc.clearDisplay(0);
  lc.setLed(0, 3, 3, true);
  lc.setLed(0, 3, 4, true);
  lc.setLed(0, 4, 3, true);
  lc.setLed(0, 4, 4, true);
}

void triggerGameOver() {
  gameState = STATE_GAMEOVER;

  //Flash the current snake position on/off a few times
  // as visual feedback that the game has ended. 
  for (int i = 0; i < 4; i++) {
    lc.clearDisplay(0);
    delay(150);
    drawGame();
    delay(150);
  }
  //loop() now waits in STATE_GAMEOVER for a button press,
  //which sends it back to STATE_WAITING
}