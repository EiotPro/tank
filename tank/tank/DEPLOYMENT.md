# Tank Monitoring System - Deployment Guide

## 📋 Overview

This guide provides step-by-step instructions for deploying the complete tank monitoring system including hardware setup, firmware flashing, database configuration, and web dashboard deployment.

## 🛠 Hardware Requirements

### Transmitter (TX) Node
- Raspberry Pi Pico (standard)
- RAK3172 LoRa module
- Ultrasonic water level sensor (analog output)
- Power supply (5V USB)

### Receiver (RX) Node
- Raspberry Pi Pico W (WiFi enabled)
- RAK3172 LoRa module
- Power supply (5V USB)
- WiFi network access

### Pin Connections

#### RAK3172 LoRa Module (Both TX and RX)
- VCC → 3.3V
- GND → GND
- TX → GPIO 4 (Serial1 RX)
- RX → GPIO 5 (Serial1 TX)
- RST → GPIO 2

#### Ultrasonic Sensor (TX Node Only)
- VCC → 5V
- GND → GND
- SIG → A0 (Analog input)

## 🚀 Step 1: Hardware Setup

### 1.1 TX Node Setup
1. Connect RAK3172 LoRa module to Pico
2. Connect ultrasonic sensor to A0 pin
3. Power on the TX node
4. Verify serial output shows sensor readings

### 1.2 RX Node Setup
1. Connect RAK3172 LoRa module to Pico W
2. Ensure Pico W has WiFi connectivity
3. Power on the RX node
4. Verify serial output shows "RX Node initialization complete"

## 💾 Step 2: Firmware Deployment

### 2.1 Prerequisites
- Install PlatformIO IDE or VSCode with PlatformIO extension
- Connect both Pico boards via USB

### 2.2 Upload TX Firmware
```bash
# Navigate to tank directory
cd tank

# Upload TX firmware
pio run -e tx --upload

# Monitor TX serial output
pio device monitor -e tx
```

### 2.3 Upload RX Firmware
```bash
# Upload RX firmware (with WiFi support)
pio run -e rx --upload

# Monitor RX serial output
pio device monitor -e rx
```

### 2.4 Verify Communication
1. TX should show water level readings every 10 seconds
2. RX should show received LoRa packets
3. Verify HEX payload encoding/decoding works correctly

## 🗄 Step 3: Database Setup

### 3.1 Create Database
1. Log into your MySQL server
2. Create the database:
```sql
CREATE DATABASE `dwljzndr/shahin`;
```

### 3.2 Run Database Schema
1. Execute the SQL script:
```bash
mysql -h localhost -u dwljzndr_a -p dwljzndr/shahin < tank-web/database/tank_data.sql
```

2. Verify tables were created:
```sql
SHOW TABLES;
DESCRIBE tank_data;
DESCRIBE tank_config;
```

### 3.3 Update API Key
1. Edit `tank-web/api/config.php`
2. Change `API_KEY` to a secure random string:
```php
define('API_KEY', 'your_secure_random_key_here');
```

3. Update the database with the new key:
```sql
UPDATE api_keys SET api_key = 'your_secure_random_key_here' WHERE id = 1;
```

## 🌐 Step 4: Web Dashboard Deployment

### 4.1 File Structure
Ensure the following structure exists:
```
tank-web/
├── api/
│   ├── config.php
│   ├── tank_update.php
│   └── get_tank_data.php
├── dashboard/
│   └── index.php
└── database/
    └── tank_data.sql
```

### 4.2 Web Server Configuration
1. Upload all files to your web server
2. Ensure PHP 7.4+ is installed
3. Enable PDO MySQL extension
4. Set proper file permissions:
```bash
chmod 755 tank-web/
chmod 644 tank-web/api/*.php
chmod 644 tank-web/dashboard/*.php
chmod 755 tank-web/api/tank_update.php
```

### 4.3 Test API Endpoints
1. Test data reception:
```bash
curl -X POST http://iotlogic.in/tank/api/tank_update.php \
  -H "Content-Type: application/json" \
  -d '{"tank_id":1,"level":150,"api_key":"your_api_key"}'
```

2. Test data retrieval:
```bash
curl http://iotlogic.in/tank/api/get_tank_data.php
```

## ⚙️ Step 5: Configuration

