# 🔍 RX Node Diagnostic Report

## Critical Issues Identified

### **ISSUE #1: Default Values Show 200cm / 100%** ⚠️
**Location:** `src/rx/main.cpp` lines 19-20  
**Problem:**
```cpp
int currentWaterLevel = 0;
int currentPercentage = 0;
```
**Why This Breaks:**
- The RX code maps rawSensorValue (0-1023) to waterLevel (200-0 cm)
- When `rawSensorValue = 0`, it maps to `waterLevel = 200 cm`
- When `waterLevel = 0`, percentage calculates to `100%`
- **Result:** Before any LoRa packet arrives, portal shows 200 cm / 100%

**Fix Required:**
- Initialize to `-1` (invalid sentinel value)
- Add `hasLoRaData` flag to track if real data received
- Update web portal to show "Waiting for data..." when no packets

---

### **ISSUE #2: Blocking WiFi Reconnection Loop** 🚫
**Location:** `src/rx/main.cpp` lines 250-256  
**Problem:**
```cpp
while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    ledStatus.update();
    attempts++;
}
```
**Why This Breaks:**
- WiFi reconnection takes up to **10 seconds** (20 × 500ms)
- During this time, **LoRa reception is completely blocked**
- Periodic WiFi checks (line 617-625) trigger every 30 seconds
- **Result:** LoRa packets are missed during reconnection attempts

**Fix Required:**
- Replace blocking `while` loop with non-blocking state machine
- Use `millis()` timing instead of `delay()`
- Allow LoRa processing to continue during WiFi operations

---

### **ISSUE #3: Slow AP Mode Startup** 🐢
**Location:** `src/rx/main.cpp` setup() function  
**Problem:**
- Multiple `delay(1000)` and `delay(500)` calls in sequence
- LoRa initialization happens **before** WiFi setup
- Total startup time: **8-10 seconds** before AP visible

**Delays Found:**
- Line 460: `delay(1000)` - Serial init
- Line 518: `delay(2000)` - LoRa reset
- Line 522: `delay(3000)` - LoRa init wait
- Line 530-577: Multiple 500ms delays in LoRa config

**Fix Required:**
- Reduce unnecessary delays
- Start AP mode earlier in boot sequence
- Overlap initialization tasks where possible

---

### **ISSUE #4: No Packet Validation** ⚠️
**Location:** `src/rx/main.cpp` lines 652-674  
**Problem:**
```cpp
int rawSensorValue = hexToDec(hexPayload);
// No validation that rawSensorValue is within 0-1023 range
// No CRC check, length check, or bounds validation
```
**Why This Breaks:**
- Corrupted packets accepted as valid
- Out-of-range values (e.g., 9999) mapped to nonsense readings
- No way to detect transmission errors

**Fix Required:**
- Validate rawSensorValue is 0-1023
- Check hex payload length (should be 4 chars for 2-byte value)
- Add packet timestamp and staleness detection

---

### **ISSUE #5: Excessive Blocking Delays in Main Loop** 🔥
**Location:** `src/rx/main.cpp` loop() function  
**Problem:**
- Line 736: `delay(100)` in LoRa test
- Line 741: `delay(200)` ensuring RX mode
- Line 747: `delay(100)` at end of loop
- **Total:** ~400ms of blocking per loop iteration

**Why This Breaks:**
- LoRa packets can arrive at any time
- 400ms delay means packets might be missed
- WiFi handleClient() not called promptly in config mode

**Fix Required:**
- Remove all `delay()` calls from loop()
- Use millis() timing for periodic tasks
- Keep loop iteration time under 10ms

---

### **ISSUE #6: No Data Timeout Detection** ⏰
**Problem:**
- If TX node stops transmitting, RX shows last received values forever
- No indication that data is stale
- User can't tell if system is working

**Fix Required:**
- Track timestamp of last valid packet
- If no packet received for 30s, show "No recent data"
- Add visual indicator in web portal

---

## Recommended Fix Priority

1. **HIGH PRIORITY** - Fix default values and add hasLoRaData flag
2. **HIGH PRIORITY** - Remove blocking WiFi reconnection
3. **MEDIUM** - Add packet validation and bounds checking
4. **MEDIUM** - Remove delays from main loop
5. **LOW** - Optimize AP startup time
6. **LOW** - Add data timeout detection

---

## Configuration Verification

### LoRa Settings (MUST MATCH ON TX AND RX):
✅ Frequency: 865000000 Hz  
✅ Spreading Factor: 7  
✅ Bandwidth: 1 (125 kHz)  
✅ Coding Rate: 1 (4/5)  
✅ Preamble: 8  
✅ TX Power: 14 dBm  

### TX Configuration:
✅ Ultrasonic sensor enabled  
✅ Transmission interval: 5000ms (5 seconds)  
✅ Tank max depth: 200 cm  

### Expected Data Flow:
TX reads distance → Maps to 0-1023 → Converts to HEX → Sends via LoRa  
RX receives HEX → Converts to 0-1023 → Maps to 200-0 cm → Displays

---

## Next Steps

Run the automated fixes to:
1. Initialize variables to -1
2. Add hasLoRaData flag
3. Implement non-blocking WiFi watchdog
4. Add packet validation
5. Remove blocking delays
6. Update web portal UI

After fixes, the RX should:
- Show "Waiting for data..." until first packet arrives
- Never disconnect during LoRa reception
- Display AP portal within 3-5 seconds of boot
- Properly validate and display live tank data
