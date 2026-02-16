/***
  Tamagotchi Angel
  ---------------------------------
  This demo uses Adafruit SSD1306 library to draw
  a small angel and react to 3 buttons.

  Buttons:
  ASK  -> D2
  PRAY -> D3
  PET  -> D4
***/

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "bitmaps.h" // contains bitmaps
#include <CuteBuzzerSounds.h> // buzzer sound library

// OLED CONFIG 
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);

// BUTTON PINS
const int buttonAsk = 2;
const int buttonPray = 3;
const int buttonPet = 4;

// BUZZER PIN
const int buzzerPin= 5;

// TILT SWITCH
const int tiltPin = 6;
int tiltState = HIGH;
int lastTiltState = HIGH;

// ASK + SHAKE logic
bool waitingForShake = false;
bool showingAskPrompt = false;
unsigned long askPromptStart = 0;
const unsigned long ASK_PROMPT_DURATION = 4000; // 4 seconds

// INTERACTIONS
enum InteractionMode {
  MODE_IDLE,
  MODE_WAIT_SHAKE,
  MODE_SHOW_ANSWER
};

InteractionMode mode = MODE_IDLE;
unsigned long modeStartTime = 0;

//STATES
enum AngelState {
  ANGEL_NEUTRAL,
  ANGEL_POLITE,
  ANGEL_ANNOYED,
  ANGEL_SURPRISED
};

AngelState angelState = ANGEL_NEUTRAL;
AngelState previousState = ANGEL_NEUTRAL;

// ANIMATION 
int angelBaseY = 0;      // center Y for fullscreen sprite 
int angelOffsetY = 0;   // offset for flying animation
int floatDirection = 1;
unsigned long lastFloatUpdate = 0;
const unsigned long floatInterval = 120; // ms

// TIMING
unsigned long lastPrayTime = 0;
const unsigned long PRAY_TIMEOUT = 5000; // 15 seconds

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
  pinMode(tiltPin, INPUT_PULLUP);

  cute.init(buzzerPin);
  cute.play(S_CONNECTION); // start up sound

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();

  drawAngel();
  display.display();
}

// MAIN LOOP
void loop() {

  checkButtons();
  checkTilt();
  updateState();
  updateAngelFloat();

  // MODE HANDLING
  if (mode == MODE_IDLE) {
    drawAngel();
  }

  else if (mode == MODE_WAIT_SHAKE) {

    drawAngel();   // surprised angel animates

    // draw prompt text ON TOP
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(8, 54);
    display.print("Shake for answers");
    display.display();

    // Timeout: cancel asking if no shake
    if (millis() - modeStartTime > 5000) {
      mode = MODE_IDLE;
      angelState = ANGEL_NEUTRAL;
      waitingForShake = false;
    }
  }

  else if (mode == MODE_SHOW_ANSWER) {

    // Answer stays visible for 3 sec
    if (millis() - modeStartTime > 3000) {
      mode = MODE_IDLE;
      angelState = ANGEL_NEUTRAL;
    }
  }
}

// BUTTON HANDLING
void checkButtons() {

  // ASK BUTTON
  if (digitalRead(buttonAsk) == LOW && !waitingForShake) {

    waitingForShake = true;
    showingAskPrompt = true;
    askPromptStart = millis();

    previousState = angelState;
    angelState = ANGEL_SURPRISED;

    
    mode = MODE_WAIT_SHAKE;
    modeStartTime = millis();

    delay(250); 

  } else if (digitalRead(buttonAsk) == HIGH && waitingForShake && !showingAskPrompt) {
    // This condition means button was released and we were waiting for a shake
    // but the prompt is no longer showing (meaning it timed out or an answer was shown) → reset waiting for shake
    waitingForShake = false;
  }

  // PRAY BUTTON
  if (digitalRead(buttonPray) == LOW) {
    angelState = ANGEL_POLITE;
    lastPrayTime = millis();
    petCounter = 0;
    drawAngel();
    delay(250);
  }

  // PET BUTTON
  if (digitalRead(buttonPet) == LOW) {
  petCounter++;

  if (petCounter >= PET_THRESHOLD) {
    drawAngelNumber();
    delay(3000);            // show blessing
    petCounter = 0; 
    drawAngel();
  }

  delay(250);
}

}

