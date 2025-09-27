#include <Arduino.h>
#include <HardwareSerial.h>
#include <WiFi.h>
#include "config.h"

// Global variables
int currentWaterLevel = 0;
int currentPercentage = 0;
bool wifiConnected = false;
unsigned long lastWiFiCheck = 0;
const unsigned long WIFI_CHECK_INTERVAL = 30000; // Check WiFi every 30 seconds

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

// Function to connect to WiFi
void connectToWiFi() {
   if (WiFi.status() == WL_CONNECTED) {
     wifiConnected = true;
     return;
   }

   Serial.println("Connecting to WiFi...");
   WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

   int attempts = 0;
   while (WiFi.status() != WL_CONNECTED && attempts < 20) {
     delay(500);
     Serial.print(".");
     attempts++;
   }

   if (WiFi.status() == WL_CONNECTED) {
     wifiConnected = true;
     Serial.println("\nWiFi connected successfully!");
     Serial.print("IP Address: ");
     Serial.println(WiFi.localIP());
   } else {
     wifiConnected = false;
     Serial.println("\nWiFi connection failed!");
   }
 }

// Function to send data to website
bool sendDataToWebsite(int waterLevel, int percentage) {
    if (!wifiConnected) {
      Serial.println("WiFi not connected, skipping data send");
      return false;
    }

    WiFiClient client;
    String url = "http://" + String(WEBSITE_HOST) + ":" + String(WEBSITE_PORT) + API_ENDPOINT;

    // Create JSON payload
    String jsonPayload = "{\"tank_id\":1,\"level\":" + String(waterLevel) +
                        ",\"percentage\":" + String(percentage) +
                        ",\"api_key\":\"" + String(API_KEY) + "\"}";

    Serial.println("Sending data to website...");
    Serial.println("URL: " + url);
    Serial.println("Payload: " + jsonPayload);

    // Connect to server
    if (!client.connect(WEBSITE_HOST, WEBSITE_PORT)) {
      Serial.println("Connection to website failed!");
      return false;
    }

    // Send HTTP POST request
    String request = "POST " + String(API_ENDPOINT) + " HTTP/1.1\r\n";
    request += "Host: " + String(WEBSITE_HOST) + "\r\n";
    request += "Content-Type: application/json\r\n";
    request += "Content-Length: " + String(jsonPayload.length()) + "\r\n";
    request += "Connection: close\r\n\r\n";
    request += jsonPayload;

    client.print(request);

    // Wait for response
    unsigned long timeout = millis();
    while (client.available() == 0) {
      if (millis() - timeout > HTTP_TIMEOUT) {
        Serial.println("Client timeout!");
        client.stop();
        return false;
      }
    }

    // Read response
    String response = "";
    while (client.available()) {
      response += client.readStringUntil('\r');
    }

    Serial.println("Response: " + response);

    // Check if response contains "200 OK"
    if (response.indexOf("200 OK") >= 0) {
      Serial.println("Data sent successfully to website!");
      client.stop();
      return true;
    } else {
      Serial.println("Failed to send data to website");
      client.stop();
      return false;
    }
  }

// Function to retry sending data with exponential backoff
bool retrySendData(int waterLevel, int percentage, int maxAttempts) {
   for (int attempt = 1; attempt <= maxAttempts; attempt++) {
      Serial.println("Attempt " + String(attempt) + " of " + String(maxAttempts));

      if (sendDataToWebsite(waterLevel, percentage)) {
        return true; // Success, exit retry loop
      }

      if (attempt < maxAttempts) {
        Serial.println("Retrying in " + String(RETRY_DELAY / 1000) + " seconds...");
        delay(RETRY_DELAY);
      }
   }

   Serial.println("Failed to send data after " + String(maxAttempts) + " attempts");
   return false;
 }

