/////////////////////////////////////////////////////////////////////////////
//
//  A Simple Game of Snake
//  written by Tyler Edwards for the badge created in Hackerbox 0020,
//  but should work on any ESP32 and Adafruit ILI9341 screen
//
//  Tyler on GitHub: https://github.com/HailTheBDFL
//
//  Hackerboxes: http://www.hackerboxes.com/
//
//  To begin the game, press the select/start/fire/A button on HB badge (default pin 15)
//
// For M5Sack porting, refer to https://macsbug.wordpress.com/2018/01/14/esp32-snake-with-m5stack/

float gameSpeed = 2;  //Higher numbers are faster

// #include <Adafruit_GFX.h>
// #include <Adafruit_ILI9341.h>
// #include <SPI.h>
#include <M5Stack.h>

//Pinout for display (currently for HB0020 Badge)
// #define _cs    19
// #define _dc    22
// #define _mosi  23
// #define _sclk  26
// #define _rst   21
// #define _miso  25

int threshold = 40; //threshold for touch
boolean start = false; //will not start without say-so
unsigned long offsetT = 0; //time delay for touch
unsigned long offsetM = 0; //time delay for main loop

float gs;
int headX = 1;        //coordinates for head
int headY = 1;
int beenHeadX[470];   //coordinates to clear later
int beenHeadY[470];
int changeX = 0;      //the direction of the snake
int changeY = 1;
boolean lastMoveH = false; //to keep from going back on oneself
int score = 1;
int foodX;            //coordinates of food
int foodY;
boolean eaten = true; //if true a new food will be made
int loopCount = 0; //number of times the loop has run
int clearPoint = 0;  //when the loopCount is reset
boolean clearScore = false;

int oldheadX, oldheadY;
//initialize the display
// Adafruit_ILI9341 tft = Adafruit_ILI9341(_cs, _dc, _mosi, _sclk, _rst, _miso);

void setup() {
  gs = 1000 / gameSpeed; //calculated gameSpeed in milliseconds
  
  memset(beenHeadX, 0, 470); //initiate beenHead with a bunch of zeros
  memset(beenHeadY, 0, 470);
  
  // tft.begin();           //turn on display
  M5.begin();

  M5.Lcd.setRotation(1);
  
  M5.Lcd.fillScreen(ILI9341_WHITE);                //sets background
  M5.Lcd.fillRect(3, 21, 316, 226, ILI9341_BLUE);
  
  M5.Lcd.setTextColor(ILI9341_WHITE); //Start notification
  M5.Lcd.setTextSize(3);
  M5.Lcd.setCursor(80, 90);
  M5.Lcd.print(">START<");

  M5.Lcd.setTextColor(ILI9341_BLACK); //Score keeper
  M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(5, 3);
  M5.Lcd.print("Length: ");
  printScore();

  // randomSeed(analogRead(6)); //make every game unique
  randomSeed(analogRead(26)); //make every game unique

  // touchAttachInterrupt(27, left, threshold);  //Touch input
  // touchAttachInterrupt(12, right, threshold);
  // touchAttachInterrupt(13, down, threshold);
  // touchAttachInterrupt(14, up, threshold);
  // touchAttachInterrupt(15, select, threshold);
}

