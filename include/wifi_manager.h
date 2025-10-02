#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include "wifi_credentials.h"

// 🌐 WiFi Manager for Pico W with Beautiful Web Portal
class WiFiManager {
private:
    WebServer server;
    DNSServer dnsServer;
    WiFiCredentials* credentials;
    bool ap_mode;
    String ap_ssid;
    String ap_password;
    unsigned long ap_start_time;
    
    // Live data tracking
    int* current_water_level;
    int* current_percentage;
    bool* wifi_connected;
    
    static const unsigned long AP_TIMEOUT = 600000; // 10 minutes
    static const byte DNS_PORT = 53;
    
    // 🎨 Beautiful HTML/CSS/JS Web Portal
    const char* getPortalHTML() {
        return R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Tank Monitor Setup</title>
    <style>
        * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
        }
        
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
            animation: slideIn 0.5s ease-out;
        }
        
        @keyframes slideIn {
            from {
                opacity: 0;
                transform: translateY(-30px);
            }
            to {
                opacity: 1;
                transform: translateY(0);
            }
        }
        
        .header {
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            color: white;
            padding: 30px 20px;
            text-align: center;
        }
        
        .header h1 {
            font-size: 1.8rem;
            margin-bottom: 5px;
        }
        
        .header p {
            opacity: 0.9;
            font-size: 0.9rem;
        }
        
        .content {
            padding: 30px;
        }
        
        .step {
            margin-bottom: 30px;
        }
        
        .step-title {
            font-size: 1.1rem;
            color: #333;
            margin-bottom: 15px;
            font-weight: 600;
        }
        
        .form-group {
            margin-bottom: 20px;
        }
        
        label {
            display: block;
            color: #555;
            font-size: 0.9rem;
            margin-bottom: 8px;
            font-weight: 500;
        }
        
        input, select {
            width: 100%;
            padding: 12px 15px;
            border: 2px solid #e0e0e0;
            border-radius: 10px;
            font-size: 1rem;
            transition: all 0.3s ease;
            font-family: inherit;
        }
        
        input:focus, select:focus {
            outline: none;
            border-color: #667eea;
            box-shadow: 0 0 0 3px rgba(102, 126, 234, 0.1);
        }
        
        .wifi-list {
            max-height: 200px;
            overflow-y: auto;
            border: 2px solid #e0e0e0;
            border-radius: 10px;
            padding: 10px;
        }
        
        .wifi-item {
            padding: 12px;
            border-radius: 8px;
            cursor: pointer;
            transition: all 0.2s ease;
            display: flex;
            align-items: center;
            gap: 10px;
        }
        
        .wifi-item:hover {
            background: #f5f5f5;
        }
        
        .wifi-item.selected {
            background: #667eea;
            color: white;
        }
        
        .signal-strength {
            font-size: 1.2rem;
        }
        
        .btn {
            width: 100%;
            padding: 15px;
            border: none;
            border-radius: 10px;
            font-size: 1rem;
            font-weight: 600;
            cursor: pointer;
            transition: all 0.3s ease;
            text-transform: uppercase;
            letter-spacing: 0.5px;
        }
        
        .btn-primary {
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            color: white;
        }
        
        .btn-primary:hover {
            transform: translateY(-2px);
            box-shadow: 0 10px 30px rgba(102, 126, 234, 0.4);
        }
        
        .btn-primary:active {
            transform: translateY(0);
        }
        
        .btn-secondary {
            background: #f0f0f0;
            color: #333;
            margin-top: 10px;
        }
        
        .btn-secondary:hover {
            background: #e0e0e0;
        }
        
        .status-message {
            padding: 15px;
            border-radius: 10px;
            margin-top: 20px;
            text-align: center;
            font-weight: 500;
            display: none;
        }
        
        .status-success {
            background: #d4edda;
            color: #155724;
            border: 1px solid #c3e6cb;
        }
        
        .status-error {
            background: #f8d7da;
            color: #721c24;
            border: 1px solid #f5c6cb;
        }
        
        .status-info {
            background: #d1ecf1;
            color: #0c5460;
            border: 1px solid #bee5eb;
        }
        
        .loader {
            border: 3px solid #f3f3f3;
            border-top: 3px solid #667eea;
            border-radius: 50%;
            width: 30px;
            height: 30px;
            animation: spin 1s linear infinite;
            margin: 20px auto;
            display: none;
        }
        
        @keyframes spin {
            0% { transform: rotate(0deg); }
            100% { transform: rotate(360deg); }
        }
        
        .toggle-section {
            margin-top: 20px;
            padding-top: 20px;
            border-top: 1px solid #e0e0e0;
        }
        
        .toggle-header {
            cursor: pointer;
            color: #667eea;
            font-weight: 600;
            display: flex;
            align-items: center;
            gap: 10px;
        }
        
        .toggle-content {
            max-height: 0;
            overflow: hidden;
            transition: max-height 0.3s ease;
        }
        
        .toggle-content.open {
            max-height: 500px;
        }
        
        .info-box {
            background: #f0f7ff;
            border-left: 4px solid #667eea;
            padding: 15px;
            border-radius: 8px;
            margin-top: 20px;
        }
        
        .info-box h4 {
            color: #667eea;
            margin-bottom: 8px;
        }
        
        .info-box p {
            color: #555;
            font-size: 0.9rem;
            line-height: 1.6;
        }
        
        /* Slider Styles */
        .slider-container {
            margin-top: 15px;
            padding: 15px;
            background: rgba(255,255,255,0.1);
            border-radius: 8px;
        }
        
        .slider-label {
            font-size: 0.9rem;
            opacity: 0.9;
            margin-bottom: 8px;
            text-align: center;
        }
        
        .slider-value {
            font-size: 1.5rem;
            font-weight: bold;
            text-align: center;
            margin-bottom: 10px;
        }
        
        .slider {
            -webkit-appearance: none;
            width: 100%;
            height: 8px;
            border-radius: 5px;
            background: rgba(255,255,255,0.3);
            outline: none;
            transition: all 0.2s;
        }
        
        .slider::-webkit-slider-thumb {
            -webkit-appearance: none;
            appearance: none;
            width: 24px;
            height: 24px;
            border-radius: 50%;
            background: white;
            cursor: pointer;
            box-shadow: 0 2px 8px rgba(0,0,0,0.3);
            transition: all 0.2s;
        }
        
        .slider::-webkit-slider-thumb:hover {
            transform: scale(1.2);
            box-shadow: 0 3px 12px rgba(0,0,0,0.4);
        }
        
        .slider::-moz-range-thumb {
            width: 24px;
            height: 24px;
            border-radius: 50%;
            background: white;
            cursor: pointer;
            border: none;
            box-shadow: 0 2px 8px rgba(0,0,0,0.3);
            transition: all 0.2s;
        }
        
        .slider::-moz-range-thumb:hover {
            transform: scale(1.2);
            box-shadow: 0 3px 12px rgba(0,0,0,0.4);
        }
        
        .slider-marks {
            display: flex;
            justify-content: space-between;
            margin-top: 5px;
            font-size: 0.75rem;
            opacity: 0.7;
        }
    </style>
