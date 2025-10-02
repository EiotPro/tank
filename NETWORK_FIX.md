# 🌐 Network Connectivity Issue - Cannot Reach Server

## ❌ Current Problem

Your Pico W **IS connected to WiFi** but **CANNOT reach your XAMPP server** at `10.97.116.57:80`.

### Error Message:
```
🌐 Sending data to website...
🔍 Testing main API endpoint...
❌ Cannot connect to server
🚫 API not accessible - check server configuration
💡 Try: https://iotlogic.in/Tank/tank/api/tank_update.php
💡 Make sure API endpoint is configured correctly
```

### What This Means:
- ✅ Pico W is connected to your WiFi network
- ✅ LoRa is receiving data properly  
- ❌ Pico W **cannot connect** to your computer's IP (`10.97.116.57`)
- ❌ XAMPP server is either not running or not reachable

---

## 🔍 Diagnosis Steps

### Step 1: Check if XAMPP is Running

**On Windows:**
1. Open XAMPP Control Panel
2. Check if **Apache** is running (should show green "Running")
3. If not, click **Start** next to Apache

**Quick Check:**
- Open browser on your computer
- Go to `http://localhost/Tank/tank/api/tank_update.php`
- Should show some response (not 404)

---

### Step 2: Verify Your Computer's IP Address

The configured IP is `10.97.116.57`. Let's verify this is still correct:

**Check your IP:**
```powershell
ipconfig
```

Look for "IPv4 Address" under your WiFi adapter. It should match `10.97.116.57`.

**Common Issues:**
- ❌ IP address changed (DHCP gave you a new IP)
- ❌ Connected to different WiFi network
- ❌ VPN is active and changing routing

---

### Step 3: Check Firewall Settings

Windows Firewall might be blocking the connection:

**Allow XAMPP through Firewall:**
1. Open **Windows Defender Firewall**
2. Click **Allow an app through firewall**
3. Find **Apache HTTP Server** or **httpd.exe**
4. Make sure BOTH "Private" and "Public" are checked
5. If not listed, click **Change settings** → **Allow another app** → Browse to:
   ```
   C:\xampp\apache\bin\httpd.exe
   ```

---

### Step 4: Test Connection from Computer

**From your Windows computer:**
```powershell
# Test if web server is accessible
curl http://10.97.116.57/Tank/tank/api/tank_update.php
```

Or open in browser:
```
http://10.97.116.57/Tank/tank/api/tank_update.php
```

