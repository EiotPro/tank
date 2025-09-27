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
  delay(500);

  // Configure LoRa parameters using config values
  String p2pCommand = "AT+P2P=" +
                     String(LORA_FREQUENCY) + ":" +
                     String(LORA_SPREADING_FACTOR) + ":" +
                     String(LORA_BANDWIDTH) + ":" +
                     String(LORA_CODING_RATE) + ":" +
                     String(LORA_PREAMBLE_LENGTH) + ":" +
                     String(LORA_TX_POWER);

  Serial.println("Configuring LoRa parameters...");
  Serial.println("Command: " + p2pCommand);
  Serial1.println(p2pCommand);
  delay(500);

  // Show configured parameters
  Serial.println("📊 LoRa Parameters Set:");
  Serial.println("   Frequency: " + String(LORA_FREQUENCY) + " Hz");
  Serial.println("   Spreading Factor: " + String(LORA_SPREADING_FACTOR));
  Serial.println("   Bandwidth: " + String(LORA_BANDWIDTH));
  Serial.println("   Coding Rate: " + String(LORA_CODING_RATE));
  Serial.println("   Preamble Length: " + String(LORA_PREAMBLE_LENGTH));
  Serial.println("   TX Power: " + String(LORA_TX_POWER) + " dBm");

  // TX doesn't need RX
  Serial1.println("AT+PRECV=0");
  delay(500);

  // Test LoRa module communication
  Serial.println("Testing LoRa module...");
  Serial1.println("AT");
  delay(500);

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

    Serial.println("\n========================================");
    Serial.println("📤 TRANSMITTING WATER LEVEL DATA");
    Serial.println("========================================");
    Serial.print("📡 Raw sensor value: ");
    Serial.println(analogRead(WATER_SENSOR_PIN));
    Serial.print("💧 Water level: ");
    Serial.print(waterLevel);
    Serial.println(" cm");
    Serial.print("📈 Tank fill: ");
    Serial.print(percentage);
    Serial.println("%");

    // Convert to HEX payload (2 bytes: water level in cm)
    String hexPayload = decimalToHex(waterLevel);

    Serial.print("🔢 HEX payload to send: ");
    Serial.println(hexPayload);
    Serial.println("📡 Sending via LoRa...");

    // Send data via LoRa
    if (sendLoRaData(hexPayload)) {
      Serial.println("✅ LoRa transmission successful!");
      Serial.println("🚀 Data is on its way to receiver");
    } else {
      Serial.println("❌ LoRa transmission failed!");
      Serial.println("🔄 Will retry on next transmission");
    }
    Serial.println("========================================");

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

  // Periodic status update for debugging
  static unsigned long lastStatusUpdate = 0;
  if (millis() - lastStatusUpdate > 15000) {  // Every 15 seconds
    Serial.println("TX Status: Active and monitoring water level...");
    Serial.print("Next transmission in: ");
    Serial.print((TRANSMISSION_INTERVAL - (millis() - lastTransmission)) / 1000);
    Serial.println(" seconds");
    lastStatusUpdate = millis();
  }

  delay(100);  // Small delay to prevent overwhelming the system
}
