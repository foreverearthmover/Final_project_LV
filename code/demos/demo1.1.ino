/***
  Tamagotchi Angel
  ---------------------------------
  This demo ues Adafruit SSD1306 library to draw
  a small angel and react to 3 buttons.

  Buttons:
  ASK  -> D2a
  PRAY -> D3
  PET  -> D4
***/

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// OLED CONFIG 
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);

// BUTTON PINS
const int buttonAsk = 2;
const int buttonPray = 3;
const int buttonPet = 4;

//STATES
enum AngelState {
  NEUTRAL,
  PRAYED,
  RUDE,
  BLESSED
};

AngelState currentState = NEUTRAL;

// TIMING
unsigned long lastPrayTime = 0;
const unsigned long PRAY_TIMEOUT = 15000; // 15 seconds

// ANGEL NUMBERS
int angelNumbers[] = {111, 222, 333, 444, 555, 777, 888, 999};
const int ANGEL_COUNT = 8;

// ASK ANSWERS
const char* neutralAnswers[] = {
  "Maybe.",
  "Hard to say.",
  "The signs are unclear.",
  "Ask again later."
};

const char* prayedAnswers[] = {  // should it really only agree?
  "Yes. Trust it.",
  "The universe agrees.",
  "This is your path.",
  "It will unfold soon."
};

const char* rudeAnswers[] = {
  "You already know.",
  "Why ask me?",
  "No.",
  "Stop bothering me."
};

// PET COUNTER
int petCounter = 0;
const int PET_THRESHOLD = 5;

// SETUP 
void setup() {
  pinMode(buttonAsk, INPUT_PULLUP);
  pinMode(buttonPray, INPUT_PULLUP);
  pinMode(buttonPet, INPUT_PULLUP);

  randomSeed(analogRead(A0)); // randomness

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();

  drawNeutralAngel();
  display.display();
}

// MAIN LOOP
void loop() {
  checkButtons();
  updateState();
}

// -BUTTON HANDLING
void checkButtons() {

  // ASK BUTTON
  if (digitalRead(buttonAsk) == LOW) {
    handleAsk();
    delay(250); 
  }

  // PRAY BUTTON
  if (digitalRead(buttonPray) == LOW) {
    currentState = PRAYED;
    lastPrayTime = millis();
    petCounter = 0;
    drawCurrentState();
    delay(250);
  }

  // PET BUTTON
  if (digitalRead(buttonPet) == LOW) {
  petCounter++;

  if (petCounter >= PET_THRESHOLD) {
    currentState = BLESSED;
    drawAngelNumber();
    delay(3000);            // show blessing
    petCounter = 0;
    currentState = NEUTRAL; // return to normal
    drawCurrentState();
  }

  delay(250);
}

}

// STATE UPDATE
void updateState() {
  // If prayed but too much time passed → rude
  if (currentState == PRAYED &&
      millis() - lastPrayTime > PRAY_TIMEOUT) {
    currentState = RUDE;
    drawCurrentState();

  }
}

void drawCurrentState() {
  switch (currentState) {
    case NEUTRAL:
      drawNeutralAngel();
      break;
    case PRAYED:
      drawPrayedAngel();
      break;
    case RUDE:
      drawRudeAngel();
      break;
    case BLESSED:
      drawBlessedAngelFace();
      break;
  }
}

// ASK LOGIC
void handleAsk() {
  display.clearDisplay();
  drawCurrentState(); // draw face first

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(10, 50);

  if (currentState == PRAYED) {
    int i = random(0, 4);
    display.print(prayedAnswers[i]);
  }
  else if (currentState == RUDE) {
    int i = random(0, 4);
    display.print(rudeAnswers[i]);
  }
  else {
    int i = random(0, 4);
    display.print(neutralAnswers[i]);
  }

  display.display();

  delay(3000);        // Let user read
  drawCurrentState(); // return to angel
}


// DRAWING FUNCTIONS

// Base angel body + halo
void drawAngelBase() {
  display.clearDisplay();

  // Halo
  display.drawCircle(64, 12, 8, WHITE);

  // Head
  display.drawCircle(64, 26, 10, WHITE);

  // Body
  display.drawLine(64, 36, 64, 52, WHITE);

  // Wings
  display.drawLine(54, 38, 44, 48, WHITE);
  display.drawLine(74, 38, 84, 48, WHITE);
}

// Neutral face
void drawNeutralAngel() {
  drawAngelBase();

  // Eyes
  display.drawPixel(61, 26, WHITE);
  display.drawPixel(67, 26, WHITE);

  // Mouth
  display.drawLine(61, 30, 67, 30, WHITE);

  display.display();
}

// Prayed face (happy)
void drawPrayedAngel() {
  drawAngelBase();

  // Happy eyes
  display.drawPixel(61, 25, WHITE);
  display.drawPixel(67, 25, WHITE);

  // Smile (pixel curve)
  display.drawPixel(61, 30, WHITE);
  display.drawPixel(62, 31, WHITE);
  display.drawPixel(63, 32, WHITE);
  display.drawPixel(64, 32, WHITE);
  display.drawPixel(65, 32, WHITE);
  display.drawPixel(66, 31, WHITE);
  display.drawPixel(67, 30, WHITE);

  display.display();
}


// Rude face (annoyed)
void drawRudeAngel() {
  drawAngelBase();

  display.drawLine(59, 24, 63, 24, WHITE);
  display.drawLine(65, 24, 69, 24, WHITE);
  display.drawLine(61, 31, 67, 31, WHITE);

  display.display();
}

void drawBlessedAngelFace() {
  drawAngelBase();

  // Sparkly eyes
  display.drawPixel(60, 25, WHITE);
  display.drawPixel(62, 24, WHITE);
  display.drawPixel(66, 24, WHITE);
  display.drawPixel(68, 25, WHITE);

  // Big smile
  display.drawPixel(61, 30, WHITE);
  display.drawPixel(62, 31, WHITE);
  display.drawPixel(63, 32, WHITE);
  display.drawPixel(64, 33, WHITE);
  display.drawPixel(65, 32, WHITE);
  display.drawPixel(66, 31, WHITE);
  display.drawPixel(67, 30, WHITE);

  display.display();
}

// Special events
void drawAngelNumber() {
  display.clearDisplay();

  int index = random(0, ANGEL_COUNT);
  int number = angelNumbers[index];

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(10, 10);
  display.print("You received an");

  display.setCursor(10, 22);
  display.print("angel number:");

  display.setTextSize(2);
  display.setCursor(40, 40);
  display.print(number);

  display.display();

  // Reset after blessing
  petCounter = 0;
}
