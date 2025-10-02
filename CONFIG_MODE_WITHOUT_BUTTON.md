# How to Enter Configuration Mode (Without Physical Button)

## 🎯 Overview

You can enter WiFi configuration mode using **4 different methods** - no physical button required!

---

## ✅ Method 1: Serial Command (Easiest & Recommended)

### Steps:
1. Connect to Pico W via USB
2. Open serial monitor (115200 baud)
3. Type one of these commands:
   - `config`
   - `setup`
   - `wifi`
4. Press Enter

### Example:
```
📊 RX Status: Waiting for optimized LoRa packets...
📶 WiFi Connected: YES
config                    <-- YOU TYPE THIS
🔧 Serial command received: Entering config mode...
========================================
🔧 ENTERING CONFIGURATION MODE
========================================
```

**Result:** Device enters AP mode immediately!

---

## ⚡ Method 2: Triple Power Cycle

### Steps:
1. Power OFF the Pico W
2. Wait 1 second
3. Power ON
4. Wait 2-3 seconds (until you see serial output)
5. Power OFF again
6. Wait 1 second
7. Power ON
8. Wait 2-3 seconds
9. Power OFF again
10. Wait 1 second
11. Power ON (3rd time)

**Important:** All 3 power cycles must happen within 10 seconds total.

### Serial Output:
```
⚡ Quick power cycle detected (1/3)
⚡ Quick power cycle detected (2/3)
⚡ Quick power cycle detected (3/3)
🔧 Triple power cycle detected! Entering config mode...
```

**Result:** Device enters AP mode automatically!

---

## 🔄 Method 3: Auto-Trigger (After Failed Connections)

If WiFi connection fails **3 times in a row**, the device automatically enters config mode.

### How it works:
1. Device tries to connect to saved WiFi
2. Connection fails → Attempt 1/3
3. Device tries again
4. Connection fails → Attempt 2/3
5. Device tries again
6. Connection fails → Attempt 3/3
7. **Automatic entry to config mode!**

### Serial Output:
```
❌ WiFi connection failed!
   Attempt 3/3

⚠️  Maximum retry attempts reached!
🔧 Automatically entering config mode...
```

**Use case:** When you change your WiFi router or move the device to a new location.

---

## 📋 Method 4: Clear Credentials Command

Reset WiFi credentials and force config mode on next boot.

### Steps:
1. Open serial monitor (115200 baud)
2. Type one of these commands:
   - `clear`
   - `reset`
3. Press Enter

### Serial Output:
```
🗑️  Clearing WiFi credentials...
✅ Credentials cleared. Rebooting...
(device reboots)
⚠️  No WiFi credentials found!
   Starting in configuration mode...
```

**Result:** Device reboots and starts in config mode!

---

## 📱 Other Useful Serial Commands

Open serial monitor and type any of these:

| Command | Action |
|---------|--------|
| `config`, `setup`, `wifi` | Enter configuration mode |
| `clear`, `reset` | Clear credentials & reboot |
| `status`, `info` | Show system status |
| `reboot`, `restart` | Restart device |
| `help`, `?` | Show all commands |

---

## 🖥️ Example: Using Serial Monitor

### PlatformIO Serial Monitor:
```bash
python -m platformio device monitor -b 115200
```

### Arduino IDE:
1. Tools → Serial Monitor
2. Set baud rate to 115200
3. Type commands and press Enter

### PuTTY / Tera Term:
1. Connect to COM port
2. Settings: 115200 baud, 8N1
3. Type commands and press Enter

---

## 🎓 Status Command Output

Type `status` or `info` to see current system state:

```
📊 SYSTEM STATUS:
   WiFi: Connected
   IP: 192.168.1.100
   SSID: MyHomeWiFi
   Has Credentials: Yes
   Saved SSID: MyHomeWiFi
   In Config Mode: No
   Uptime: 1234s
```

---

## 🔧 Configuration Settings

In `src/rx/main.cpp`, you can adjust these settings:

```cpp
// Disable physical button (default)
#define USE_CONFIG_BUTTON false

// Maximum WiFi retry attempts before auto config mode
#define MAX_WIFI_RETRY_ATTEMPTS 3

// Boot count file for power cycle detection
#define BOOT_COUNT_FILE "/boot_count.txt"
```

