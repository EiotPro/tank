# WARP.md

This file provides guidance to WARP (warp.dev) when working with code in this repository.

## System Architecture

This is an IoT water tank monitoring system with three main components:
- **Hardware Layer**: ESP32/Raspberry Pi Pico W devices with LoRa communication
- **Backend Layer**: PHP REST API with MySQL database
- **Frontend Layer**: Real-time web dashboard with Chart.js visualization

### High-Level Data Flow
```
Transmitter (TX Node) → LoRa Wireless → Receiver (RX Node) → WiFi/HTTP → PHP API → MySQL → Web Dashboard
```

The system uses **optimized LoRa transmission** - only raw sensor values (0-1023) are sent wirelessly, with all calculations done on the receiver to minimize bandwidth and power consumption.

## Key Configuration Files

- **`include/config.h`**: Master hardware configuration for both TX/RX nodes
  - LoRa parameters (frequency: 865MHz, spreading factor, power, etc.)
  - Network settings (WiFi credentials, API endpoints)
  - Tank specifications (max depth: 200cm)
  - Transmission intervals (currently 5 seconds for testing)

- **`tank/api/config.php`**: Web application configuration
  - Database credentials (MySQL connection)
  - API authentication key (must match hardware config)
  - Rate limiting settings
  - Timezone configuration

## Common Development Commands

### Hardware Development (PlatformIO)
```bash
# Build and upload TX firmware (Transmitter)
platformio run -e tx --upload

# Build and upload RX firmware (Receiver with WiFi)
platformio run -e rx --upload

# Monitor serial output for debugging
platformio device monitor -e tx  # or -e rx

# Clean build cache
platformio run -e tx --target clean

# Install/update dependencies
platformio pkg install
```

### Database Operations
```bash
# Import database schema
mysql -h localhost -u root -p dwljzndr_tank < tank/database/dwljzndr_tank.sql

# Connect to database for queries
mysql -h localhost -u root -p dwljzndr_tank

# Backup data
mysqldump -u root -p dwljzndr_tank > backup_$(date +%Y%m%d).sql
```

### Web Development
```bash
# Test API endpoint
curl -X POST http://localhost/tank/api/tank_update.php \
  -H "Content-Type: application/json" \
  -d '{"tank_id":1,"level":150,"percentage":75,"api_key":"iotlogic"}'

# Check API status
curl http://localhost/tank/api/check_endpoint.php

# View live dashboard
# Navigate to: http://localhost/tank/dashboard/
```

## Database Schema Overview

### Core Tables
- **`tank_data`**: Raw sensor readings (id, tank_id, level, percentage, timestamp)
- **`tank_config`**: Tank configuration (tank_id, tank_name, max_depth, min_depth, location)
- **`alert_settings`**: Alert thresholds (tank_id, alert_type, threshold, is_enabled)
- **`system_logs`**: Application logs (level, message, source, created_at)

### Views for Performance
- **`latest_tank_readings`**: Latest reading per tank with calculated status
- **`hourly_averages`**: Aggregated hourly data for charts (last 24 hours)

## API Architecture

### Authentication
All API requests require the key `"iotlogic"` (configurable in both hardware and PHP configs).

### Key Endpoints
- **POST `/tank/api/tank_update.php`**: Receive sensor data from hardware
- **GET `/tank/api/get_tank_data.php?tank_id=1`**: Fetch latest and historical data
- **GET `/tank/api/check_endpoint.php`**: System health check

### Data Validation
- Tank levels: 0-200cm (configurable max depth)
- Percentages: 0-100%
- Rate limiting: 60 requests/minute default
- JSON payload validation for all POST requests

## Hardware Communication Protocol

### TX Node Operation
1. Reads analog sensor (A0 pin) every 5 seconds
2. Converts raw value (0-1023) to 4-digit HEX
3. Transmits via LoRa: `AT+PSEND=HEX_VALUE`
4. Minimal power consumption design

