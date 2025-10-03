# 🔧 TX Sensor Troubleshooting Guide

## Issue: Transmitter Sending `0000` (200cm / 100%)

### Root Cause Identified:
Your **ultrasonic sensor is not returning valid readings**. The TX is defaulting to `0`, which the RX correctly interprets as 200cm/100%.

---

## 📊 What the Serial Output Tells Us:

```
📦 Raw HEX payload: 0000
📡 Raw sensor value received: 0
💧 Water level: 200 cm
📈 Tank fill: 100%
```

**This means:**
- TX sent hex `0000` (decimal 0)
- RX received it correctly
- RX mapped 0 → 200cm (correct math: map(0, 0, 1023, 200, 0) = 200)
- **The problem is on the TX side!**

---

## ✅ Fixes Applied:

### 1. **Enhanced TX Diagnostics**
The new TX firmware will show detailed sensor info:

```
🔧 Sensor Pins:
   TRIG: GPIO15
   ECHO: GPIO14

🔍 Sensor Response:
   Distance returned: -1 cm  ← This means SENSOR FAILED!

❌❌❌ CRITICAL: Sensor read failed!
🔴 Possible issues:
   1. Sensor not connected
   2. Wrong GPIO pins (check TRIG/ECHO)
   3. Sensor power issue
   4. Timeout (no echo received)
   5. Distance out of range (< 2cm or > 400cm)
```

### 2. **Startup Sensor Test**
TX now tests the sensor 3 times at boot:

```
🧪 Testing Ultrasonic Sensor...
Test #1:
❌ Test failed. Retrying...
Test #2:
❌ Test failed. Retrying...
Test #3:
❌ Test failed. Retrying...

⚠️⚠️⚠️  SENSOR INITIALIZATION FAILED!
🚨 Please check:
   - HC-SR04 connected to GPIO15 (TRIG) and GPIO14 (ECHO)
   - Sensor powered (VCC + GND)
   - No obstacles in front of sensor
```

---

## 🔍 Troubleshooting Steps:

### Step 1: Flash the New TX Firmware
```powershell
python -m platformio run -e tx -t upload
```

### Step 2: Open Serial Monitor (115200 baud)
```powershell
python -m platformio device monitor -b 115200
```

### Step 3: Check Boot Messages
Look for the sensor test results. If you see:
```
⚠️⚠️⚠️  SENSOR INITIALIZATION FAILED!
```
Then the sensor is NOT working.

---

## 🔌 Hardware Checklist:

### HC-SR04 Wiring:
```
HC-SR04 Pin    →    Pico W Pin
─────────────────────────────────
VCC            →    3.3V or 5V (VBUS)
GND            →    GND
TRIG           →    GPIO15 (pin 20)
ECHO           →    GPIO14 (pin 19)
```

### Common Issues:

1. **❌ Wrong Pins:**
   - Check `config.h`:
     ```cpp
     #define ULTRASONIC_TRIG_PIN 15  // Must match physical wiring
     #define ULTRASONIC_ECHO_PIN 14  // Must match physical wiring
     ```

2. **❌ Sensor Not Powered:**
   - HC-SR04 needs **5V** for reliable operation
   - Connect VCC to **VBUS (pin 40)** not 3V3
   - Verify GND is connected

3. **❌ Loose Connections:**
   - Check all wires are firmly connected
   - Test continuity with multimeter

4. **❌ Sensor Distance:**
   - HC-SR04 range: **2cm to 400cm**
   - If nothing is in front (open air), sensor times out
   - **Test with a wall or object 50-100cm away**

5. **❌ Bad Sensor:**
   - Try a different HC-SR04
   - Some cheap sensors are defective

---

## 🧪 Quick Test:

### Test 1: Check GPIO Pins
Add this to TX `setup()` temporarily:
```cpp
Serial.println("Testing GPIO pins...");
pinMode(ULTRASONIC_TRIG_PIN, OUTPUT);
pinMode(ULTRASONIC_ECHO_PIN, INPUT);

for (int i = 0; i < 5; i++) {
  digitalWrite(ULTRASONIC_TRIG_PIN, HIGH);
  delay(100);
  digitalWrite(ULTRASONIC_TRIG_PIN, LOW);
  delay(100);
  Serial.println("Trigger pulse " + String(i+1));
}
Serial.println("If sensor LED blinks, wiring is correct!");
```

### Test 2: Manual Distance Measurement
Point sensor at a wall 50cm away, reset TX, check serial:
```
✅ Sensor working! Distance: 50 cm
```

If still fails:
```
❌ Test failed. Retrying...
```
= **Hardware problem confirmed**

---

## 📝 Expected Serial Output (Working Sensor):

### On Boot:
```
🧪 Testing Ultrasonic Sensor...
========================================
Test #1:
✅ Sensor working! Distance: 87 cm

✅ Sensor initialized successfully!
========================================
```

### Every 5 Seconds:
```
========================================
📤 READING SENSOR & TRANSMITTING DATA
========================================
🔊 Triggering ultrasonic pulse...
🔧 Sensor Pins:
   TRIG: GPIO15
   ECHO: GPIO14
🔍 Sensor Response:
   Distance returned: 85 cm

📊 Ultrasonic Measurements:
   Distance to surface: 85 cm
   Water level: 115 cm
   Tank fill: 57%

📡 LoRa Transmission:
   Raw value: 587
   HEX payload: 024B

✅ Data sent successfully via LoRa!
========================================
```

---

## 🎯 What You Should Do NOW:

1. **Flash new TX firmware** (already compiled)
2. **Connect serial monitor** to TX node
3. **Look for sensor test results** at boot
4. **If sensor fails:**
   - Check wiring (especially VCC = 5V, not 3.3V)
   - Point sensor at object 50-100cm away
   - Try different GPIO pins if needed
   - Replace sensor if defective

5. **If sensor works:**
   - You'll see real distance values
   - RX will show correct water levels
   - Problem solved! 🎉

---

## 🆘 Still Not Working?

### Option A: Use Analog Sensor Instead
In `config.h`:
```cpp
#define USE_ULTRASONIC false  // Disable ultrasonic
#define WATER_SENSOR_PIN A0   // Use analog on A0
```

### Option B: Change GPIO Pins
In `config.h`:
```cpp
#define ULTRASONIC_TRIG_PIN 16  // Try GPIO 16
#define ULTRASONIC_ECHO_PIN 17  // Try GPIO 17
```

### Option C: Test with Known-Good Sensor
Borrow a working HC-SR04 to eliminate hardware failure.

---

## 📞 Debug Commands:

In serial monitor, you can also test live:
```
> status    (show current readings)
> test      (manually trigger sensor test)
```

---

**Once sensor is working, you'll immediately see correct values on both TX and RX!**

The RX firmware is working perfectly - it's just processing what the TX sends. Fix the TX sensor, and everything will work. 🚀
