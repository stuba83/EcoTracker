# Phase 2 - Sensor Integration Guide

## Overview

This phase focuses on integrating external sensors (Air Quality and GPS) with your Wio Terminal. You'll learn to connect hardware, read sensor data, and create a comprehensive environmental monitoring dashboard.

**Duration:** 2-3 weeks  
**Goal:** Integrate air quality sensor and GPS module to create a functional environmental monitoring device

## Prerequisites

- Completed Phase 1 successfully
- Air Quality Sensor module
- GPS Module
- Jumper wires (male-to-female and male-to-male)
- Breadboard (recommended)
- Basic understanding of sensor connections

## Hardware Connections

### Wio Terminal Pinout Reference

```
Wio Terminal Grove Connectors:
├── Grove A (I2C): SDA=D20, SCL=D21, 3.3V, GND
├── Grove B (I2C): SDA=D20, SCL=D21, 3.3V, GND  
├── Grove C (UART): TX=D0, RX=D1, 3.3V, GND
└── Grove D (Digital): D3, D5, 3.3V, GND

GPIO Pins Available:
├── Digital: D0-D8, D10, D13, D16-D19, D20-D21
├── Analog: A0-A8
├── PWM: D0, D1, D3, D5, D6, D9, D10, D12, D13
└── Power: 3.3V, 5V, GND
```

## Step 1: Air Quality Sensor Integration

### 1.1 Sensor Connection

**For Grove Air Quality Sensor (or similar I2C sensor):**

```
Air Quality Sensor → Wio Terminal
├── VCC (Red)    → 3.3V
├── GND (Black)  → GND
├── SDA (White)  → D20 (Grove A/B)
└── SCL (Yellow) → D21 (Grove A/B)
```

**For Analog Air Quality Sensor (MQ series):**

```
MQ Air Sensor → Wio Terminal
├── VCC → 3.3V
├── GND → GND
└── AO  → A0 (Analog Pin)
```

### 1.2 Air Quality Sensor Code

For the Grove Air Quality Sensor v1.3 (analog sensor), we'll use a simple .ino approach:

### 1.3 Air Quality Test Sketch

```cpp
// File: src/wio-terminal/examples/air_quality_test.ino

#include "TFT_eSPI.h"

TFT_eSPI tft;

// Air Quality Sensor Configuration
const int AIR_QUALITY_PIN = A0;

void setup() {
    Serial.begin(115200);
    tft.begin();
    tft.setRotation(3);
    tft.fillScreen(TFT_BLACK);
    
    // Configure analog pin
    pinMode(AIR_QUALITY_PIN, INPUT);
    
    Serial.println("Grove Air Quality Sensor v1.3 - Test");
    displayHeader();
}

void loop() {
    // Read sensor data
    int rawValue = analogRead(AIR_QUALITY_PIN);
    float voltage = (rawValue / 1024.0) * 3.3;
    
    // Convert to air quality values
    float aqi = map(rawValue, 0, 1024, 0, 500);
    float pm25 = aqi * 0.5;
    float pm10 = aqi * 0.8;
    String level = getAirQualityLevel(aqi);
    
    // Update display
    displayAirQualityData(rawValue, aqi, pm25, pm10, level, voltage);
    
    // Print to Serial
    Serial.print("Raw: "); Serial.print(rawValue);
    Serial.print(" | Voltage: "); Serial.print(voltage, 2);
    Serial.print(" | AQI: "); Serial.print(aqi, 0);
    Serial.print(" | Level: "); Serial.println(level);
    
    delay(2000);
}

String getAirQualityLevel(float aqi) {
    if (aqi <= 50) return "Good";
    else if (aqi <= 100) return "Moderate";
    else if (aqi <= 150) return "Unhealthy for Sensitive";
    else if (aqi <= 200) return "Unhealthy";
    else if (aqi <= 300) return "Very Unhealthy";
    else return "Hazardous";
}

void displayHeader() {
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_GREEN);
    tft.setTextSize(2);
    tft.drawString("EcoTracker", 50, 10);
    
    tft.setTextColor(TFT_CYAN);
    tft.setTextSize(1);
    tft.drawString("Air Quality Monitor", 20, 40);
}

void displayAirQualityData(int raw, float aqi, float pm25, float pm10, String level, float voltage) {
    // Clear data area
    tft.fillRect(0, 60, 320, 180, TFT_BLACK);
    
    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(1);
    
    // Raw sensor value
    tft.drawString("Raw Value: " + String(raw), 20, 70);
    tft.drawString("Voltage: " + String(voltage, 2) + "V", 20, 90);
    
    // AQI
    tft.drawString("Air Quality Index:", 20, 120);
    tft.setTextColor(getAQIColor(aqi));
    tft.setTextSize(2);
    tft.drawString(String(aqi, 0), 20, 140);
    
    // Level
    tft.setTextColor(TFT_YELLOW);
    tft.setTextSize(1);
    tft.drawString("Level: " + level, 20, 170);
    
    // PM values (estimated)
    tft.setTextColor(TFT_WHITE);
    tft.drawString("PM2.5: " + String(pm25, 1) + " μg/m³", 20, 190);
    tft.drawString("PM10:  " + String(pm10, 1) + " μg/m³", 20, 210);
    
    // Status indicator circle
    tft.fillCircle(280, 120, 20, getAQIColor(aqi));
}

uint16_t getAQIColor(float aqi) {
    if (aqi <= 50) return TFT_GREEN;
    else if (aqi <= 100) return TFT_YELLOW;
    else if (aqi <= 150) return 0xFD20; // Orange
    else if (aqi <= 200) return TFT_RED;
    else return TFT_MAGENTA;
}
```

