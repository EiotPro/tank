# Tank Monitoring System - Web Components

A complete web-based tank monitoring solution that receives data from LoRa-enabled Raspberry Pi Pico W devices and displays real-time tank level information.

## 🌟 Features

- **Real-time Monitoring**: Live tank level updates with auto-refresh
- **Interactive Dashboard**: Responsive web interface with charts and visualizations
- **Historical Data**: 24-hour trend analysis with hourly averages
- **Mobile Friendly**: Optimized for mobile and desktop devices
- **Secure API**: RESTful API with authentication and rate limiting
- **Database Integration**: MySQL backend with optimized queries
- **Visual Tank Display**: Animated tank visualization with water level
- **Status Indicators**: Color-coded status with alerts
- **Multiple Tank Support**: Scalable for multiple tanks (configurable)

## 📁 Project Structure

```
tank-web/
├── api/                    # PHP API endpoints
│   ├── config.php         # Database and API configuration
│   ├── tank_update.php    # Receive tank data from devices
│   └── get_tank_data.php  # Provide data for dashboard
├── dashboard/             # Web dashboard
│   └── index.php          # Main dashboard interface
├── database/              # Database schema and migrations
│   └── tank_data.sql      # Complete database setup
├── DEPLOYMENT.md          # Deployment and setup guide
├── SECURITY.md            # Security configuration guide
└── README.md              # This file
```

## 🚀 Quick Start

### 1. Database Setup

```bash
# Import the database schema
mysql -h localhost -u dwljzndr_a -p dwljzndr/shahin < tank-web/database/tank_data.sql
```

### 2. Configuration

Edit `tank-web/api/config.php`:
```php
// Update with your database credentials
define('DB_HOST', 'localhost');
define('DB_USER', 'dwljzndr_a');
define('DB_PASS', 'Amir@123#khan');
define('DB_NAME', 'dwljzndr/shahin');

// Generate a secure API key
define('API_KEY', 'your_secure_api_key_here');
```

### 3. Web Server Setup

1. Upload files to your web server
2. Ensure PHP 7.4+ and PDO MySQL extension are installed
3. Access the dashboard: `http://iotlogic.in/tank/dashboard/`

## 🔌 API Endpoints

### POST /tank/api/tank_update.php

Receives tank data from LoRa devices.

**Request Body:**
```json
{
  "tank_id": 1,
  "level": 150,
  "percentage": 75,
  "api_key": "your_api_key"
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
    "timestamp": "2024-01-01 12:00:00"
  }
}
```

### GET /tank/api/get_tank_data.php

Provides data for the dashboard.

**Response:**
```json
{
  "status": "OK",
  "tank_id": 1,
  "latest": { /* latest reading */ },
  "recent": [ /* recent readings */ ],
  "hourly": [ /* hourly averages */ ],
  "timestamp": "2024-01-01 12:00:00"
}
```

## 📊 Dashboard Features

### Real-time Updates
- Auto-refresh every 30 seconds
- Live tank level visualization
- Real-time status indicators

### Charts and Graphs
- **Live Chart**: Tank level over the last 24 hours
- **Hourly Averages**: Bar chart of average levels per hour
- **Interactive**: Hover for detailed information

### Tank Visualization
- Animated tank with water level
- Color-coded status indicators
- Responsive design for all devices

### Status Monitoring
- **Current Level**: Real-time water level in cm
- **Fill Percentage**: Percentage of tank capacity
- **Status**: Full/High/Medium/Low/Empty with color coding
- **Last Update**: Time since last reading

## 🔒 Security Features

- **API Key Authentication**: Secure API access
- **Rate Limiting**: Prevents API abuse (60 requests/minute)
- **Input Validation**: Comprehensive data validation
- **SQL Injection Protection**: PDO prepared statements
- **XSS Protection**: Input sanitization
- **HTTPS Ready**: Configured for secure connections

## 🗄 Database Schema

### Main Tables

#### `tank_data`
- Stores all sensor readings
- Optimized with indexes for performance
- Automatic timestamping

#### `tank_config`
- Tank configuration settings
- Maximum/minimum depth settings
- Tank metadata

#### `api_keys`
- API key management
- Access control and tracking

#### `alert_settings`
- Configurable alert thresholds
- Multiple alert types supported

### Views

#### `latest_tank_readings`
- Latest reading for each tank
- Includes calculated status and percentage

#### `hourly_averages`
- Hourly aggregated data
- Used for trend analysis

## 📱 Responsive Design

- **Mobile First**: Optimized for mobile devices
- **Tablet Support**: Adaptive layout for tablets
- **Desktop Enhanced**: Full features on desktop
- **Touch Friendly**: Large touch targets
- **Fast Loading**: Optimized assets and queries

## 🔧 Configuration Options

### Tank Settings
```sql
-- Configure tank parameters
UPDATE tank_config SET
  max_depth = 200,
  min_depth = 20,
  tank_name = 'Main Water Tank'
WHERE tank_id = 1;
```

### Alert Configuration
```sql
-- Set up alerts
INSERT INTO alert_settings (tank_id, alert_type, threshold, is_enabled)
VALUES
(1, 'low_level', 20, 1),
(1, 'high_level', 180, 1);
```

### API Rate Limiting
```php
// Adjust in config.php
define('MAX_REQUESTS_PER_MINUTE', 60);
```

## 🧪 Testing

### API Testing
```bash
# Test data submission
curl -X POST http://iotlogic.in/tank/api/tank_update.php \
  -H "Content-Type: application/json" \
  -d '{"tank_id":1,"level":150,"api_key":"your_key"}'

# Test data retrieval
curl http://iotlogic.in/tank/api/get_tank_data.php
```

### Dashboard Testing
1. Open in multiple browsers
2. Test responsive design
3. Verify auto-refresh functionality
4. Check chart interactions

## 🚨 Troubleshooting

### Common Issues

#### Dashboard Not Loading
- Check PHP/database configuration
- Verify file permissions
- Check web server error logs

#### Charts Not Displaying
- Ensure Chart.js is loading
- Check browser console for errors
- Verify API endpoints are accessible

#### Data Not Updating
- Check API key configuration
- Verify database connection
- Check firewall settings

### Debug Mode

Enable debug mode in `config.php`:
```php
ini_set('display_errors', 1);
error_reporting(E_ALL);
```

## 📈 Performance Optimization

### Database Optimization
- Regular index maintenance
- Query optimization
- Archive old data

### Caching
- Implement Redis for session caching
- Cache frequently accessed data
- Optimize static assets

### Monitoring
- Database query performance
- API response times
- System resource usage

## 🔄 Updates and Maintenance

### Regular Tasks
- Monitor system logs
- Check database size
- Update security configurations
- Test backup procedures

### Version History
- **v1.0.0**: Initial release
- **v1.1.0**: Enhanced security features
- **v1.2.0**: Performance optimizations

## 📞 Support

For technical support:
1. Check the troubleshooting section
2. Review system logs
3. Verify configuration settings
4. Test with minimal setup

## 📄 License

This project is part of the LoRa Tank Monitoring System.

---

**Version**: 1.0.0
**Last Updated**: <?php echo date('Y-m-d H:i:s'); ?>
**Compatibility**: PHP 7.4+, MySQL 5.7+, Raspberry Pi Pico W