### RX Node Operation  
1. Listens continuously: `AT+PRECV=65535`
2. Receives HEX payload from LoRa
3. Converts HEX to sensor value, calculates water level and percentage
4. Sends HTTP POST to web API with calculated values
5. Handles WiFi reconnection and API retry logic

### LoRa Configuration
- **Frequency**: 865MHz (ISM band)
- **Spreading Factor**: 7 (balance of range vs data rate)
- **Bandwidth**: 125kHz
- **TX Power**: 14dBm
- **Range**: ~8-15km depending on environment

## Web Dashboard Features

### Real-time Components
- **3D Animated Tank Visualization**: CSS3 animations with water effects
- **Live Charts**: Chart.js with 1H/6H/24H time periods
- **Status Indicators**: Color-coded status badges with animations
- **Auto-refresh**: 30-second intervals (configurable)

### Alert System
- **Low Level**: Configurable threshold (default 20cm)
- **High Level**: Configurable threshold (default 180cm)  
- **Offline Detection**: No data for >5 minutes
- **Browser Notifications**: Desktop alerts for critical events

## Development Environment Setup

### Required Software
- **PlatformIO**: For embedded development
- **XAMPP/WAMP**: Local web server (Apache + MySQL + PHP)
- **MySQL 5.7+**: Database engine
- **PHP 7.4+**: Web application runtime

### File Structure Understanding
- **`src/tx/`**: Transmitter firmware code
- **`src/rx/`**: Receiver firmware code  
- **`tank/api/`**: REST API backend
- **`tank/dashboard/`**: Web frontend
- **`tank/database/`**: SQL schema files

## Troubleshooting Common Issues

### Hardware Issues
- **No LoRa communication**: Check antenna connections and frequency settings
- **WiFi connection failure**: Verify SSID/password in `config.h`
- **Sensor readings erratic**: Check analog sensor wiring to A0 pin
- **API POST failures**: Verify API_KEY matches between hardware and PHP

### Software Issues
- **Dashboard not loading**: Check Apache/MySQL services in XAMPP
- **Charts not updating**: Verify API endpoints return valid JSON
- **Database errors**: Check credentials in `tank/api/config.php`
- **CORS errors**: Ensure proper headers in API responses

### Performance Optimization
- **Database indexing**: Already optimized with composite indexes on (tank_id, timestamp)
- **Data retention**: Consider archiving data older than 30 days
- **Chart performance**: Limit data points for better rendering
- **LoRa optimization**: Current HEX transmission is already optimal

## Security Considerations

### API Security
- Change default API key from "iotlogic" in production
- Implement rate limiting (currently 60 req/min)
- Use HTTPS in production deployment
- Validate and sanitize all input data

### Database Security  
- Use non-root database user with limited privileges
- Regular backups with proper retention policies
- Monitor for SQL injection attempts in logs

## Deployment Notes

### Hardware Deployment
- Mount TX node with clear line-of-sight to RX node
- Ensure stable power supply for RX node
- Test LoRa range and signal strength in actual environment
- Configure appropriate transmission intervals for battery life

### Web Deployment
- Update database credentials for production
- Enable HTTPS/SSL certificates
- Configure proper PHP error handling
- Set up monitoring and alerting for system health

## Development Tips

- **Serial Debugging**: Always monitor serial output during development
- **API Testing**: Use curl or Postman to test endpoints independently
- **Database Queries**: Test complex queries directly in MySQL before implementing
- **Hardware Testing**: Test LoRa communication before deploying to final locations
- **Version Control**: Use git branches for experimental features

## Performance Characteristics

- **LoRa Range**: 8-15km (depending on terrain and SF)
- **Battery Life**: ~6-12 months with optimized transmission (TX node)  
- **Database Growth**: ~1MB per month per tank
- **API Response Time**: <200ms for data queries
- **Dashboard Load Time**: <3 seconds on modern browsers