void loop() {

  // Add for M5Stack Input
  // if(M5.BtnA.isPressed() && M5.BtnC.isPressed()){up();}
  // if(M5.BtnB.isPressed() && M5.BtnC.isPressed()){down();}
  // if(M5.BtnA.isPressed()){left  ();}
  // if(M5.BtnB.isPressed()){right ();}
  if(M5.BtnC.isPressed()){select();}

  if(M5.BtnA.isPressed()) {
    // heading to Right
    if (oldheadX < headX) up();
    // heading to Left
    if (oldheadX > headX) down();       
    // heading to up
    if (oldheadY > headY) left();    
    // heading to down            
    if (oldheadY < headY) right();    
  }

  if(M5.BtnB.isPressed()) {
    // heading to Right
    if (oldheadX < headX) down();
    // heading to Left
    if (oldheadX > headX) up();       
    // heading to up
    if (oldheadY > headY) right();    
    // heading to down            
    if (oldheadY < headY) left();    
  }    

  if (clearScore and start) { //resets score from last game, won't clear
    score = 1;                //until new game starts so you can show off
    printScore();             //your own score
    clearScore = false;
  }

  if (millis() - offsetM > gs and start) {
    beenHeadX[loopCount] = headX;  //adds current head coordinates to be
    beenHeadY[loopCount] = headY;  //covered later

    // save current position to determine the direction
    oldheadX = headX;
    oldheadY = headY;
    
    headX = headX + (changeX);  //head moved
    headY = headY + (changeY); 

    
    if (headX - foodX == 0 and headY - foodY == 0) { //food
      score += 1;
      printScore();
      eaten = true;
    }

    loopCount += 1; //loopCount used for addressing, mostly
    
    if (loopCount > 467) {            //if loopCount exceeds size of
      clearPoint = loopCount - score; //beenHead arrays, reset to zero
      loopCount = 0;
    }
    
    drawDot(headX, headY); //head is drawn
    
    if (loopCount - score >= 0) { //if array has not been reset
      eraseDot(beenHeadX[loopCount - score], beenHeadY[loopCount - score]);
    }  //covers end of tail
    else {
      eraseDot(beenHeadX[clearPoint], beenHeadY[clearPoint]);
      clearPoint += 1;
    }
 
    if (eaten) {     //randomly create a new piece of food if last was eaten
      foodX = random(2, 26);
      foodY = random(2, 18);
      eaten = false;
    }

    drawDotRed(foodX, foodY); //draw the food

    if (headX > 26 or headX < 1 or headY < 1 or headY > 18) { //Boudaries
      endGame();
    }

    if (loopCount - score < 0) {         //check to see if head is on tail
      for (int j = 0; j < loopCount; j++) {
        if (headX == beenHeadX[j] and headY == beenHeadY[j]) {
          endGame();
        }
      }
      for (int k = clearPoint; k < 467; k++) {
        if (headX == beenHeadX[k] and headY == beenHeadY[k]) {
          endGame();
        }
      }
    }
    else {
      for (int i = loopCount - (score - 1); i < loopCount; i++) {
        if (headX == beenHeadX[i] and headY == beenHeadY[i]) {
          endGame();
        }
      }
    }
    
    offsetM = millis(); //reset game loop timer
  }

  // Add
  M5.update();

}

void endGame() {
  M5.Lcd.fillRect(3, 21, 316, 226, ILI9341_BLUE); //deletes the old game
  
  eaten = true; //new food will be created
  
  M5.Lcd.setCursor(80, 90);       //Retry message
  M5.Lcd.setTextSize(3);
  M5.Lcd.setTextColor(ILI9341_WHITE);
  M5.Lcd.print("RETRY?");
  M5.Lcd.setTextColor(ILI9341_BLACK); //sets back to scoreboard settings
  M5.Lcd.setTextSize(2);
  
  M5.Lcd.setCursor(5, 3);
  M5.Lcd.print("Length: ");
  
  headX = 1;              //reset snake
  headY = 1;
  changeX = 0;
  changeY = 1;
  lastMoveH = false;

  memset(beenHeadX, 0, 470); //clear the beenHead arrays
  memset(beenHeadY, 0, 470); //probably not necessary

  loopCount = 0;
  clearScore = true;
  start = false;     //stops game
}

void drawDot(int x, int y) {
  M5.Lcd.fillRect(12*(x-1)+5, 12*(y-1)+23, 10, 10, ILI9341_WHITE);
}

void drawDotRed(int x, int y) {
  M5.Lcd.fillRect(12*(x-1)+5, 12*(y-1)+23, 10, 10, ILI9341_RED);
}

void eraseDot(int x, int y) {
  M5.Lcd.fillRect(12*(x-1)+5, 12*(y-1)+23, 10, 10, ILI9341_BLUE);
}

void printScore() {
  M5.Lcd.fillRect(88, 3, 50, 16, ILI9341_WHITE);//clears old score
  M5.Lcd.setCursor(88, 3);
  M5.Lcd.print(score);                            //prints current score
}

void up() {
  if (millis() - offsetT > gs and lastMoveH) { //lastMoveH makes sure you can't go back on yourself
    changeX = 0;   //changes the direction of the snake
    changeY = -1;
    offsetT = millis();
    lastMoveH = false;
  }
}

void down() {
  if (millis() - offsetT > gs and lastMoveH) {
    changeX = 0;
    changeY = 1;
    offsetT = millis();
    lastMoveH = false;
  }
}

void left() {
  if (millis() - offsetT > gs and !lastMoveH) {
    changeX = -1;
    changeY = 0;
    offsetT = millis();
    lastMoveH = true;
  }
}

void right() {
  if (millis() - offsetT > gs and !lastMoveH) {
    changeX = 1;
    changeY = 0;
    offsetT = millis();
    lastMoveH = true;
  }
}

void select() {
  if (millis() - offsetT > gs and !start) {
    M5.Lcd.fillRect(80, 90, 126, 24, ILI9341_BLUE); //Erase start message
    start = true;                                //allows loop to start
    offsetT = millis();
  }
}
