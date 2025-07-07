// File: src/wio-terminal/examples/menu_system.ino

#include "TFT_eSPI.h"

TFT_eSPI tft;
#define TFT_GRAY 0x7BEF  // Gris personalizado
// Menu system variables
int currentMenu = 0;
const int maxMenus = 4;
String menuItems[] = {"Sensor Status", "System Info", "Settings", "Exit"};

void setup() {
  Serial.begin(115200);
  tft.begin();
  tft.setRotation(3);
  tft.fillScreen(TFT_BLACK);
  
  // Initialize buttons
  pinMode(WIO_KEY_A, INPUT_PULLUP);
  pinMode(WIO_KEY_B, INPUT_PULLUP);
  pinMode(WIO_5S_UP, INPUT_PULLUP);
  pinMode(WIO_5S_DOWN, INPUT_PULLUP);
  pinMode(WIO_5S_PRESS, INPUT_PULLUP);
  
  displayMenu();
}

void loop() {
  static bool lastButtonUp = HIGH;
  static bool lastButtonDown = HIGH;
  static bool lastButtonPress = HIGH;
  static bool lastButtonA = HIGH;
  
  bool currentButtonUp = digitalRead(WIO_5S_UP);
  bool currentButtonDown = digitalRead(WIO_5S_DOWN);
  bool currentButtonPress = digitalRead(WIO_5S_PRESS);
  bool currentButtonA = digitalRead(WIO_KEY_A);
  
  // Navigate up
  if (lastButtonUp == HIGH && currentButtonUp == LOW) {
    currentMenu = (currentMenu - 1 + maxMenus) % maxMenus;
    displayMenu();
  }
  
  // Navigate down
  if (lastButtonDown == HIGH && currentButtonDown == LOW) {
    currentMenu = (currentMenu + 1) % maxMenus;
    displayMenu();
  }
  
  // Select menu item
  if (lastButtonPress == HIGH && currentButtonPress == LOW) {
    selectMenuItem(currentMenu);
  }
  
  // Back to menu
  if (lastButtonA == HIGH && currentButtonA == LOW) {
    displayMenu();
  }
  
  lastButtonUp = currentButtonUp;
  lastButtonDown = currentButtonDown;
  lastButtonPress = currentButtonPress;
  lastButtonA = currentButtonA;
  
  delay(50);
}

void displayMenu() {
  tft.fillScreen(TFT_BLACK);
  
  // Header
  tft.setTextColor(TFT_GREEN);
  tft.setTextSize(2);
  tft.drawString("EcoTracker", 50, 10);
  
  tft.setTextColor(TFT_CYAN);
  tft.setTextSize(1);
  tft.drawString("Main Menu", 20, 40);
  
  // Menu items
  for (int i = 0; i < maxMenus; i++) {
    if (i == currentMenu) {
      tft.setTextColor(TFT_YELLOW);
      tft.drawString("> " + menuItems[i], 20, 70 + (i * 20));
    } else {
      tft.setTextColor(TFT_WHITE);
      tft.drawString("  " + menuItems[i], 20, 70 + (i * 20));
    }
  }
  
  // Instructions
  tft.setTextColor(TFT_GRAY);
  tft.setTextSize(1);
  tft.drawString("UP/DOWN: Navigate", 20, 180);
  tft.drawString("PRESS: Select", 20, 200);
  tft.drawString("A: Back", 20, 220);
}

void selectMenuItem(int menuIndex) {
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_GREEN);
  tft.setTextSize(2);
  tft.drawString("EcoTracker", 50, 10);
  
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(1);
  
  switch(menuIndex) {
    case 0: // Sensor Status
      tft.drawString("Sensor Status", 20, 50);
      tft.drawString("Air Quality: Not Connected", 20, 80);
      tft.drawString("GPS: Not Connected", 20, 100);
      tft.drawString("IMU: Connected", 20, 120);
      break;
      
    case 1: // System Info
      tft.drawString("System Information", 20, 50);
      tft.drawString("Device: Wio Terminal", 20, 80);
      tft.drawString("Version: 1.0.0", 20, 100);
      tft.drawString("Memory: Available", 20, 120);
      break;
      
    case 2: // Settings
      tft.drawString("Settings", 20, 50);
      tft.drawString("WiFi: Not Configured", 20, 80);
      tft.drawString("Sample Rate: 30s", 20, 100);
      tft.drawString("Display: Auto", 20, 120);
      break;
      
    case 3: // Exit
      tft.drawString("Goodbye!", 20, 50);
      delay(1000);
      displayMenu();
      return;
  }
  
  tft.setTextColor(TFT_GRAY);
  tft.drawString("Press A to return", 20, 200);
}