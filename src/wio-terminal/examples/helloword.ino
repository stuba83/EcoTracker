// Corrected code for Wio Terminal 2020
// Fixes SPI and conflicting library issues

#include "TFT_eSPI.h"
#include <SPI.h>

TFT_eSPI tft;

#define LCD_BACKLIGHT (72Ul)

// Specific pins for Wio Terminal 2020
#define TFT_CS   15
#define TFT_DC   2
#define TFT_RST  4

void setup() {
  Serial.begin(115200);
  delay(3000);
  
  Serial.println("=== WIO 2020 - CORRECTED VERSION ===");
  
  // STEP 1: Configure backlight
  pinMode(LCD_BACKLIGHT, OUTPUT);
  digitalWrite(LCD_BACKLIGHT, HIGH);
  Serial.println("✓ Backlight activated");
  
  // STEP 2: Configure pins manually
  pinMode(TFT_CS, OUTPUT);
  pinMode(TFT_DC, OUTPUT);
  pinMode(TFT_RST, OUTPUT);
  
  digitalWrite(TFT_CS, HIGH);   // Deactivate chip select
  digitalWrite(TFT_DC, HIGH);   // Data mode
  digitalWrite(TFT_RST, HIGH);  // No reset
  
  Serial.println("✓ Pins configured");
  
  // STEP 3: Manual display reset
  Serial.println("Executing manual reset...");
  digitalWrite(TFT_RST, LOW);   // Reset low
  delay(20);
  digitalWrite(TFT_RST, HIGH);  // Reset high
  delay(150);  // Wait for stabilization
  
  Serial.println("✓ Reset completed");
  
  // STEP 4: Initialize SPI (without setFrequency which doesn't exist in SAMD)
  SPI.begin();
  // On SAMD frequency is configured automatically
  Serial.println("✓ SPI initialized");
  
  // STEP 5: Specific SPI configuration for SAMD51
  SPISettings spiSettings(8000000, MSBFIRST, SPI_MODE0);  // 8MHz
  
  // STEP 6: Manual ILI9341 initialization
  Serial.println("Initializing ILI9341 controller...");
  
  SPI.beginTransaction(spiSettings);
  initILI9341Manual();
  SPI.endTransaction();
  
  Serial.println("✓ Controller manually initialized");
  
  // STEP 7: Now try with TFT_eSPI
  Serial.println("Initializing TFT_eSPI...");
  tft.begin();
  tft.setRotation(3);
  
  Serial.println("✓ TFT_eSPI initialized");
  
  // STEP 8: Basic color test
  Serial.println("Testing basic colors...");
  
  // Black
  tft.fillScreen(TFT_BLACK);
  Serial.println("Black - do you see black screen?");
  delay(2000);
  
  // White
  tft.fillScreen(TFT_WHITE);
  Serial.println("White - do you see white screen?");
  delay(2000);
  
  // Red
  tft.fillScreen(TFT_RED);
  Serial.println("Red - do you see red screen?");
  delay(2000);
  
  // Green
  tft.fillScreen(TFT_GREEN);
  Serial.println("Green - do you see green screen?");
  delay(2000);
  
  // Blue
  tft.fillScreen(TFT_BLUE);
  Serial.println("Blue - do you see blue screen?");
  delay(2000);
  
  // STEP 9: Final content
  tft.fillScreen(TFT_BLACK);
  
  // Main text
  tft.setTextColor(TFT_GREEN);
  tft.setTextSize(2);
  tft.setCursor(50, 50);
  tft.print("EcoTracker");
  
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(1);
  tft.setCursor(20, 100);
  tft.print("Phase 1: Setup Complete");
  
  tft.setTextColor(TFT_CYAN);
  tft.setCursor(20, 120);
  tft.print("Wio Terminal 2020 Working!");
  
  // Test shapes
  tft.drawRect(10, 150, 100, 50, TFT_YELLOW);
  tft.fillRect(15, 155, 90, 40, TFT_BLUE);
  
  // Circle
  tft.fillCircle(200, 175, 20, TFT_MAGENTA);
  
  Serial.println("=== TEST COMPLETED ===");
  Serial.println("Do you see colors, text and shapes?");
}

void initILI9341Manual() {
  // Basic ILI9341 initialization using direct SPI
  
  digitalWrite(TFT_CS, LOW);  // Activate chip select
  
  // Software Reset
  writeCommandSafe(0x01);
  delay(150);
  
  // Sleep Out
  writeCommandSafe(0x11);
  delay(120);
  
  // Memory Access Control (orientation)
  writeCommandSafe(0x36);
  writeDataSafe(0x48);  // MY=0, MX=1, MV=0, ML=0, BGR=1, MH=0
  
  // Pixel Format Set (16-bit color)
  writeCommandSafe(0x3A);
  writeDataSafe(0x55);
  
  // Display Function Control
  writeCommandSafe(0xB6);
  writeDataSafe(0x08);
  writeDataSafe(0x82);
  writeDataSafe(0x27);
  
  // Display On
  writeCommandSafe(0x29);
  delay(120);
  
  digitalWrite(TFT_CS, HIGH);  // Deactivate chip select
}

void writeCommandSafe(uint8_t cmd) {
  digitalWrite(TFT_DC, LOW);   // Command mode
  SPI.transfer(cmd);
}

void writeDataSafe(uint8_t data) {
  digitalWrite(TFT_DC, HIGH);  // Data mode
  SPI.transfer(data);
}

void loop() {
  // Operation indicator
  static unsigned long lastUpdate = 0;
  static int counter = 0;
  
  if (millis() - lastUpdate > 2000) {
    // Update counter
    tft.fillRect(200, 150, 100, 20, TFT_BLACK);
    tft.setTextColor(TFT_YELLOW);
    tft.setTextSize(1);
    tft.setCursor(200, 150);
    tft.print("Run: ");
    tft.print(counter++);
    
    Serial.print("Loop: ");
    Serial.println(counter);
    
    lastUpdate = millis();
  }
}