</head>
<body>
    <div class="container">
        <div class="header">
            <h1>🌊 Tank Monitor</h1>
            <p>WiFi Configuration Portal</p>
        </div>
        
        <div class="content">
            <!-- Live Data Dashboard -->
            <div class="info-box" id="liveDataBox" style="background: linear-gradient(135deg, #667eea 0%, #764ba2 100%); border: none; color: white;">
                <h4 style="color: white; font-size: 1.2rem;">📊 Live Tank Status</h4>
                <div style="display: grid; grid-template-columns: 1fr 1fr; gap: 15px; margin-top: 15px;">
                    <div style="background: rgba(255,255,255,0.2); padding: 15px; border-radius: 8px; text-align: center;">
                        <div style="font-size: 0.9rem; opacity: 0.9;">Water Level</div>
                        <div style="font-size: 2rem; font-weight: bold; margin: 5px 0;" id="waterLevel">--</div>
                        <div style="font-size: 0.85rem; opacity: 0.8;">cm</div>
                    </div>
                    <div style="background: rgba(255,255,255,0.2); padding: 15px; border-radius: 8px; text-align: center;">
                        <div style="font-size: 0.9rem; opacity: 0.9;">Tank Fill</div>
                        <div style="font-size: 2rem; font-weight: bold; margin: 5px 0;" id="tankPercentage">--</div>
                        <div style="font-size: 0.85rem; opacity: 0.8;">%</div>
                    </div>
                </div>
                <div style="margin-top: 15px; padding: 10px; background: rgba(255,255,255,0.1); border-radius: 6px; text-align: center; font-size: 0.9rem;">
                    <span style="opacity: 0.9;">WiFi Status:</span> 
                    <strong id="wifiStatus">Checking...</strong>
                </div>
                
                <!-- Refresh Rate Slider -->
                <div class="slider-container">
                    <div class="slider-label">🔄 Auto-Refresh Rate</div>
                    <div class="slider-value" id="refreshValue">5 seconds</div>
                    <input type="range" min="1" max="100" value="5" class="slider" id="refreshSlider" oninput="updateRefreshRate(this.value)">
                    <div class="slider-marks">
                        <span>1s</span>
                        <span>1min</span>
                        <span>10min</span>
                        <span>60min</span>
                    </div>
                </div>
                
                <div style="margin-top: 10px; text-align: center; font-size: 0.8rem; opacity: 0.7;" id="lastUpdate">Last updated: --</div>
            </div>
            
            <!-- Step 1: Select WiFi -->
            <div class="step" id="step1">
                <div class="step-title">📡 Step 1: Select Your WiFi Network</div>
                
                <p style="color: #666; font-size: 0.9rem; margin-bottom: 10px;">Click the button below to scan for available networks:</p>
                <button class="btn btn-secondary" onclick="scanNetworks()">🔄 Scan Networks</button>
                
                <div class="loader" id="scanLoader"></div>
                
                <div id="networkList" style="margin-top: 15px;"></div>
                
                <div class="form-group" style="margin-top: 20px;">
                    <label for="ssid">Or Enter SSID Manually:</label>
                    <input type="text" id="ssid" placeholder="Network Name">
                </div>
                
                <div class="form-group">
                    <label for="password">WiFi Password:</label>
                    <input type="password" id="password" placeholder="Enter password">
                    <label style="margin-top: 8px; display: flex; align-items: center; gap: 5px; cursor: pointer;">
                        <input type="checkbox" onclick="togglePassword()"> Show Password
                    </label>
                </div>
            </div>
            
            <!-- Advanced Settings (Collapsible) -->
            <div class="toggle-section">
                <div class="toggle-header" onclick="toggleAdvanced()">
                    <span>⚙️ Advanced Settings</span>
                    <span id="toggleIcon">▼</span>
                </div>
                <div class="toggle-content" id="advancedSettings">
                    <div class="form-group" style="margin-top: 15px;">
                        <label for="api_host">API Host:</label>
                        <input type="text" id="api_host" value="10.97.116.57" placeholder="e.g., 192.168.1.100">
                    </div>
                    
                    <div class="form-group">
                        <label for="api_port">API Port:</label>
                        <input type="number" id="api_port" value="80" placeholder="80">
                    </div>
                    
                    <div class="form-group">
                        <label for="api_endpoint">API Endpoint:</label>
                        <input type="text" id="api_endpoint" value="/Tank/tank/api/tank_update.php">
                    </div>
                </div>
            </div>
            
            <!-- Save Button -->
            <button class="btn btn-primary" onclick="saveConfig()">💾 Save & Connect</button>
            
            <!-- Status Messages -->
            <div class="loader" id="saveLoader"></div>
            <div class="status-message" id="statusMessage"></div>
            
            <!-- Info Box -->
            <div class="info-box">
                <h4>ℹ️ Quick Guide:</h4>
                <p>1. Scan or select your WiFi network<br>
                2. Enter the password<br>
                3. Click "Save & Connect"<br>
                4. Device will restart and connect</p>
            </div>
        </div>
    </div>
    
    <script>
        let selectedSSID = '';
        
        // Scan for WiFi networks
        function scanNetworks() {
            document.getElementById('scanLoader').style.display = 'block';
            document.getElementById('networkList').innerHTML = '';
            
            fetch('/scan')
                .then(response => response.json())
                .then(data => {
                    document.getElementById('scanLoader').style.display = 'none';
                    displayNetworks(data.networks);
                })
                .catch(error => {
                    document.getElementById('scanLoader').style.display = 'none';
                    showStatus('Failed to scan networks', 'error');
                });
        }
        
        // Display network list
        function displayNetworks(networks) {
            const container = document.getElementById('networkList');
            
            if (networks.length === 0) {
                container.innerHTML = '<p style="text-align:center; color:#999; padding:20px;">No networks found</p>';
                return;
            }
            
            container.innerHTML = '<div class="wifi-list"></div>';
            const list = container.querySelector('.wifi-list');
            
            networks.forEach(network => {
                const item = document.createElement('div');
                item.className = 'wifi-item';
                item.innerHTML = `
                    <span class="signal-strength">${getSignalIcon(network.rssi)}</span>
                    <span>${network.ssid}</span>
                    <span style="margin-left:auto; font-size:0.8rem; color:#999;">${network.rssi} dBm</span>
                `;
                item.onclick = () => selectNetwork(network.ssid, item);
                list.appendChild(item);
            });
        }
        
        // Select network
        function selectNetwork(ssid, element) {
            document.querySelectorAll('.wifi-item').forEach(el => el.classList.remove('selected'));
            element.classList.add('selected');
            selectedSSID = ssid;
            document.getElementById('ssid').value = ssid;
        }
        
        // Get signal icon
        function getSignalIcon(rssi) {
            if (rssi > -50) return '📶';
            if (rssi > -60) return '📶';
            if (rssi > -70) return '📡';
            return '📡';
        }
        
        // Toggle password visibility
        function togglePassword() {
            const input = document.getElementById('password');
            input.type = input.type === 'password' ? 'text' : 'password';
        }
        
        // Toggle advanced settings
        function toggleAdvanced() {
            const content = document.getElementById('advancedSettings');
            const icon = document.getElementById('toggleIcon');
            content.classList.toggle('open');
            icon.textContent = content.classList.contains('open') ? '▲' : '▼';
        }
        
        // Update refresh rate slider
        let refreshInterval = null;
        let currentRefreshRate = 5; // Default 5 seconds
        
        function updateRefreshRate(value) {
            // Convert slider value to seconds
            // 1-60 = seconds (1s to 60s)
            // 61-100 = minutes mapped to 1-60 minutes
            let seconds;
            let displayText;
            
            if (value <= 60) {
                seconds = parseInt(value);
                displayText = seconds + (seconds === 1 ? ' second' : ' seconds');
            } else {
                // Map 61-100 to 1-60 minutes
                const minutes = Math.round((value - 60) * 1.5);
                seconds = minutes * 60;
                displayText = minutes + (minutes === 1 ? ' minute' : ' minutes');
            }
            
            currentRefreshRate = seconds * 1000; // Convert to milliseconds
            document.getElementById('refreshValue').textContent = displayText;
            
            // Restart the interval with new rate
            if (refreshInterval) {
                clearInterval(refreshInterval);
            }
            refreshInterval = setInterval(fetchLiveData, currentRefreshRate);
        }
        
        // Save configuration
        function saveConfig() {
            const ssid = document.getElementById('ssid').value;
            const password = document.getElementById('password').value;
            const api_host = document.getElementById('api_host').value;
            const api_port = document.getElementById('api_port').value;
            const api_endpoint = document.getElementById('api_endpoint').value;
            const api_key = 'iotlogic'; // Hardcoded API key
            
            if (!ssid || !password) {
                showStatus('Please enter WiFi credentials', 'error');
                return;
            }
            
            document.getElementById('saveLoader').style.display = 'block';
            showStatus('Saving configuration...', 'info');
            
            const data = {
                ssid: ssid,
                password: password,
                api_host: api_host,
                api_port: parseInt(api_port),
                api_endpoint: api_endpoint,
                api_key: api_key
            };
            
            fetch('/save', {
                method: 'POST',
                headers: {'Content-Type': 'application/json'},
                body: JSON.stringify(data)
            })
            .then(response => response.json())
            .then(data => {
                document.getElementById('saveLoader').style.display = 'none';
                if (data.success) {
                    showStatus('✅ Configuration saved! Restarting...', 'success');
                    setTimeout(() => {
                        showStatus('Device is connecting to WiFi. This page will close.', 'info');
                    }, 2000);
                } else {
                    showStatus('❌ Failed to save configuration', 'error');
                }
            })
            .catch(error => {
                document.getElementById('saveLoader').style.display = 'none';
                showStatus('❌ Connection error', 'error');
            });
        }
        
        // Show status message
        function showStatus(message, type) {
            const statusDiv = document.getElementById('statusMessage');
            statusDiv.className = `status-message status-${type}`;
            statusDiv.textContent = message;
            statusDiv.style.display = 'block';
            
            if (type === 'success') {
                setTimeout(() => {
                    statusDiv.style.display = 'none';
                }, 5000);
            }
        }
        
        // Fetch live data
        function fetchLiveData() {
            fetch('/data')
                .then(response => response.json())
                .then(data => {
                    document.getElementById('waterLevel').textContent = data.water_level;
                    document.getElementById('tankPercentage').textContent = data.percentage;
                    document.getElementById('wifiStatus').textContent = data.wifi_connected ? '✅ Connected' : '❌ Disconnected';
                    
                    const now = new Date();
                    document.getElementById('lastUpdate').textContent = 'Last updated: ' + now.toLocaleTimeString();
                })
                .catch(error => {
                    document.getElementById('wifiStatus').textContent = '⚠️  No Data';
                });
        }
        
        // Auto-load behavior
        window.onload = function() {
            fetchLiveData();  // Fetch data immediately
            // DON'T auto-scan - let user click the button to avoid disconnections
            // setTimeout(scanNetworks, 1000);  // REMOVED to prevent auto-disconnect
            
            // Start auto-refresh with default rate (5 seconds)
            refreshInterval = setInterval(fetchLiveData, 5000); // 5000ms = 5 seconds
        };
    </script>
