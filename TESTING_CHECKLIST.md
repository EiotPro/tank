# WiFi AP Configuration System - Testing Checklist
## Raspberry Pi Pico W Tank Monitor

---

## 🧪 Pre-Test Setup

### Hardware Requirements:
- ✅ Raspberry Pi Pico W (with WiFi)
- ✅ USB cable for power and serial monitoring
- ✅ Config button (GPIO 16 to GND)
- ✅ RAK3172 LoRa module (for full system test)
- ✅ Computer/Phone with WiFi capabilities

### Software Requirements:
- ✅ PlatformIO installed
- ✅ Serial monitor ready (115200 baud)
- ✅ Web browser (Chrome, Firefox, Safari, etc.)
- ✅ Code compiled successfully

---

## 📋 Test Scenarios

### Test 1: First Boot (No Credentials)

**Expected Behavior:**
- Device boots into AP mode automatically
- LED fast blinking
- AP network visible: `PicoW-Tank-XXXX`
- Configuration portal accessible at `192.168.4.1`

**Test Steps:**
1. ✅ Flash firmware to fresh Pico W
2. ✅ Power on device
3. ✅ Check serial output for boot messages
4. ✅ Verify LED is fast blinking
5. ✅ Scan WiFi networks on phone/computer
6. ✅ Confirm `PicoW-Tank-XXXX` network appears

**Serial Output Should Show:**
```
🚀 PICO W TANK MONITOR - SMART BOOT
✅ LED Status Manager initialized on pin 25
✅ Config button initialized on GPIO16
📂 Initializing credential storage...
⚠️  No WiFi credentials found!
   Starting in configuration mode...
🔧 ENTERING CONFIGURATION MODE
📱 Configuration Portal Active!
   SSID: PicoW-Tank-XXXX
   IP: 192.168.4.1
```

**Pass Criteria:**
- [ ] Device boots into AP mode
- [ ] LED fast blinking (200ms interval)
- [ ] Serial shows "No WiFi credentials found"
- [ ] AP network visible
- [ ] No crashes or errors

---

### Test 2: Configuration Portal Access

**Expected Behavior:**
- Can connect to AP
- Portal loads at `192.168.4.1`
- Beautiful web interface displays
- Network scanner button present

**Test Steps:**
1. ✅ Connect phone/computer to `PicoW-Tank-XXXX`
2. ✅ Open browser
3. ✅ Navigate to `http://192.168.4.1`
4. ✅ Verify portal loads
5. ✅ Check UI elements (buttons, fields, styling)

**Pass Criteria:**
- [ ] Successfully connect to AP
- [ ] Portal loads without errors
- [ ] All UI elements visible
- [ ] CSS styling applied correctly
- [ ] No JavaScript console errors

---

### Test 3: WiFi Network Scanning

**Expected Behavior:**
- Scan button triggers network scan
- Available networks display with signal strength
- Networks are selectable
- Signal strength icons show correctly

**Test Steps:**
1. ✅ Click "Scan Networks" button
2. ✅ Wait for scan to complete (~3-5 seconds)
3. ✅ Verify networks appear in list
4. ✅ Check signal strength indicators (●●●● / ●●● / ●● / ●)
5. ✅ Select a network
6. ✅ Verify SSID populates in field

**Pass Criteria:**
- [ ] Scan button works
- [ ] At least 1 network detected
- [ ] Signal strength indicators correct
- [ ] Network selection populates SSID field
- [ ] Proper sorting (strongest first)

---

### Test 4: WiFi Configuration Save

**Expected Behavior:**
- Can enter WiFi credentials
- Save button stores configuration
- Device reboots automatically
- Connects to configured WiFi

**Test Steps:**
1. ✅ Enter valid SSID (or select from scan)
2. ✅ Enter valid WiFi password
3. ✅ (Optional) Enter API endpoint and key
4. ✅ Click "Save Configuration"
5. ✅ Monitor serial output
6. ✅ Wait for device reboot
7. ✅ Verify WiFi connection

**Serial Output Should Show:**
```
📥 Configuration received:
   SSID: MyHomeWiFi
   Password: ********
   API Endpoint: /api/tank_update_fixed.php
✅ Configuration saved to flash!
🔄 Rebooting in 3 seconds...
```

**After Reboot:**
```
📂 Initializing credential storage...
✅ Found saved WiFi credentials
   SSID: MyHomeWiFi
📡 Connecting to WiFi...
.......
✅ WiFi connected successfully!
   IP Address: 192.168.1.XXX
```

**Pass Criteria:**
- [ ] Configuration saves successfully
- [ ] Device reboots automatically
- [ ] Credentials persist after reboot
- [ ] Device connects to configured WiFi
- [ ] LED changes to solid (connected)

---

### Test 5: LED Status Indicators

**Expected Behavior:**
- LED shows correct patterns for each state
- Smooth transitions between states
- Visible feedback for all operations

