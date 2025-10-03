#include <Arduino.h>
#include <HardwareSerial.h>
#include <WiFi.h>
#include "config.h"
#include "wifi_credentials.h"
#include "wifi_manager.h"
#include "led_status.h"

// Hardware Configuration
#define CONFIG_BUTTON_PIN 17  // GPIO pin for configuration button (hold to enter AP mode) - OPTIONAL
#define BUTTON_HOLD_TIME 3000 // Hold button for 3 seconds to enter config mode
#define USE_CONFIG_BUTTON false  // Set to true if you have a physical button wired

// Alternative config mode triggers
#define MAX_WIFI_RETRY_ATTEMPTS 3  // After 3 failed attempts, enter config mode
#define BOOT_COUNT_FILE "/boot_count.txt"  // File to track power cycles

// Global variables
int currentWaterLevel = -1;  // Initialize to -1 (invalid) until first packet
int currentPercentage = -1;  // Initialize to -1 (invalid) until first packet
bool wifiConnected = false;
unsigned long lastWiFiCheck = 0;
const unsigned long WIFI_CHECK_INTERVAL = 60000; // Check WiFi every 60 seconds (increased from 30s)

// LoRa data tracking
volatile bool hasLoRaData = false;  // Flag to indicate if we've received valid LoRa data
volatile unsigned long lastLoRaMillis = 0;  // Timestamp of last valid LoRa packet
const unsigned long DATA_TIMEOUT_MS = 30000;  // 30 seconds - mark data as stale if no packet

// System managers
WiFiCredentials wifiCreds;
WiFiManager wifiMgr;
LEDStatusManager ledStatus;

// Configuration mode state
bool inConfigMode = false;
unsigned long buttonPressStart = 0;
bool buttonWasPressed = false;
int wifiRetryCount = 0;
unsigned long lastBootTime = 0;

// Forward declaration
void enterConfigMode();
void checkSerialCommands();
bool checkTriplePowerCycle();
void clearBootCount();

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

/**
 * Check if config button is being held (OPTIONAL - only if USE_CONFIG_BUTTON is true)
 */
void checkConfigButton() {
   if (!USE_CONFIG_BUTTON) return;  // Skip if no button configured
   
   bool buttonPressed = (digitalRead(CONFIG_BUTTON_PIN) == LOW); // Active LOW (with pullup)
   
   if (buttonPressed && !buttonWasPressed) {
     // Button just pressed
     buttonPressStart = millis();
     buttonWasPressed = true;
     Serial.println("🔘 Config button pressed...");
   }
   else if (buttonPressed && buttonWasPressed) {
     // Button is being held
     if (millis() - buttonPressStart >= BUTTON_HOLD_TIME && !inConfigMode) {
       Serial.println("🔘 Config button held - entering AP mode!");
       enterConfigMode();
     }
   }
   else if (!buttonPressed && buttonWasPressed) {
     // Button released
     buttonWasPressed = false;
     Serial.println("🔘 Config button released");
   }
}

/**
 * Check for serial commands to enter config mode
 */
void checkSerialCommands() {
   if (Serial.available()) {
     String command = Serial.readStringUntil('\n');
     command.trim();
     command.toLowerCase();
     
     if (command == "config" || command == "setup" || command == "wifi") {
       Serial.println("\n🔧 Serial command received: Entering config mode...");
       enterConfigMode();
     }
     else if (command == "clear" || command == "reset") {
       Serial.println("\n🗑️  Clearing WiFi credentials...");
       wifiCreds.clear();
       Serial.println("✅ Credentials cleared. Rebooting...");
       delay(2000);
       rp2040.reboot();
     }
     else if (command == "status" || command == "info") {
       Serial.println("\n📊 SYSTEM STATUS:");
       Serial.println("   WiFi: " + String(wifiConnected ? "Connected" : "Disconnected"));
       if (wifiConnected) {
         Serial.println("   IP: " + WiFi.localIP().toString());
         Serial.println("   SSID: " + WiFi.SSID());
       }
       Serial.println("   Has Credentials: " + String(wifiCreds.hasCredentials() ? "Yes" : "No"));
       if (wifiCreds.hasCredentials()) {
         Serial.println("   Saved SSID: " + wifiCreds.getSSID());
       }
       Serial.println("   In Config Mode: " + String(inConfigMode ? "Yes" : "No"));
       Serial.println("   Uptime: " + String(millis() / 1000) + "s");
     }
     else if (command == "help" || command == "?") {
       Serial.println("\n📋 AVAILABLE COMMANDS:");
       Serial.println("   config/setup/wifi - Enter configuration mode");
       Serial.println("   clear/reset       - Clear WiFi credentials");
       Serial.println("   status/info       - Show system status");
       Serial.println("   help/?            - Show this help");
       Serial.println("   reboot            - Restart device");
     }
     else if (command == "reboot" || command == "restart") {
       Serial.println("\n🔄 Rebooting...");
       delay(1000);
       rp2040.reboot();
     }
   }
}