</body>
</html>
        )rawliteral";
    }

public:
    unsigned long lastScanTime = 0;
    const unsigned long SCAN_COOLDOWN = 10000; // 10 seconds between scans
    bool scanInProgress = false;
    
    WiFiManager() 
        : server(80), credentials(nullptr), ap_mode(false), ap_start_time(0),
          current_water_level(nullptr), current_percentage(nullptr), wifi_connected(nullptr),
          lastScanTime(0), scanInProgress(false) {
    }
    
    void setCredentials(WiFiCredentials* cred) {
        credentials = cred;
    }
    
    void setLiveData(int* water_level, int* percentage, bool* wifi_conn) {
        current_water_level = water_level;
        current_percentage = percentage;
        wifi_connected = wifi_conn;
    }
    
    // 🚀 Start Access Point Mode
    bool startAP() {
        Serial.println("🌐 Starting Access Point Mode...");
        
        // Generate unique AP SSID with last 4 chars of MAC
        uint8_t mac[6];
        WiFi.macAddress(mac);
        char macStr[5];
        sprintf(macStr, "%02X%02X", mac[4], mac[5]);
        ap_ssid = "TankMonitor-" + String(macStr);
        ap_password = "12345678";  // 8-digit PIN (required by most phones)
        
        Serial.println("AP SSID: " + ap_ssid);
        Serial.println("AP Password: " + ap_password);
        
        // Disconnect from any previous WiFi
        WiFi.disconnect(true);
        delay(100);
        
        // Start AP with NULL password for truly open network
        WiFi.mode(WIFI_AP);
        
        // Configure AP with specific IP settings
        IPAddress local_IP(192, 168, 4, 1);
        IPAddress gateway(192, 168, 4, 1);
        IPAddress subnet(255, 255, 255, 0);
        
        WiFi.softAPConfig(local_IP, gateway, subnet);
        
        // Start AP with 4-digit password
        if (!WiFi.softAP(ap_ssid.c_str(), ap_password.c_str())) {
            Serial.println("❌ Failed to start AP");
            return false;
        }
        
        delay(1000);
        
        IPAddress IP = WiFi.softAPIP();
        Serial.println("✅ AP Started Successfully!");
        Serial.println("📱 Connect to: " + ap_ssid);
        Serial.println("🌐 Portal IP: http://" + IP.toString());
        Serial.println("🌐 Or just open any webpage (captive portal)");
        
        // Setup DNS server for captive portal
        dnsServer.start(DNS_PORT, "*", IP);
        Serial.println("✅ DNS Server started (captive portal enabled)");
        
        // Setup web server routes
        setupWebServer();
        
        server.begin();
        ap_mode = true;
        ap_start_time = millis();
        
        Serial.println("🎉 Web Portal Ready!");
        Serial.println("💡 Captive portal will auto-open when you connect");
        return true;
    }
    
    // 🌐 Setup web server routes
    void setupWebServer() {
        // Main portal page
        server.on("/", HTTP_GET, [this]() {
            server.send(200, "text/html", getPortalHTML());
        });
        
        // Captive portal detection endpoints
        server.on("/generate_204", HTTP_GET, [this]() {  // Android
            server.send(200, "text/html", getPortalHTML());
        });
        server.on("/gen_204", HTTP_GET, [this]() {  // Android alternative
            server.send(200, "text/html", getPortalHTML());
        });
        server.on("/hotspot-detect.html", HTTP_GET, [this]() {  // iOS
            server.send(200, "text/html", getPortalHTML());
        });
        server.on("/connecttest.txt", HTTP_GET, [this]() {  // Windows
            server.send(200, "text/html", getPortalHTML());
        });
        server.on("/success.txt", HTTP_GET, [this]() {  // Firefox
            server.send(200, "text/html", getPortalHTML());
        });
        
        // Live data endpoint
        server.on("/data", HTTP_GET, [this]() {
            String json = "{";
            json += "\"water_level\":" + String(current_water_level ? *current_water_level : 0) + ",";
            json += "\"percentage\":" + String(current_percentage ? *current_percentage : 0) + ",";
            json += "\"wifi_connected\":" + String(wifi_connected && *wifi_connected ? "true" : "false");
            json += "}";
            server.send(200, "application/json", json);
        });
        
        // Scan WiFi networks (with cooldown to prevent blocking)
        server.on("/scan", HTTP_GET, [this]() {
            // Check if scan is too frequent
            if (millis() - lastScanTime < SCAN_COOLDOWN) {
                Serial.println("⏱️  Scan requested too soon, returning empty");
                server.send(200, "application/json", "{\"networks\":[]}");
                return;
            }
            
            if (scanInProgress) {
                Serial.println("⏱️  Scan already in progress");
                server.send(200, "application/json", "{\"networks\":[]}");
                return;
            }
            
            Serial.println("📡 Scanning WiFi networks...");
            scanInProgress = true;
            lastScanTime = millis();
            
            // Scan networks (synchronous - Pico W doesn't support async)
            int n = WiFi.scanNetworks();
            scanInProgress = false;
            
            if (n == -1) {
                Serial.println("❌ Scan failed");
                server.send(200, "application/json", "{\"networks\":[]}");
                return;
            }
            
            String json = "{\"networks\":[";
            for (int i = 0; i < n && i < 20; i++) { // Limit to 20 networks
                if (i > 0) json += ",";
                json += String("{\"ssid\":\"") + WiFi.SSID(i) + String("\",\"rssi\":") + String(WiFi.RSSI(i)) + String("}");
            }
            json += "]}";
            
            Serial.println("✅ Scan complete, found " + String(n) + " networks");
            server.send(200, "application/json", json);
        });
        
        // Save configuration
        server.on("/save", HTTP_POST, [this]() {
            if (server.hasArg("plain")) {
                String body = server.arg("plain");
                Serial.println("📝 Received config: " + body);
                
                // Parse JSON (simple parsing for now)
                // In production, use ArduinoJson
                
                // Extract values (simplified - implement proper JSON parsing)
                int ssidStart = body.indexOf("\"ssid\":\"") + 8;
                int ssidEnd = body.indexOf("\"", ssidStart);
                String ssid = body.substring(ssidStart, ssidEnd);
                
                int passStart = body.indexOf("\"password\":\"") + 12;
                int passEnd = body.indexOf("\"", passStart);
                String password = body.substring(passStart, passEnd);
                
                int hostStart = body.indexOf("\"api_host\":\"") + 12;
                int hostEnd = body.indexOf("\"", hostStart);
                String api_host = body.substring(hostStart, hostEnd);
                
                int endpointStart = body.indexOf("\"api_endpoint\":\"") + 16;
                int endpointEnd = body.indexOf("\"", endpointStart);
                String api_endpoint = body.substring(endpointStart, endpointEnd);
                
                int keyStart = body.indexOf("\"api_key\":\"") + 11;
                int keyEnd = body.indexOf("\"", keyStart);
                String api_key = body.substring(keyStart, keyEnd);
                
                int portStart = body.indexOf("\"api_port\":") + 11;
                int portEnd = body.indexOf(",", portStart);
                if (portEnd == -1) portEnd = body.indexOf("}", portStart);
                uint16_t api_port = body.substring(portStart, portEnd).toInt();
                
                // Save to storage
                if (credentials->save(ssid.c_str(), password.c_str(), 
                                      api_host.c_str(), api_endpoint.c_str(), 
                                      api_key.c_str(), api_port)) {
                    server.send(200, "application/json", "{\"success\":true}");
                    
                    Serial.println("✅ Configuration saved! Restarting...");
                    delay(2000);
                    rp2040.reboot();
                } else {
                    server.send(500, "application/json", "{\"success\":false}");
                }
            } else {
                server.send(400, "application/json", "{\"success\":false,\"error\":\"No data\"}");
            }
        });
        
        // 404 handler - redirect everything to portal (captive portal)
        server.onNotFound([this]() {
            server.send(200, "text/html", getPortalHTML());
        });
    }
    
    // 🔄 Handle web server
    void handleClient() {
        if (ap_mode) {
            dnsServer.processNextRequest();  // Handle DNS requests for captive portal
            server.handleClient();
            
            // Check AP timeout
            if (millis() - ap_start_time > AP_TIMEOUT) {
                Serial.println("⏰ AP Mode timeout - restarting");
                rp2040.reboot();
            }
        }
    }
    
    // 📡 Connect to WiFi using saved credentials
    bool connectToWiFi(const char* ssid, const char* password, int timeout_seconds = 30) {
        Serial.println("📡 Connecting to WiFi: " + String(ssid));
        
        WiFi.mode(WIFI_STA);
        WiFi.begin(ssid, password);
        
        int attempts = 0;
        int max_attempts = timeout_seconds * 2; // Check every 500ms
        
        while (WiFi.status() != WL_CONNECTED && attempts < max_attempts) {
            delay(500);
            Serial.print(".");
            attempts++;
        }
        
        if (WiFi.status() == WL_CONNECTED) {
            Serial.println("\n✅ WiFi Connected!");
            Serial.println("📍 IP Address: " + WiFi.localIP().toString());
            return true;
        } else {
            Serial.println("\n❌ WiFi Connection Failed!");
            return false;
        }
    }
    
    bool isAPMode() {
        return ap_mode;
    }
    
    String getAPSSID() {
        return ap_ssid;
    }
    
    // Alias for startAP
    bool startAPMode() {
        return startAP();
    }
};

#endif // WIFI_MANAGER_H
