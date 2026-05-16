#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include "MAX30105.h"
#include <Adafruit_Sensor.h>
#include <heartRate.h>
#include <spo2_algorithm.h>

// --- Sensor Objects ---
Adafruit_MPU6050 mpu;
MAX30105 particleSensor;

// --- Pins ---
const int emgPin = A1;
const int tempPin = A0;
const float voltageRef = 5.0;

// --- Timing ---
unsigned long lastTempMillis = 0;
const unsigned long tempInterval = 500; // Temp update interval in ms

void setup() {
  Serial.begin(115200);
  while (!Serial);

  // --- MPU6050 Setup ---
  if (!mpu.begin()) {
    Serial.println("MPU6050 not found!");
    while (1);
  }
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

  // --- MAX30105 Setup ---
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) {
    Serial.println("MAX30105 not found!");
    while (1);
  }
  particleSensor.setup(60, 4, 2, 100, 411, 4096); // LED, avg, mode, rate, pulse, adc

  Serial.println("Sensors initialized");
}

// --- Main Loop ---
void loop() {
  // --- EMG: fast sampling (~1000 Hz) ---
  int emgValue = analogRead(emgPin);
  Serial.print("EMG:");
  Serial.print(emgValue);
  Serial.print("\t");

  // --- MPU6050: accel + gyro ---
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  Serial.print("AccelX:");
  Serial.print(a.acceleration.x);
  Serial.print("\tAccelY:");
  Serial.print(a.acceleration.y);
  Serial.print("\tAccelZ:");
  Serial.print(a.acceleration.z);
  Serial.print("\tGyroX:");
  Serial.print(g.gyro.x);
  Serial.print("\tGyroY:");
  Serial.print(g.gyro.y);
  Serial.print("\tGyroZ:");
  Serial.print(g.gyro.z);
  Serial.print("\t");

  // --- MAX30105: RED + IR ---
  if (particleSensor.available()) {
    int red = particleSensor.getRed();
    int ir = particleSensor.getIR();
    Serial.print("RED:");
    Serial.print(red);
    Serial.print("\tIR:");
    Serial.print(ir);
    Serial.print("\t");
    particleSensor.nextSample();
  }

  // --- LM335 Temperature every 500 ms ---
  unsigned long currentMillis = millis();
  if (currentMillis - lastTempMillis >= tempInterval) {
    lastTempMillis = currentMillis;
    int raw = analogRead(tempPin);
    float voltage = raw * (voltageRef / 1023.0);
    float tempK = voltage * 100.0;        // LM335: 10 mV/K
    float tempC = tempK - 273.15;
    float tempF = tempC * 9.0 / 5.0 + 32;
    Serial.print("TempC:");
    Serial.print(tempC);
    Serial.print("\tTempF:");
    Serial.print(tempF);
    Serial.print("\t");
  }

  Serial.println(); // end of one row for Serial Plotter

  // --- Minimal delay to reach high EMG rate ---
  delayMicroseconds(1000); // ~1000 Hz
}
