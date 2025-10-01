<?php
// Database Configuration
define('DB_HOST', 'localhost');
define('DB_USER', 'root');
define('DB_PASS', '');
define('DB_NAME', 'dwljzndr_tank'); // Change as needed

// API Configuration
define('API_KEY', 'iotlogic'); // Matches C++ configuration
define('MAX_REQUESTS_PER_MINUTE', 60); // Rate limiting

// Error reporting (disable in production)
ini_set('display_errors', 0);
error_reporting(0);

// Timezone
date_default_timezone_set('Asia/Kolkata');
?>