void setup() {
   Serial.begin(SERIAL_BAUD_RATE);
   Serial1.begin(LORA_BAUD_RATE);

   // Initialize LoRa
   pinMode(RAK3172_RESET, OUTPUT);
   digitalWrite(RAK3172_RESET, HIGH);
   delay(2000);

   // Give LoRa module time to initialize
   Serial.println("Waiting for LoRa module initialization...");
   delay(3000);

   Serial.println("=== LoRa Water Tank RX Node ===");
   Serial.println("Initializing LoRa reception...");

   // LoRa P2P configuration
   Serial.println("Setting LoRa to P2P mode...");
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

   // Enable RX
   Serial1.println("AT+PRECV=65535");
   delay(500);

   // Verify RX mode is active
   Serial.println("Verifying LoRa RX mode...");
   Serial1.println("AT");
   delay(200);

   // Additional check for RX status
   Serial.println("Confirming receiver is listening...");
   Serial1.println("AT+PRECV=65535");
   delay(300);

   // Test LoRa module communication
   Serial.println("Testing LoRa module...");
   Serial1.println("AT");
   delay(500);

   // Test P2P configuration
   Serial.println("Checking P2P configuration...");
   Serial1.println("AT");
   delay(500);

   // Initialize WiFi
   Serial.println("Initializing WiFi...");
   connectToWiFi();

   Serial.println("RX Node initialization complete");
   Serial.println("Ready to receive LoRa data and forward to website");
 }

void loop() {
     // Check WiFi connection periodically
     if (millis() - lastWiFiCheck >= WIFI_CHECK_INTERVAL) {
       if (WiFi.status() != WL_CONNECTED) {
         Serial.println("WiFi disconnected, attempting to reconnect...");
         wifiConnected = false;
         connectToWiFi();
       }
       lastWiFiCheck = millis();
     }

     // Handle LoRa reception
     if (Serial1.available()) {
       String resp = Serial1.readStringUntil('\n');
       resp.trim();

       if (resp.length() > 0) {
         Serial.println("LoRa RX: " + resp);

         // Debug: Show all responses for troubleshooting
         Serial.println("Debug - Raw response: [" + resp + "]");

         // Check for different types of LoRa events
         if (resp.startsWith("+EVT:RXP2P")) {
           Serial.println("📡 P2P Packet detected!");
           Serial.println("========================================");
           Serial.println("🚀 LORA PACKET RECEIVED!");
           Serial.println("========================================");

           // Extract payload from response
           int idx = resp.lastIndexOf(':');
           if (idx != -1) {
             String hexPayload = resp.substring(idx + 1);
             Serial.print("📦 Raw HEX payload: ");
             Serial.println(hexPayload);

             // Convert HEX to decimal
             int waterLevel = hexToDec(hexPayload);
             currentWaterLevel = waterLevel;

             // Calculate percentage (assuming 200cm max depth)
             currentPercentage = map(waterLevel, 0, 200, 0, 100);
             currentPercentage = constrain(currentPercentage, 0, 100);

             Serial.println("📊 --- DECODED WATER LEVEL DATA ---");
             Serial.print("💧 Water level: ");
             Serial.print(waterLevel);
             Serial.println(" cm");
             Serial.print("📈 Tank fill: ");
             Serial.print(currentPercentage);
             Serial.println("%");
             Serial.println("========================================");

             // Send data to website if WiFi is connected
             if (wifiConnected) {
               Serial.println("🌐 Sending data to website...");
               if (retrySendData(waterLevel, currentPercentage, MAX_RETRY_ATTEMPTS)) {
                 Serial.println("✅ Data successfully sent to website!");
               } else {
                 Serial.println("❌ Failed to send data to website");
               }
             } else {
               Serial.println("📵 WiFi not connected - data stored locally only");
               Serial.println("💡 Data available for local monitoring");
             }

             // Re-enable RX
             Serial1.println("AT+PRECV=65535");
             Serial.println("🔄 Listening for next packet...");
             Serial.println("========================================");
           } else {
             Serial.println("⚠️  ERROR: Could not parse LoRa packet!");
           }
         }
       }
     }

     // Periodic status update for debugging
     static unsigned long lastStatusUpdate = 0;
     if (millis() - lastStatusUpdate > 5000) {  // Every 5 seconds
       Serial.println("========================================");
       Serial.println("📡 RX Status: Waiting for LoRa packets...");
       Serial.print("📶 WiFi Connected: ");
       Serial.println(wifiConnected ? "YES" : "NO");
       Serial.print("🕐 Uptime: ");
       Serial.print(millis() / 1000);
       Serial.println(" seconds");

       // Test LoRa module responsiveness
       Serial.println("🔍 Testing LoRa module responsiveness...");
       Serial1.println("AT");
       delay(100);

       // Ensure we're still in RX mode
       Serial.println("🔄 Ensuring RX mode is active...");
       Serial1.println("AT+PRECV=65535");
       delay(200);

       Serial.println("========================================");
       lastStatusUpdate = millis();
     }

     delay(100);
   }
