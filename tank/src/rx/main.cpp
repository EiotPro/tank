#include <Arduino.h>
#include <HardwareSerial.h>
#include <time.h>
#include "config.h"

// Global variables
int currentWaterLevel = 0;
int currentPercentage = 0;

// Function to convert HEX string to decimal
int hexToDec(String hexStr) {
  int value = 0;
  for (int i = 0; i < hexStr.length(); i++) {
    char c = hexStr.charAt(i);
    value *= 16;

    if (c >= '0' && c <= '9') {
      value += (c - '0');
    } else if (c >= 'A' && c <= 'F') {
      value += (c - 'A' + 10);
    } else if (c >= 'a' && c <= 'f') {
      value += (c - 'a' + 10);
    }
  }
  return value;
}



void setup() {
  Serial.begin(SERIAL_BAUD_RATE);
  Serial1.begin(LORA_BAUD_RATE);

  // Initialize LoRa
  pinMode(RAK3172_RESET, OUTPUT);
  digitalWrite(RAK3172_RESET, HIGH);
  delay(2000);

  Serial.println("=== LoRa Water Tank RX Node ===");
  Serial.println("Initializing LoRa reception...");

  // LoRa P2P configuration
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

  // Enable RX
  Serial1.println("AT+PRECV=65535");
  delay(200);

  Serial.println("RX Node initialization complete");
}

void loop() {
  // Handle LoRa reception
  if (Serial1.available()) {
    String resp = Serial1.readStringUntil('\n');
    resp.trim();

    if (resp.length() > 0) {
      Serial.println("LoRa RX: " + resp);

      // Check for received P2P data
      if (resp.startsWith("+EVT:RXP2P")) {
        // Extract payload from response
        int idx = resp.lastIndexOf(':');
        if (idx != -1) {
          String hexPayload = resp.substring(idx + 1);
          Serial.print("Received HEX payload: ");
          Serial.println(hexPayload);

          // Convert HEX to decimal
          int waterLevel = hexToDec(hexPayload);
          currentWaterLevel = waterLevel;

          // Calculate percentage (assuming 200cm max depth)
          currentPercentage = map(waterLevel, 0, 200, 0, 100);
          currentPercentage = constrain(currentPercentage, 0, 100);

          Serial.println("--- Water Level Data ---");
          Serial.print("Water level: ");
          Serial.print(waterLevel);
          Serial.println(" cm");
          Serial.print("Percentage: ");
          Serial.print(currentPercentage);
          Serial.println("%");

          // Re-enable RX
          Serial1.println("AT+PRECV=65535");
        }
      }
    }
  }

  delay(100);
}
