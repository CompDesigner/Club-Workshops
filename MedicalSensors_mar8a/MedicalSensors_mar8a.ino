#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include "MAX30105.h"  // SparkFun MAX3010x Library
#include <Adafruit_Sensor.h>
#include <heartRate.h>
#include <spo2_algorithm.h>

// MPU6050 object
Adafruit_MPU6050 mpu;

// MAX30105 object (for Pulse Oximeter and Heart Rate)
MAX30105 particleSensor;  

// Temperature sensor pin
const int tempSensorPin = A0;
const float voltageRef = 5.0;  // Reference voltage for ADC

// EMG sensor pin
const int emgPin = A1;  // Moved to A1 to avoid conflict with temp sensor

void setup() {
    Serial.begin(115200);
    while (!Serial); // Wait for serial connection
    Serial.println("Initializing MPU6050...");
    
    if (!mpu.begin()) {
        Serial.println("MPU6050 not found! Check wiring.");
        while (1);
    }
    
    Serial.println("MPU6050 initialized.");
        
    // Configure MPU6050 settings
    mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
    mpu.setGyroRange(MPU6050_RANGE_500_DEG);
    mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
    
    // Initialize MAX30105 (Pulse Oximeter and Heart Rate sensor)
    Serial.println("Initializing MAX30105...");
    if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) {
        Serial.println("MAX30105 sensor not found! Check wiring.");
        while (1);
     }
    Serial.println("MAX30105 initialized.");
    
    // Setup to sense a nice looking saw tooth on the plotter
    byte ledBrightness = 60; // Options: 0=Off to 255=50mA
    byte sampleAverage = 4; // Options: 1, 2, 4, 8, 16, 32
    byte ledMode = 2; // Options: 1 = Red only, 2 = Red + IR, 3 = Red + IR + Green
    byte sampleRate = 100; // Options: 50, 100, 200, 400, 800, 1000, 1600, 3200
    int pulseWidth = 411; // Options: 69, 118, 215, 411
    int adcRange = 4096; // Options: 2048, 4096, 8192, 16384
    
    particleSensor.setup(ledBrightness, sampleAverage, ledMode, sampleRate, pulseWidth, adcRange);
}

void loop() {
    // Read and print EMG signal
    int emgValue = analogRead(emgPin);
    Serial.print("EMG: ");
    Serial.println(emgValue);
    
    // Read and convert temperature sensor data
    int sensorValue = analogRead(tempSensorPin);
    float voltage = sensorValue * (voltageRef / 1023.0); // Convert ADC reading to voltage
    float temperatureK = voltage * 100.0; // Convert voltage to Kelvin
    float temperatureC = temperatureK - 273.15; // Convert to Celsius
    float temperatureF = (temperatureC * 9.0 / 5.0) + 32.0; // Convert to Fahrenheit
    
    Serial.print("Temperature: ");
    Serial.print(temperatureC);
    Serial.println(" °C");
    Serial.print(temperatureF);
    Serial.println(" °F");
    
    // Read MPU6050 accelerometer and gyroscope data
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);
    
    Serial.print("Accel X: "); Serial.print(a.acceleration.x); Serial.print(" m/s^2\t");
    Serial.print("Accel Y: "); Serial.print(a.acceleration.y); Serial.print(" m/s^2\t");
    Serial.print("Accel Z: "); Serial.print(a.acceleration.z); Serial.print(" m/s^2\n");
    
    Serial.print("Gyro X: "); Serial.print(g.gyro.x); Serial.print(" rad/s\t");
    Serial.print("Gyro Y: "); Serial.print(g.gyro.y); Serial.print(" rad/s\t");
    Serial.print("Gyro Z: "); Serial.print(g.gyro.z); Serial.print(" rad/s\n");
    
    Serial.print("MPU Temp: "); Serial.print(temp.temperature); Serial.println(" °C\n");
    
    // Read and print MAX30105 data (Pulse Oximeter and Heart Rate)
    if (particleSensor.available()) {
    int red = particleSensor.getRed();
    int ir = particleSensor.getIR();

    Serial.print("RED: ");
    Serial.print(red);
    Serial.print("\tIR: ");
    Serial.println(ir);

    // Additional heart rate and SpO2 calculations can be added here//

    particleSensor.nextSample();
    }

    Serial.println();
    
    
    delay(500); // Delay to prevent overwhelming output
}