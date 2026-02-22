
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h> 
#include <Fonts/FreeSans9pt7b.h>
//#include <Fonts/FreeSans12pt7b.h>

#define OLED_RESET 4
Adafruit_SSD1306 display(128, 64, &Wire, OLED_RESET);

// Melody notes (frequencies)
const int c = 261;
const int d = 294;
const int e = 329;
const int f = 349;
const int g = 391;
const int gS = 415;
const int a = 440;
const int aS = 455;
const int b = 466;
const int cH = 523;
const int cSH = 554;
const int dH = 587;
const int dSH = 622;
const int eH = 659;
const int fH = 698;
const int fSH = 740;
const int gH = 784;
const int gSH = 830;
const int aH = 880;

// Player's spaceship bitmap (16x16)
const unsigned char PROGMEM playerShipBitmap[] = {
  0x00, 0x00, 0x00, 0x00, 0x1C, 0x00, 0x3F, 0xF0, 0x3C, 0x00, 0x3C, 0x00, 0xFF, 0x00, 0x7F, 0xFF,
  0x7F, 0xFF, 0xFF, 0x00, 0x3C, 0x00, 0x3C, 0x00, 0x1F, 0xF0, 0x1C, 0x00, 0x00, 0x00, 0x00, 0x00
};

// Player's bullet coordinates
int playerBulletX = 0;
int playerBulletY = 0;
bool playerBulletActive = false;

// Enemy (Death Star) movement
int enemyY = 8;            // vertical position
int enemyDirection = 0;    // 0 = down, 1 = up
bool gameOver = false;

// Enemy bullets (up to 4 active)
int enemyBulletX1 = 95;
int enemyBulletY1 = 0;
int enemyBulletX2 = 95;
int enemyBulletY2 = 0;
int enemyBulletX3 = 95;
int enemyBulletY3 = 0;
int enemyBulletX4 = 95;
int enemyBulletY4 = 0;

int score = 0;
int bulletSpeed = 3;           // enemy bullet speed
int enemyMoveStep = 1;         // how many pixels enemy moves per frame
int minShotDelay = 600;        // minimum delay between enemy shots (ms)
int maxShotDelay = 1200;       // maximum delay between enemy shots (ms)
int enemyDiameter = 10;        // enemy size

int lives = 5;
bool shotTimerStarted = false; // flag for random shot timer
int shotsFired = 0;            // number of active enemy bullets (0-4)
int level = 1;
int enemyCenterX = 95;         // fixed X position of enemy

unsigned long shotStartTime = 0;
unsigned long shotDelay = 0;
unsigned long currentTime = 0;
unsigned long levelTime = 0;   // time when level last increased
int playerY = 30;              // player's vertical position

