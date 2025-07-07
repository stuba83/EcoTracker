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
        
        // *** CÓDIGO DE DEBUG AGREGADO AQUÍ ***
        if (sentence.length() > 5) {
            Serial.println("NMEA: " + sentence);
        }
        
        // Buscar múltiples tipos de sentencias
        if (sentence.startsWith("$GPGGA") || 
            sentence.startsWith("$GNGGA") || 
            sentence.startsWith("$GPGLL") ||
            sentence.startsWith("$GPRMC")) {
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
