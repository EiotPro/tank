#include <Arduino.h>
#include <HardwareSerial.h>
#include <stdio.h>
#include "config.h"

// Function to read water level sensor
int readWaterLevel() {
  // Read analog value from sensor (0-1023)
  int sensorValue = analogRead(WATER_SENSOR_PIN);

  // Convert to water level in cm
  // Assuming sensor gives higher values when tank is full
  // Adjust this calculation based on your specific sensor
  int waterLevel = map(sensorValue, 0, 1023, TANK_MAX_DEPTH, 0);

  // Constrain to valid range
  waterLevel = constrain(waterLevel, 0, TANK_MAX_DEPTH);

  return waterLevel;
}

// Function to convert water level to percentage
int calculatePercentage(int waterLevel) {
  int percentage = map(waterLevel, 0, TANK_MAX_DEPTH, 0, 100);
  percentage = constrain(percentage, 0, 100);
  return percentage;
}

// Function to convert decimal to 2-byte HEX string
String decimalToHex(int decimal) {
  // Convert to 2-byte value (0-65535)
  uint16_t value = (uint16_t)decimal;

  // Convert to HEX string
  char hexStr[5];  // 4 digits + null terminator
  sprintf(hexStr, "%04X", value);

  return String(hexStr);
}

// Function to send LoRa data
bool sendLoRaData(String hexData) {
  // Send data using AT command
  String command = "AT+PSEND=" + hexData;
  Serial1.println(command);

  // Wait for response
  unsigned long startTime = millis();
  while (millis() - startTime < 2000) {  // 2 second timeout
    if (Serial1.available()) {
      String response = Serial1.readStringUntil('\n');
      response.trim();

      if (response.indexOf("OK") != -1) {
        Serial.println("LoRa transmission successful");
        return true;
      } else if (response.indexOf("ERROR") != -1) {
        Serial.println("LoRa transmission failed");
        return false;
      }
    }
  }

  Serial.println("LoRa transmission timeout");
  return false;
}

void setup() {
  Serial.begin(SERIAL_BAUD_RATE);
  Serial1.begin(LORA_BAUD_RATE);

  pinMode(RAK3172_RESET, OUTPUT);
  digitalWrite(RAK3172_RESET, HIGH);
  delay(2000);

  Serial.println("=== LoRa Water Tank TX Node ===");
  Serial.println("Initializing...");

  // Initialize water level sensor pin
  pinMode(WATER_SENSOR_PIN, INPUT);

  // LoRa P2P configuration
  Serial.println("Configuring LoRa P2P mode...");
  Serial1.println("AT+NWM=0");  // Set to P2P mode
  delay(200);

  // Configure LoRa parameters using config values
  String p2pCommand = "AT+P2P=" +
                     String(LORA_FREQUENCY) + ":" +
                     String(LORA_SPREADING_FACTOR) + ":" +
                     String(LORA_BANDWIDTH) + ":" +
                     String(LORA_CODING_RATE) + ":" +
                     String(LORA_PREAMBLE_LENGTH) + ":" +
                     String(LORA_TX_POWER);

  Serial1.println(p2pCommand);
  delay(200);

  // TX doesn't need RX
  Serial1.println("AT+PRECV=0");
  delay(200);

  Serial.println("TX Node initialization complete");
  Serial.println("Starting water level monitoring...");
  Serial.print("Transmission interval: ");
  Serial.print(TRANSMISSION_INTERVAL / 1000);
  Serial.println(" seconds");
}

void loop() {
  static unsigned long lastTransmission = 0;

  // Check if it's time to send data
  if (millis() - lastTransmission >= TRANSMISSION_INTERVAL) {
    // Read water level sensor
    int waterLevel = readWaterLevel();
    int percentage = calculatePercentage(waterLevel);

    Serial.println("\n--- Water Level Reading ---");
    Serial.print("Raw sensor value: ");
    Serial.println(analogRead(WATER_SENSOR_PIN));
    Serial.print("Water level: ");
    Serial.print(waterLevel);
    Serial.println(" cm");
    Serial.print("Tank fill: ");
    Serial.print(percentage);
    Serial.println("%");

    // Convert to HEX payload (2 bytes: water level in cm)
    String hexPayload = decimalToHex(waterLevel);

    Serial.print("HEX payload: ");
    Serial.println(hexPayload);

    // Send data via LoRa
    if (sendLoRaData(hexPayload)) {
      Serial.println("Data sent successfully");
    } else {
      Serial.println("Failed to send data");
    }

    lastTransmission = millis();
  }

  // Handle any incoming serial data (for debugging)
  if (Serial1.available()) {
    String response = Serial1.readStringUntil('\n');
    response.trim();
    if (response.length() > 0) {
      Serial.print("LoRa Response: ");
      Serial.println(response);
    }
  }

  delay(100);  // Small delay to prevent overwhelming the system
}