void loop() {
  if (gameOver == 0) {
    display.clearDisplay();


    // Random shot timer logic
    if (shotTimerStarted == 0) {
      shotStartTime = millis();
      shotDelay = random(minShotDelay, maxShotDelay);
      shotTimerStarted = 1;
    }
    currentTime = millis();

    // Level progression every 50 seconds
    if ((currentTime - levelTime) > 50000) {
      levelTime = currentTime;
      level = level + 1;

      bulletSpeed = bulletSpeed + 1;        // enemy bullets faster
      if (level % 2 == 0) {
        enemyMoveStep = enemyMoveStep + 1;  // enemy moves faster
        enemyDiameter = enemyDiameter - 1;   // enemy shrinks
      }
      minShotDelay = minShotDelay - 50;
      maxShotDelay = maxShotDelay - 50;
    }

    // Time to fire a new enemy bullet
    if ((shotDelay + shotStartTime) < currentTime) {
      shotTimerStarted = 0;
      shotsFired = shotsFired + 1;
      if (shotsFired == 1) {
        enemyBulletX1 = 95;
        enemyBulletY1 = enemyY;
      }
      if (shotsFired == 2) {
        enemyBulletX2 = 95;
        enemyBulletY2 = enemyY;
      }
      if (shotsFired == 3) {
        enemyBulletX3 = 95;
        enemyBulletY3 = enemyY;
      }
      if (shotsFired == 4) {
        enemyBulletX4 = 95;
        enemyBulletY4 = enemyY;
      }
    }

    // Draw and move enemy bullets
    if (shotsFired > 0) {
      display.drawCircle(enemyBulletX1, enemyBulletY1, 2, 1);
      enemyBulletX1 = enemyBulletX1 - bulletSpeed;
    }
    if (shotsFired > 1) {
      display.drawCircle(enemyBulletX2, enemyBulletY2, 1, 1);
      enemyBulletX2 = enemyBulletX2 - bulletSpeed;
    }
    if (shotsFired > 2) {
      display.drawCircle(enemyBulletX3, enemyBulletY3, 4, 1);
      enemyBulletX3 = enemyBulletX3 - bulletSpeed;
    }
    if (shotsFired > 3) {
      display.drawCircle(enemyBulletX4, enemyBulletY4, 2, 1);
      enemyBulletX4 = enemyBulletX4 - bulletSpeed;
    }

    // Player controls (buttons)
    if (digitalRead(11) == 0 && playerY >= 2) {
      playerY = playerY - 2;   // move up
    }
    if (digitalRead(10) == 0 && playerY <= 46) {
      playerY = playerY + 2;   // move down
    }

    // Fire player bullet
    if (digitalRead(9) == 0 && playerBulletActive == 0) {
      playerBulletActive = 1;
      playerBulletX = 6;
      playerBulletY = playerY + 8;
      tone(A0, 1200, 20);
    }

    // Move player bullet
    if (playerBulletActive == 1) {
      playerBulletX = playerBulletX + 8;
      display.drawLine(playerBulletX, playerBulletY, playerBulletX + 4, playerBulletY, 1);
    }

    // Draw player ship
    display.drawBitmap(4, playerY, playerShipBitmap, 16, 16, 1);
    // Draw enemy (Death Star)
    display.fillCircle(enemyCenterX, enemyY, enemyDiameter, 1);
    display.fillCircle(enemyCenterX + 2, enemyY + 3, enemyDiameter / 3, 0);

    // Display score, lives, level, time
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(33, 57);
    display.println("score:");
    display.setCursor(68, 57);
    display.println(score);

    display.setCursor(33, 0);
    display.println("lives:");
    display.setCursor(68, 0);
    display.println(lives);

    display.setCursor(110, 0);
    display.println("L:");
    display.setCursor(122, 0);
    display.println(level);

    display.setCursor(108, 57);
    display.println(currentTime / 1000);
    display.display();

    // Remove player bullet if off screen
    if (playerBulletX > 128) playerBulletActive = 0;

    // Enemy vertical movement (bouncing)
    if (enemyDirection == 0) {
      enemyY = enemyY + enemyMoveStep;
    } else {
      enemyY = enemyY - enemyMoveStep;
    }
    if (enemyY >= (64 - enemyDiameter))
      enemyDirection = 1;
    if (enemyY <= enemyDiameter)
      enemyDirection = 0;

    // Check if player bullet hits enemy
    if (playerBulletY >= enemyY - enemyDiameter && playerBulletY <= enemyY + enemyDiameter) {
      if (playerBulletX > (enemyCenterX - enemyDiameter) && playerBulletX < (enemyCenterX + enemyDiameter)) {
        playerBulletX = -20;          // remove bullet
        tone(A0, 500, 20);
        score = score + 1;
        playerBulletActive = 0;
      }
    }

    int playerCenterY = playerY + 8;  // center of player ship

    // Check collision of enemy bullets with player
    if (enemyBulletY1 >= playerCenterY - 8 && enemyBulletY1 <= playerCenterY + 8) {
      if (enemyBulletX1 < 12 && enemyBulletX1 > 4) {
        enemyBulletX1 = 95;
        enemyBulletY1 = -50;
        tone(A0, 100, 100);
        lives = lives - 1;
      }
    }
    if (enemyBulletY2 >= playerCenterY - 8 && enemyBulletY2 <= playerCenterY + 8) {
      if (enemyBulletX2 < 12 && enemyBulletX2 > 4) {
        enemyBulletX2 = -50;
        enemyBulletY2 = -50;
        tone(A0, 100, 100);
        lives = lives - 1;
      }
    }
    if (enemyBulletY3 >= playerCenterY - 8 && enemyBulletY3 <= playerCenterY + 8) {
      if (enemyBulletX3 < 12 && enemyBulletX3 > 4) {
        enemyBulletX3 = -50;
        enemyBulletY3 = -50;
        tone(A0, 100, 100);
        lives = lives - 1;
      }
    }
    if (enemyBulletY4 >= playerCenterY - 8 && enemyBulletY4 <= playerCenterY + 8) {
      if (enemyBulletX4 < 12 && enemyBulletX4 > 4) {
        enemyBulletX4 = 200;
        enemyBulletY4 = -50;
        shotsFired = 0;
        tone(A0, 100, 100);
        lives = lives - 1;
      }
    }

    // Reset bullet counter if last bullet off screen
    if (enemyBulletX4 < 1) {
      shotsFired = 0;
      enemyBulletX4 = 200;
    }

    if (lives == 0)
      gameOver = 1;
  }

  // Game over screen
  if (gameOver == 1) {
    if (lives == 0) {
      // Play sad melody
      tone(A0, 200, 300);
      delay(300);
      tone(A0, 250, 200);
      delay(200);
      tone(A0, 300, 300);
      delay(300);
      lives = 5; // restore for next game
    }
    display.clearDisplay();
    display.setFont();
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(7, 10);
    display.println("GAME OVER!");
    display.setTextSize(1);
    display.setCursor(7, 30);
    display.println("score:");
    display.setCursor(44, 30);
    display.println(score);
    display.setCursor(7, 40);
    display.println("level:");
    display.setCursor(44, 40);
    display.println(level);
    display.setCursor(7, 50);
    display.println("time(s):");
    display.setCursor(60, 50);
    display.println(currentTime / 1000);
    display.display();

    // Restart game if button pressed
    if (digitalRead(9) == 0) {
      tone(A0, 280, 300);
      delay(300);
      tone(A0, 250, 200);
      delay(200);
      tone(A0, 370, 300);
      delay(300);
      resetGame();
    }
  }
}

void resetGame() {
  playerBulletX = 0;
  playerBulletY = 0;
  playerBulletActive = false;
  enemyY = 8;
  enemyDirection = 0;
  gameOver = 0;
  enemyBulletX1 = 95;
  enemyBulletY1 = 0;
  enemyBulletX2 = 95;
  enemyBulletY2 = 0;
  enemyBulletX3 = 95;
  enemyBulletY3 = 0;
  enemyBulletX4 = 95;
  enemyBulletY4 = 0;
  score = 0;

  bulletSpeed = 3;
  enemyMoveStep = 1;
  minShotDelay = 600;
  maxShotDelay = 1200;
  enemyDiameter = 12;

  lives = 5;
  shotTimerStarted = 0;
  shotsFired = 0;
  level = 1;
  shotStartTime = 0;
  shotDelay = 0;
  currentTime = 0;
  levelTime = 0;
}