**Test Steps:**
1. ✅ **AP Mode**: Fast blink (200ms)
2. ✅ **Connecting**: Slow pulse (1s fade)
3. ✅ **Connected**: Solid ON
4. ✅ **Transmitting**: Quick flash
5. ✅ **Error**: Fast blink (200ms)

**Pass Criteria:**
- [ ] AP mode: Fast blink visible
- [ ] Connecting: Smooth pulsing effect
- [ ] Connected: LED stays solid
- [ ] Transmit flash visible during data send
- [ ] Error state: Fast blink (red if RGB LED)

---

### Test 6: Configuration Button

**Expected Behavior:**
- Button press detected
- Holding for 3 seconds enters AP mode
- Device switches from normal operation to config mode

**Test Steps:**
1. ✅ Start with device connected to WiFi (normal mode)
2. ✅ Press and hold config button
3. ✅ Monitor serial output
4. ✅ Hold for 3 seconds
5. ✅ Release button
6. ✅ Verify AP mode starts

**Serial Output Should Show:**
```
🔘 Config button pressed...
(hold for 3 seconds)
🔘 Config button held - entering AP mode!
🔧 ENTERING CONFIGURATION MODE
📱 Configuration Portal Active!
```

**Pass Criteria:**
- [ ] Button press detected
- [ ] Hold time ~3 seconds required
- [ ] AP mode starts on release
- [ ] LED changes to fast blink
- [ ] Portal accessible
- [ ] Normal operation suspended

---

### Test 7: WiFi Auto-Reconnect

**Expected Behavior:**
- Device detects WiFi disconnection
- Attempts automatic reconnection
- LED shows connecting state
- Reconnects successfully

**Test Steps:**
1. ✅ Device connected to WiFi (normal operation)
2. ✅ Turn off WiFi router or change password
3. ✅ Monitor serial output
4. ✅ Wait for disconnection detection (~30s)
5. ✅ Restore WiFi router
6. ✅ Verify auto-reconnection

**Serial Output Should Show:**
```
📵 WiFi disconnected, attempting to reconnect...
📡 Connecting to WiFi...
   SSID: MyHomeWiFi
.......
✅ WiFi connected successfully!
```

**Pass Criteria:**
- [ ] Disconnection detected within 30s
- [ ] LED changes to pulsing (connecting)
- [ ] Automatic reconnection attempted
- [ ] Successful reconnection
- [ ] Normal operation resumes

---

### Test 8: WiFi Connection Failure Handling

**Expected Behavior:**
- Invalid credentials don't crash system
- Error state indicated
- Device can be reconfigured

**Test Steps:**
1. ✅ Enter invalid WiFi password
2. ✅ Save configuration
3. ✅ Device reboots
4. ✅ Monitor connection attempt
5. ✅ Verify error handling
6. ✅ Use button to reconfigure

**Serial Output Should Show:**
```
📡 Connecting to WiFi...
.....................
❌ WiFi connection failed!
💡 Hold config button to reconfigure
```

**Pass Criteria:**
- [ ] Connection failure detected
- [ ] LED shows error state (fast blink)
- [ ] No system crash
- [ ] Config button still works
- [ ] Can reconfigure successfully

---

### Test 9: Credential Persistence

**Expected Behavior:**
- Credentials survive power cycles
- Device auto-connects after power loss
- No reconfiguration needed

**Test Steps:**
1. ✅ Configure device with valid credentials
2. ✅ Verify successful connection
3. ✅ Power off device completely
4. ✅ Wait 10 seconds
5. ✅ Power on device
6. ✅ Verify auto-connection

**Serial Output Should Show:**
```
📂 Initializing credential storage...
✅ Found saved WiFi credentials
   SSID: MyHomeWiFi
📡 Connecting to WiFi...
✅ WiFi connected successfully!
```

**Pass Criteria:**
- [ ] Credentials loaded from flash
- [ ] Auto-connection successful
- [ ] No need to reconfigure
- [ ] LED shows correct sequence
- [ ] Normal operation resumes

---

### Test 10: LoRa Integration (Full System Test)

**Expected Behavior:**
- LoRa reception works in all WiFi states
- Data forwarding works when connected
- LED flashes during transmission
- No interference between systems

**Test Steps:**
1. ✅ Configure WiFi and connect
2. ✅ Send LoRa packet from transmitter
3. ✅ Monitor serial for LoRa reception
4. ✅ Verify data processing
5. ✅ Check web server data upload
6. ✅ Monitor LED transmission flash

**Serial Output Should Show:**
```
📡 LoRa RX: +EVT:RXP2P:...
🚀 OPTIMIZED LORA PACKET RECEIVED!
📦 Raw HEX payload: 1A2
📡 Raw sensor value received: 418
💧 Water level: 118 cm
📈 Tank fill: 59%
🌐 Sending data to website...
✅ Data successfully sent to website!
```

**Pass Criteria:**
- [ ] LoRa packets received
- [ ] Data processed correctly
- [ ] WiFi transmission successful
- [ ] LED flashes during upload
- [ ] No system interference
- [ ] Normal operation continues

---

### Test 11: Advanced Settings (API Configuration)