## Step 2: GPS Module Integration

### 2.1 GPS Connection

**For Grove GPS Module or UART GPS:**

```
GPS Module → Wio Terminal
├── VCC (Red)    → 3.3V or 5V
├── GND (Black)  → GND
├── TX (White)   → D1 (RX - Grove C)
└── RX (Yellow)  → D0 (TX - Grove C)
```

### 2.2 GPS Handler Code

For GPS processing, we'll create simple functions within the .ino file:

### 2.3 GPS Test Sketch

```cpp
// File: src/wio-terminal/examples/gps_test.ino

#include "TFT_eSPI.h"

TFT_eSPI tft;

// GPS Data Structure
struct GPSData {
    float latitude;
    float longitude;
    int satellites;
    bool isValid;
    String timestamp;
};

GPSData currentGPS;

void setup() {
    Serial.begin(115200);
    Serial1.begin(9600); // GPS Module on Serial1 (Grove C)
    
    tft.begin();
    tft.setRotation(3);
    tft.fillScreen(TFT_BLACK);
    
    // Initialize GPS data
    currentGPS.isValid = false;
    
    Serial.println("GPS Module Test - Move outdoors for signal");
    displayHeader();
}

void loop() {
    // Update GPS data
    if (updateGPS()) {
        Serial.println("GPS data updated");
    }
    
    // Update display
    displayGPSData(currentGPS);
    
    // Print to Serial
    if (currentGPS.isValid) {
        Serial.print("GPS: ");
        Serial.print(currentGPS.latitude, 6);
        Serial.print(", ");
        Serial.print(currentGPS.longitude, 6);
        Serial.print(" | Satellites: ");
        Serial.println(currentGPS.satellites);
    } else {
        Serial.println("GPS: Waiting for fix...");
    }
    
    delay(1000);
}

bool updateGPS() {
    if (Serial1.available()) {
        String sentence = Serial1.readStringUntil('\n');
        sentence.trim();
        
        if (sentence.startsWith("$GPGGA") || sentence.startsWith("$GNGGA")) {
            return parseNMEA(sentence);
        }
    }
    return false;
}

bool parseNMEA(String sentence) {
    // Simple NMEA parsing for GPGGA sentences
    int commaIndex[14];
    int commaCount = 0;
    
    // Find all comma positions
    for (int i = 0; i < sentence.length() && commaCount < 14; i++) {
        if (sentence.charAt(i) == ',') {
            commaIndex[commaCount] = i;
            commaCount++;
        }
    }
    
    if (commaCount < 6) return false;
    
    // Extract data
    String timeStr = sentence.substring(commaIndex[0] + 1, commaIndex[1]);
    String latStr = sentence.substring(commaIndex[1] + 1, commaIndex[2]);
    String latDir = sentence.substring(commaIndex[2] + 1, commaIndex[3]);
    String lonStr = sentence.substring(commaIndex[3] + 1, commaIndex[4]);
    String lonDir = sentence.substring(commaIndex[4] + 1, commaIndex[5]);
    String fixQuality = sentence.substring(commaIndex[5] + 1, commaIndex[6]);
    String satCount = sentence.substring(commaIndex[6] + 1, commaIndex[7]);
    
    // Check if we have a valid fix
    if (fixQuality.toInt() == 0) {
        currentGPS.isValid = false;
        return false;
    }
    
    // Convert coordinates
    if (latStr.length() > 0 && lonStr.length() > 0) {
        float lat = latStr.toFloat();
        float lon = lonStr.toFloat();
        
        currentGPS.latitude = convertToDecimal(lat, latDir.charAt(0));
        currentGPS.longitude = convertToDecimal(lon, lonDir.charAt(0));
        currentGPS.satellites = satCount.toInt();
        currentGPS.timestamp = timeStr;
        currentGPS.isValid = true;
        
        return true;
    }
    
    return false;
}

float convertToDecimal(float coordinate, char direction) {
    int degrees = (int)(coordinate / 100);
    float minutes = coordinate - (degrees * 100);
    float decimal = degrees + (minutes / 60.0);
    
    if (direction == 'S' || direction == 'W') {
        decimal = -decimal;
    }
    
    return decimal;
}

void displayHeader() {
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_GREEN);
    tft.setTextSize(2);
    tft.drawString("EcoTracker", 50, 10);
    
    tft.setTextColor(TFT_CYAN);
    tft.setTextSize(1);
    tft.drawString("GPS Location Monitor", 20, 40);
}

void displayGPSData(GPSData data) {
    // Clear data area
    tft.fillRect(0, 60, 320, 180, TFT_BLACK);
    
    if (data.isValid) {
        // Valid GPS data
        tft.setTextColor(TFT_GREEN);
        tft.setTextSize(1);
        tft.drawString("GPS Status: FIXED", 20, 70);
        
        tft.setTextColor(TFT_WHITE);
        tft.drawString("Latitude:", 20, 100);
        tft.drawString(String(data.latitude, 6), 20, 120);
        
        tft.drawString("Longitude:", 20, 150);
        tft.drawString(String(data.longitude, 6), 20, 170);
        
        tft.setTextColor(TFT_YELLOW);
        tft.drawString("Satellites: " + String(data.satellites), 20, 200);
        
        // GPS indicator
        tft.fillCircle(280, 100, 15, TFT_GREEN);
        
    } else {
        // No GPS fix
        tft.setTextColor(TFT_RED);
        tft.setTextSize(1);
        tft.drawString("GPS Status: SEARCHING", 20, 70);
        
        tft.setTextColor(TFT_WHITE);
        tft.drawString("Waiting for satellites...", 20, 100);
        tft.drawString("Please move outdoors", 20, 120);
        tft.drawString("for better signal", 20, 140);
        
        // Blinking indicator
        static bool blink = false;
        static unsigned long lastBlink = 0;
        if (millis() - lastBlink > 500) {
            blink = !blink;
            lastBlink = millis();
        }
        
        if (blink) {
            tft.fillCircle(280, 100, 15, TFT_RED);
        }
    }
}
```