### 5.1 RX Node Configuration
Edit `tank/include/config.h`:
```cpp
// WiFi Configuration
#define WIFI_SSID "YourWiFiNetwork"
#define WIFI_PASSWORD "YourWiFiPassword"

// Website Configuration
#define WEBSITE_HOST "iotlogic.in"
#define WEBSITE_PORT 80
#define API_ENDPOINT "/tank/api/tank_update.php"
#define API_KEY "your_secure_api_key"
```

### 5.2 Tank Configuration
1. Access the dashboard: `http://iotlogic.in/tank/dashboard/`
2. Verify tank configuration in database:
```sql
SELECT * FROM tank_config WHERE tank_id = 1;
```

### 5.3 LoRa Configuration
Verify LoRa parameters match between TX and RX:
```cpp
#define LORA_FREQUENCY 865000000  // 865 MHz
#define LORA_SPREADING_FACTOR 7
#define LORA_BANDWIDTH 1          // 125 kHz
#define LORA_CODING_RATE 1        // 4/5
```

## 🧪 Step 6: Testing

### 6.1 Hardware Testing
1. **TX Node Test**:
   - Power on TX node
   - Verify serial output shows sensor readings
   - Check LoRa transmission status

2. **RX Node Test**:
   - Power on RX node
   - Verify WiFi connection
   - Check LoRa packet reception
   - Verify HTTP data transmission

### 6.2 Software Testing
1. **API Testing**:
   - Test POST endpoint with valid data
   - Test POST endpoint with invalid data
   - Verify rate limiting works

2. **Dashboard Testing**:
   - Access dashboard in browser
   - Verify charts display correctly
   - Test auto-refresh functionality

### 6.3 End-to-End Testing
1. Fill tank partially and monitor readings
2. Verify data appears on dashboard within 30 seconds
3. Test low water level alerts
4. Verify historical data display

## 🔒 Step 7: Security Hardening

### 7.1 API Security
1. Use HTTPS for all communications
2. Implement strong API key
3. Enable rate limiting
4. Add IP whitelisting if needed

### 7.2 Database Security
1. Use strong database passwords
2. Limit database user privileges
3. Enable MySQL query logging
4. Regular backup schedule

### 7.3 Web Security
1. Keep PHP updated
2. Use HTTPS for dashboard
3. Implement proper session management
4. Add input validation

## 📊 Step 8: Monitoring and Maintenance

### 8.1 System Monitoring
1. Monitor RX node serial output
2. Check database growth
3. Monitor API response times
4. Set up alerts for system failures

### 8.2 Data Management
1. Implement data retention policy
2. Regular database optimization
3. Backup critical data
4. Archive old sensor data

### 8.3 Performance Optimization
1. Optimize database queries
2. Implement caching if needed
3. Monitor system resources
4. Scale hardware if necessary

## 🚨 Troubleshooting

### Common Issues

#### TX Node Issues
- **No sensor readings**: Check A0 connection and sensor power
- **Transmission failures**: Verify LoRa module connections
- **Serial garbled**: Check baud rate settings

#### RX Node Issues
- **WiFi connection fails**: Verify SSID/password in config.h
- **No LoRa packets**: Check antenna and LoRa parameters
- **HTTP failures**: Verify website URL and API key

#### Web Issues
- **Dashboard not loading**: Check PHP/database configuration
- **Charts not updating**: Verify API endpoints are accessible
- **Database errors**: Check connection credentials

### Debug Commands
```bash
# Check WiFi status on RX
Serial.println(WiFi.status());

# Test HTTP connection
http.begin("http://iotlogic.in/tank/api/tank_update.php");

# Check database connection
<?php phpinfo(); ?>
```

## 📞 Support

For issues and questions:
1. Check the troubleshooting section
2. Review serial output for error messages
3. Verify hardware connections
4. Test with minimal configurations
5. Check system logs in database

## 🔄 Updates and Maintenance

### Firmware Updates
1. Modify source code as needed
2. Test changes on development hardware
3. Deploy to production hardware
4. Monitor for issues after updates

### Web Updates
1. Backup database before updates
2. Test changes on staging environment
3. Deploy to production
4. Monitor system performance

---

**Deployment Status**: ✅ Ready for deployment

**Last Updated**: <?php echo date('Y-m-d H:i:s'); ?>

**Version**: 1.0.0