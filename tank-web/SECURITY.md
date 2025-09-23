# Security Configuration Guide

## 🔐 API Security

### 1. API Key Management

#### Generate Secure API Key
```bash
# Generate a random 32-character API key
openssl rand -hex 32
```

#### Update Configuration
1. Edit `tank-web/api/config.php`:
```php
define('API_KEY', 'your_generated_secure_key_here');
```

2. Update database:
```sql
UPDATE api_keys SET api_key = 'your_generated_secure_key_here' WHERE id = 1;
```

3. Update RX firmware configuration in `tank/include/config.h`:
```cpp
#define API_KEY "your_generated_secure_key_here"
```

### 2. Rate Limiting

The system includes built-in rate limiting (60 requests per minute per API key). To modify:

```php
// In config.php
define('MAX_REQUESTS_PER_MINUTE', 60); // Adjust as needed
```

### 3. Input Validation

All input is validated and sanitized:
- JSON format validation
- Required field checks
- Data type validation
- Range validation for numeric values

## 🛡️ Database Security

### 1. Database User Privileges

Create a dedicated database user with minimal privileges:

```sql
CREATE USER 'tank_monitor'@'localhost' IDENTIFIED BY 'secure_password_here';
GRANT SELECT, INSERT ON `dwljzndr/shahin`.* TO 'tank_monitor'@'localhost';
FLUSH PRIVILEGES;
```

### 2. Update Connection Credentials

Update `tank-web/api/config.php`:
```php
define('DB_HOST', 'localhost');
define('DB_USER', 'tank_monitor');
define('DB_PASS', 'secure_password_here');
define('DB_NAME', 'dwljzndr/shahin');
```

### 3. Connection Security

- Use PDO prepared statements (already implemented)
- Enable SSL for database connections in production
- Implement connection pooling for high traffic

## 🌐 Web Security

### 1. HTTPS Configuration

Enable HTTPS for all web communications:

```apache
# Apache configuration
<VirtualHost *:443>
    SSLEngine on
    SSLCertificateFile /path/to/certificate.crt
    SSLCertificateKeyFile /path/to/private.key
    DocumentRoot /path/to/tank-web
</VirtualHost>
```

### 2. Security Headers

Add security headers to PHP files:

```php
// Add to top of PHP files
header('X-Content-Type-Options: nosniff');
header('X-Frame-Options: DENY');
header('X-XSS-Protection: 1; mode=block');
header('Strict-Transport-Security: max-age=31536000');
```

### 3. File Permissions

Set proper file permissions:

```bash
# Secure file permissions
chmod 755 tank-web/
chmod 644 tank-web/api/*.php
chmod 644 tank-web/dashboard/*.php
chmod 600 tank-web/api/config.php
```

## 🔧 Hardware Security

### 1. RX Node Security

#### WiFi Security
- Use WPA3 encryption if available
- Use strong WiFi passwords (12+ characters)
- Consider MAC address filtering

#### Firmware Security
- Keep firmware updated
- Use secure boot if available
- Implement firmware signing

### 2. Network Security

#### Firewall Configuration
```bash
# Allow only necessary ports
sudo ufw allow 80
sudo ufw allow 443
sudo ufw allow from RX_IP_ADDRESS
sudo ufw enable
```

#### Network Segmentation
- Place IoT devices on separate network
- Use VLANs to isolate tank monitoring system
- Implement network access control

## 📊 Monitoring and Logging

### 1. System Logs

The system logs all API requests and errors:

```sql
-- View recent logs
SELECT * FROM system_logs ORDER BY created_at DESC LIMIT 100;
```

### 2. Security Monitoring

Monitor for suspicious activities:
- Multiple failed API requests
- Unusual data patterns
- Unexpected IP addresses

### 3. Alert Configuration

Set up alerts for security events:

```sql
-- Configure alerts in database
INSERT INTO alert_settings (tank_id, alert_type, threshold, is_enabled)
VALUES
(1, 'security_breach', 5, 1),  -- Alert after 5 failed attempts
(1, 'unusual_activity', 100, 1); -- Alert for unusual data patterns
```

## 🔍 Security Testing

### 1. Penetration Testing

Test the system for vulnerabilities:

```bash
# Test SQL injection
curl -X POST http://iotlogic.in/tank/api/tank_update.php \
  -d "tank_id=1'; DROP TABLE tank_data;--"

# Test XSS
curl -X POST http://iotlogic.in/tank/api/tank_update.php \
  -d '{"tank_id":1,"level":"<script>alert(1)</script>"}'
```

### 2. Security Scanning

Use security tools:
- OWASP ZAP for web vulnerability scanning
- SQLMap for database security testing
- Wireshark for network traffic analysis

## 🚨 Incident Response

### 1. Security Incident Procedure

1. **Detection**: Monitor logs for suspicious activity
2. **Assessment**: Determine the scope and impact
3. **Containment**: Isolate affected systems
4. **Recovery**: Restore from backups if necessary
5. **Lessons Learned**: Document and improve security

### 2. Emergency Contacts

- System Administrator: [Your Contact Info]
- Security Team: [Security Contact Info]
- Backup Contact: [Emergency Contact Info]

## 📋 Security Checklist

- [ ] API key is strong and unique
- [ ] Database user has minimal privileges
- [ ] HTTPS is enabled
- [ ] Security headers are implemented
- [ ] File permissions are correct
- [ ] Firewall is configured
- [ ] Logs are monitored
- [ ] Backups are tested
- [ ] Security updates are applied
- [ ] Penetration testing completed

## 🔄 Maintenance

### Regular Security Tasks

1. **Weekly**:
   - Review system logs
   - Check for security updates
   - Monitor API usage patterns

2. **Monthly**:
   - Test backup procedures
   - Review user access
   - Update security configurations

3. **Quarterly**:
   - Conduct security audits
   - Update documentation
   - Review incident response procedures

### Security Updates

Keep all components updated:
- PHP version and extensions
- Web server software
- Database server
- Firmware on hardware devices

---

**Security Status**: 🔒 Configured

**Last Security Review**: <?php echo date('Y-m-d'); ?>

**Next Review Due**: <?php echo date('Y-m-d', strtotime('+30 days')); ?>