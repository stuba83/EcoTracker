// EcoTracker - Combined Environmental Monitor
// Phase 2 Final Application
// File: combined_environmental_monitor.ino

#include "TFT_eSPI.h"

TFT_eSPI tft;

// Air Quality Sensor Configuration
const int AIR_QUALITY_PIN = A0;

// GPS Data Structure (using simulated Cartago coordinates for now)
struct GPSData {
    float latitude;
    float longitude;
    int satellites;
    bool isValid;
    String timestamp;
    String location;
};

// Environmental Data Structure
struct EnvironmentalReading {
    // Air Quality
    int rawAirValue;
    float voltage;
    float aqi;
    float pm25;
    float pm10;
    String airQualityLevel;
    
    // Location
    GPSData location;
    
    // System
    unsigned long timestamp;
    int readingNumber;
};

// Global variables
EnvironmentalReading currentReading;
GPSData currentGPS;
int currentScreen = 0; // 0=main, 1=air quality, 2=gps, 3=data log, 4=settings
const int maxScreens = 5;
int savedReadings = 0;
bool dataLogging = false;
unsigned long lastReadingTime = 0;
const unsigned long READING_INTERVAL = 5000; // 5 seconds

void setup() {
    Serial.begin(115200);
    delay(2000);
    
    // Initialize display
    tft.begin();
    tft.setRotation(3);
    tft.fillScreen(TFT_BLACK);
    
    // Initialize sensors
    Serial.println("=== EcoTracker Phase 2 - Environmental Monitor ===");
    
    // Air Quality Sensor
    pinMode(AIR_QUALITY_PIN, INPUT);
    Serial.println("✓ Air Quality Sensor initialized");
    
    // Initialize GPS with Cartago coordinates (simulated)
    initializeGPS();
    Serial.println("✓ GPS initialized (simulated mode)");
    
    // Initialize buttons
    pinMode(WIO_KEY_A, INPUT_PULLUP);
    pinMode(WIO_KEY_B, INPUT_PULLUP);
    pinMode(WIO_KEY_C, INPUT_PULLUP);
    pinMode(WIO_5S_UP, INPUT_PULLUP);
    pinMode(WIO_5S_DOWN, INPUT_PULLUP);
    pinMode(WIO_5S_LEFT, INPUT_PULLUP);
    pinMode(WIO_5S_RIGHT, INPUT_PULLUP);
    pinMode(WIO_5S_PRESS, INPUT_PULLUP);
    
    Serial.println("✓ All systems ready!");
    Serial.println("Controls: UP/DOWN=Navigate, PRESS=Select, A=Save, B=Log, C=Reset");
    Serial.println("================================================");
    
    // Initialize first reading
    currentReading.readingNumber = 0;
    updateSensors();
    displayCurrentScreen();
}

void loop() {
    // Handle button input
    handleButtons();
    
    // Update sensors periodically
    if (millis() - lastReadingTime > READING_INTERVAL) {
        updateSensors();
        lastReadingTime = millis();
        
        // Auto-save if logging is enabled
        if (dataLogging) {
            saveCurrentReading();
        }
    }
    
    // Update display every 2 seconds
    static unsigned long lastDisplayUpdate = 0;
    if (millis() - lastDisplayUpdate > 2000) {
        displayCurrentScreen();
        lastDisplayUpdate = millis();
    }
    
    delay(100);
}

void initializeGPS() {
    // Initialize with Cartago Centro coordinates (simulated)
    currentGPS.latitude = 9.8647;     // Cartago Centro
    currentGPS.longitude = -83.9197;   // Cartago Centro
    currentGPS.satellites = 8;         // Simulated good signal
    currentGPS.isValid = true;
    currentGPS.location = "Cartago Centro, CR";
    currentGPS.timestamp = "120000";   // 12:00:00
}

