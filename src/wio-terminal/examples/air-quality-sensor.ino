// Archivo: air_quality_test_simple.ino

#include "TFT_eSPI.h"

TFT_eSPI tft;

// Configuración del sensor
const int AIR_QUALITY_PIN = A0;

void setup() {
    Serial.begin(115200);
    tft.begin();
    tft.setRotation(3);
    tft.fillScreen(TFT_BLACK);
    
    // Configurar pin analógico
    pinMode(AIR_QUALITY_PIN, INPUT);
    
    Serial.println("Grove Air Quality Sensor v1.3 - Test");
    displayHeader();
}

void loop() {
    // Leer sensor
    int rawValue = analogRead(AIR_QUALITY_PIN);
    float voltage = (rawValue / 1024.0) * 3.3;
    
    // Convertir a valores comprensibles
    float aqi = map(rawValue, 0, 1024, 0, 500);
    float pm25 = aqi * 0.5;
    float pm10 = aqi * 0.8;
    String level = getAirQualityLevel(aqi);
    
    // Mostrar en pantalla
    displayAirQualityData(rawValue, aqi, pm25, pm10, level, voltage);
    
    // Mostrar en Serial
    Serial.print("Raw: "); Serial.print(rawValue);
    Serial.print(" | Voltage: "); Serial.print(voltage);
    Serial.print(" | AQI: "); Serial.print(aqi);
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
    // Limpiar área de datos
    tft.fillRect(0, 60, 320, 180, TFT_BLACK);
    
    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(1);
    
    // Valor crudo del sensor
    tft.drawString("Raw Value: " + String(raw), 20, 70);
    tft.drawString("Voltage: " + String(voltage, 2) + "V", 20, 90);
    
    // AQI calculado
    tft.drawString("Air Quality Index:", 20, 120);
    tft.setTextColor(getAQIColor(aqi));
    tft.setTextSize(2);
    tft.drawString(String(aqi, 0), 20, 140);
    
    // Nivel
    tft.setTextColor(TFT_YELLOW);
    tft.setTextSize(1);
    tft.drawString("Level: " + level, 20, 170);
    
    // Valores estimados
    tft.setTextColor(TFT_WHITE);
    tft.drawString("PM2.5: " + String(pm25, 1), 20, 190);
    tft.drawString("PM10: " + String(pm10, 1), 20, 210);
    
    // Indicador visual
    tft.fillCircle(280, 120, 20, getAQIColor(aqi));
}

uint16_t getAQIColor(float aqi) {
    if (aqi <= 50) return TFT_GREEN;
    else if (aqi <= 100) return TFT_YELLOW;
    else if (aqi <= 150) return 0xFD20; // Orange
    else if (aqi <= 200) return TFT_RED;
    else return TFT_MAGENTA;
}