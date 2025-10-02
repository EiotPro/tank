#include <Arduino.h>
#include <HardwareSerial.h>
#include <stdio.h>
#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include "config.h"

// Global variables for sensor
int lastValidDistance = 0;
int lastRawSensorValue = 0;
int currentWaterLevel = 0;
int currentPercentage = 0;
int currentDistance = 0;
bool loraTransmitSuccess = false;
unsigned long lastTransmitTime = 0;

// Web server and DNS for monitoring portal
WebServer server(80);
DNSServer dnsServer;
const byte DNS_PORT = 53;
bool apMode = true;  // Always run in AP mode for TX monitoring
String apSSID = "TankTX-Monitor";

// 📊 TX Monitoring Portal HTML
const char* getMonitorHTML() {
  return R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>TX Sensor Monitor</title>
    <style>
        * { margin: 0; padding: 0; box-sizing: border-box; }
        body {
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            min-height: 100vh;
            display: flex;
            align-items: center;
            justify-content: center;
            padding: 20px;
        }
        .container {
            background: white;
            border-radius: 20px;
            box-shadow: 0 20px 60px rgba(0, 0, 0, 0.3);
            max-width: 500px;
            width: 100%;
            overflow: hidden;
        }
        .header {
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            color: white;
            padding: 30px 20px;
            text-align: center;
        }
        .header h1 { font-size: 1.8rem; margin-bottom: 5px; }
        .header p { opacity: 0.9; font-size: 0.9rem; }
        .content { padding: 30px; }
        .data-card {
            background: linear-gradient(135deg, #f5f7fa 0%, #c3cfe2 100%);
            border-radius: 15px;
            padding: 20px;
            margin-bottom: 20px;
            box-shadow: 0 4px 15px rgba(0, 0, 0, 0.1);
        }
        .data-row {
            display: flex;
            justify-content: space-between;
            align-items: center;
            padding: 12px 0;
            border-bottom: 1px solid rgba(255, 255, 255, 0.3);
        }
        .data-row:last-child { border-bottom: none; }
        .data-label {
            font-size: 0.9rem;
            color: #555;
            font-weight: 500;
        }
        .data-value {
            font-size: 1.5rem;
            font-weight: 700;
            color: #667eea;
        }
        .tank-visual {
            width: 100%;
            height: 200px;
            background: linear-gradient(to bottom, #e0e0e0 0%, #f5f5f5 100%);
            border-radius: 15px;
            position: relative;
            overflow: hidden;
            border: 3px solid #667eea;
            margin: 20px 0;
        }
        .water-fill {
            position: absolute;
            bottom: 0;
            width: 100%;
            background: linear-gradient(to top, #3498db 0%, #5dade2 100%);
            transition: height 0.5s ease;
        }
        .tank-percentage {
            position: absolute;
            top: 50%;
            left: 50%;
            transform: translate(-50%, -50%);
            font-size: 2.5rem;
            font-weight: 700;
            color: #2c3e50;
            z-index: 10;
        }
        .status-badge {
            display: inline-block;
            padding: 8px 16px;
            border-radius: 20px;
            font-size: 0.85rem;
            font-weight: 600;
            text-transform: uppercase;
        }
        .status-success { background: #2ecc71; color: white; }
        .status-error { background: #e74c3c; color: white; }
        .status-warning { background: #f39c12; color: white; }
        .timestamp {
            text-align: center;
            color: #999;
            font-size: 0.85rem;
            margin-top: 15px;
        }
    </style>
</head>
<body>
    <div class="container">
        <div class="header">
            <h1>🔊 TX Sensor Monitor</h1>
            <p>Real-time Ultrasonic Sensor Data</p>
        </div>
        <div class="content">
            <div class="data-card">
                <div class="data-row">
                    <span class="data-label">📏 Distance to Water</span>
                    <span class="data-value" id="distance">--</span>
                </div>
                <div class="data-row">
                    <span class="data-label">💧 Water Level</span>
                    <span class="data-value" id="waterLevel">--</span>
                </div>
                <div class="data-row">
                    <span class="data-label">📊 Tank Fill</span>
                    <span class="data-value" id="percentage">--</span>
                </div>
            </div>
            
            <div class="tank-visual">
                <div class="water-fill" id="waterFill"></div>
                <div class="tank-percentage" id="tankPercent">0%</div>
            </div>
            
            <div class="data-card">
                <div class="data-row">
                    <span class="data-label">📡 LoRa Status</span>
                    <span class="status-badge" id="loraStatus">Waiting...</span>
                </div>
                <div class="data-row">
                    <span class="data-label">🕐 Last Transmission</span>
                    <span class="data-value" style="font-size:1rem" id="lastTransmit">--</span>
                </div>
            </div>
            
            <div class="timestamp" id="lastUpdate">Waiting for data...</div>
        </div>
    </div>
    
    <script>
        function fetchSensorData() {
            fetch('/data')
                .then(response => response.json())
                .then(data => {
                    // Update values
                    document.getElementById('distance').textContent = data.distance + ' cm';
                    document.getElementById('waterLevel').textContent = data.water_level + ' cm';
                    document.getElementById('percentage').textContent = data.percentage + '%';
                    document.getElementById('tankPercent').textContent = data.percentage + '%';
                    
                    // Update tank visual
                    document.getElementById('waterFill').style.height = data.percentage + '%';
                    
                    // Update LoRa status
                    const loraStatus = document.getElementById('loraStatus');
                    if (data.lora_success) {
                        loraStatus.textContent = '✅ Transmitted';
                        loraStatus.className = 'status-badge status-success';
                    } else {
                        loraStatus.textContent = '❌ Failed';
                        loraStatus.className = 'status-badge status-error';
                    }
                    
                    // Update last transmission time
                    document.getElementById('lastTransmit').textContent = data.last_transmit + 's ago';
                    
                    // Update timestamp
                    const now = new Date();
                    document.getElementById('lastUpdate').textContent = 'Updated: ' + now.toLocaleTimeString();
                })
                .catch(error => {
                    console.error('Error fetching data:', error);
                });
        }
        
        // Fetch immediately and then every 2 seconds
        fetchSensorData();
        setInterval(fetchSensorData, 2000);
    </script>
</body>
</html>
  )rawliteral";
}

// 📡 Function to read ultrasonic sensor using TRIGGER and ECHO method
long readUltrasonicDistance() {
  // Clear trigger pin
  digitalWrite(ULTRASONIC_TRIG_PIN, LOW);
  delayMicroseconds(2);
  
  // Send 10us HIGH pulse to TRIGGER pin
  digitalWrite(ULTRASONIC_TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(ULTRASONIC_TRIG_PIN, LOW);
  
  // Read ECHO pin - measure pulse duration
  long duration = pulseIn(ULTRASONIC_ECHO_PIN, HIGH, ULTRASONIC_TIMEOUT);
  
  if (duration == 0) {
    Serial.println("⚠️  No echo received - sensor timeout");
    return -1;
  }
  
  // Calculate distance in cm
  // Speed of sound = 343 m/s = 0.0343 cm/µs
  // Distance = (time / 2) * speed
  // Divide by 2 because signal travels to object and back
  long distance = duration * 0.0343 / 2;
  
  // Validate reading
  if (distance < 2 || distance > ULTRASONIC_MAX_DISTANCE) {
    Serial.println("⚠️  Distance out of range: " + String(distance) + " cm");
    return -1;
  }
  
  lastValidDistance = distance;
  return distance;
}

// Function to read water level
int readWaterLevel() {
  int waterLevel = 0;
  
#if USE_ULTRASONIC
  // Use ultrasonic sensor (trigger/echo method)
  long distanceToWater = readUltrasonicDistance();
  
  if (distanceToWater >= 0) {
    // Sensor mounted at TOP of tank
    // Distance = empty space above water
    // Water level = Total tank depth - empty space
    waterLevel = TANK_MAX_DEPTH - distanceToWater;
    
    // Constrain to valid range
    waterLevel = constrain(waterLevel, 0, TANK_MAX_DEPTH);
    
    Serial.print("📏 Distance to water surface: ");
    Serial.print(distanceToWater);
    Serial.println(" cm");
  } else {
    // Use last valid reading if current fails
    waterLevel = TANK_MAX_DEPTH - lastValidDistance;
    waterLevel = constrain(waterLevel, 0, TANK_MAX_DEPTH);
    Serial.println("⚠️  Using last valid reading");
  }
  
#else
  // Use analog sensor fallback
  int sensorValue = analogRead(WATER_SENSOR_PIN);
  waterLevel = map(sensorValue, 0, 1023, TANK_MAX_DEPTH, 0);
  waterLevel = constrain(waterLevel, 0, TANK_MAX_DEPTH);
#endif

  return waterLevel;
}

// Function to get raw sensor value for LoRa transmission
int getRawSensorValue() {
#if USE_ULTRASONIC
  long distanceToWater = readUltrasonicDistance();
  
  if (distanceToWater >= 0) {
    // Map distance (0-TANK_MAX_DEPTH) to 0-1023 for compatibility with receiver
    int rawValue = map(distanceToWater, 0, TANK_MAX_DEPTH, 1023, 0);
    lastRawSensorValue = constrain(rawValue, 0, 1023);
  }
  return lastRawSensorValue;
  
#else
  return analogRead(WATER_SENSOR_PIN);
#endif
}

// Function to convert water level to percentage
int calculatePercentage(int waterLevel) {
  int percentage = map(waterLevel, 0, TANK_MAX_DEPTH, 0, 100);
  percentage = constrain(percentage, 0, 100);
  return percentage;
}

// Function to convert decimal to 2-byte HEX string
String decimalToHex(int decimal) {
  uint16_t value = (uint16_t)decimal;
  char hexStr[5];
  sprintf(hexStr, "%04X", value);
  return String(hexStr);
}

// Function to send LoRa data
bool sendLoRaData(String hexData) {
  String command = "AT+PSEND=" + hexData;
  Serial1.println(command);

  unsigned long startTime = millis();
  while (millis() - startTime < 2000) {
    if (Serial1.available()) {
      String response = Serial1.readStringUntil('\n');
      response.trim();

      if (response.indexOf("OK") != -1) {
        Serial.println("✅ LoRa transmission successful");
        return true;
      } else if (response.indexOf("ERROR") != -1) {
        Serial.println("❌ LoRa transmission failed");
        return false;
      }
    }
  }

  Serial.println("⏰ LoRa transmission timeout");
  return false;
}

void setup() {
  Serial.begin(SERIAL_BAUD_RATE);
  Serial1.begin(LORA_BAUD_RATE);

  pinMode(RAK3172_RESET, OUTPUT);
  digitalWrite(RAK3172_RESET, HIGH);
  delay(2000);

  Serial.println("\n=== LoRa Water Tank TX Node ===");
  Serial.println("Initializing...");

#if USE_ULTRASONIC
  // Initialize ultrasonic sensor pins
  pinMode(ULTRASONIC_TRIG_PIN, OUTPUT);
  pinMode(ULTRASONIC_ECHO_PIN, INPUT);
  digitalWrite(ULTRASONIC_TRIG_PIN, LOW);
  
  Serial.println("📡 Ultrasonic Sensor Configuration:");
  Serial.println("   Mode: TRIGGER & ECHO method (HC-SR04)");
  Serial.println("   Trigger Pin: GPIO" + String(ULTRASONIC_TRIG_PIN));
  Serial.println("   Echo Pin: GPIO" + String(ULTRASONIC_ECHO_PIN));
  Serial.println("   Max Distance: " + String(ULTRASONIC_MAX_DISTANCE) + " cm");
#else
  pinMode(WATER_SENSOR_PIN, INPUT);
  Serial.println("📊 Using analog water sensor on pin A" + String(WATER_SENSOR_PIN));
#endif

  // LoRa P2P configuration
  Serial.println("\n🔧 Configuring LoRa P2P mode...");
  Serial1.println("AT+NWM=0");
  delay(500);

  String p2pCommand = "AT+P2P=" +
                     String(LORA_FREQUENCY) + ":" +
                     String(LORA_SPREADING_FACTOR) + ":" +
                     String(LORA_BANDWIDTH) + ":" +
                     String(LORA_CODING_RATE) + ":" +
                     String(LORA_PREAMBLE_LENGTH) + ":" +
                     String(LORA_TX_POWER);

  Serial.println("Command: " + p2pCommand);
  Serial1.println(p2pCommand);
  delay(500);

  Serial.println("\n📊 LoRa Parameters:");
  Serial.println("   Frequency: " + String(LORA_FREQUENCY) + " Hz");
  Serial.println("   Spreading Factor: " + String(LORA_SPREADING_FACTOR));
  Serial.println("   Bandwidth: " + String(LORA_BANDWIDTH));
  Serial.println("   Coding Rate: " + String(LORA_CODING_RATE));
  Serial.println("   Preamble: " + String(LORA_PREAMBLE_LENGTH));
  Serial.println("   TX Power: " + String(LORA_TX_POWER) + " dBm");

  // Disable RX on TX node
  Serial1.println("AT+PRECV=0");
  delay(500);

  // Test LoRa module
  Serial.println("\n🔍 Testing LoRa module...");
  Serial1.println("AT");
  delay(500);

  // Setup WiFi AP for monitoring
  Serial.println("\n🌐 Starting Monitoring Access Point...");
  WiFi.mode(WIFI_AP);
  
  // Generate unique SSID with MAC
  uint8_t mac[6];
  WiFi.macAddress(mac);
  char macStr[5];
  sprintf(macStr, "%02X%02X", mac[4], mac[5]);
  apSSID = "TankTX-" + String(macStr);
  
  IPAddress local_IP(192, 168, 5, 1);
  IPAddress gateway(192, 168, 5, 1);
  IPAddress subnet(255, 255, 255, 0);
  WiFi.softAPConfig(local_IP, gateway, subnet);
  
  if (WiFi.softAP(apSSID.c_str(), "12345678")) {
    Serial.println("✅ AP Started Successfully!");
    Serial.println("📱 Network: " + apSSID);
    Serial.println("🔑 Password: 12345678");
    Serial.println("🌐 Monitor URL: http://192.168.5.1");
    
    // Start DNS server
    dnsServer.start(DNS_PORT, "*", local_IP);
    Serial.println("✅ DNS Server started");
    
    // Setup web server routes
    server.on("/", HTTP_GET, []() {
      server.send(200, "text/html", getMonitorHTML());
    });
    
    server.on("/data", HTTP_GET, []() {
      String json = "{";
      json += "\"distance\":" + String(currentDistance) + ",";
      json += "\"water_level\":" + String(currentWaterLevel) + ",";
      json += "\"percentage\":" + String(currentPercentage) + ",";
      json += "\"lora_success\":" + String(loraTransmitSuccess ? "true" : "false") + ",";
      json += "\"last_transmit\":" + String((millis() - lastTransmitTime) / 1000);
      json += "}";
      server.send(200, "application/json", json);
    });
    
    // Captive portal endpoints
    server.on("/generate_204", HTTP_GET, []() {
      server.send(200, "text/html", getMonitorHTML());
    });
    server.on("/hotspot-detect.html", HTTP_GET, []() {
      server.send(200, "text/html", getMonitorHTML());
    });
    server.onNotFound([]() {
      server.send(200, "text/html", getMonitorHTML());
    });
    
    server.begin();
    Serial.println("✅ Web Server started");
    Serial.println("🎉 Monitor Portal Ready!");
  } else {
    Serial.println("❌ Failed to start AP");
  }
  
  Serial.println("\n✅ TX Node initialization complete!");
  Serial.println("🚀 Starting water level monitoring...");
  Serial.print("   Transmission interval: ");
  Serial.print(TRANSMISSION_INTERVAL / 1000);
  Serial.println(" seconds");
  Serial.println("========================================\n");
}

void loop() {
  static unsigned long lastTransmission = 0;

  // Check if it's time to send data
  if (millis() - lastTransmission >= TRANSMISSION_INTERVAL) {
    Serial.println("\n========================================");
    Serial.println("📤 READING SENSOR & TRANSMITTING DATA");
    Serial.println("========================================");

#if USE_ULTRASONIC
    // Read ultrasonic sensor using trigger/echo
    Serial.println("🔊 Triggering ultrasonic pulse...");
    long distanceToWater = readUltrasonicDistance();
    
    if (distanceToWater >= 0) {
      // Calculate water level
      int waterLevel = TANK_MAX_DEPTH - distanceToWater;
      waterLevel = constrain(waterLevel, 0, TANK_MAX_DEPTH);
      int percentage = calculatePercentage(waterLevel);
      
      // Update global variables for web portal
      currentDistance = distanceToWater;
      currentWaterLevel = waterLevel;
      currentPercentage = percentage;
      
      Serial.println("\n📊 Ultrasonic Measurements:");
      Serial.println("   Distance to surface: " + String(distanceToWater) + " cm");
      Serial.println("   Water level: " + String(waterLevel) + " cm");
      Serial.println("   Tank fill: " + String(percentage) + "%");
      
      // Get raw value for transmission
      int rawValue = getRawSensorValue();
      String hexPayload = decimalToHex(rawValue);
      
      Serial.println("\n📡 LoRa Transmission:");
      Serial.println("   Raw value: " + String(rawValue));
      Serial.println("   HEX payload: " + hexPayload);
      
      // Send via LoRa
      loraTransmitSuccess = sendLoRaData(hexPayload);
      lastTransmitTime = millis();
      
      if (loraTransmitSuccess) {
        Serial.println("\n✅ Data sent successfully via LoRa!");
      } else {
        Serial.println("\n❌ Failed to send data");
      }
    } else {
      Serial.println("⚠️  Sensor read failed - skipping transmission");
    }
    
#else
    // Use analog sensor
    int rawValue = analogRead(WATER_SENSOR_PIN);
    int waterLevel = readWaterLevel();
    int percentage = calculatePercentage(waterLevel);
    
    Serial.println("\n📊 Analog Sensor Reading:");
    Serial.println("   Raw value: " + String(rawValue));
    Serial.println("   Water level: " + String(waterLevel) + " cm");
    Serial.println("   Tank fill: " + String(percentage) + "%");
    
    String hexPayload = decimalToHex(rawValue);
    Serial.println("   HEX payload: " + hexPayload);
    
    if (sendLoRaData(hexPayload)) {
      Serial.println("\n✅ Data sent successfully!");
    } else {
      Serial.println("\n❌ Failed to send data");
    }
#endif

    Serial.println("========================================\n");
    lastTransmission = millis();
  }

  // Handle web server requests
  dnsServer.processNextRequest();
  server.handleClient();
  
  // Handle LoRa responses
  if (Serial1.available()) {
    String response = Serial1.readStringUntil('\n');
    response.trim();
    if (response.length() > 0) {
      Serial.println("LoRa: " + response);
    }
  }

  // Status update every 15 seconds
  static unsigned long lastStatusUpdate = 0;
  if (millis() - lastStatusUpdate > 15000) {
    Serial.println("💚 TX Node active - Next transmission in: " + 
                   String((TRANSMISSION_INTERVAL - (millis() - lastTransmission)) / 1000) + "s");
    lastStatusUpdate = millis();
  }

  delay(10);  // Reduced delay for responsive web server
}