void updateSensors() {
    // Read air quality sensor
    currentReading.rawAirValue = analogRead(AIR_QUALITY_PIN);
    currentReading.voltage = (currentReading.rawAirValue / 1024.0) * 3.3;
    currentReading.aqi = map(currentReading.rawAirValue, 0, 1024, 0, 500);
    currentReading.pm25 = currentReading.aqi * 0.5;
    currentReading.pm10 = currentReading.aqi * 0.8;
    currentReading.airQualityLevel = getAirQualityLevel(currentReading.aqi);
    
    // Add some realistic variation to GPS (simulate slight movement)
    static float latOffset = 0;
    static float lonOffset = 0;
    latOffset += (random(-10, 11) * 0.00001); // Small random walk
    lonOffset += (random(-10, 11) * 0.00001);
    
    // Keep within reasonable bounds (±50 meters)
    latOffset = constrain(latOffset, -0.0005, 0.0005);
    lonOffset = constrain(lonOffset, -0.0005, 0.0005);
    
    currentReading.location.latitude = 9.8647 + latOffset;
    currentReading.location.longitude = -83.9197 + lonOffset;
    currentReading.location.satellites = currentGPS.satellites + random(-1, 2);
    currentReading.location.satellites = constrain(currentReading.location.satellites, 6, 12);
    currentReading.location.isValid = true;
    currentReading.location.location = "Cartago Centro, CR";
    
    // System data
    currentReading.timestamp = millis();
    currentReading.readingNumber++;
    
    // Print summary to Serial
    Serial.print("Reading #"); Serial.print(currentReading.readingNumber);
    Serial.print(" | AQI: "); Serial.print(currentReading.aqi, 0);
    Serial.print(" ("); Serial.print(currentReading.airQualityLevel); Serial.print(")");
    Serial.print(" | Location: "); Serial.print(currentReading.location.latitude, 6);
    Serial.print(", "); Serial.println(currentReading.location.longitude, 6);
}

String getAirQualityLevel(float aqi) {
    if (aqi <= 50) return "Good";
    else if (aqi <= 100) return "Moderate";
    else if (aqi <= 150) return "Unhealthy for Sensitive";
    else if (aqi <= 200) return "Unhealthy";
    else if (aqi <= 300) return "Very Unhealthy";
    else return "Hazardous";
}