// STATE UPDATE
void updateState() {
  // If prayed but too much time passed → rude
  if (angelState == ANGEL_POLITE &&
      millis() - lastPrayTime > PRAY_TIMEOUT) {
    angelState = ANGEL_ANNOYED;
    cute.play(S_OHOOH); // annoyed sound
    drawAngel();
  }
}

void updateAngelFloat() {
  if (millis() - lastFloatUpdate > floatInterval) {
    lastFloatUpdate = millis();

    angelOffsetY += floatDirection;

    if (angelOffsetY >= 1 || angelOffsetY <= -1) {
      floatDirection *= -1;
    }
  }
}


// check tilt state
void checkTilt() {
  tiltState = digitalRead(tiltPin);

  if (mode == MODE_WAIT_SHAKE &&
      lastTiltState == HIGH &&
      tiltState == LOW) {

    showingAskPrompt = false;
    angelState = previousState;
    handleAsk();               // show answer
  }

  lastTiltState = tiltState;
}

// ASK LOGIC
void handleAsk() {

  mode = MODE_SHOW_ANSWER;
  modeStartTime = millis();

  display.clearDisplay();
  cute.play(S_BUTTON_PUSHED);

  // draw angel (not surprised anymore)
  drawAngel();

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(10, 55);

  if (angelState == ANGEL_POLITE) {
    display.print(prayedAnswers[random(0,4)]);
  }
  else if (angelState == ANGEL_ANNOYED) {
    display.print(rudeAnswers[random(0,4)]);
  }
  else {
    display.print(neutralAnswers[random(0,4)]);
  }

  display.display();
}

// DRAWING FUNCTIONS
void drawAngel() {
  display.clearDisplay();

  int y = angelBaseY + angelOffsetY;

  // Draw angel with different faces
  switch (angelState) {
    case ANGEL_NEUTRAL:
      display.drawBitmap(0, y, neutralAngel, 128, 64, SSD1306_WHITE);
      break;

    case ANGEL_POLITE:
      display.drawBitmap(0, y, politeAngel, 128, 64, SSD1306_WHITE);
      break;

    case ANGEL_ANNOYED:
      display.drawBitmap(0, y, annoyedAngel, 128, 64, SSD1306_WHITE);
      break;

    case ANGEL_SURPRISED:
      display.drawBitmap(0, y, surprisedAngel, 128, 64, SSD1306_WHITE);
      break;
  }

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
  soundEffect(buzzerPin);

  display.display();

  // Reset after blessing
  petCounter = 0;

}

// Sound Effect Angel Number
void soundEffect(int buzzer)  {

  #define NOTE_C4  262
  #define NOTE_E4  330
  #define NOTE_F4  349
  #define NOTE_C5  523
  #define REST      0


  // change this to make the song slower or faster
  int tempo = 200;

  int melody[] = {
    NOTE_F4,8, NOTE_E4,8, NOTE_C4,4, NOTE_F4,4,
    NOTE_C5,-2, 
    
  };

  // sizeof gives the number of bytes, each int value is composed of two bytes (16 bits)
  // there are two values per note (pitch and duration), so for each note there are four bytes
  int notes = sizeof(melody) / sizeof(melody[0]) / 2;

  // this calculates the duration of a whole note in ms
  int wholenote = (60000 * 4) / tempo;
  int divider = 0, noteDuration = 0;

  // iterate over the notes of the melody. 
  // Remember, the array is twice the number of notes (notes + durations)
  for (int thisNote = 0; thisNote < notes * 2; thisNote = thisNote + 2) {

    // calculates the duration of each note
    divider = melody[thisNote + 1];
    if (divider > 0) {
      // regular note, just proceed
      noteDuration = (wholenote) / divider;
    } else if (divider < 0) {
      // dotted notes are represented with negative durations!!
      noteDuration = (wholenote) / abs(divider);
      noteDuration *= 1.5; // increases the duration in half for dotted notes
    }

    // we only play the note for 90% of the duration, leaving 10% as a pause
    tone(buzzer, melody[thisNote], noteDuration*0.9);

    // Wait for the specief duration before playing the next note.
    delay(noteDuration);
    
    // stop the waveform generation before the next note.
    noTone(buzzer);
  }

}
