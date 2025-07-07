// File: src/wio-terminal/examples/button_test.ino

#include "TFT_eSPI.h"

TFT_eSPI tft;

// Button pins
#define BUTTON_A WIO_KEY_A
#define BUTTON_B WIO_KEY_B  
#define BUTTON_C WIO_KEY_C
#define BUTTON_UP WIO_5S_UP
#define BUTTON_DOWN WIO_5S_DOWN
#define BUTTON_LEFT WIO_5S_LEFT
#define BUTTON_RIGHT WIO_5S_RIGHT
#define BUTTON_PRESS WIO_5S_PRESS

void setup() {
  Serial.begin(115200);
  tft.begin();
  tft.setRotation(3);
  tft.fillScreen(TFT_BLACK);
  
  // Initialize buttons
  pinMode(BUTTON_A, INPUT_PULLUP);
  pinMode(BUTTON_B, INPUT_PULLUP);
  pinMode(BUTTON_C, INPUT_PULLUP);
  pinMode(BUTTON_UP, INPUT_PULLUP);
  pinMode(BUTTON_DOWN, INPUT_PULLUP);
  pinMode(BUTTON_LEFT, INPUT_PULLUP);
  pinMode(BUTTON_RIGHT, INPUT_PULLUP);
  pinMode(BUTTON_PRESS, INPUT_PULLUP);
  
  displayButtonStatus();
}

void loop() {
  static bool lastButtonState[8] = {HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH};
  bool currentButtonState[8];
  
  // Read all buttons
  currentButtonState[0] = digitalRead(BUTTON_A);
  currentButtonState[1] = digitalRead(BUTTON_B);
  currentButtonState[2] = digitalRead(BUTTON_C);
  currentButtonState[3] = digitalRead(BUTTON_UP);
  currentButtonState[4] = digitalRead(BUTTON_DOWN);
  currentButtonState[5] = digitalRead(BUTTON_LEFT);
  currentButtonState[6] = digitalRead(BUTTON_RIGHT);
  currentButtonState[7] = digitalRead(BUTTON_PRESS);
  
  // Check for button presses
  for (int i = 0; i < 8; i++) {
    if (lastButtonState[i] == HIGH && currentButtonState[i] == LOW) {
      handleButtonPress(i);
      lastButtonState[i] = LOW;
    } else if (lastButtonState[i] == LOW && currentButtonState[i] == HIGH) {
      lastButtonState[i] = HIGH;
    }
  }
  
  delay(50);
}

void handleButtonPress(int buttonIndex) {
  String buttonNames[] = {"A", "B", "C", "UP", "DOWN", "LEFT", "RIGHT", "PRESS"};
  
  Serial.println("Button pressed: " + buttonNames[buttonIndex]);
  
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_YELLOW);
  tft.setTextSize(2);
  tft.drawString("Button: " + buttonNames[buttonIndex], 20, 100);
  
  delay(500);
  displayButtonStatus();
}

void displayButtonStatus() {
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_GREEN);
  tft.setTextSize(2);
  tft.drawString("EcoTracker", 50, 20);
  
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(1);
  tft.drawString("Press any button to test", 20, 60);
  tft.drawString("A, B, C or 5-way switch", 20, 80);
  tft.drawString("Check Serial Monitor", 20, 120);
}