**Expected Behavior:**
- Advanced settings section expands/collapses
- API endpoint and key saved correctly
- Credentials used in data transmission

**Test Steps:**
1. ✅ Open configuration portal
2. ✅ Click "Advanced Settings"
3. ✅ Enter API endpoint (e.g., `/api/tank_update_fixed.php`)
4. ✅ Enter API key
5. ✅ Save configuration
6. ✅ Trigger data transmission
7. ✅ Verify correct API usage

**Pass Criteria:**
- [ ] Advanced section toggles correctly
- [ ] API fields accept input
- [ ] Settings save to flash
- [ ] Correct API endpoint used in requests
- [ ] API key included in payload

---

### Test 12: Portal UI/UX

**Expected Behavior:**
- Responsive design works on mobile
- All animations smooth
- Password toggle works
- Status messages clear

**Test Steps:**
1. ✅ Test on mobile device (phone)
2. ✅ Test on tablet
3. ✅ Test on desktop
4. ✅ Toggle password visibility
5. ✅ Check animations (loading spinner, fade effects)
6. ✅ Verify status messages appear

**Pass Criteria:**
- [ ] Responsive on all screen sizes
- [ ] Password show/hide toggle works
- [ ] Smooth animations
- [ ] Clear status feedback
- [ ] No layout issues
- [ ] Touch-friendly on mobile

---

## 🔍 Stress Testing

### Test 13: Multiple Rapid Configurations

**Test Steps:**
1. ✅ Configure WiFi
2. ✅ Immediately enter config mode (button)
3. ✅ Change to different WiFi
4. ✅ Repeat 3-5 times
5. ✅ Monitor for crashes or memory leaks

**Pass Criteria:**
- [ ] No crashes
- [ ] Each configuration saves correctly
- [ ] No memory leaks
- [ ] System remains stable

---

### Test 14: Long-Duration Operation

**Test Steps:**
1. ✅ Configure and connect
2. ✅ Leave running for 24 hours
3. ✅ Monitor WiFi connection stability
4. ✅ Check for memory leaks
5. ✅ Verify LoRa still works

**Pass Criteria:**
- [ ] WiFi stays connected
- [ ] No unexpected reboots
- [ ] LED states correct
- [ ] LoRa reception continues
- [ ] Memory usage stable

---

### Test 15: Network Switching

**Test Steps:**
1. ✅ Configure WiFi A
2. ✅ Connect successfully
3. ✅ Reconfigure to WiFi B
4. ✅ Connect successfully
5. ✅ Reconfigure back to WiFi A
6. ✅ Verify all switches work

**Pass Criteria:**
- [ ] Switches between networks
- [ ] No credential corruption
- [ ] Each connection successful
- [ ] LED states correct

---

## ✅ Test Summary Sheet

### Quick Pass/Fail Checklist:

| # | Test Name | Pass | Fail | Notes |
|---|-----------|------|------|-------|
| 1 | First Boot (No Creds) | ⬜ | ⬜ | |
| 2 | Portal Access | ⬜ | ⬜ | |
| 3 | Network Scanning | ⬜ | ⬜ | |
| 4 | Config Save | ⬜ | ⬜ | |
| 5 | LED Indicators | ⬜ | ⬜ | |
| 6 | Config Button | ⬜ | ⬜ | |
| 7 | Auto-Reconnect | ⬜ | ⬜ | |
| 8 | Connection Failure | ⬜ | ⬜ | |
| 9 | Credential Persistence | ⬜ | ⬜ | |
| 10 | LoRa Integration | ⬜ | ⬜ | |
| 11 | Advanced Settings | ⬜ | ⬜ | |
| 12 | Portal UI/UX | ⬜ | ⬜ | |
| 13 | Rapid Reconfig | ⬜ | ⬜ | |
| 14 | Long Duration | ⬜ | ⬜ | |
| 15 | Network Switching | ⬜ | ⬜ | |

---

## 🐛 Known Issues / Notes

### Document any issues found during testing:

**Issue 1:**
- Description: 
- Severity: Low / Medium / High / Critical
- Workaround: 
- Fix Status: 

**Issue 2:**
- Description: 
- Severity: 
- Workaround: 
- Fix Status: 

---

## 📝 Testing Environment

**Hardware:**
- Pico W Board: 
- LoRa Module: RAK3172
- Button Type: 
- Power Supply: 

**Software:**
- PlatformIO Version: 
- Framework Version: 
- Firmware Version: v1.0

**Network:**
- WiFi Router: 
- Frequency: 2.4GHz / 5GHz
- Security: WPA2 / WPA3 / Open

**Date Tested:** _________________
**Tester Name:** _________________
**Test Duration:** _________________

---

## 🎯 Next Steps After Testing

1. ✅ All tests passed → Deploy to production
2. ⚠️ Minor issues → Document and fix
3. ❌ Major failures → Debug and retest
4. 📊 Performance issues → Optimize code
5. 📚 Update documentation with findings

---

**Last Updated:** 2025-10-01  
**Version:** 1.0