/**
 * Check for triple power cycle (power on/off 3 times within 10 seconds)
 * This allows entering config mode by cycling power 3 times quickly
 */
bool checkTriplePowerCycle() {
   // Check if boot count file exists
   if (!LittleFS.exists(BOOT_COUNT_FILE)) {
     // First boot or file deleted - create it
     File f = LittleFS.open(BOOT_COUNT_FILE, "w");
     if (f) {
       f.println("1");
       f.println(String(millis()));
       f.close();
     }
     return false;
   }
   
   // Read boot count and last boot time
   File f = LittleFS.open(BOOT_COUNT_FILE, "r");
   if (!f) return false;
   
   int bootCount = f.readStringUntil('\n').toInt();
   unsigned long lastBoot = f.readStringUntil('\n').toInt();
   f.close();
   
   unsigned long timeSinceLastBoot = millis();
   
   // If last boot was within 10 seconds, increment counter
   if (timeSinceLastBoot < 10000 && lastBoot > 0) {
     bootCount++;
     Serial.println("⚡ Quick power cycle detected (" + String(bootCount) + "/3)");
     
     if (bootCount >= 3) {
       Serial.println("🔧 Triple power cycle detected! Entering config mode...");
       // Clear boot count
       LittleFS.remove(BOOT_COUNT_FILE);
       return true;
     }
   } else {
     // More than 10 seconds since last boot - reset counter
     bootCount = 1;
   }
   
   // Update boot count file
   f = LittleFS.open(BOOT_COUNT_FILE, "w");
   if (f) {
     f.println(String(bootCount));
     f.println(String(millis()));
     f.close();
   }
   
   return false;
}

/**
 * Clear boot count after successful WiFi connection
 */
void clearBootCount() {
   if (LittleFS.exists(BOOT_COUNT_FILE)) {
     LittleFS.remove(BOOT_COUNT_FILE);
   }
}

/**
 * Enter configuration mode (AP mode)
 */
void enterConfigMode() {
   Serial.println("\n========================================");
   Serial.println("🔧 ENTERING CONFIGURATION MODE");
   Serial.println("========================================");
   
   inConfigMode = true;
   ledStatus.setStatus(LED_AP_MODE);
   
   // Start WiFi Manager in AP mode
   wifiMgr.startAPMode();
   
   Serial.println("📱 Configuration Portal Active!");
   Serial.println("   SSID: " + wifiMgr.getAPSSID());
   Serial.println("   IP: 192.168.4.1");
   Serial.println("   Open browser and connect to configure WiFi");
   Serial.println("========================================\n");
}

/**
 * Connect to WiFi using saved credentials (NON-BLOCKING)
 */
