# Phase 1 - Wio Terminal Setup Guide

## Overview

This phase focuses on setting up your development environment and getting familiar with the Wio Terminal. You'll configure Arduino IDE, test basic functionality, and create your first simple applications.

**Duration:** 1-2 weeks  
**Goal:** Master Wio Terminal basics and create foundation for sensor integration

## Prerequisites

- Wio Terminal device
- USB-C cable
- Computer with Arduino IDE installed
- Basic understanding of C++ programming

## Step 1: Arduino IDE Configuration

### 1.1 Install Board Package

1. Open Arduino IDE
2. Go to **File > Preferences**
3. Add the following URL to **Additional Boards Manager URLs**:
   ```
   https://files.seeedstudio.com/arduino/package_seeeduino_boards_index.json
   ```
4. Go to **Tools > Board > Boards Manager**
5. Search for "Seeed SAMD" and install **Seeed SAMD Boards**
6. Select **Tools > Board > Seeed SAMD Boards > Wio Terminal**

### 1.2 Install Required Libraries

Install these libraries via **Tools > Manage Libraries**:

```
- Grove - LCD RGB Backlight (for LCD display)
- TFT_eSPI (for Wio Terminal screen)
- Seeed Arduino FS (for file system)
- ArduinoJson (for JSON handling)
- Seeed Arduino LIS3DHTR (for IMU sensor)
- WiFi (built-in)
- HTTPClient (built-in)
```

### 1.3 Configure Board Settings

- **Board:** Wio Terminal
- **Port:** Select your Wio Terminal's COM port
- **Upload Speed:** 921600

## Step 2: Hello World Test

### 2.1 Basic Display Test

Create your first sketch to test the LCD display:

```cpp
// File: src/wio-terminal/examples/hello_world.ino

#include "TFT_eSPI.h"

TFT_eSPI tft;

void setup() {
  Serial.begin(115200);
  tft.begin();
  tft.setRotation(3);
  tft.fillScreen(TFT_BLACK);
  
  // Display welcome message
  tft.setTextColor(TFT_GREEN);
  tft.setTextSize(2);
  tft.drawString("EcoTracker", 50, 50);
  
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(1);
  tft.drawString("Phase 1: Setup Complete", 20, 100);
  
  Serial.println("Hello World - Display Test Complete");
}

void loop() {
  // Empty loop for now
}
```

### 2.2 Upload and Test

1. Connect Wio Terminal via USB-C
2. Upload the sketch
3. Verify the display shows "EcoTracker"
4. Check Serial Monitor for "Hello World" message

## Step 3: Button and Input Testing

### 3.1 Button Test Sketch

Test the built-in buttons:

```cpp
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
```

## Step 4: Built-in Sensors Testing

### 4.1 IMU Sensor Test

Test the built-in accelerometer and gyroscope:

```cpp
// File: src/wio-terminal/examples/imu_test.ino

#include "TFT_eSPI.h"
#include "LIS3DHTR.h"

TFT_eSPI tft;
LIS3DHTR<TwoWire> lis;

void setup() {
  Serial.begin(115200);
  tft.begin();
  tft.setRotation(3);
  tft.fillScreen(TFT_BLACK);
  
  // Initialize IMU
  lis.begin(Wire1);
  if (!lis) {
    Serial.println("ERROR: IMU not detected");
    tft.setTextColor(TFT_RED);
    tft.drawString("IMU Error", 20, 100);
    while(1);
  }
  
  lis.setOutputDataRate(LIS3DHTR_DATARATE_25HZ);
  lis.setFullScaleRange(LIS3DHTR_RANGE_2G);
  
  Serial.println("IMU initialized successfully");
  displayIMUHeader();
}

void loop() {
  // Read accelerometer data
  float x = lis.getAccelerationX();
  float y = lis.getAccelerationY();
  float z = lis.getAccelerationZ();
  
  // Update display
  displayIMUData(x, y, z);
  
  // Print to Serial
  Serial.print("X: "); Serial.print(x);
  Serial.print(" Y: "); Serial.print(y);
  Serial.print(" Z: "); Serial.println(z);
  
  delay(500);
}

void displayIMUHeader() {
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_GREEN);
  tft.setTextSize(2);
  tft.drawString("EcoTracker", 50, 10);
  
  tft.setTextColor(TFT_CYAN);
  tft.setTextSize(1);
  tft.drawString("IMU Sensor Test", 20, 40);
  tft.drawString("Accelerometer (G)", 20, 60);
}

void displayIMUData(float x, float y, float z) {
  // Clear data area
  tft.fillRect(0, 80, 320, 160, TFT_BLACK);
  
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(1);
  
  // Display X axis
  tft.drawString("X: " + String(x, 2), 20, 90);
  
  // Display Y axis
  tft.drawString("Y: " + String(y, 2), 20, 110);
  
  // Display Z axis
  tft.drawString("Z: " + String(z, 2), 20, 130);
  
  // Simple orientation indicator
  if (abs(x) > 0.5) {
    tft.setTextColor(TFT_YELLOW);
    tft.drawString("Tilted X", 20, 160);
  } else if (abs(y) > 0.5) {
    tft.setTextColor(TFT_YELLOW);
    tft.drawString("Tilted Y", 20, 160);
  } else {
    tft.setTextColor(TFT_GREEN);
    tft.drawString("Level", 20, 160);
  }
}
```

## Step 5: Basic Menu System

### 5.1 Simple Menu Interface

Create a basic menu system for navigation:

```cpp
// File: src/wio-terminal/examples/menu_system.ino

#include "TFT_eSPI.h"

TFT_eSPI tft;

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
```

## Learning Objectives Checklist

By the end of Phase 1, you should be able to:

- [ ] Configure Arduino IDE for Wio Terminal
- [ ] Upload and run basic sketches
- [ ] Use the LCD display effectively
- [ ] Read button inputs and handle events
- [ ] Access built-in sensors (IMU)
- [ ] Create simple menu systems
- [ ] Debug using Serial Monitor
- [ ] Understand Wio Terminal pin layout

## Troubleshooting

### Common Issues

1. **Board not detected:**
   - Check USB cable connection
   - Try different USB port
   - Restart Arduino IDE

2. **Upload fails:**
   - Press reset button twice quickly
   - Check board and port settings
   - Verify correct drivers installed

3. **Display not working:**
   - Check rotation settings
   - Verify TFT_eSPI library installed
   - Try different display initialization

4. **Buttons not responding:**
   - Check pin definitions
   - Verify INPUT_PULLUP mode
   - Add debouncing delays

## Next Steps

Once you've completed all tests successfully:

1. **Document your learnings** in a personal notes file
2. **Commit your example code** to the repository
3. **Move to Phase 2** - Sensor Integration
4. **Plan your sensor connections** based on Wio Terminal pinout

## Files to Commit

```
src/wio-terminal/examples/
├── hello_world.ino
├── button_test.ino
├── imu_test.ino
└── menu_system.ino
```

Commit message: `Phase 1 complete: Wio Terminal basic functionality tests`

---

**Phase 1 Status:** ✅ Complete  
**Next Phase:** [Phase 2 - Sensor Integration](phase-2-sensors.md)  
**Estimated Time:** 1-2 weeks