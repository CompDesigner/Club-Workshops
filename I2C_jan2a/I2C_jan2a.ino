#include <Arduino.h>
#include <LiquidCrystal.h>

// Define pin numbers for ultrasonic sensor
const int trigPin= 3;  // Connect to the trigger pin of the ultrasonic sensor
const int echoPin= 2;  // Connect to the echo pin of the ultrasonic sensor

// Define LCD pins
const int rs= 16, en= 17, d4= 18, d5= 19, d6= 20, d7= 21;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7); // Initialize the LCD

void setup() {
  Serial.begin(9600);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // Initialize the LCD
  lcd.begin(16, 2);
  lcd.clear(); // Clear the LCD screen
}

void loop() {
  long duration, distance;
  
  // Trigger ultrasonic sensor
  digitalWrite(trigPin, LOW);
  delay(2);
  digitalWrite(trigPin, HIGH);
  delay(10);
  digitalWrite(trigPin, LOW);

  //Read the duration of the pulse
  duration= pulseIn(echoPin, HIGH);

  //Calculate the distance in centimeters
  distance= duration * 0.034 / 2;

  // Display the distance on the LCD
  lcd.clear(); // Clear the LCD screen
  lcd.setCursor(0, 0);
  lcd.print("Distance: ");
  lcd.print(distance);
  lcd.print(" cm");

  //Print the distance to the Serial Monitor
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.print(" cm\n");

  //Wait for a second before the next reading
  delay(1000);  
}