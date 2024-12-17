#include <Wire.h>

const int triggerPin = 2;              // Shared Trigger pin
const int echoPins[10] = {3, 4, 5, 6, 7, 8, 9, 10, 11, 12}; // Separate Echo pins
long duration;
int distances[10];  // Array to hold distances

void setup() {
  Wire.begin(8); // Initialize as I2C slave with address 8
  pinMode(triggerPin, OUTPUT);
  for (int i = 0; i < 10; i++) {
    pinMode(echoPins[i], INPUT);
  }
  Wire.onRequest(sendUltrasonicData); // I2C event for master request
}

void loop() {
  for (int i = 0; i < 10; i++) {
    // Trigger the sensor
    digitalWrite(triggerPin, LOW);
    delayMicroseconds(2);
    digitalWrite(triggerPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(triggerPin, LOW);

    // Read Echo signal for the current sensor
    duration = pulseIn(echoPins[i], HIGH, 30000); // Timeout in microseconds
    if (duration == 0) {
      distances[i] = -1; // No object detected
    } else {
      distances[i] = duration * 0.034 / 2; // Convert to cm
    }
    delay(50); // Small delay for stability
  }
}

void sendUltrasonicData() {
  for (int i = 0; i < 10; i++) {
    Wire.write((uint8_t)(distances[i] >> 8));  // Send high byte
    Wire.write((uint8_t)(distances[i] & 0xFF)); // Send low byte
  }
}
