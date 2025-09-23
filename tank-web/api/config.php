<?php
// Database Configuration
define('DB_HOST', 'localhost');
define('DB_USER', 'dwljzndr_a');
define('DB_PASS', 'Amir@123#khan');
define('DB_NAME', 'dwljzndr/shahin');

// API Configuration
define('API_KEY', 'YOUR_SECURE_API_KEY_HERE'); // Change this to a secure key
define('MAX_REQUESTS_PER_MINUTE', 60); // Rate limiting

// Error reporting (disable in production)
ini_set('display_errors', 0);
error_reporting(0);

// Timezone
date_default_timezone_set('Asia/Kolkata');
?>