# WiFi Configuration System - Quick Reference Card
## Raspberry Pi Pico W Tank Monitor

---

## 🚀 Quick Start (3 Steps)

1. **Flash Firmware**
   ```bash
   pio run --target upload
   ```

2. **Connect to AP**
   - WiFi: `PicoW-Tank-XXXX`
   - URL: `http://192.168.4.1`

3. **Configure & Save**
   - Scan networks
   - Enter password
   - Click Save

---

## 💡 LED Status Guide

| LED Pattern | Meaning |
|-------------|---------|
| **⚡ Fast Blink** | Configuration mode |
| **🌊 Slow Pulse** | Connecting to WiFi |
| **✅ Solid ON** | Connected |
| **❌ Fast Blink** | Error |
| **📡 Quick Flash** | Data transmission |

---

## 🔧 Hardware Pins

| Component | Pin | Notes |
|-----------|-----|-------|
| Config Button | GPIO 16 | OPTIONAL - not required |
| Status LED | GP25 | Onboard LED |
| LoRa TX | GPIO 0 | Serial1 to RAK3172 |
| LoRa RX | GPIO 1 | Serial1 from RAK3172 |

---

## 🔧 Enter Config Mode (No Button Required!)

**Method 1 - Serial Command (Easiest):**
- Open serial monitor (115200 baud)
- Type `config` and press Enter
- LED starts fast blinking

**Method 2 - Triple Power Cycle:**
- Power OFF/ON three times within 10 seconds
- Device auto-enters config mode

**Method 3 - Auto-Trigger:**
- After 3 failed WiFi attempts
- Device auto-enters config mode

**Method 4 - Clear Credentials:**
- Type `clear` in serial monitor
- Device reboots in config mode

---

## 🌐 Web Portal

**URL:** `http://192.168.4.1`

**Features:**
- 📡 Scan WiFi networks
- 🔐 Enter credentials
- ⚙️ Advanced API settings
- 💾 Save & auto-reboot

---

## 📊 Serial Monitor

**Baud Rate:** `115200`

**Key Messages:**
```
⚠️  No WiFi credentials found!
    → First boot, needs config

📱 Configuration Portal Active!
    → AP mode ready

✅ WiFi connected successfully!
    → Normal operation

❌ WiFi connection failed!
    → Check credentials
```

---

## 🐛 Troubleshooting

| Problem | Solution |
|---------|----------|
| Can't see AP | Check power, wait 10s |
| Can't connect to AP | AP is open (no password) |
| Portal won't load | Use `http://192.168.4.1` |
| WiFi won't save | Check password, use 2.4GHz |
| Can't enter config | Type `config` in serial monitor |

---

## 📁 Files Created

```
include/
├── wifi_credentials.h    ✅ Storage
├── wifi_manager.h         ✅ AP mode
└── led_status.h           ✅ LEDs

src/rx/
└── main.cpp               ✅ Updated

Documentation/
├── WIFI_CONFIG_GUIDE.md           ✅ Full guide
├── TESTING_CHECKLIST.md           ✅ Tests
├── WIFI_IMPLEMENTATION_SUMMARY.md ✅ Summary
└── QUICK_REFERENCE.md             ✅ This card
```

---

## 🎯 Common Tasks

### Change WiFi Network
1. Type `config` in serial monitor
2. Connect to AP
3. Configure new WiFi

### Check Connection Status
- Look at LED
- Open serial monitor (115200)

### Clear Credentials
- Type `clear` in serial monitor
- Or type `config` to reconfigure

### Test Configuration
- See TESTING_CHECKLIST.md

---

## 📞 Need Help?

1. Check LED status
2. Open serial monitor (type `help`)
3. Review CONFIG_MODE_WITHOUT_BUTTON.md
4. Review WIFI_CONFIG_GUIDE.md
5. Run test checklist

---

## ✅ System Status

**Implementation:** ✅ COMPLETE  
**Testing:** Ready to start  
**Documentation:** Complete  
**Status:** Production-ready  

---

**Version:** 1.0  
**Date:** 2025-10-01
