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