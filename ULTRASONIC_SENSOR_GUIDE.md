# 🔊 Ultrasonic Sensor Integration - HC-SR04

## ✅ Integration Complete!

The TX (transmitter) node now supports **HC-SR04 ultrasonic sensor** using the **TRIGGER and ECHO** method to measure water tank levels.

---

## 📡 How It Works

### Trigger & Echo Method:
1. **TRIGGER Pin** sends a 10µs HIGH pulse
2. Sensor emits ultrasonic sound wave (40kHz)
3. **ECHO Pin** receives pulse duration (time for sound to bounce back)
4. Calculate distance: `Distance = (Time × Speed of Sound) / 2`

### Water Level Calculation:
```
Sensor mounted at TOP of tank
Distance = Empty space above water
Water Level = Tank Depth - Distance to Water
```

---

## ⚙️ Configuration

### Default Pin Assignment:
```cpp
ULTRASONIC_TRIG_PIN = 15  // GPIO15 - Trigger pin
ULTRASONIC_ECHO_PIN = 14  // GPIO14 - Echo pin
```

### Change Pins:
Edit `include/config.h`:
```cpp
#define ULTRASONIC_TRIG_PIN 20  // Change to GPIO20
#define ULTRASONIC_ECHO_PIN 21  // Change to GPIO21
```

### Enable/Disable:
```cpp
#define USE_ULTRASONIC true   // true = ultrasonic, false = analog sensor
```

---

## 🔌 Wiring HC-SR04 to Pico W

### Connections:
```
HC-SR04          Pico W
--------         ------
VCC       -->    VBUS (5V) or 3.3V
GND       -->    GND
TRIG      -->    GPIO15 (default, configurable)
ECHO      -->    GPIO14 (default, configurable)
```

### Important Notes:
- ⚠️ **ECHO pin voltage**: HC-SR04 outputs 5V on ECHO
  - Pico W GPIO is 3.3V tolerant
  - **Use a voltage divider** (2 resistors) to protect GPIO:
    ```
    ECHO --> 1kΩ --> GPIO14 --> 2kΩ --> GND
    ```
  - Or use 3.3V compatible HC-SR04 modules

- ✅ **Power**: HC-SR04 works with both 5V and 3.3V
  - Better performance with 5V (longer range)
  - Connect VCC to VBUS pin (5V from USB)

---

## 📊 Sensor Specifications

### HC-SR04 Specs:
- **Range**: 2cm to 400cm
- **Accuracy**: ±3mm
- **Angle**: 15° cone
- **Frequency**: 40kHz ultrasonic
- **Trigger**: 10µs HIGH pulse
- **Echo**: Pulse width = distance
- **Power**: 5V (or 3.3V)

### Timing:
```
Speed of Sound: 343 m/s = 0.0343 cm/µs
Distance (cm) = (Echo Duration / 2) × 0.0343
```

---

## 🛠️ Installation Steps

### Step 1: Update Configuration
1. Open `include/config.h`
2. Set `USE_ULTRASONIC true`
3. Set your GPIO pins:
   ```cpp
   #define ULTRASONIC_TRIG_PIN 15  // Your choice
   #define ULTRASONIC_ECHO_PIN 14  // Your choice
   ```
4. Set tank depth:
   ```cpp
   #define TANK_MAX_DEPTH 200  // Your tank depth in cm
   ```

### Step 2: Wire the Sensor
```
1. Connect VCC to VBUS (5V)
2. Connect GND to GND
3. Connect TRIG to GPIO15 (through 330Ω resistor - optional)
4. Connect ECHO to GPIO14 (through voltage divider - recommended)
```

### Step 3: Compile & Flash TX Node
```bash
python -m platformio run -e tx --target upload
```

### Step 4: Test
1. Open Serial Monitor (115200 baud)
2. Watch for sensor readings every 5 seconds
3. Verify distance and water level calculations

---

## 📝 Example Serial Output

```
=== LoRa Water Tank TX Node ===
Initializing...
📡 Ultrasonic Sensor Configuration:
   Mode: TRIGGER & ECHO method (HC-SR04)
   Trigger Pin: GPIO15
   Echo Pin: GPIO14
   Max Distance: 400 cm

🔧 Configuring LoRa P2P mode...
✅ TX Node initialization complete!
🚀 Starting water level monitoring...
   Transmission interval: 5 seconds

========================================
📤 READING SENSOR & TRANSMITTING DATA
========================================
🔊 Triggering ultrasonic pulse...
📏 Distance to water surface: 82 cm

📊 Ultrasonic Measurements:
   Distance to surface: 82 cm
   Water level: 118 cm
   Tank fill: 59%

📡 LoRa Transmission:
   Raw value: 427
   HEX payload: 01AB

✅ Data sent successfully via LoRa!
========================================
```

---

## 🎯 Pin Selection Guide

### Available GPIO Pins on Pico W:
```
Recommended for Ultrasonic:
- GPIO 14, 15 (default) ✅
- GPIO 16, 17
- GPIO 18, 19
- GPIO 20, 21
- GPIO 26, 27, 28

Avoid These Pins:
- GPIO 0, 1 (UART TX/RX - used by LoRa)
- GPIO 2 (RAK3172 Reset)
- GPIO 25 (Onboard LED)
- GPIO 29 (ADC_VREF)
```