void connectToWiFi() {
   if (WiFi.status() == WL_CONNECTED) {
     wifiConnected = true;
     ledStatus.setStatus(LED_CONNECTED);
     return;
   }

   ledStatus.setStatus(LED_CONNECTING);
   Serial.println("📡 Connecting to WiFi...");
   
   // Load credentials from storage
   if (!wifiCreds.hasCredentials()) {
     Serial.println("❌ No WiFi credentials found!");
     ledStatus.setStatus(LED_ERROR);
     enterConfigMode();
     return;
   }
   
   String ssid = wifiCreds.getSSID();
   String password = wifiCreds.getPassword();
   
   Serial.println("   SSID: " + ssid);
   WiFi.begin(ssid.c_str(), password.c_str());

   // Non-blocking connection - check status with timeout
   unsigned long startAttempt = millis();
   int attempts = 0;
   const int MAX_CONNECT_TIME = 10000; // 10 seconds timeout
   
   while (WiFi.status() != WL_CONNECTED && (millis() - startAttempt) < MAX_CONNECT_TIME) {
     delay(100);  // Very short delay - allows LoRa to still function
     
     if ((millis() - startAttempt) % 500 == 0) {
       Serial.print(".");
       ledStatus.update();
     }
     
     // Process LoRa even during WiFi connection attempt
     if (Serial1.available()) {
       String resp = Serial1.readStringUntil('\n');
       // Buffer for later processing if needed
     }
   }

   if (WiFi.status() == WL_CONNECTED) {
     wifiConnected = true;
     wifiRetryCount = 0;  // Reset retry counter on success
     ledStatus.setStatus(LED_CONNECTED);
     Serial.println("\n✅ WiFi connected successfully!");
     Serial.print("   IP Address: ");
     Serial.println(WiFi.localIP());
     clearBootCount();  // Clear power cycle counter
   } else {
     wifiConnected = false;
     wifiRetryCount++;
     ledStatus.setStatus(LED_ERROR);
     Serial.println("\n❌ WiFi connection failed!");
     Serial.println("   Attempt " + String(wifiRetryCount) + "/" + String(MAX_WIFI_RETRY_ATTEMPTS));
     Serial.println("\n💡 To enter config mode:");
     Serial.println("   • Type 'config' in serial monitor");
     Serial.println("   • Power cycle 3 times within 10 seconds");
     if (USE_CONFIG_BUTTON) {
       Serial.println("   • Hold config button for 3 seconds");
     }
     
     // After multiple failures, automatically enter config mode
     if (wifiRetryCount >= MAX_WIFI_RETRY_ATTEMPTS) {
       Serial.println("\n⚠️  Maximum retry attempts reached!");
       Serial.println("🔧 Automatically entering config mode...");
       delay(1000);  // Reduced from 2000ms
       enterConfigMode();
     }
   }
 }

// Function to test API accessibility
bool testAPIAccessibility() {
    WiFiClient client;

    Serial.println("🔍 Testing main API endpoint...");

    if (!client.connect(WEBSITE_HOST, WEBSITE_PORT)) {
       Serial.println("❌ Cannot connect to server");
       return false;
    }

    // Test main API endpoint with GET request (main endpoint only accepts POST for data)
    String request = "GET " + String(API_ENDPOINT) + " HTTP/1.1\r\n";
    request += "Host: " + String(WEBSITE_HOST) + "\r\n";
    request += "User-Agent: ESP32-Test\r\n";
    request += "Connection: close\r\n\r\n";

    client.print(request);

    unsigned long timeout = millis();
    while (client.available() == 0) {
       if (millis() - timeout > 5000) {
         Serial.println("⏰ Test timeout - endpoint may be blocked");
         client.stop();
         return false;
       }
    }

    String response = "";
    while (client.available()) {
       String line = client.readStringUntil('\n');
       response += line;
    }

    // Check for success indicators
    if (response.indexOf("status") >= 0 || response.indexOf("error") >= 0) {
        Serial.println("✅ API endpoint is accessible");
        client.stop();
        return true;
    } else if (response.indexOf("405") >= 0) {
        Serial.println("✅ API endpoint accessible (405 = method not allowed for GET)");
        client.stop();
        return true;
    } else if (response.indexOf("403") >= 0) {
       Serial.println("❌ API endpoint blocked: 403 Forbidden");
       Serial.println("💡 Check if files are uploaded to server");
       client.stop();
       return false;
    } else if (response.indexOf("404") >= 0) {
       Serial.println("❌ API endpoint not found: 404");
       Serial.println("💡 Check file path: " + String(API_ENDPOINT));
       client.stop();
       return false;
    } else {
       Serial.println("❓ Unexpected response: " + response.substring(0, 150));
       client.stop();
       return false;
    }
}