## Step 3: Combined Environmental Monitor

### 3.1 Main Application Code

```cpp
// File: src/wio-terminal/main/main.ino

#include "TFT_eSPI.h"
#include "air_quality_sensor.h"
#include "gps_module.h"

TFT_eSPI tft;
AirQualitySensor airSensor(A0, false);
GPSModule gps(&Serial1);

// Menu system
int currentScreen = 0; // 0=main, 1=air quality, 2=gps, 3=settings
const int maxScreens = 4;

struct EnvironmentalData {
    float aqi;
    float pm25;
    float pm10;
    String airQualityLevel;
    GPSData location;
    unsigned long timestamp;
};

EnvironmentalData currentReading;

void setup() {
    Serial.begin(115200);
    delay(2000);
    
    // Initialize display
    tft.begin();
    tft.setRotation(3);
    tft.fillScreen(TFT_BLACK);
    
    // Initialize sensors
    Serial.println("Initializing sensors...");
    
    if (airSensor.begin()) {
        Serial.println("✓ Air Quality Sensor initialized");
    } else {
        Serial.println("✗ Air Quality Sensor failed");
    }
    
    if (gps.begin(9600)) {
        Serial.println("✓ GPS Module initialized");
    } else {
        Serial.println("✗ GPS Module failed");
    }
    
    // Initialize buttons
    pinMode(WIO_KEY_A, INPUT_PULLUP);
    pinMode(WIO_KEY_B, INPUT_PULLUP);
    pinMode(WIO_KEY_C, INPUT_PULLUP);
    pinMode(WIO_5S_UP, INPUT_PULLUP);
    pinMode(WIO_5S_DOWN, INPUT_PULLUP);
    pinMode(WIO_5S_PRESS, INPUT_PULLUP);
    
    Serial.println("EcoTracker Phase 2 - Ready!");
    displayCurrentScreen();
}

void loop() {
    // Update sensors
    updateSensors();
    
    // Handle button input
    handleButtons();
    
    // Update display every 2 seconds
    static unsigned long lastUpdate = 0;
    if (millis() - lastUpdate > 2000) {
        displayCurrentScreen();
        lastUpdate = millis();
    }
    
    delay(100);
}

void updateSensors() {
    // Update air quality
    currentReading.aqi = airSensor.readAQI();
    currentReading.pm25 = airSensor.readPM25();
    currentReading.pm10 = airSensor.readPM10();
    currentReading.airQualityLevel = airSensor.getAirQualityLevel(currentReading.aqi);
    
    // Update GPS
    gps.update();
    currentReading.location = gps.getData();
    currentReading.timestamp = millis();
}

void handleButtons() {
    static bool lastButtonUp = HIGH;
    static bool lastButtonDown = HIGH;
    static bool lastButtonPress = HIGH;
    static bool lastButtonA = HIGH;
    static bool lastButtonB = HIGH;
    static bool lastButtonC = HIGH;
    
    bool currentButtonUp = digitalRead(WIO_5S_UP);
    bool currentButtonDown = digitalRead(WIO_5S_DOWN);
    bool currentButtonPress = digitalRead(WIO_5S_PRESS);
    bool currentButtonA = digitalRead(WIO_KEY_A);
    bool currentButtonB = digitalRead(WIO_KEY_B);
    bool currentButtonC = digitalRead(WIO_KEY_C);
    
    // Navigate screens
    if (lastButtonUp == HIGH && currentButtonUp == LOW) {
        currentScreen = (currentScreen - 1 + maxScreens) % maxScreens;
        displayCurrentScreen();
    }
    
    if (lastButtonDown == HIGH && currentButtonDown == LOW) {
        currentScreen = (currentScreen + 1) % maxScreens;
        displayCurrentScreen();
    }
    
    // Action buttons
    if (lastButtonA == HIGH && currentButtonA == LOW) {
        saveCurrentReading();
    }
    
    if (lastButtonB == HIGH && currentButtonB == LOW) {
        toggleDataLogging();
    }
    
    if (lastButtonC == HIGH && currentButtonC == LOW) {
        resetData();
    }
    
    // Update button states
    lastButtonUp = currentButtonUp;
    lastButtonDown = currentButtonDown;
    lastButtonPress = currentButtonPress;
    lastButtonA = currentButtonA;
    lastButtonB = currentButtonB;
    lastButtonC = currentButtonC;
}

void displayCurrentScreen() {
    switch (currentScreen) {
        case 0:
            displayMainScreen();
            break;
        case 1:
            displayAirQualityScreen();
            break;
        case 2:
            displayGPSScreen();
            break;
        case 3:
            displaySettingsScreen();
            break;
    }
}

void displayMainScreen() {
    tft.fillScreen(TFT_BLACK);
    
    // Header
    tft.setTextColor(TFT_GREEN);
    tft.setTextSize(2);
    tft.drawString("EcoTracker", 50, 10);
    
    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(1);
    tft.drawString("Environmental Monitor", 20, 40);
    
    // Quick status
    tft.setTextColor(TFT_CYAN);
    tft.drawString("AQI: " + String(currentReading.aqi, 0), 20, 70);
    tft.drawString("Level: " + currentReading.airQualityLevel, 20, 90);
    
    if (currentReading.location.isValid) {
        tft.setTextColor(TFT_GREEN);
        tft.drawString("GPS: Fixed (" + String(currentReading.location.satellites) + " sats)", 20, 110);
        tft.drawString(gps.getLocationString(), 20, 130);
    } else {
        tft.setTextColor(TFT_RED);
        tft.drawString("GPS: Searching...", 20, 110);
    }
    
    // Navigation help
    tft.setTextColor(TFT_GRAY);
    tft.drawString("UP/DOWN: Navigate screens", 20, 180);
    tft.drawString("A:Save B:Log C:Reset", 20, 200);
    
    // Screen indicator
    tft.drawString("Screen: Main (1/4)", 20, 220);
}

void displayAirQualityScreen() {
    tft.fillScreen(TFT_BLACK);
    
    // Header
    tft.setTextColor(TFT_GREEN);
    tft.setTextSize(2);
    tft.drawString("Air Quality", 50, 10);
    
    // AQI with color coding
    tft.setTextColor(getAQIColor(currentReading.aqi));
    tft.setTextSize(3);
    tft.drawString(String(currentReading.aqi, 0), 50, 50);
    
    tft.setTextColor(TFT_YELLOW);
    tft.setTextSize(1);
    tft.drawString(currentReading.airQualityLevel, 20, 100);
    
    // Detailed readings
    tft.setTextColor(TFT_WHITE);
    tft.drawString("PM2.5: " + String(currentReading.pm25, 1) + " μg/m³", 20, 130);
    tft.drawString("PM10:  " + String(currentReading.pm10, 1) + " μg/m³", 20, 150);
    
    // Visual indicator
    tft.fillRect(200, 50, 100, 40, getAQIColor(currentReading.aqi));
    
    tft.setTextColor(TFT_GRAY);
    tft.drawString("Screen: Air Quality (2/4)", 20, 220);
}

void displayGPSScreen() {
    tft.fillScreen(TFT_BLACK);
    
    // Header
    tft.setTextColor(TFT_GREEN);
    tft.setTextSize(2);
    tft.drawString("GPS Location", 30, 10);
    
    if (currentReading.location.isValid) {
        tft.setTextColor(TFT_GREEN);
        tft.setTextSize(1);
        tft.drawString("Status: FIXED", 20, 50);
        
        tft.setTextColor(TFT_WHITE);
        tft.drawString("Latitude:", 20, 80);
        tft.drawString(String(currentReading.location.latitude, 6), 20, 100);
        
        tft.drawString("Longitude:", 20, 130);
        tft.drawString(String(currentReading.location.longitude, 6), 20, 150);
        
        tft.setTextColor(TFT_YELLOW);
        tft.drawString("Satellites: " + String(currentReading.location.satellites), 20, 180);
        
    } else {
        tft.setTextColor(TFT_RED);
        tft.setTextSize(1);
        tft.drawString("Status: SEARCHING", 20, 50);
        tft.setTextColor(TFT_WHITE);
        tft.drawString("Move to open area", 20, 80);
        tft.drawString("for better signal", 20, 100);
    }
    
    tft.setTextColor(TFT_GRAY);
    tft.drawString("Screen: GPS (3/4)", 20, 220);
}

void displaySettingsScreen() {
    tft.fillScreen(TFT_BLACK);
    
    // Header
    tft.setTextColor(TFT_GREEN);
    tft.setTextSize(2);
    tft.drawString("Settings", 70, 10);
    
    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(1);
    tft.drawString("Sample Rate: 2 seconds", 20, 50);
    tft.drawString("Data Logging: Disabled", 20, 70);
    tft.drawString("Alert Threshold: AQI > 100", 20, 90);
    
    tft.setTextColor(TFT_CYAN);
    tft.drawString("System Info:", 20, 120);
    tft.setTextColor(TFT_WHITE);
    tft.drawString("Version: Phase 2", 20, 140);
    tft.drawString("Uptime: " + String(millis() / 1000) + "s", 20, 160);
    
    tft.setTextColor(TFT_GRAY);
    tft.drawString("Screen: Settings (4/4)", 20, 220);
}

uint16_t getAQIColor(float aqi) {
    if (aqi <= 50) return TFT_GREEN;
    else if (aqi <= 100) return TFT_YELLOW;
    else if (aqi <= 150) return TFT_ORANGE;
    else if (aqi <= 200) return TFT_RED;
    else return TFT_PURPLE;
}

void saveCurrentReading() {
    Serial.println("=== SAVED READING ===");
    Serial.print("Timestamp: "); Serial.println(currentReading.timestamp);
    Serial.print("AQI: "); Serial.println(currentReading.aqi);
    Serial.print("PM2.5: "); Serial.println(currentReading.pm25);
    Serial.print("PM10: "); Serial.println(currentReading.pm10);
    Serial.print("Level: "); Serial.println(currentReading.airQualityLevel);
    
    if (currentReading.location.isValid) {
        Serial.print("Location: "); 
        Serial.print(currentReading.location.latitude, 6);
        Serial.print(", ");
        Serial.println(currentReading.location.longitude, 6);
    }
    Serial.println("==================");
}

void toggleDataLogging() {
    static bool logging = false;
    logging = !logging;
    Serial.println(logging ? "Data logging ENABLED" : "Data logging DISABLED");
}

void resetData() {
    Serial.println("Data reset requested");
    currentReading.aqi = 0;
    currentReading.pm25 = 0;
    currentReading.pm10 = 0;
    currentReading.airQualityLevel = "Unknown";
}
```

## Testing and Calibration

### Phase 2 Testing Checklist

- [ ] Air quality sensor reads values
- [ ] GPS acquires satellite fix (test outdoors)
- [ ] All screens navigate properly
- [ ] Button functions work correctly
- [ ] Data saves to Serial Monitor
- [ ] Sensor readings seem reasonable

### Troubleshooting Common Issues

1. **Air Quality Sensor Issues:**
   - Check wiring connections
   