void handleButtons() {
    static bool lastButtonUp = HIGH;
    static bool lastButtonDown = HIGH;
    static bool lastButtonLeft = HIGH;
    static bool lastButtonRight = HIGH;
    static bool lastButtonPress = HIGH;
    static bool lastButtonA = HIGH;
    static bool lastButtonB = HIGH;
    static bool lastButtonC = HIGH;
    
    bool currentButtonUp = digitalRead(WIO_5S_UP);
    bool currentButtonDown = digitalRead(WIO_5S_DOWN);
    bool currentButtonLeft = digitalRead(WIO_5S_LEFT);
    bool currentButtonRight = digitalRead(WIO_5S_RIGHT);
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
    
    if (lastButtonLeft == HIGH && currentButtonLeft == LOW) {
        currentScreen = (currentScreen - 1 + maxScreens) % maxScreens;
        displayCurrentScreen();
    }
    
    if (lastButtonRight == HIGH && currentButtonRight == LOW) {
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
    
    // Center press for quick save
    if (lastButtonPress == HIGH && currentButtonPress == LOW) {
        saveCurrentReading();
    }
    
    // Update button states
    lastButtonUp = currentButtonUp;
    lastButtonDown = currentButtonDown;
    lastButtonLeft = currentButtonLeft;
    lastButtonRight = currentButtonRight;
    lastButtonPress = currentButtonPress;
    lastButtonA = currentButtonA;
    lastButtonB = currentButtonB;
    lastButtonC = currentButtonC;
}

void displayCurrentScreen() {
    switch (currentScreen) {
        case 0:
            displayMainDashboard();
            break;
        case 1:
            displayAirQualityDetail();
            break;
        case 2:
            displayLocationDetail();
            break;
        case 3:
            displayDataLog();
            break;
        case 4:
            displaySettings();
            break;
    }
}

void displayMainDashboard() {
    tft.fillScreen(TFT_BLACK);
    
    // Header
    tft.setTextColor(TFT_GREEN);
    tft.setTextSize(2);
    tft.drawString("EcoTracker", 80, 5);
    
    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(1);
    tft.drawString("Environmental Monitor", 70, 30);
    
    // Air Quality section
    tft.setTextColor(TFT_CYAN);
    tft.drawString("Air Quality", 10, 55);
    
    tft.setTextColor(getAQIColor(currentReading.aqi));
    tft.setTextSize(2);
    tft.drawString(String(currentReading.aqi, 0), 10, 75);
    
    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(1);
    tft.drawString(currentReading.airQualityLevel, 10, 100);
    
    // Location section
    tft.setTextColor(TFT_CYAN);
    tft.drawString("Location", 160, 55);
    
    if (currentReading.location.isValid) {
        tft.setTextColor(TFT_GREEN);
        tft.drawString("GPS Fixed", 160, 75);
        tft.setTextColor(TFT_WHITE);
        tft.drawString(String(currentReading.location.satellites) + " sats", 160, 90);
        tft.drawString("Cartago Centro", 160, 105);
    } else {
        tft.setTextColor(TFT_RED);
        tft.drawString("No GPS", 160, 75);
    }
    
    // Real-time data
    tft.setTextColor(TFT_YELLOW);
    tft.drawString("Live Data:", 10, 130);
    tft.setTextColor(TFT_WHITE);
    tft.drawString("PM2.5: " + String(currentReading.pm25, 1), 10, 145);
    tft.drawString("PM10: " + String(currentReading.pm10, 1), 10, 160);
    tft.drawString("Raw: " + String(currentReading.rawAirValue), 160, 145);
    tft.drawString("Voltage: " + String(currentReading.voltage, 2) + "V", 160, 160);
    
    // Status indicators
    tft.fillCircle(290, 70, 15, getAQIColor(currentReading.aqi));
    if (currentReading.location.isValid) {
        tft.fillCircle(290, 100, 8, TFT_GREEN);
    }
    
    // Logging status
    if (dataLogging) {
        tft.setTextColor(TFT_GREEN);
        tft.drawString("● LOGGING", 10, 180);
    } else {
        tft.setTextColor(TFT_GRAY);
        tft.drawString("○ Log Off", 10, 180);
    }
    
    tft.setTextColor(TFT_WHITE);
    tft.drawString("Readings: " + String(savedReadings), 160, 180);
    
    // Navigation help
    tft.setTextColor(TFT_GRAY);
    tft.drawString("↑↓: Navigate  A:Save  B:Log  C:Reset", 10, 205);
    tft.drawString("Main Dashboard (1/5)", 10, 220);
}

void displayAirQualityDetail() {
    tft.fillScreen(TFT_BLACK);
    
    // Header
    tft.setTextColor(TFT_GREEN);
    tft.setTextSize(2);
    tft.drawString("Air Quality", 60, 10);
    
    // Large AQI display
    tft.setTextColor(getAQIColor(currentReading.aqi));
    tft.setTextSize(4);
    tft.drawString(String(currentReading.aqi, 0), 50, 50);
    
    // Quality level
    tft.setTextColor(TFT_YELLOW);
    tft.setTextSize(1);
    tft.drawString(currentReading.airQualityLevel, 10, 100);
    
    // Detailed readings
    tft.setTextColor(TFT_WHITE);
    tft.drawString("Particulate Matter:", 10, 125);
    tft.drawString("PM2.5: " + String(currentReading.pm25, 1) + " μg/m³", 10, 140);
    tft.drawString("PM10:  " + String(currentReading.pm10, 1) + " μg/m³", 10, 155);
    
    tft.drawString("Sensor Data:", 160, 125);
    tft.drawString("Raw: " + String(currentReading.rawAirValue), 160, 140);
    tft.drawString("Voltage: " + String(currentReading.voltage, 2) + "V", 160, 155);
    
    // Visual indicator bar
    int barWidth = map(currentReading.aqi, 0, 500, 0, 280);
    tft.fillRect(20, 180, barWidth, 15, getAQIColor(currentReading.aqi));
    tft.drawRect(19, 179, 282, 17, TFT_WHITE);
    
    // Scale indicators
    tft.setTextColor(TFT_GRAY);
    tft.drawString("0", 20, 200);
    tft.drawString("250", 145, 200);
    tft.drawString("500", 280, 200);
    
    tft.setTextColor(TFT_GRAY);
    tft.drawString("Air Quality Detail (2/5)", 10, 220);
}

void displayLocationDetail() {
    tft.fillScreen(TFT_BLACK);
    
    // Header
    tft.setTextColor(TFT_GREEN);
    tft.setTextSize(2);
    tft.drawString("Location", 90, 10);
    
    if (currentReading.location.isValid) {
        // GPS Status
        tft.setTextColor(TFT_GREEN);
        tft.setTextSize(1);
        tft.drawString("GPS Status: ACTIVE (Simulated)", 10, 45);
        
        // Coordinates
        tft.setTextColor(TFT_WHITE);
        tft.drawString("Coordinates:", 10, 70);
        tft.setTextSize(1);
        tft.drawString("Lat: " + String(currentReading.location.latitude, 6), 10, 90);
        tft.drawString("Lon: " + String(currentReading.location.longitude, 6), 10, 105);
        
        // Location info
        tft.setTextColor(TFT_CYAN);
        tft.drawString("Location: " + currentReading.location.location, 10, 130);
        
        tft.setTextColor(TFT_YELLOW);
        tft.drawString("Satellites: " + String(currentReading.location.satellites), 10, 150);
        
        // Map representation (simple grid)
        tft.setTextColor(TFT_GRAY);
        tft.drawString("Position Map:", 10, 175);
        
        // Simple map grid
        for (int i = 0; i < 5; i++) {
            for (int j = 0; j < 3; j++) {
                tft.drawRect(160 + i*20, 180 + j*15, 18, 13, TFT_GRAY);
            }
        }
        
        // Current position marker
        tft.fillRect(178, 188, 4, 4, TFT_RED);
        
    } else {
        // No GPS
        tft.setTextColor(TFT_RED);
        tft.setTextSize(1);
        tft.drawString("GPS Status: NOT AVAILABLE", 10, 45);
        
        tft.setTextColor(TFT_WHITE);
        tft.drawString("Using simulated coordinates", 10, 70);
        tft.drawString("for Cartago Centro", 10, 85);
        
        tft.setTextColor(TFT_YELLOW);
        tft.drawString("Install modern GPS module", 10, 110);
        tft.drawString("for real location tracking", 10, 125);
    }
    
    tft.setTextColor(TFT_GRAY);
    tft.drawString("Location Detail (3/5)", 10, 220);
}

void displayDataLog() {
    tft.fillScreen(TFT_BLACK);
    
    // Header
    tft.setTextColor(TFT_GREEN);
    tft.setTextSize(2);
    tft.drawString("Data Log", 90, 10);
    
    // Log status
    tft.setTextColor(TFT_CYAN);
    tft.setTextSize(1);
    tft.drawString("Logging Status:", 10, 45);
    
    if (dataLogging) {
        tft.setTextColor(TFT_GREEN);
        tft.drawString("● ACTIVE (Auto-save every 5s)", 10, 60);
    } else {
        tft.setTextColor(TFT_RED);
        tft.drawString("○ INACTIVE (Manual save only)", 10, 60);
    }
    
    // Statistics
    tft.setTextColor(TFT_WHITE);
    tft.drawString("Total Readings: " + String(savedReadings), 10, 85);
    tft.drawString("Current Reading: #" + String(currentReading.readingNumber), 10, 100);
    
    // Recent readings summary
    tft.setTextColor(TFT_CYAN);
    tft.drawString("Current Session:", 10, 125);
    
    tft.setTextColor(TFT_WHITE);
    tft.drawString("AQI: " + String(currentReading.aqi, 0) + " (" + currentReading.airQualityLevel + ")", 10, 140);
    tft.drawString("Location: " + currentReading.location.location, 10, 155);
    tft.drawString("Uptime: " + String(millis() / 1000) + " seconds", 10, 170);
    
    // Memory usage (simulated)
    int memoryUsed = (savedReadings * 64) % 1000; // Simulate memory usage
    tft.setTextColor(TFT_YELLOW);
    tft.drawString("Memory: " + String(memoryUsed) + "/1000 KB", 10, 190);
    
    tft.setTextColor(TFT_GRAY);
    tft.drawString("Data Log (4/5)", 10, 220);
}

void displaySettings() {
    tft.fillScreen(TFT_BLACK);
    
    // Header
    tft.setTextColor(TFT_GREEN);
    tft.setTextSize(2);
    tft.drawString("Settings", 90, 10);
    
    // System settings
    tft.setTextColor(TFT_CYAN);
    tft.setTextSize(1);
    tft.drawString("System Configuration:", 10, 45);
    
    tft.setTextColor(TFT_WHITE);
    tft.drawString("Sample Rate: 5 seconds", 10, 65);
    tft.drawString("Data Format: JSON", 10, 80);
    tft.drawString("GPS Mode: Simulated", 10, 95);
    
    // Sensor settings
    tft.setTextColor(TFT_CYAN);
    tft.drawString("Sensor Configuration:", 10, 120);
    
    tft.setTextColor(TFT_WHITE);
    tft.drawString("Air Quality: Grove v1.3 (Analog)", 10, 140);
    tft.drawString("GPS: Simulated Cartago Centro", 10, 155);
    
    // System info
    tft.setTextColor(TFT_CYAN);
    tft.drawString("System Information:", 10, 180);
    
    tft.setTextColor(TFT_WHITE);
    tft.drawString("Version: Phase 2 Final", 10, 200);
    tft.drawString("Uptime: " + String(millis() / 60000) + " minutes", 160, 200);
    
    tft.setTextColor(TFT_GRAY);
    tft.drawString("Settings (5/5)", 10, 220);
}

uint16_t getAQIColor(float aqi) {
    if (aqi <= 50) return TFT_GREEN;
    else if (aqi <= 100) return TFT_YELLOW;
    else if (aqi <= 150) return 0xFD20; // Orange
    else if (aqi <= 200) return TFT_RED;
    else return TFT_MAGENTA;
}

void saveCurrentReading() {
    savedReadings++;
    
    Serial.println("=== READING SAVED ===");
    Serial.print("Reading #"); Serial.println(savedReadings);
    Serial.print("Timestamp: "); Serial.println(currentReading.timestamp);
    Serial.print("AQI: "); Serial.print(currentReading.aqi);
    Serial.print(" ("); Serial.print(currentReading.airQualityLevel); Serial.println(")");
    Serial.print("PM2.5: "); Serial.print(currentReading.pm25); Serial.println(" μg/m³");
    Serial.print("PM10: "); Serial.print(currentReading.pm10); Serial.println(" μg/m³");
    Serial.print("Raw Sensor: "); Serial.print(currentReading.rawAirValue);
    Serial.print(" ("); Serial.print(currentReading.voltage); Serial.println("V)");
    Serial.print("Location: "); Serial.print(currentReading.location.latitude, 6);
    Serial.print(", "); Serial.println(currentReading.location.longitude, 6);
    Serial.print("Satellites: "); Serial.println(currentReading.location.satellites);
    Serial.println("==================");
    
    // Visual feedback
    tft.fillRect(0, 0, 320, 20, TFT_GREEN);
    tft.setTextColor(TFT_BLACK);
    tft.setTextSize(1);
    tft.drawString("SAVED! Reading #" + String(savedReadings), 10, 5);
    delay(1000);
}

void toggleDataLogging() {
    dataLogging = !dataLogging;
    
    if (dataLogging) {
        Serial.println("=== DATA LOGGING ENABLED ===");
        Serial.println("Auto-saving every 5 seconds");
    } else {
        Serial.println("=== DATA LOGGING DISABLED ===");
        Serial.println("Manual save mode");
    }
    
    // Visual feedback
    tft.fillRect(0, 0, 320, 20, dataLogging ? TFT_GREEN : TFT_RED);
    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(1);
    tft.drawString(dataLogging ? "LOGGING ON" : "LOGGING OFF", 10, 5);
    delay(1000);
}

void resetData() {
    savedReadings = 0;
    currentReading.readingNumber = 0;
    dataLogging = false;
    
    Serial.println("=== DATA RESET ===");
    Serial.println("All counters reset to zero");
    
    // Visual feedback
    tft.fillRect(0, 0, 320, 20, TFT_BLUE);
    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(1);
    tft.drawString("DATA RESET", 10, 5);
    delay(1000);
}