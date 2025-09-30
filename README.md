# 🚰 Tank Monitoring System

A complete IoT solution for wireless water tank level monitoring using LoRa technology, featuring real-time data visualization, alerting systems, and web-based dashboard.

## 📋 Table of Contents

- [System Overview](#-system-overview)
- [Features](#-features)
- [Project Structure](#-project-structure)
- [Hardware Requirements](#-hardware-requirements)
- [Software Requirements](#-software-requirements)
- [Installation & Setup](#-installation--setup)
- [Configuration](#-configuration)
- [Usage Guide](#-usage-guide)
- [API Documentation](#-api-documentation)
- [Database Schema](#-database-schema)
- [Troubleshooting](#-troubleshooting)
- [Deployment](#-deployment)
- [Security](#-security)
- [Maintenance](#-maintenance)
- [Development](#-development)

## 🌟 System Overview

This system consists of three main components:

1. **Transmitter Node**: Battery-powered device with water level sensor and LoRa module
2. **Receiver Node**: Raspberry Pi Pico W with LoRa module for data reception and processing
3. **Web Dashboard**: PHP/MySQL web application with real-time monitoring and visualization

### 🔄 Data Flow Architecture

```
Water Sensor → Transmitter → LoRa Wireless → Receiver → WiFi → Web Server → Dashboard
     ↓           ↓              ↓           ↓         ↓        ↓         ↓
Raw Analog → Raw Digital → Optimized HEX → Calculations → HTTP → Database → Charts/Alerts
```

## ✨ Features

### 🎯 Core Features

#### 📡 **LoRa Communication**
- **Long Range**: Up to 15km line-of-sight communication
- **Low Power**: Optimized for battery-powered transmitters
- **Reliable**: Robust P2P communication protocol
- **Configurable**: Adjustable frequency, power, and data rate

#### 📊 **Real-time Monitoring**
- **Live Data**: Real-time water level updates
- **Historical Charts**: 24-hour data visualization with Chart.js
- **Multiple Timeframes**: 1H, 6H, 24H chart views
- **Status Indicators**: Visual tank level with animations

#### 🚨 **Alert System**
- **Threshold Alerts**: Configurable low/high level warnings
- **Browser Notifications**: Desktop notification support
- **Alert History**: Complete alert tracking and management
- **Status Monitoring**: System health and connectivity alerts

#### 📱 **Responsive Dashboard**
- **Mobile Optimized**: Touch-friendly interface
- **Dark/Light Theme**: Theme switching capability
- **Modern UI**: Glassmorphism design with animations
- **Multi-section**: Overview, Alerts, and Settings tabs

### 🔧 Advanced Features

#### 📈 **Data Analytics**
- **Hourly Averages**: Statistical data processing
- **Usage Patterns**: Water consumption analysis
- **Performance Metrics**: System performance monitoring
- **Data Export**: CSV/PDF report generation

#### 🔒 **Security & Reliability**
- **API Key Authentication**: Secure data transmission
- **Rate Limiting**: Protection against abuse
- **Error Handling**: Comprehensive error management
- **Data Validation**: Input sanitization and validation

## 📁 Project Structure

```
tank-monitoring-system/
├── .gitignore                          # Git ignore rules
├── platformio.ini                      # PlatformIO build configuration
├── assets/                             # Static assets (empty)
├── include/                            # Hardware configuration
│   ├── config.h                        # C++ hardware configuration
│   └── README                          # Hardware documentation
├── logs/                               # System logs (empty)
└── tank/                               # Web application
    ├── DEPLOYMENT.md                   # Deployment guide
    ├── SECURITY.md                     # Security documentation
    ├── index.php                       # Main entry point (redirects to dashboard)
    ├── api/                           # REST API endpoints
    │   ├── .htaccess                  # Apache configuration
    │   ├── check_endpoint.php         # System status check
    │   ├── config.php                 # API configuration
    │   ├── get_tank_data.php          # Data retrieval endpoint
    │   ├── simple_test.php            # Simple testing endpoint
    │   ├── tank_update.php            # Primary data submission endpoint
    │   ├── tank_update_fixed.php      # Fixed data submission endpoint
    │   └── test_connection.php        # Database connection test
    ├── dashboard/                     # Web dashboard interface
    │   └── index.php                  # Main dashboard (109KB)
    └── database/                      # Database schema files
        ├── dwljzndr_tank.sql          # Production database schema
        ├── tank.sql                   # Development database schema
        └── tank_clean.sql             # Clean database schema
```

## 🛠 Hardware Requirements

### Transmitter Node (TX)
- **Microcontroller**: Raspberry Pi Pico (standard)
- **LoRa Module**: RAK3172 (868-915MHz)
- **Sensor**: Analog water level sensor (0-5V output)
- **Power**: Battery or 5V DC power supply
- **Antenna**: 868/915MHz LoRa antenna

### Receiver Node (RX)
- **Microcontroller**: Raspberry Pi Pico W (WiFi enabled)
- **LoRa Module**: RAK3172 (868-915MHz)
- **Connectivity**: WiFi for internet access
- **Power**: USB or 5V DC power supply
- **Antenna**: 868/915MHz LoRa antenna

### Pin Connections

#### RAK3172 LoRa Module (Both TX and RX)
```
RAK3172    →    Pico/Pico W
VCC        →    3.3V
GND        →    GND
TX         →    GPIO 4 (Serial1 RX)
RX         →    GPIO 5 (Serial1 TX)
RST        →    GPIO 2
```

#### Ultrasonic Sensor (TX Node Only)
```
Sensor     →    Pico
VCC        →    5V
GND        →    GND
SIG        →    A0 (Analog input)
```

## 💻 Software Requirements

### Development Environment
- **PlatformIO**: IDE for embedded development
- **VSCode**: Code editor with PlatformIO extension
- **Git**: Version control system

### Web Server
- **Server**: Windows/Linux with XAMPP/WAMP/LAMP
- **PHP**: Version 7.4 or higher
- **MySQL**: Version 5.7 or higher
- **Web Server**: Apache/Nginx with mod_rewrite enabled

### Libraries and Dependencies

#### Hardware Libraries (PlatformIO)
```ini
bblanchon/ArduinoJson @ ^6.21.3    # JSON parsing and generation
sandeepmistry/LoRa @ ^0.8.0        # LoRa communication
```

#### Web Libraries (CDN)
- **Bootstrap 5.3.0**: Responsive UI framework
- **Chart.js**: Data visualization
- **Font Awesome 6.4.0**: Icons
- **Google Fonts (Inter)**: Typography

## 🚀 Installation & Setup

### Step 1: Environment Setup

#### 1.1 Install PlatformIO
```bash
# Install PlatformIO CLI
pip install platformio

# Or install VSCode extension
code --install-extension platformio.platformio-ide
```

#### 1.2 Install Web Server
```bash
# Download and install XAMPP for Windows
# https://www.apachefriends.org/index.html

# Or install LAMP stack on Linux
sudo apt update
sudo apt install apache2 mysql-server php libapache2-mod-php
```

#### 1.3 Clone Repository
```bash
git clone <repository-url>
cd tank-monitoring-system
```

### Step 2: Hardware Setup

#### 2.1 TX Node Setup
1. Connect RAK3172 LoRa module to Pico as shown in pin connections
2. Connect ultrasonic sensor to A0 pin
3. Power on the TX node
4. Verify serial output shows sensor readings

#### 2.2 RX Node Setup
1. Connect RAK3172 LoRa module to Pico W
2. Ensure Pico W has WiFi connectivity
3. Power on the RX node
4. Verify serial output shows "RX Node initialization complete"

### Step 3: Firmware Deployment

#### 3.1 Upload TX Firmware
```bash
# Navigate to project directory
cd tank-monitoring-system

# Upload TX firmware
platformio run -e tx --upload

# Monitor TX serial output
platformio device monitor -e tx
```

#### 3.2 Upload RX Firmware
```bash
# Upload RX firmware (with WiFi support)
platformio run -e rx --upload

# Monitor RX serial output
platformio device monitor -e rx
```

#### 3.3 Verify Communication
1. TX should show water level readings every 5 seconds
2. RX should show received LoRa packets
3. Verify HEX payload encoding/decoding works correctly

### Step 4: Database Setup

#### 4.1 Create Database
```sql
-- Create the database
CREATE DATABASE dwljzndr_tank;

-- Use the database
USE dwljzndr_tank;
```

#### 4.2 Import Schema
```bash
# Import the database schema
mysql -h localhost -u root -p dwljzndr_tank < tank/database/dwljzndr_tank.sql
```

#### 4.3 Verify Tables
```sql
-- Check created tables
SHOW TABLES;

-- Verify tank_data table structure
DESCRIBE tank_data;

-- Verify tank_config table structure
DESCRIBE tank_config;
```

### Step 5: Web Application Setup

#### 5.1 Configure API
Edit `tank/api/config.php`:
```php
<?php
// Database Configuration
define('DB_HOST', 'localhost');
define('DB_USER', 'root');
define('DB_PASS', '');
define('DB_NAME', 'dwljzndr_tank');

// API Configuration
define('API_KEY', 'iotlogic'); // Change to secure key
define('MAX_REQUESTS_PER_MINUTE', 60);

// Error reporting (disable in production)
ini_set('display_errors', 0);
error_reporting(0);

// Timezone
date_default_timezone_set('Asia/Kolkata');
?>
```

#### 5.2 Deploy Web Files
1. Copy `tank/` directory to web server document root
2. Ensure proper permissions:
```bash
chmod -R 755 tank/
chmod 644 tank/api/*.php
chmod 644 tank/dashboard/*.php
```

#### 5.3 Test Installation
1. Access dashboard: `http://localhost/tank/dashboard/`
2. Verify API endpoints are accessible
3. Check database connectivity

## ⚙️ Configuration

### Hardware Configuration (`include/config.h`)

#### LoRa Parameters
```cpp
#define LORA_FREQUENCY 865000000      // 865 MHz
#define LORA_SPREADING_FACTOR 7       // Data rate vs range
#define LORA_BANDWIDTH 1              // 125 kHz
#define LORA_CODING_RATE 1            // 4/5 error correction
#define LORA_PREAMBLE_LENGTH 8        // Packet preamble
#define LORA_TX_POWER 14              // 14 dBm transmission power
```

#### System Parameters
```cpp
#define TRANSMISSION_INTERVAL 5000    // 5 seconds (reduced for testing)
#define TANK_MAX_DEPTH 200            // Maximum tank depth in cm
#define TANK_MIN_DEPTH 20             // Minimum tank depth in cm
```

#### Network Configuration
```cpp
#define WIFI_SSID "YourWiFiNetwork"   // WiFi network name
#define WIFI_PASSWORD "YourPassword"  // WiFi password
#define WEBSITE_HOST "192.168.1.100"  // Web server IP
#define WEBSITE_PORT 80               // Web server port
#define API_ENDPOINT "/tank/api/tank_update.php"
#define API_KEY "iotlogic"            // Must match PHP config
```

### Web Configuration (`tank/api/config.php`)

#### Database Settings
```php
define('DB_HOST', 'localhost');
define('DB_USER', 'root');
define('DB_PASS', '');
define('DB_NAME', 'dwljzndr_tank');
```

#### API Settings
```php
define('API_KEY', 'iotlogic');
define('MAX_REQUESTS_PER_MINUTE', 60);
```

## 📖 Usage Guide

### Starting the System

1. **Power on Transmitter**: Insert battery or connect power
2. **Power on Receiver**: Connect to power (USB or DC)
3. **Start Web Server**: Launch XAMPP and start Apache/MySQL services
4. **Access Dashboard**: Open `http://localhost/tank/dashboard/`

### Dashboard Sections

- **Overview**: Real-time tank status and charts
- **Alerts**: Alert history and threshold management
- **Settings**: System configuration and preferences

### Hardware Operation

#### Transmitter (TX) Node
- Reads analog sensor every 5 seconds
- Encodes data to HEX format for LoRa transmission
- Transmits water level data wirelessly
- LED indicators for transmission status

#### Receiver (RX) Node
- Listens for LoRa packets continuously
- Decodes received HEX data
- Validates and processes sensor readings
- Forwards data to web server via HTTP POST

## 🔌 API Documentation

### Base URL
```
http://localhost/tank/api/
```

### Authentication
All API requests require authentication via API key:
```
X-API-Key: iotlogic
```

### Endpoints

#### 1. Submit Tank Data
**Endpoint:** `POST /tank/api/tank_update.php`

**Request Body:**
```json
{
    "tank_id": 1,
    "level": 150,
    "percentage": 75,
    "api_key": "iotlogic"
}
```

**Response:**
```json
{
    "status": "OK",
    "message": "Data stored successfully",
    "data": {
        "tank_id": 1,
        "level": 150,
        "percentage": 75,
        "timestamp": "2025-01-01 12:00:00"
    }
}
```

#### 2. Get Tank Data
**Endpoint:** `GET /tank/api/get_tank_data.php?tank_id=1`

**Response:**
```json
{
    "status": "OK",
    "tank_id": 1,
    "latest": {
        "tank_id": 1,
        "level": 150,
        "percentage": 75,
        "timestamp": "2025-01-01 12:00:00",
        "status": "High",
        "status_class": "normal"
    },
    "recent": [...],
    "hourly": [...],
    "timestamp": "2025-01-01 12:00:00"
}
```

#### 3. System Status Check
**Endpoint:** `GET /tank/api/check_endpoint.php`

**Response:**
```json
{
    "status": "OK",
    "message": "System is operational",
    "timestamp": "2025-01-01 12:00:00"
}
```

#### 4. Database Connection Test
**Endpoint:** `GET /tank/api/test_connection.php`

**Response:**
```json
{
    "status": "OK",
    "message": "Database connection successful"
}
```

## 🗄 Database Schema

### Core Tables

#### `tank_data`
Stores individual sensor readings:
```sql
CREATE TABLE tank_data (
    id INT PRIMARY KEY AUTO_INCREMENT,
    tank_id INT NOT NULL,
    level INT NOT NULL COMMENT 'Water level in cm',
    percentage INT NOT NULL COMMENT 'Fill percentage (0-100)',
    timestamp DATETIME DEFAULT CURRENT_TIMESTAMP
);
```

#### `tank_config`
Tank configuration settings:
```sql
CREATE TABLE tank_config (
    id INT PRIMARY KEY AUTO_INCREMENT,
    tank_id INT UNIQUE NOT NULL,
    tank_name VARCHAR(100) NOT NULL,
    max_depth INT DEFAULT 200 COMMENT 'Maximum depth in cm',
    min_depth INT DEFAULT 20 COMMENT 'Minimum depth in cm',
    location VARCHAR(255),
    description TEXT,
    is_active TINYINT DEFAULT 1
);
```

#### `alert_settings`
Alert threshold configuration:
```sql
CREATE TABLE alert_settings (
    id INT PRIMARY KEY AUTO_INCREMENT,
    tank_id INT NOT NULL,
    alert_type ENUM('low_level','high_level','offline') NOT NULL,
    threshold INT NOT NULL,
    is_enabled TINYINT DEFAULT 1
);
```

### Views

#### `latest_tank_readings`
Latest readings with tank information:
```sql
CREATE VIEW latest_tank_readings AS
SELECT td.tank_id, tc.tank_name, tc.max_depth, tc.min_depth,
       td.level, td.percentage, td.timestamp,
       CASE
         WHEN td.percentage >= 80 THEN 'Full'
         WHEN td.percentage >= 50 THEN 'High'
         WHEN td.percentage >= 20 THEN 'Medium'
         WHEN td.percentage > 0 THEN 'Low'
         ELSE 'Empty'
       END AS status,
       CASE
         WHEN td.percentage <= 20 THEN 'warning'
         WHEN td.percentage <= 10 THEN 'danger'
         ELSE 'normal'
       END AS status_class
FROM tank_data td
JOIN tank_config tc ON td.tank_id = tc.tank_id
WHERE td.timestamp = (
    SELECT MAX(td2.timestamp)
    FROM tank_data td2
    WHERE td2.tank_id = td.tank_id
);
```

#### `hourly_averages`
Hourly aggregated data:
```sql
CREATE VIEW hourly_averages AS
SELECT tank_id,
       DATE_FORMAT(timestamp, '%Y-%m-%d %H:00:00') AS hour_start,
       AVG(level) AS avg_level,
       AVG(percentage) AS avg_percentage,
       MIN(level) AS min_level,
       MAX(level) AS max_level,
       COUNT(*) AS reading_count
FROM tank_data
WHERE timestamp >= CURRENT_TIMESTAMP - INTERVAL 24 HOUR
GROUP BY tank_id, DATE_FORMAT(timestamp, '%Y-%m-%d %H:00:00')
ORDER BY tank_id, hour_start;
```

## 🐛 Troubleshooting

### Common Issues

#### Hardware Issues

**TX Node Problems:**
- **No sensor readings**: Check A0 connection and sensor power
- **Transmission failures**: Verify LoRa module connections and antenna
- **Serial garbled**: Check baud rate settings (115200)

**RX Node Problems:**
- **WiFi connection fails**: Verify SSID/password in `config.h`
- **No LoRa packets**: Check antenna and LoRa parameters match TX
- **HTTP failures**: Verify website URL and API key

#### Software Issues

**Web Server Problems:**
- **Dashboard not loading**: Check PHP/MySQL service status
- **Charts not updating**: Verify API endpoints accessibility
- **Database errors**: Check connection credentials in `config.php`

**API Issues:**
- **Invalid API key**: Ensure API key matches between hardware and PHP config
- **Rate limiting**: Check `MAX_REQUESTS_PER_MINUTE` setting
- **Database connection**: Verify database exists and user permissions

### Debug Commands

#### Hardware Debugging
```bash
# Monitor TX serial output
platformio device monitor -e tx

# Monitor RX serial output
platformio device monitor -e rx

# Check WiFi status (in RX code)
Serial.println(WiFi.status());
```

#### Web Debugging
```bash
# Test API endpoint
curl -X POST http://localhost/tank/api/tank_update.php \
  -H "Content-Type: application/json" \
  -d '{"tank_id":1,"level":150,"api_key":"iotlogic"}'

# Check PHP error logs
tail -f /xampp/apache/logs/error_log

# Test database connection
mysql -h localhost -u root -p dwljzndr_tank
```

## 🚀 Deployment

### Production Deployment Checklist

#### Pre-deployment
- [ ] Update API key to secure random string
- [ ] Configure production database credentials
- [ ] Set up HTTPS/SSL certificate
- [ ] Configure firewall rules
- [ ] Set up backup strategy

#### Hardware Deployment
- [ ] Mount TX node near tank with clear line-of-sight to RX
- [ ] Ensure stable power supply for both nodes
- [ ] Install antennas in optimal locations
- [ ] Test communication range and reliability

#### Web Deployment
- [ ] Upload files to production web server
- [ ] Configure web server virtual host
- [ ] Set up SSL certificate
- [ ] Configure database on production server
- [ ] Set up monitoring and alerting

### Performance Optimization

#### Database Optimization
```sql
-- Add indexes for better performance
CREATE INDEX idx_tank_timestamp ON tank_data(tank_id, timestamp);
CREATE INDEX idx_timestamp ON tank_data(timestamp);

-- Regular cleanup of old data
DELETE FROM tank_data WHERE timestamp < DATE_SUB(NOW(), INTERVAL 30 DAY);
```

#### Web Optimization
- Enable PHP opcache
- Configure MySQL query cache
- Set up CDN for static assets
- Implement database connection pooling

## 🔒 Security

### API Security
- **Strong API Keys**: Use cryptographically secure random keys
- **Rate Limiting**: Implemented (60 requests/minute default)
- **Input Validation**: All inputs sanitized and validated
- **HTTPS Only**: Enforce HTTPS in production

### Database Security
- **Strong Credentials**: Use complex passwords
- **Limited Permissions**: Grant minimal required permissions
- **Regular Backups**: Implement automated backup strategy
- **Audit Logging**: Track all database access

### Web Security
- **XSS Protection**: Output encoding implemented
- **CSRF Protection**: Consider implementing for forms
- **Session Security**: Secure session management
- **File Permissions**: Restrict file access permissions

## 🔧 Maintenance

### Regular Maintenance Tasks

#### Weekly
- [ ] Check battery levels on TX node
- [ ] Verify WiFi connectivity on RX node
- [ ] Review system logs for errors
- [ ] Test alert notifications

#### Monthly
- [ ] Clean sensor probes if needed
- [ ] Check antenna connections
- [ ] Verify data accuracy with manual measurements
- [ ] Update firmware if security patches available

#### Quarterly
- [ ] Review and optimize database performance
- [ ] Update SSL certificates
- [ ] Test backup and recovery procedures
- [ ] Review and update security measures

### Monitoring

#### System Health Checks
- Monitor RX node serial output for errors
- Check database growth and performance
- Monitor API response times
- Set up alerts for system failures

#### Data Management
- Implement data retention policy (30 days default)
- Regular database optimization
- Backup critical configuration and data
- Archive historical data as needed

## 💻 Development

### Development Environment Setup

#### 1. PlatformIO Setup
```bash
# Install PlatformIO
pip install platformio

# Clone repository
git clone <repository-url>
cd tank-monitoring-system

# Install dependencies
platformio pkg install
```

#### 2. Web Development Setup
```bash
# Install XAMPP or similar
# Place project in htdocs/tank/

# Start services
# Apache + MySQL
```

### Code Structure

#### Hardware Components
```
src/
├── tx/                    # Transmitter firmware
│   └── main.cpp          # TX node logic
└── rx/                   # Receiver firmware
    └── main.cpp          # RX node logic
```

#### Web Components
```
tank/
├── api/                  # REST API
├── dashboard/           # Web interface
└── database/            # SQL schemas
```

### Adding New Features

#### New Sensor Support (Hardware)
```cpp
// In transmitter (tx/main.cpp)
int readNewSensor() {
    // Implement new sensor reading
    return sensorValue;
}

// Send combined data
String payload = String(rawWater) + "," + String(newSensorValue);
```

#### New Dashboard Section (Web)
```php
// Add to dashboard/index.php
<div id="newSection" class="content-section">
    <!-- New section content -->
</div>
```

### Testing

#### Hardware Testing
```bash
# Test individual components
platformio test --filter "test_transmitter"
platformio test --filter "test_receiver"

# Monitor serial output
platformio device monitor
```

#### Web Testing
```bash
# Test API endpoints
curl http://localhost/tank/api/check_endpoint.php

# Load test the dashboard
# Use browser dev tools for performance testing
```

## 📊 Performance Characteristics

### LoRa Performance
| Spreading Factor | Data Rate | Range | Air Time (10 bytes) |
|------------------|-----------|-------|-------------------|
| SF7 | 5469 bps | ~8km | ~20ms |
| SF8 | 3125 bps | ~12km | ~40ms |
| SF9 | 1758 bps | ~15km | ~80ms |
| SF10 | 977 bps | ~18km | ~160ms |

### System Performance
| Metric | Value | Notes |
|--------|-------|-------|
| Transmission Interval | 5 seconds | Configurable |
| Web Refresh Rate | 30 seconds | Configurable |
| Database Storage | ~1MB/month | Per tank |
| Memory Usage (Pico W) | ~50KB | Flash usage |
| Power Consumption (TX) | ~50mA | During transmission |

## 📈 Version History

### v2.0.0 (Current)
- ✅ **Optimization**: Minimal data transmission
- ✅ **Enhanced UI**: Modern glassmorphism design
- ✅ **Alert System**: Comprehensive notification system
- ✅ **Mobile Support**: Responsive design
- ✅ **Chart Integration**: Real-time data visualization

### v1.0.0
- ✅ Basic LoRa communication
- ✅ Simple web dashboard
- ✅ Database integration
- ✅ Real-time monitoring

## 🎯 Quick Start Summary

1. **Setup Hardware**: Connect sensors and LoRa modules
2. **Configure Network**: Update WiFi and server settings in `include/config.h`
3. **Upload Code**: Flash transmitter and receiver firmware via PlatformIO
4. **Start Server**: Launch XAMPP with database
5. **Monitor**: Access dashboard at `http://localhost/tank/dashboard/`

For detailed setup instructions, see [Installation & Setup](#-installation--setup) section above.

---

**Last Updated:** <?php echo date('Y-m-d H:i:s'); ?>  
**Version:** 2.0.0  
**Compatibility:** PlatformIO, Arduino Framework, Raspberry Pi Pico W