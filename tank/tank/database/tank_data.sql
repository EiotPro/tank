-- Tank Monitoring System Database Schema
-- Execute this script to create the required tables

-- Tank data table (main data storage)
CREATE TABLE IF NOT EXISTS `tank_data` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `tank_id` int(11) NOT NULL,
  `level` int(11) NOT NULL COMMENT 'Water level in cm',
  `percentage` int(11) NOT NULL COMMENT 'Fill percentage (0-100)',
  `timestamp` datetime DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`id`),
  KEY `idx_tank_timestamp` (`tank_id`, `timestamp`),
  KEY `idx_timestamp` (`timestamp`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- Tank configuration table
CREATE TABLE IF NOT EXISTS `tank_config` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `tank_id` int(11) NOT NULL,
  `tank_name` varchar(100) NOT NULL,
  `max_depth` int(11) NOT NULL DEFAULT 200 COMMENT 'Maximum depth in cm',
  `min_depth` int(11) NOT NULL DEFAULT 20 COMMENT 'Minimum depth in cm',
  `location` varchar(255) DEFAULT NULL,
  `description` text,
  `is_active` tinyint(1) NOT NULL DEFAULT 1,
  `created_at` datetime DEFAULT CURRENT_TIMESTAMP,
  `updated_at` datetime DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  PRIMARY KEY (`id`),
  UNIQUE KEY `uk_tank_id` (`tank_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- API keys table for security
CREATE TABLE IF NOT EXISTS `api_keys` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `api_key` varchar(255) NOT NULL,
  `tank_id` int(11) DEFAULT NULL,
  `description` varchar(255) DEFAULT NULL,
  `is_active` tinyint(1) NOT NULL DEFAULT 1,
  `created_at` datetime DEFAULT CURRENT_TIMESTAMP,
  `last_used` datetime DEFAULT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `uk_api_key` (`api_key`),
  KEY `idx_tank_id` (`tank_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- Alert settings table
CREATE TABLE IF NOT EXISTS `alert_settings` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `tank_id` int(11) NOT NULL,
  `alert_type` enum('low_level','high_level','offline') NOT NULL,
  `threshold` int(11) NOT NULL,
  `is_enabled` tinyint(1) NOT NULL DEFAULT 1,
  `created_at` datetime DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`id`),
  UNIQUE KEY `uk_tank_alert_type` (`tank_id`, `alert_type`),
  KEY `idx_tank_id` (`tank_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- System logs table
CREATE TABLE IF NOT EXISTS `system_logs` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `level` enum('INFO','WARNING','ERROR','CRITICAL') NOT NULL,
  `message` text NOT NULL,
  `source` varchar(100) DEFAULT NULL,
  `created_at` datetime DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`id`),
  KEY `idx_level` (`level`),
  KEY `idx_created_at` (`created_at`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- Insert default tank configuration
INSERT INTO `tank_config` (`tank_id`, `tank_name`, `max_depth`, `min_depth`, `location`, `description`)
VALUES
(1, 'Main Water Tank', 200, 20, 'Ground Floor', 'Primary water storage tank with ultrasonic monitoring');

-- Insert default API key (change this in production!)
INSERT INTO `api_keys` (`api_key`, `tank_id`, `description`)
VALUES
('YOUR_SECURE_API_KEY_HERE', 1, 'Default API key for tank monitoring');

-- Insert default alert settings
INSERT INTO `alert_settings` (`tank_id`, `alert_type`, `threshold`, `is_enabled`)
VALUES
(1, 'low_level', 20, 1),   -- Alert when level drops below 20cm
(1, 'high_level', 180, 1),  -- Alert when level exceeds 180cm
(1, 'offline', 300, 1);     -- Alert if no data received for 5 minutes (300 seconds)

-- Create a view for latest tank readings
CREATE OR REPLACE VIEW `latest_tank_readings` AS
SELECT
    td.tank_id,
    tc.tank_name,
    tc.max_depth,
    tc.min_depth,
    td.level,
    td.percentage,
    td.timestamp,
    CASE
        WHEN td.percentage >= 80 THEN 'Full'
        WHEN td.percentage >= 50 THEN 'High'
        WHEN td.percentage >= 20 THEN 'Medium'
        WHEN td.percentage > 0 THEN 'Low'
        ELSE 'Empty'
    END as status,
    CASE
        WHEN td.percentage <= 20 THEN 'warning'
        WHEN td.percentage <= 10 THEN 'danger'
        ELSE 'normal'
    END as status_class
FROM `tank_data` td
JOIN `tank_config` tc ON td.tank_id = tc.tank_id
WHERE td.timestamp = (
    SELECT MAX(timestamp)
    FROM `tank_data` td2
    WHERE td2.tank_id = td.tank_id
);

-- Create a view for hourly averages (last 24 hours)
CREATE OR REPLACE VIEW `hourly_averages` AS
SELECT
    tank_id,
    DATE_FORMAT(timestamp, '%Y-%m-%d %H:00:00') as hour_start,
    AVG(level) as avg_level,
    AVG(percentage) as avg_percentage,
    MIN(level) as min_level,
    MAX(level) as max_level,
    COUNT(*) as reading_count
FROM `tank_data`
WHERE timestamp >= DATE_SUB(NOW(), INTERVAL 24 HOUR)
GROUP BY tank_id, DATE_FORMAT(timestamp, '%Y-%m-%d %H:00:00')
ORDER BY tank_id, hour_start;

-- Create indexes for better performance
CREATE INDEX IF NOT EXISTS `idx_tank_data_timestamp` ON `tank_data` (`timestamp`);
CREATE INDEX IF NOT EXISTS `idx_tank_data_tank_timestamp` ON `tank_data` (`tank_id`, `timestamp` DESC);
CREATE INDEX IF NOT EXISTS `idx_system_logs_created_at` ON `system_logs` (`created_at`);
CREATE INDEX IF NOT EXISTS `idx_api_keys_active` ON `api_keys` (`is_active`, `api_key`);

-- Insert sample data for testing (optional)
INSERT INTO `tank_data` (`tank_id`, `level`, `percentage`, `timestamp`) VALUES
(1, 150, 75, DATE_SUB(NOW(), INTERVAL 10 MINUTE)),
(1, 148, 74, DATE_SUB(NOW(), INTERVAL 8 MINUTE)),
(1, 152, 76, DATE_SUB(NOW(), INTERVAL 6 MINUTE)),
(1, 149, 74, DATE_SUB(NOW(), INTERVAL 4 MINUTE)),
(1, 151, 75, DATE_SUB(NOW(), INTERVAL 2 MINUTE));