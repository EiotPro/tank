# WiFi AP Configuration System - Usage Guide
## Raspberry Pi Pico W Tank Monitor

---

## 📋 Table of Contents
1. [Overview](#overview)
2. [Hardware Setup](#hardware-setup)
3. [File Structure](#file-structure)
4. [How It Works](#how-it-works)
5. [Usage Instructions](#usage-instructions)
6. [LED Status Indicators](#led-status-indicators)
7. [Configuration Portal](#configuration-portal)
8. [Troubleshooting](#troubleshooting)
9. [API Reference](#api-reference)

---

## 🎯 Overview

This system replaces hardcoded WiFi credentials with a smart, user-friendly configuration portal. The Pico W can:

- ✅ Start in **Access Point (AP) mode** for initial configuration
- ✅ Save WiFi credentials and API settings **persistently** (LittleFS)
- ✅ Auto-connect to saved WiFi on subsequent boots
- ✅ Provide **visual LED feedback** for all states
- ✅ Support **manual reconfiguration** via button press
- ✅ Auto-recover from WiFi connection failures
- ✅ Serve a **beautiful web portal** for configuration

### Key Features:
- **Zero hardcoded credentials** - All settings configurable via web portal
- **Smart boot logic** - Auto-detects first boot vs normal operation
- **WiFi network scanner** - Shows available networks with signal strength
- **Persistent storage** - Settings saved to flash memory (JSON format)
- **LED status indicators** - Visual feedback for every state
- **Easy reconfiguration** - Hold button for 3 seconds to enter config mode

---

## 🔧 Hardware Setup

### Required Components:
1. **Raspberry Pi Pico W** (with WiFi)
2. **Configuration Button** (optional but recommended)
3. **Status LED** (onboard LED used by default)
4. **LoRa Module** (RAK3172) for receiving tank data

### Wiring Diagram:

```
┌─────────────────────────────────────┐
│     Raspberry Pi Pico W             │
│                                     │
│  GPIO 17 ─────┐                    │
│               │                    │
│               ├── [Button] ── GND  │
│               │                    │
│               └── (Internal Pullup)│
│                                     │
│  GP25 (LED) ───── Onboard LED      │
│                                     │
│  GPIO 0 (TX1) ───── RAK3172 RX     │
│  GPIO 1 (RX1) ───── RAK3172 TX     │
│                                     │
└─────────────────────────────────────┘
```

### Pin Configuration:
| Pin | Function | Description |
|-----|----------|-------------|
| GPIO 16 | Config Button | Hold for 3s to enter AP mode |
| GP25 | Status LED | Onboard LED (LED_BUILTIN) |
| GPIO 0 | LoRa TX | Serial1 TX to RAK3172 |
| GPIO 1 | LoRa RX | Serial1 RX from RAK3172 |

**Button Connection:**
- One side to GPIO 16
- Other side to GND
- Internal pullup enabled (active LOW)

---

## 📁 File Structure

```
Tank/
├── include/
│   ├── wifi_credentials.h     # Persistent credential storage
│   ├── wifi_manager.h          # AP mode & web portal
│   └── led_status.h            # LED status indicators
├── src/
│   └── rx/
│       ├── main.cpp            # Smart boot logic
│       └── config.h            # Hardware configuration
├── platformio.ini              # Project configuration
└── WIFI_CONFIG_GUIDE.md        # This guide
```

### Key Files:

#### 1. **wifi_credentials.h**
- Manages persistent storage using LittleFS
- Stores: SSID, Password, API Endpoint, API Key
- JSON format saved to `/wifi_config.json`

#### 2. **wifi_manager.h**
- Creates WiFi Access Point
- Serves configuration web portal
- Scans for available networks
- Handles save/reboot logic

#### 3. **led_status.h**
- Manages LED visual feedback
- Supports multiple states (AP, connecting, connected, error, transmitting)
- Smooth pulsing and blinking patterns

#### 4. **main.cpp**
- Smart boot logic
- Button monitoring
- LoRa reception and data forwarding
- WiFi connection management

---

## ⚙️ How It Works

### Boot Flow Diagram:

```
┌─────────────┐
│   POWER ON  │
└──────┬──────┘
       │
       ▼
┌──────────────────┐
│ Initialize       │
│ - Serial         │
│ - LED Status     │
│ - Button         │
│ - LittleFS       │
└──────┬───────────┘
       │
       ▼
   ┌───────────┐
   │ Check for │
   │Saved Creds│
   └─────┬─────┘
         │
    ┌────┴────┐
    │         │
    NO       YES
    │         │
    ▼         ▼
┌────────┐ ┌────────────┐
│Enter AP│ │Connect to  │
│ Mode   │ │Saved WiFi  │
└────────┘ └──────┬─────┘
    │             │
    │        ┌────┴────┐
    │        │         │
    │     SUCCESS   FAIL
    │        │         │
    │        ▼         ▼
    │   ┌────────┐ ┌────────┐
    │   │Normal  │ │Show    │
    │   │Operation│ │Error   │
    │   └────────┘ └────┬───┘
    │                   │
    │    ┌──────────────┘
    │    │
    ▼    ▼
┌──────────────┐
│ Configuration│
│    Portal    │
│  (AP Mode)   │
└──────────────┘
```

### State Machine:

1. **First Boot (No Credentials)**
   - LED: Fast blinking (AP mode)
   - Starts AP: `PicoW-Tank-XXXX`
   - Serves web portal at `192.168.4.1`

2. **Normal Boot (Has Credentials)**
   - LED: Slow pulsing (connecting)
   - Attempts WiFi connection
   - On success: LED solid (connected)
   - On failure: LED fast blink (error) → Enter AP mode

3. **Manual Config Mode**
   - Hold button for 3 seconds
   - LED: Fast blinking (AP mode)
   - Starts AP and serves portal

4. **Runtime Operation**
   - LED: Solid (connected)
   - Monitors WiFi connection
   - Flash LED on data transmission
   - Auto-reconnect if disconnected

---

## 📱 Usage Instructions

### First Time Setup:

#### Step 1: Flash the Firmware
```bash
cd Tank
pio run --target upload
```

#### Step 2: Power On the Pico W
- LED will start **fast blinking** (AP mode)
- Serial output will show:
  ```
  🚀 PICO W TANK MONITOR - SMART BOOT
  ⚠️  No WiFi credentials found!
  📱 Configuration Portal Active!
     SSID: PicoW-Tank-ABCD
     IP: 192.168.4.1
  ```

#### Step 3: Connect to the AP
1. On your phone/computer, go to WiFi settings
2. Connect to network: `PicoW-Tank-XXXX`
3. Password: (none - open network by default)

#### Step 4: Open Configuration Portal
1. Open browser
2. Go to: `http://192.168.4.1`
3. You'll see the configuration portal

#### Step 5: Configure WiFi
1. Click **"Scan Networks"** button
2. Select your WiFi network from the list
3. Enter WiFi password
4. (Optional) Expand **"Advanced Settings"**:
   - Enter API Endpoint (e.g., `/api/tank_update_fixed.php`)
   - Enter API Key
5. Click **"Save Configuration"**

#### Step 6: Device Reboots
- Device will automatically reboot
- LED will **pulse slowly** (connecting)
- Once connected, LED stays **solid** (connected)
- Serial output shows:
  ```
  ✅ WiFi connected successfully!
     IP Address: 192.168.1.100
  ✅ RX Node initialization complete
  ```

### Reconfiguring WiFi (Changing Networks):

#### Method 1: Using Button
1. Press and hold the config button for **3 seconds**
2. LED will start **fast blinking** (AP mode)
3. Follow steps 3-6 from First Time Setup

#### Method 2: Clear Credentials via Serial
1. Connect via serial monitor
2. (You can add a serial command handler if needed)

### Normal Operation:

Once configured, the device will:
1. **Auto-start** and connect to saved WiFi
2. **Receive** LoRa data from tank sensor
3. **Process** water level calculations
4. **Transmit** data to web server
5. **Monitor** WiFi connection (every 30s)
6. **Auto-reconnect** if connection lost

---

## 💡 LED Status Indicators

The onboard LED provides visual feedback for different states:

| LED Pattern | Status | Description |
|-------------|--------|-------------|
| **Fast Blink** (200ms) | 🔧 **AP Mode** | Configuration portal active |
| **Slow Pulse** (1s) | 🔄 **Connecting** | Attempting WiFi connection |
| **Solid ON** | ✅ **Connected** | WiFi connected successfully |
| **Fast Blink** (200ms) | ❌ **Error** | Connection failed or error |
| **Quick Flash** (100ms) | 📡 **Transmitting** | Sending data to server |
| **OFF** | ⚫ **Off** | System idle or powered off |

### LED Behavior Examples:

**First Boot:**
```
Fast Blink → Fast Blink → Fast Blink ...
(AP mode - waiting for configuration)
```

**Connecting:**
```
Pulse ... Pulse ... Pulse ...
(Attempting to connect to saved WiFi)
```

**Connected & Operating:**
```
Solid ON ─── Flash ─── Solid ON ─── Flash ───
             (data)                  (data)
```

**Connection Lost:**
```
Solid ON → Fast Blink → Pulse ... → Solid ON
           (error)      (reconnect) (success)
```

---

## 🌐 Configuration Portal

### Portal Features:

#### Main Interface:
- 📡 **WiFi Network Scanner** - Shows available networks with signal strength
- 🔐 **Password Field** - Show/hide toggle for security
- ⚙️ **Advanced Settings** - Collapsible section for API configuration
- 💾 **Save Button** - Stores configuration and reboots
- 📊 **Real-time Status** - Shows connection progress

#### WiFi Network List:
```
┌─────────────────────────────────────┐
│ Available Networks                  │
├─────────────────────────────────────┤
│ ●●●● MyHomeWiFi            [Select] │
│ ●●●  OfficeNetwork         [Select] │
│ ●●   GuestWiFi             [Select] │
│ ●    WeakSignal            [Select] │
└─────────────────────────────────────┘
```

#### Signal Strength Indicators:
- **●●●●** Excellent (>-60 dBm)
- **●●●** Good (-60 to -70 dBm)
- **●●** Fair (-70 to -80 dBm)
- **●** Weak (<-80 dBm)

### Portal Endpoints:

| Endpoint | Method | Description |
|----------|--------|-------------|
| `/` | GET | Main configuration page (HTML) |
| `/scan` | GET | Scan WiFi networks (returns JSON) |
| `/save` | POST | Save configuration (accepts JSON) |

### Example API Calls:

#### Scan Networks:
```http
GET http://192.168.4.1/scan

Response:
{
  "networks": [
    {"ssid": "MyHomeWiFi", "rssi": -45, "encryption": true},
    {"ssid": "GuestNetwork", "rssi": -72, "encryption": false}
  ]
}
```

#### Save Configuration:
```http
POST http://192.168.4.1/save
Content-Type: application/json

{
  "ssid": "MyHomeWiFi",
  "password": "mypassword123",
  "api_endpoint": "/api/tank_update_fixed.php",
  "api_key": "your-api-key-here"
}

Response:
{
  "success": true,
  "message": "Configuration saved! Rebooting..."
}
```

---

## 🐛 Troubleshooting

### Problem: Can't see the AP network

**Symptoms:**
- No `PicoW-Tank-XXXX` network visible
- LED not blinking

**Solutions:**
1. Check power supply (Pico W needs stable 5V)
2. Verify firmware upload was successful
3. Check serial output for errors
4. Try resetting the device
5. Look for the AP on 2.4GHz only (Pico W doesn't support 5GHz)

### Problem: Can't connect to AP

**Symptoms:**
- See the network but can't connect
- Connection times out

**Solutions:**
1. AP is open (no password required)
2. Try "Forget" network and reconnect
3. Disable mobile data on phone
4. Try from a different device
5. Check serial output for WiFi errors

### Problem: Configuration portal won't load

**Symptoms:**
- Connected to AP but `192.168.4.1` won't load
- Browser shows "Can't reach this page"

**Solutions:**
1. Verify IP address is exactly `192.168.4.1`
2. Try `http://` (not `https://`)
3. Disable VPN if active
4. Check if device got DHCP address (should be 192.168.4.x)
5. Try different browser
6. Check serial output - portal should be running

### Problem: WiFi won't save

**Symptoms:**
- Configuration seems to save but doesn't connect
- Boots back into AP mode

**Solutions:**
1. Verify WiFi password is correct (case-sensitive)
2. Check that WiFi network is 2.4GHz (not 5GHz)
3. Ensure network name has no special characters
4. Check serial output for save errors
5. Try clearing saved credentials:
   - Connect via serial
   - LittleFS may need reformatting

### Problem: Won't connect to saved WiFi

**Symptoms:**
- Credentials saved but can't connect
- LED pulses then goes to fast blink (error)

**Solutions:**
1. Verify WiFi network is in range
2. Check WiFi password is correct
3. Ensure network is 2.4GHz only
4. Check router settings (MAC filtering, etc.)
5. Try reconfiguring with button press
6. Monitor serial output for connection errors

### Problem: Button not working

**Symptoms:**
- Holding button doesn't enter AP mode
- No response from button press

**Solutions:**
1. Verify button is wired correctly (GPIO 16 to GND)
2. Check button is not stuck
3. Try different button
4. Verify button hold time (3 seconds)
5. Check serial output when pressing button

### Problem: LED not working

**Symptoms:**
- No LED feedback
- LED stuck in one state

**Solutions:**
1. Onboard LED should work automatically (GP25)
2. Check if LED physically damaged
3. Verify LED isn't disabled in code
4. Check serial output for LED manager status
5. Try external LED on different pin

---

## 📚 API Reference

### WiFiCredentials Class

```cpp
WiFiCredentials wifiCreds;

// Initialize LittleFS and load credentials
wifiCreds.begin();

// Check if credentials exist
bool hasCredentials();

// Load credentials from flash
bool load();

// Save credentials to flash
bool save(String ssid, String password, String api_endpoint, String api_key);

// Clear stored credentials
bool clear();

// Getters
String getSSID();
String getPassword();
String getAPIEndpoint();
String getAPIKey();
```

### WiFiManager Class

```cpp
WiFiManager wifiMgr;

// Start Access Point mode
void startAPMode();

// Handle web server requests (call in loop)
void handleClient();

// Connect to WiFi using credentials
bool connectToWiFi(String ssid, String password);

// Check if in AP mode
bool isAPMode();

// Get AP SSID
String getAPSSID();
```

### LEDStatusManager Class

```cpp
LEDStatusManager ledStatus;

// Initialize LED
void begin();

// Set LED status
void setStatus(LEDStatus status);
// Status options: LED_OFF, LED_AP_MODE, LED_CONNECTING, 
//                LED_CONNECTED, LED_ERROR, LED_TRANSMITTING

// Update LED (call in loop)
void update();

// Flash LED for transmission
void flashTransmit();

// Get current status
LEDStatus getStatus();

// Manual control
void setLED(bool state);
void setBrightness(int level);
```

### Example Usage:

```cpp
#include "wifi_credentials.h"
#include "wifi_manager.h"
#include "led_status.h"

WiFiCredentials wifiCreds;
WiFiManager wifiMgr;
LEDStatusManager ledStatus;

void setup() {
  Serial.begin(115200);
  
  // Initialize LED
  ledStatus.begin();
  ledStatus.setStatus(LED_OFF);
  
  // Initialize credential storage
  wifiCreds.begin();
  
  // Check for saved credentials
  if (!wifiCreds.hasCredentials()) {
    Serial.println("No credentials - entering AP mode");
    ledStatus.setStatus(LED_AP_MODE);
    wifiMgr.startAPMode();
  } else {
    Serial.println("Found credentials - connecting");
    ledStatus.setStatus(LED_CONNECTING);
    
    if (wifiMgr.connectToWiFi(wifiCreds.getSSID(), wifiCreds.getPassword())) {
      ledStatus.setStatus(LED_CONNECTED);
      Serial.println("Connected!");
    } else {
      ledStatus.setStatus(LED_ERROR);
      Serial.println("Connection failed");
    }
  }
}

void loop() {
  // Update LED animations
  ledStatus.update();
  
  // Handle web portal if in AP mode
  if (wifiMgr.isAPMode()) {
    wifiMgr.handleClient();
  }
}
```

---

## 🔐 Security Considerations

### Current Implementation:
- ✅ AP is **open** (no password) for easy first-time setup
- ✅ Credentials stored in flash (not transmitted in plain text)
- ✅ Web portal served over HTTP (local network only)

### Optional Security Enhancements:

1. **Add AP Password:**
   ```cpp
   WiFi.softAP(ap_ssid.c_str(), "your_password_here");
   ```

2. **Enable HTTPS:**
   - Requires SSL certificates
   - Adds complexity for embedded system

3. **Add Authentication:**
   - Add login page before configuration
   - Store hashed password

4. **Limit Configuration Time:**
   ```cpp
   if (millis() - apStartTime > 300000) { // 5 minutes
     Serial.println("Config timeout - rebooting");
     ESP.restart();
   }
   ```

5. **Clear Credentials on Multiple Failed Attempts:**
   ```cpp
   if (failedAttempts > 5) {
     wifiCreds.clear();
     Serial.println("Too many failures - clearing credentials");
   }
   ```

---

## 📊 Serial Monitor Commands

The system outputs detailed logs via serial (115200 baud):

### Boot Messages:
```
🚀 PICO W TANK MONITOR - SMART BOOT
========================================
✅ LED Status Manager initialized on pin 25
✅ Config button initialized on GPIO16
   Hold button for 3s to enter config mode

📂 Initializing credential storage...
✅ Found saved WiFi credentials
   SSID: MyHomeWiFi
   API Endpoint: /api/tank_update_fixed.php
   Attempting to connect...

📡 Connecting to WiFi...
   SSID: MyHomeWiFi
.......
✅ WiFi connected successfully!
   IP Address: 192.168.1.100

========================================
✅ RX Node initialization complete
   WiFi: Connected
   Ready to receive LoRa data
========================================
```

### AP Mode Messages:
```
========================================
🔧 ENTERING CONFIGURATION MODE
========================================
💡 LED: AP MODE (fast blink)
📱 Configuration Portal Active!
   SSID: PicoW-Tank-A1B2
   IP: 192.168.4.1
   Open browser and connect to configure WiFi
========================================
```

### Runtime Messages:
```
📡 RX Status: Waiting for optimized LoRa packets...
📶 WiFi Connected: YES
🕐 Uptime: 1234 seconds
========================================
🚀 OPTIMIZED LORA PACKET RECEIVED!
📦 Raw HEX payload: 1A2
📡 Raw sensor value received: 418
💧 Water level: 118 cm
📈 Tank fill: 59%
🌐 Sending data to website...
✅ Data successfully sent to website!
========================================
```

---

## 🎓 Additional Resources

### PlatformIO Documentation:
- [Raspberry Pi Pico W Platform](https://docs.platformio.org/en/latest/boards/raspberrypi/pico.html)
- [LittleFS Filesystem](https://arduino-pico.readthedocs.io/en/latest/fs.html)

### WiFi Libraries:
- [Arduino WiFi Library](https://www.arduino.cc/en/Reference/WiFi)
- [ESP8266WebServer](https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266WebServer)

### Hardware:
- [Raspberry Pi Pico W Datasheet](https://datasheets.raspberrypi.com/picow/pico-w-datasheet.pdf)
- [RAK3172 LoRa Module](https://docs.rakwireless.com/Product-Categories/WisDuo/RAK3172-Module/)

---

## 🤝 Support & Contributing

For issues or questions:
1. Check this guide first
2. Review serial output for errors
3. Verify hardware connections
4. Test with minimal configuration

---

## 📝 Version History

- **v1.0** - Initial release with AP configuration system
  - WiFi credential storage (LittleFS)
  - Web portal with network scanner
  - LED status indicators
  - Configuration button support
  - Smart boot logic

---

## 📄 License

This project is part of the Tank Monitor IoT system.

---

**Last Updated:** 2025-10-01  
**Author:** Tank Monitor Development Team