**Expected Result:**
- Should get some response (even if it's an error about missing POST data)
- Should NOT timeout or fail to connect

---

### Step 5: Verify Both Devices are on Same Network

**The Pico W and your computer MUST be on the same WiFi network.**

**Check Pico W WiFi:**
- From Serial Monitor, type: `status`
- Look at the IP address the Pico got
- Should be in same subnet as your computer (e.g., `10.97.116.x`)

**Example - Good:**
```
Computer IP:  10.97.116.57
Pico W IP:    10.97.116.123  ✅ Same subnet (10.97.116.x)
```

**Example - Bad:**
```
Computer IP:  10.97.116.57
Pico W IP:    192.168.1.45   ❌ Different subnet!
```

---

## ✅ Solutions

### Solution 1: Get Your Current Computer IP

**Run this command:**
```powershell
ipconfig | findstr IPv4
```

**Update config.h with the correct IP:**

1. Note the IP address shown (e.g., `10.97.115.200`)
2. Edit `include/config.h`:
   ```cpp
   #define WEBSITE_HOST "10.97.115.200"  // Update this to YOUR current IP
   ```
3. Recompile and flash:
   ```bash
   python -m platformio run -e rx --target upload
   ```

---

### Solution 2: Use Static IP for Your Computer

**Set a static IP so it doesn't change:**

1. Open **Network & Internet Settings**
2. Click your WiFi connection
3. Click **Properties**
4. Under **IP settings**, click **Edit**
5. Select **Manual**
6. Enable **IPv4**
7. Set:
   - **IP address**: `10.97.116.57` (or whatever you want)
   - **Subnet mask**: `255.255.255.0`
   - **Gateway**: (your router's IP, usually `10.97.116.1`)
   - **DNS**: `8.8.8.8` or your router's IP
8. Click **Save**

---

### Solution 3: Allow Through Firewall

**Create inbound rule:**

```powershell
# Run PowerShell as Administrator
New-NetFirewallRule -DisplayName "XAMPP Apache" -Direction Inbound -Action Allow -Protocol TCP -LocalPort 80
```

Or manually:
1. Windows Defender Firewall → Advanced Settings
2. **Inbound Rules** → **New Rule**
3. **Port** → **TCP** → **Specific local ports**: `80`
4. **Allow the connection**
5. Check all profiles (Domain, Private, Public)
6. Name: "XAMPP Apache"

---

### Solution 4: Test with Known IP

**Quick test - use computer's hostname:**

```cpp
#define WEBSITE_HOST "YOUR-PC-NAME.local"  // Replace with your PC name
```

Or just use `localhost` if Pico can resolve it (usually won't work).

---

### Solution 5: Use Router IP Forwarding (Advanced)

If you have access to router settings, you can:
1. Reserve an IP for your computer (DHCP reservation)
2. Make it static in router
3. Never changes even after reboot

---

## 🧪 Testing Commands

### From Serial Monitor:
```
status          - Show current WiFi status and IP
config          - Enter config mode to change WiFi/API settings
reboot          - Restart device
```

### From PowerShell (on your computer):
```powershell
# Check your IP
ipconfig

# Test if port 80 is open
Test-NetConnection -ComputerName 10.97.116.57 -Port 80

# Test XAMPP
curl http://10.97.116.57/Tank/tank/api/tank_update.php

# Check firewall rules
Get-NetFirewallRule | Where-Object {$_.DisplayName -like "*Apache*"}
```

---

## 📊 Common Scenarios

### Scenario 1: IP Changed
```
Problem: DHCP gave you new IP 10.97.115.200
Solution: Update config.h OR set static IP
```

### Scenario 2: Firewall Blocking
```
Problem: Firewall blocks port 80 from network
Solution: Allow Apache through firewall
```

### Scenario 3: Different Networks
```
Problem: Pico on WiFi, computer on Ethernet
Solution: Connect both to same WiFi network
```

### Scenario 4: XAMPP Not Running
```
Problem: Apache service stopped
Solution: Start Apache in XAMPP Control Panel
```

### Scenario 5: Wrong API Path
```
Problem: File not found (404)
Solution: Verify file exists at C:\xampp\htdocs\Tank\tank\api\tank_update.php
```

---

## 🎯 Quick Fix (Recommended)

### **Option A: Use Portal to Update Settings**

1. Type `config` in serial monitor
2. Device enters AP mode
3. Connect to `TankMonitor-XXXX` (password: `12345678`)
4. Open portal at `http://192.168.4.1`
5. Update API host to your current IP
6. Save and reboot

### **Option B: Update Code and Reflash**

1. Check your current IP:
   ```powershell
   ipconfig | findstr IPv4
   ```

2. Update `config.h`:
   ```cpp
   #define WEBSITE_HOST "YOUR_IP_HERE"  // e.g., "10.97.115.200"
   ```

3. Rebuild and flash:
   ```bash
   python -m platformio run -e rx --target upload
   ```

---

## 🔧 Permanent Fix

### Set Static IP for Your Computer:

**Windows Settings:**
1. Settings → Network & Internet
2. WiFi → Your network → Properties
3. IP settings → Edit → Manual
4. Set static IP: `10.97.116.57`
5. Subnet: `255.255.255.0`
6. Gateway: (your router, usually `10.97.116.1`)
7. DNS: `8.8.8.8`
8. Save

**Verify:**
```powershell
ipconfig
# Should show: IPv4 Address. . . : 10.97.116.57
```

Now your IP will never change!

---

## 📝 Checklist

Before asking for help, verify:

- [ ] XAMPP Control Panel shows Apache is **Running** (green)
- [ ] You can access `http://localhost/Tank/tank/api/tank_update.php` in browser
- [ ] Your computer's IP matches what's in `config.h` (`10.97.116.57`)
- [ ] Windows Firewall allows Apache (port 80)
- [ ] Pico W is connected to same WiFi network as computer
- [ ] Pico W's IP is in same subnet as computer
- [ ] File exists: `C:\xampp\htdocs\Tank\tank\api\tank_update.php`

---

## ✅ Expected Serial Output (When Working)

```
📡 LORA PACKET RECEIVED!
💧 Water level: 118 cm
📈 Tank fill: 59%
🌐 Sending data to website...
Host: 10.97.116.57
Port: 80
Endpoint: /Tank/tank/api/tank_update.php
🔍 Testing main API endpoint...
✅ API endpoint is accessible
🚀 API is accessible, sending data...
Attempt 1 of 3
🔌 Connecting to 10.97.116.57:80...
✅ Connected to server successfully
📤 Sending HTTP request...
✅ Data sent successfully to website!
🔄 Listening for next packet...
```

---

## 🎉 Summary

**Root Cause**: Network connectivity issue between Pico W and computer

**Most Likely Issues:**
1. Computer IP address changed
2. Windows Firewall blocking connections
3. XAMPP Apache not running
4. Devices on different networks

**Best Fix**: 
1. Set static IP for your computer
2. Allow Apache through firewall
3. Update Pico W settings via portal if needed

---

**Need Help?** Type `status` in Serial Monitor to see current network info!