// Function to send data to website
bool sendDataToWebsite(int waterLevel, int percentage) {
    if (!wifiConnected) {
       Serial.println("WiFi not connected, skipping data send");
       return false;
    }

    WiFiClient client;

    // Create JSON payload with API key for main endpoint
    String jsonPayload = "{\"tank_id\":1,\"level\":" + String(waterLevel) +
                          ",\"percentage\":" + String(percentage) +
                          ",\"api_key\":\"" + String(API_KEY) + "\"}";

    Serial.println("🌐 Sending data to website...");
    Serial.println("Host: " + String(WEBSITE_HOST));
    Serial.println("Port: " + String(WEBSITE_PORT));
    Serial.println("Endpoint: " + String(API_ENDPOINT));
    Serial.println("Payload: " + jsonPayload);

    // First, try to test if API directory is accessible
    Serial.println("🔍 Testing API accessibility...");

    // Connect to server (HTTP only, no HTTPS)
    Serial.println("🔌 Connecting to " + String(WEBSITE_HOST) + ":" + String(WEBSITE_PORT) + "...");
    if (!client.connect(WEBSITE_HOST, WEBSITE_PORT)) {
       Serial.println("❌ Connection to website failed!");
       Serial.println("Check host: " + String(WEBSITE_HOST) + ":" + String(WEBSITE_PORT));
       return false;
    }

    Serial.println("✅ Connected to server successfully");

    // Send HTTP POST request (CORRECT FORMAT for HTTP)
    String request = "POST " + String(API_ENDPOINT) + " HTTP/1.1\r\n";
    request += "Host: " + String(WEBSITE_HOST) + "\r\n";
    request += "User-Agent: ESP32-Tank-Monitor\r\n";
    request += "Content-Type: application/json\r\n";
    request += "Content-Length: " + String(jsonPayload.length()) + "\r\n";
    request += "Connection: close\r\n\r\n";
    request += jsonPayload + "\r\n";  // Extra line ending

    Serial.println("📤 Sending HTTP request...");
    client.print(request);

    // Wait for response
    unsigned long timeout = millis();
    while (client.available() == 0) {
       if (millis() - timeout > HTTP_TIMEOUT) {
         Serial.println("⏰ Client timeout!");
         client.stop();
         return false;
       }
    }

    // Read response
    String response = "";
    while (client.available()) {
       String line = client.readStringUntil('\n');
       response += line;
       // Also print each line for debugging
       Serial.println("Response line: " + line);
    }

    Serial.println("Full Response: " + response);

    // Check for success (HTTP 200 or 201)
    if (response.indexOf("200") >= 0 || response.indexOf("201") >= 0) {
       Serial.println("✅ Data sent successfully to website!");
       client.stop();
       return true;
    } else if (response.indexOf("403") >= 0) {
       Serial.println("🚫 403 Forbidden - Check API endpoint and permissions");
       client.stop();
       return false;
    } else if (response.indexOf("404") >= 0) {
       Serial.println("❌ 404 Not Found - Check API endpoint URL");
       client.stop();
       return false;
    } else if (response.indexOf("500") >= 0) {
       Serial.println("💥 500 Server Error - Check server configuration");
       client.stop();
       return false;
    } else {
       Serial.println("❓ Unexpected response - check server logs");
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
   delay(1000); // Wait for serial to initialize
   
   Serial.println("\n\n========================================");
   Serial.println("🚀 PICO W TANK MONITOR - SMART BOOT");
   Serial.println("========================================\n");
   
   // Initialize LED status
   ledStatus.begin();
   ledStatus.setStatus(LED_OFF);
   
   // Initialize config button (optional)
   if (USE_CONFIG_BUTTON) {
     pinMode(CONFIG_BUTTON_PIN, INPUT_PULLUP);
     Serial.println("✅ Config button initialized on GPIO" + String(CONFIG_BUTTON_PIN));
     Serial.println("   Hold button for 3s to enter config mode\n");
   } else {
     Serial.println("ℹ️  Physical config button disabled");
     Serial.println("\n📋 Alternative config mode triggers:");
     Serial.println("   1. Serial command: Type 'config' in serial monitor");
     Serial.println("   2. Power cycle: Power on/off 3 times within 10 seconds");
     Serial.println("   3. Auto-trigger: After 3 failed WiFi connection attempts");
     Serial.println("   4. Type 'help' for all commands\n");
   }
   
   // Initialize WiFi credential storage
   Serial.println("📂 Initializing credential storage...");
   wifiCreds.begin();
   wifiCreds.load();
   
   // Link credentials to WiFi manager
   wifiMgr.setCredentials(&wifiCreds);
   
   // Link live data to WiFi manager for dashboard
   wifiMgr.setLiveData(&currentWaterLevel, &currentPercentage, &wifiConnected);
   
   // Check for triple power cycle trigger
   if (checkTriplePowerCycle()) {
     enterConfigMode();
   }
   
   // Smart boot logic: Check for saved credentials
   if (!wifiCreds.hasCredentials()) {
     Serial.println("\n⚠️  No WiFi credentials found!");
     Serial.println("   Starting in configuration mode...\n");
     enterConfigMode();
   } else {
     Serial.println("\n✅ Found saved WiFi credentials");
     Serial.println("   SSID: " + wifiCreds.getSSID());
     Serial.println("   API Endpoint: " + wifiCreds.getAPIEndpoint());
     Serial.println("   Attempting to connect...\n");
   }
   
   // Initialize LoRa
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

   // Connect to WiFi (unless in config mode)
   if (!inConfigMode) {
     Serial.println("\n📡 Initializing WiFi connection...");
     connectToWiFi();
   }

   Serial.println("\n========================================");
   if (inConfigMode) {
     Serial.println("⚙️  System in CONFIGURATION MODE");
     Serial.println("   Connect to: " + wifiMgr.getAPSSID());
     Serial.println("   IP: 192.168.4.1");
   } else {
     Serial.println("✅ RX Node initialization complete");
     Serial.println("   WiFi: " + String(wifiConnected ? "Connected" : "Disconnected"));
     Serial.println("   Ready to receive LoRa data");
   }
   Serial.println("========================================\n");
}

void loop() {
     // Update LED status
     ledStatus.update();
     
     // Check for serial commands (always active)
     checkSerialCommands();
     
     // Check config button (only if enabled)
     if (USE_CONFIG_BUTTON) {
       checkConfigButton();
     }
     
     // If in config mode, handle web server and skip normal operations
     if (inConfigMode) {
       wifiMgr.handleClient();
       return;
     }
     
     // ⏰ DATA TIMEOUT CHECK: Mark data as stale if no packet received recently
     if (hasLoRaData && (millis() - lastLoRaMillis > DATA_TIMEOUT_MS)) {
       Serial.println("⚠️  WARNING: No LoRa data received for " + String(DATA_TIMEOUT_MS / 1000) + " seconds");
       Serial.println("🚨 Marking data as stale...");
       hasLoRaData = false;
       // Keep last values but flag as stale for web portal
     }
     
     // Check WiFi connection periodically (non-blocking)
     if (millis() - lastWiFiCheck >= WIFI_CHECK_INTERVAL) {
       if (WiFi.status() != WL_CONNECTED && wifiConnected) {
         Serial.println("📵 WiFi disconnected, will attempt reconnect...");
         wifiConnected = false;
         ledStatus.setStatus(LED_CONNECTING);
         // Attempt non-blocking reconnect with saved credentials
         if (wifiCreds.hasCredentials()) {
           WiFi.begin(wifiCreds.getSSID().c_str(), wifiCreds.getPassword().c_str());
         }
       } else if (WiFi.status() == WL_CONNECTED && !wifiConnected) {
         Serial.println("✅ WiFi reconnected!");
         wifiConnected = true;
         ledStatus.setStatus(LED_CONNECTED);
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
          Serial.println("🚀 OPTIMIZED LORA PACKET RECEIVED!");
          Serial.println("========================================");

          // Extract payload from response
          int idx = resp.lastIndexOf(':');
          if (idx != -1) {
            String hexPayload = resp.substring(idx + 1);
            hexPayload.trim();
            Serial.print("📦 Raw HEX payload: ");
            Serial.println(hexPayload);

            // ✅ VALIDATION: Check payload length (should be 4 hex chars for 2-byte value)
            if (hexPayload.length() != 4) {
              Serial.println("⚠️  ERROR: Invalid payload length (expected 4, got " + String(hexPayload.length()) + ")");
              Serial.println("🛑 Packet discarded");
              Serial1.println("AT+PRECV=65535");  // Re-enable RX
              return;
            }

            // 🚀 Convert HEX to raw sensor value (0-1023)
            int rawSensorValue = hexToDec(hexPayload);
            Serial.print("📡 Raw sensor value received: ");
            Serial.println(rawSensorValue);

            // ✅ VALIDATION: Bounds check (must be 0-1023)
            if (rawSensorValue < 0 || rawSensorValue > 1023) {
              Serial.println("⚠️  ERROR: Raw value out of range (0-1023): " + String(rawSensorValue));
              Serial.println("🛑 Packet discarded");
              Serial1.println("AT+PRECV=65535");  // Re-enable RX
              return;
            }

            // 💧 Calculate water level from raw sensor value
            // TX sends: map(distance, 0, 200, 1023, 0)
            // So: Raw 1023 = 0cm distance (full tank = 200cm water)
            //     Raw 0 = 200cm distance (empty tank = 0cm water)
            // Therefore: waterLevel = Raw * 200 / 1023
            int waterLevel = map(rawSensorValue, 0, 1023, 0, TANK_MAX_DEPTH);
            waterLevel = constrain(waterLevel, 0, TANK_MAX_DEPTH);
            
            // 📈 Calculate percentage
            int percentage = map(waterLevel, 0, TANK_MAX_DEPTH, 0, 100);
            percentage = constrain(percentage, 0, 100);

            // ✅ UPDATE GLOBALS: Mark data as valid and update timestamp
            currentWaterLevel = waterLevel;
            currentPercentage = percentage;
            hasLoRaData = true;
            lastLoRaMillis = millis();

            Serial.println("🧮 --- CALCULATIONS DONE ON RECEIVER ---");
            Serial.print("💧 Water level: ");
            Serial.print(waterLevel);
            Serial.println(" cm");
            Serial.print("📈 Tank fill: ");
            Serial.print(percentage);
            Serial.println("%");
            Serial.println("✅ Packet validated and processed successfully!");
            Serial.println("========================================");

             // Send data to website if WiFi is connected
             if (wifiConnected) {
               Serial.println("🌐 Sending data to website...");
               
               // Flash LED to indicate transmission
               ledStatus.flashTransmit();

               // Use saved API endpoint from credentials
               String apiEndpoint = wifiCreds.getAPIEndpoint();
               if (apiEndpoint.length() == 0) {
                 apiEndpoint = API_ENDPOINT; // Fallback to config.h
               }

               // First test if API is accessible
               if (testAPIAccessibility()) {
                 Serial.println("🚀 API is accessible, sending data...");
                 if (retrySendData(waterLevel, currentPercentage, MAX_RETRY_ATTEMPTS)) {
                   Serial.println("✅ Data successfully sent to website!");
                 } else {
                   Serial.println("❌ Failed to send data to website");
                   ledStatus.setStatus(LED_ERROR);
                   delay(2000);
                   ledStatus.setStatus(LED_CONNECTED);
                 }
               } else {
                 Serial.println("🚫 API not accessible - check server configuration");
                 Serial.println("💡 Try: https://iotlogic.in" + apiEndpoint);
                 Serial.println("💡 Make sure API endpoint is configured correctly");
               }
             } else {
               Serial.println("📵 WiFi not connected - data stored locally only");
               Serial.println("💡 Data available for local monitoring");
             }

             // Re-enable RX
             Serial1.println("AT+PRECV=65535");
             Serial.println("🔄 Listening for next optimized packet...");
             Serial.println("========================================");
           } else {
             Serial.println("⚠️  ERROR: Could not parse LoRa packet!");
           }
         }
       }
     }

     // Periodic status update for debugging (non-blocking)
     static unsigned long lastStatusUpdate = 0;
     static unsigned long lastLoRaCheck = 0;
     
     if (millis() - lastStatusUpdate > 10000) {  // Every 10 seconds (reduced frequency)
       Serial.println("========================================");
       Serial.println("📡 RX Status: Waiting for optimized LoRa packets...");
       Serial.print("📶 WiFi Connected: ");
       Serial.println(wifiConnected ? "YES" : "NO");
       Serial.print("📊 Data Status: ");
       Serial.println(hasLoRaData ? "Valid" : "No data/Stale");
       if (hasLoRaData) {
         Serial.print("   Last packet: ");
         Serial.print((millis() - lastLoRaMillis) / 1000);
         Serial.println("s ago");
       }
       Serial.print("🕐 Uptime: ");
       Serial.print(millis() / 1000);
       Serial.println(" seconds");
       Serial.println("========================================");
       lastStatusUpdate = millis();
     }
     
     // Periodically ensure LoRa RX mode is active (every 30s, non-blocking)
     if (millis() - lastLoRaCheck > 30000) {
       Serial.println("🔄 Re-enabling LoRa RX mode...");
       Serial1.println("AT+PRECV=65535");
       lastLoRaCheck = millis();
     }

     // NO DELAY - keep loop responsive for LoRa packets!
   }
