#include <Wire.h>
#include "DualMC33926MotorShield.h"

DualMC33926MotorShield md;  // Motor driver for 2 motors (M1 and M2)
DualMC33926MotorShield md2(2, 3, A2, 5, 6, A3, 11, 13); // Motor driver for 1 motor (M1)
const int SLAVE_ADDR = 8;   // I2C slave address of a2
int distances[10];          // Array to hold sensor data

// Variables for movement based on x and y distances
int targetX = 0, targetY = 0; // New x and y distances from computer

void setup() {
  Serial.begin(115200);
  Wire.begin();   // Join I2C bus as master
  md.init();      // Initialize first motor shield
  md2.init();     // Initialize second motor shield

  Serial.println("Master Arduino - 3 Wheel Omni Drive with Sensor Data Relay");
}

void loop() {
  // 1. Request Data from Slave
  Wire.requestFrom(SLAVE_ADDR, 20); // Request 10 integers (2 bytes each)
  for (int i = 0; i < 10; i++) {
    if (Wire.available() >= 2) {
      distances[i] = (Wire.read() << 8) | Wire.read(); // Combine high and low bytes
    }
  }

  // 2. Send Data to Computer
  Serial.print("Sensor Data: ");
  for (int i = 0; i < 10; i++) {
    if (distances[i] == -1) {
      Serial.print("N/A ");
    } else {
      Serial.print(distances[i]);
      Serial.print(" cm ");
    }
  }
  Serial.println();

  // 3. Receive Processed Data from Computer
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n'); // Read input until newline
    parseInput(input); // Parse x and y distances from computer
  }

  // 4. Control Motors to Move to Target
  moveToTarget(targetX, targetY);

  delay(200); // Loop delay for stability
}

void parseInput(String input) {
  // Parse input in the format "X:<value> Y:<value>"
  int xIndex = input.indexOf('X');
  int yIndex = input.indexOf('Y');

  if (xIndex != -1 && yIndex != -1) {
    targetX = input.substring(xIndex + 2, yIndex).toInt(); // Extract X value
    targetY = input.substring(yIndex + 2).toInt();         // Extract Y value

    Serial.print("New Target - X: ");
    Serial.print(targetX);
    Serial.print(" | Y: ");
    Serial.println(targetY);
  }
}

void moveToTarget(int x, int y) {
  int speed1, speed2, speed3;

  if (x > 0 && y == 0) { // Move Right
    speed1 = -200;
    speed2 = 200;
    speed3 = 200;
  } else if (x < 0 && y == 0) { // Move Left
    speed1 = 200;
    speed2 = -200;
    speed3 = -200;
  } else if (y > 0 && x == 0) { // Move Forward
    speed1 = 200;
    speed2 = 200;
    speed3 = -200;
  } else if (y < 0 && x == 0) { // Move Backward
    speed1 = -200;
    speed2 = -200;
    speed3 = 200;
  } else { // Stop
    speed1 = 0;
    speed2 = 0;
    speed3 = 0;
  }

  // Set motor speeds
  md.setM1Speed(speed1);  // Motor 1 on md
  md.setM2Speed(speed2);  // Motor 2 on md
  md2.setM1Speed(speed3); // Motor 1 on md2
}
