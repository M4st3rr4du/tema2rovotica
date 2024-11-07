#include <Arduino.h>

const int ledRed = 6;
const int ledGreen = 5;
const int ledBlue = 4;
const int buttonStart = 3;
const int buttonLevel = 2;

char* words[] = {
    (char*)"apple", (char*)"banana", (char*)"cherry", (char*)"date", 
    (char*)"elephant", (char*)"fig", (char*)"grape", (char*)"honey", 
    (char*)"igloo", (char*)"jungle", (char*)"kite", (char*)"lemon", 
    (char*)"mango", (char*)"nest", (char*)"orange", (char*)"plum", 
    (char*)"quartz", (char*)"river", (char*)"star", (char*)"tree", 
    (char*)"unicorn", (char*)"violet", (char*)"water", (char*)"xylophone", 
    (char*)"yellow", (char*)"zebra"
};

void changeLevel();
void startGameInterrupt();
void countdown();
void typeWords();

unsigned int levelIndex = 0;
unsigned long lastButtonPress = 0;
const int debounceTime = 50;
volatile bool startFlag = false;
unsigned long gameStartTime = 0;
unsigned long maxTime = 5000;
unsigned int score = 0;
bool isPlaying = false;

void setup() {
  Serial.begin(9600);
  pinMode(ledRed, OUTPUT);
  pinMode(ledGreen, OUTPUT);
  pinMode(ledBlue, OUTPUT);
  pinMode(buttonStart, INPUT_PULLUP);
  pinMode(buttonLevel, INPUT_PULLUP);
  digitalWrite(ledRed, HIGH);
  digitalWrite(ledGreen, HIGH);
  digitalWrite(ledBlue, HIGH);
  attachInterrupt(digitalPinToInterrupt(buttonStart), startGameInterrupt, FALLING);
  attachInterrupt(digitalPinToInterrupt(buttonLevel), changeLevel, FALLING);
}

void changeLevel() {
  if (millis() - lastButtonPress > debounceTime) {
    lastButtonPress = millis();
    levelIndex = (levelIndex + 1) % 3;

    if (levelIndex == 0) {
      maxTime = 5000;
      Serial.println("Easy mode selected!");
    } else if (levelIndex == 1) {
      maxTime = 3000;
      Serial.println("Medium mode selected!");
    } else {
      maxTime = 2000;
      Serial.println("Hard mode selected!");
    }
  }
}

void startGameInterrupt() {
  if (isPlaying) {
    isPlaying = false; 
    Serial.println("Game stopped.");
    digitalWrite(ledRed, HIGH);
    digitalWrite(ledGreen, HIGH);
    digitalWrite(ledBlue, HIGH);
    score = 0; 
  } else {
    startFlag = true; 
  }
}

void countdown() {
  unsigned long currentMillis = millis();
  static unsigned long lastBlink = 0;
  static int counter = 3;
  static bool ledStatus = false;
  if (currentMillis - lastBlink >= 1000 && counter > 0) {
    lastBlink = currentMillis;
    Serial.println(counter);
    counter--;
    ledStatus = !ledStatus;
    digitalWrite(ledRed, ledStatus);
    digitalWrite(ledGreen, ledStatus);
    digitalWrite(ledBlue, ledStatus);
  } else if (counter == 0) {
    digitalWrite(ledRed, HIGH);
    digitalWrite(ledGreen, HIGH);
    digitalWrite(ledBlue, HIGH);
    counter = 3;
    gameStartTime = millis();
    isPlaying = true; 
  }
}

void typeWords() {
  while (millis() - gameStartTime < 30000 && isPlaying) {
    int randomIndex = random(0, 25); 
    char* currentWord = words[randomIndex];

    Serial.print("Type the word: ");
    Serial.println(currentWord);

    char typedWord[20] = ""; 
    int i = 0;
    bool correct = true;
    unsigned long wordStartTime = millis();

    digitalWrite(ledRed, LOW);
    digitalWrite(ledGreen, LOW);

    while (currentWord[i] != '\0' && (millis() - wordStartTime) < maxTime && isPlaying) {
      if (Serial.available() > 0) {
        char typedChar = Serial.read();

        if (typedChar == '\b') { 
          if (i > 0) {
            i--; 
            typedWord[i] = '\0'; 
            Serial.print("\b \b");
          }
        } else if (typedChar == currentWord[i]) {
          typedWord[i] = typedChar; 
          digitalWrite(ledRed, LOW);
          digitalWrite(ledGreen, HIGH);
          Serial.print(typedChar);
          i++;
        } else {
          digitalWrite(ledRed, HIGH);
          digitalWrite(ledGreen, LOW);
          correct = false;
        }
      }
    }

    if (correct && currentWord[i] == '\0') {
      score++;
      Serial.println("\nCorrect word!");
    } else {
      Serial.println("\nIncorrect word or time ran out!");
      score--;
    }
  }
  if(score != 0 && score != 65533){
    Serial.print("Final score: ");
    Serial.println(score);
    score = 0;
  }
  isPlaying = false; 
}

void loop() {
  if (startFlag) {
    startFlag = false;
    countdown();
    typeWords();
  }
}