### Pin Pair Examples:
```cpp
// Option 1 (Default):
#define ULTRASONIC_TRIG_PIN 15
#define ULTRASONIC_ECHO_PIN 14

// Option 2:
#define ULTRASONIC_TRIG_PIN 16
#define ULTRASONIC_ECHO_PIN 17

// Option 3:
#define ULTRASONIC_TRIG_PIN 20
#define ULTRASONIC_ECHO_PIN 21

// Option 4:
#define ULTRASONIC_TRIG_PIN 26
#define ULTRASONIC_ECHO_PIN 27
```

---

## 🔧 Voltage Divider Circuit

### Why Needed?
HC-SR04 ECHO pin outputs **5V**, but Pico W GPIO is **3.3V max**.

### Circuit Diagram:
```
HC-SR04 ECHO (5V)
      |
      ├── R1 (1kΩ) ──┬── To Pico GPIO14
                      |
                      R2 (2kΩ)
                      |
                     GND

Output Voltage = 5V × (R2 / (R1 + R2))
               = 5V × (2kΩ / 3kΩ)
               = 3.33V  ✅ Safe!
```

### Alternative:
Use a **3.3V compatible HC-SR04** module (no voltage divider needed).

---

## 🧪 Testing & Calibration

### Test 1: Sensor Response
1. Flash TX node
2. Open Serial Monitor
3. Move hand in front of sensor
4. Watch distance change in real-time

### Test 2: Full Range
1. Place sensor at tank top
2. Empty tank → Should show ~200cm distance
3. Full tank → Should show ~0cm distance

### Test 3: LoRa Transmission
1. Power both TX and RX nodes
2. TX sends data every 5 seconds
3. RX receives and displays on dashboard

---

## ⚠️ Troubleshooting

### "No echo received - sensor timeout"
**Causes:**
- Sensor not powered
- Wrong pin assignment
- Echo pin not connected
- Sensor out of range

**Fix:**
- Check wiring
- Verify power (5V)
- Test with multimeter
- Ensure object within 2-400cm

### "Distance out of range"
**Causes:**
- Reading < 2cm or > 400cm
- Sensor misaligned
- Surface absorbs sound (foam, fabric)

**Fix:**
- Point sensor at flat, hard surface
- Ensure clear path (no obstacles)
- Check tank depth setting

### Erratic Readings
**Causes:**
- Voltage issues
- Electrical noise
- Temperature changes

**Fix:**
- Add 100µF capacitor across VCC/GND
- Keep wires short
- Use shielded cable for echo pin

---

## 📈 Data Flow

```
TX Node:
┌─────────────────────────────────────┐
│  1. Trigger ultrasonic pulse        │
│  2. Measure echo duration            │
│  3. Calculate distance               │
│  4. Calculate water level            │
│  5. Map to 0-1023 (compatibility)    │
│  6. Convert to HEX                   │
│  7. Send via LoRa                    │
└─────────────────────────────────────┘
              LoRa
                ↓
RX Node:
┌─────────────────────────────────────┐
│  1. Receive LoRa packet              │
│  2. Convert HEX to raw value         │
│  3. Calculate water level            │
│  4. Calculate percentage             │
│  5. Send to XAMPP server             │
│  6. Display on dashboard             │
└─────────────────────────────────────┘
```

---

## 🔄 Switching Between Sensors

### Use Ultrasonic (Default):
```cpp
#define USE_ULTRASONIC true
```

### Use Analog Sensor:
```cpp
#define USE_ULTRASONIC false
```

**No code changes needed!** Just change config and recompile.

---

## 💾 Memory Usage

### TX Node with Ultrasonic:
```
RAM:   [===       ]  26.9% (70,532 bytes)
Flash: [==        ]  16.5% (344,584 bytes)
```

✅ **Very efficient!** Plenty of room for future features.

---

## 📦 What's Included

### Files Modified:
1. ✅ `include/config.h` - Sensor configuration
2. ✅ `src/tx/main.cpp` - Ultrasonic sensor code

### Features Added:
1. ✅ **Trigger & Echo method** implementation
2. ✅ **Distance measurement** in cm
3. ✅ **Water level calculation**
4. ✅ **Error handling** (timeout, out of range)
5. ✅ **Fallback** to last valid reading
6. ✅ **Detailed serial logging**
7. ✅ **LoRa transmission** of sensor data

---

## 🎉 Summary

### What You Get:
- ✅ **HC-SR04 ultrasonic sensor support**
- ✅ **Trigger & Echo method** for distance measurement
- ✅ **Configurable GPIO pins** (change anytime)
- ✅ **Automatic water level calculation**
- ✅ **LoRa transmission** to RX node
- ✅ **Real-time dashboard** display
- ✅ **Error handling & validation**

### How to Use:
1. Wire HC-SR04 to Pico W (TRIG→GPIO15, ECHO→GPIO14)
2. Flash TX node: `python -m platformio run -e tx --target upload`
3. Verify readings in Serial Monitor
4. Data automatically sent to RX → Dashboard

### How to Change Pins:
1. Edit `include/config.h`
2. Change `ULTRASONIC_TRIG_PIN` and `ULTRASONIC_ECHO_PIN`
3. Recompile and flash
4. Done!

---

**Your ultrasonic sensor is ready! Mount it at the top of your tank and enjoy accurate water level monitoring!** 🎊

---

**Build Status**: ✅ SUCCESS  
**Date**: 2025-10-02  
**Firmware**: `.pio/build/tx/firmware.uf2`