### To Enable Physical Button (if you add one later):
Change this line:
```cpp
#define USE_CONFIG_BUTTON true  // Enable button
```

---

## 📊 Configuration Mode Entry Priority

The device checks for config mode triggers in this order:

1. **Triple power cycle** ✅ (checked at boot)
2. **No credentials** ✅ (checked at boot)
3. **Serial command** ✅ (checked continuously)
4. **WiFi retry limit** ✅ (after 3 failures)
5. **Physical button** ⚠️ (if enabled)

---

## 🎯 Recommendations

### For Development/Testing:
- **Best:** Serial command (`config`)
- Fast, easy, no waiting

### For Field Deployment:
- **Best:** Auto-trigger (3 failed attempts)
- Automatic recovery from WiFi changes

### For Emergency Access:
- **Best:** Triple power cycle
- Works even if serial is not accessible

---

## 💡 Tips

### Tip 1: Keep Serial Monitor Open
Always keep serial monitor open during initial setup and testing. You'll see:
- Boot messages
- WiFi connection status
- Config mode triggers
- Error messages

### Tip 2: Use Tab Completion
Some serial terminals support tab completion. Try typing `con` + Tab.

### Tip 3: Power Cycle Timing
For triple power cycle:
- Power OFF: Pull USB cable
- Power ON: Plug USB cable back in
- Wait 2-3 seconds between cycles (enough to see boot message)
- Complete all 3 cycles within 10 seconds total

### Tip 4: Check LED Status
Watch the onboard LED:
- **Fast blinking** = Config mode (AP active)
- **Slow pulsing** = Connecting to WiFi
- **Solid ON** = Connected successfully

---

## 🐛 Troubleshooting

### Problem: Serial commands not working

**Solution:**
1. Check serial connection (USB cable)
2. Verify baud rate is 115200
3. Make sure you press Enter after typing command
4. Check that device is not in config mode already

### Problem: Triple power cycle not working

**Solution:**
1. Make sure all 3 cycles happen within 10 seconds
2. Wait 2-3 seconds between each cycle (let boot start)
3. Check serial output for "Quick power cycle detected" messages
4. If it fails, wait 10 seconds and try again

### Problem: Auto-trigger not working

**Solution:**
1. Check `MAX_WIFI_RETRY_ATTEMPTS` is set to 3
2. Make sure WiFi is actually failing (wrong password, network down)
3. Each attempt takes ~10 seconds, so wait ~30 seconds total

---

## 📝 Configuration Workflow

### First Time Setup:
1. Flash firmware
2. Device boots with no credentials
3. **Automatically enters config mode**
4. Connect to `TankMonitor-XXXX` network
5. Open `http://192.168.4.1`
6. Configure WiFi
7. Done!

### Changing WiFi Network:
**Option A - Serial Command:**
1. Open serial monitor
2. Type `config`
3. Connect to AP
4. Configure new WiFi

**Option B - Clear & Reboot:**
1. Open serial monitor
2. Type `clear`
3. Device reboots in config mode
4. Configure new WiFi

**Option C - Power Cycle:**
1. Power OFF/ON three times
2. Device enters config mode
3. Configure new WiFi

---

## 🎉 Summary

You have **FOUR ways** to enter configuration mode without a physical button:

1. ✅ **Serial Command** - Type `config` (fastest)
2. ✅ **Triple Power Cycle** - 3 power cycles in 10 seconds
3. ✅ **Auto-Trigger** - After 3 WiFi failures
4. ✅ **Clear Credentials** - Type `clear` + reboot

**No button needed!** 🎊

---

## 📚 Related Documents

- `QUICK_REFERENCE.md` - Quick reference card
- `WIFI_CONFIG_GUIDE.md` - Complete WiFi configuration guide
- `TESTING_CHECKLIST.md` - Testing procedures
- `COMPILATION_SUCCESS.md` - Build information

---

**Last Updated:** 2025-10-01  
**Version:** 1.0 (No Button Required Edition)
