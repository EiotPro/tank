#include <Arduino.h>
#include <HardwareSerial.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <Arduino_JSON.h>
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

   HTTPClient http;
   String url = "http://" + String(WEBSITE_HOST) + ":" + String(WEBSITE_PORT) + API_ENDPOINT;

   // Create JSON payload
   String jsonPayload = "{\"tank_id\":1,\"level\":" + String(waterLevel) +
                       ",\"percentage\":" + String(percentage) +
                       ",\"api_key\":\"" + String(API_KEY) + "\"}";

   Serial.println("Sending data to: " + url);
   Serial.println("Payload: " + jsonPayload);

   http.begin(url);
   http.addHeader("Content-Type", "application/json");
   http.setTimeout(HTTP_TIMEOUT);

   int httpResponseCode = http.POST(jsonPayload);

   if (httpResponseCode > 0) {
     String response = http.getString();
     Serial.println("HTTP Response code: " + String(httpResponseCode));
     Serial.println("Response: " + response);

     if (httpResponseCode == 200 && response.indexOf("OK") != -1) {
       Serial.println("Data sent successfully to website");
       http.end();
       return true;
     }
   } else {
     Serial.println("Error sending data: " + String(httpResponseCode));
     Serial.println("Error: " + http.errorToString(httpResponseCode));
   }

   http.end();
   return false;
 }

// Function to retry sending data with exponential backoff
void retrySendData(int waterLevel, int percentage, int maxAttempts) {
   for (int attempt = 1; attempt <= maxAttempts; attempt++) {
     Serial.println("Attempt " + String(attempt) + " of " + String(maxAttempts));

     if (sendDataToWebsite(waterLevel, percentage)) {
       return; // Success, exit retry loop
     }

     if (attempt < maxAttempts) {
       Serial.println("Retrying in " + String(RETRY_DELAY / 1000) + " seconds...");
       delay(RETRY_DELAY);
     }
   }

   Serial.println("Failed to send data after " + String(maxAttempts) + " attempts");
 }



void setup() {
   Serial.begin(SERIAL_BAUD_RATE);
   Serial1.begin(LORA_BAUD_RATE);

   // Initialize LoRa
   pinMode(RAK3172_RESET, OUTPUT);
   digitalWrite(RAK3172_RESET, HIGH);
   delay(2000);

   Serial.println("=== LoRa Water Tank RX Node with WiFi ===");
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

           // Send data to website if WiFi is connected
           if (wifiConnected) {
             Serial.println("Attempting to send data to website...");
             retrySendData(waterLevel, currentPercentage, MAX_RETRY_ATTEMPTS);
           } else {
             Serial.println("WiFi not connected, data not sent to website");
           }

           // Re-enable RX
           Serial1.println("AT+PRECV=65535");
         }
       }
     }
   }

   delay(100);
 }
