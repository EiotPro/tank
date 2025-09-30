<?php
require_once 'config.php';

// Set headers for JSON response
header('Content-Type: application/json');
header('Access-Control-Allow-Origin: *');
header('Access-Control-Allow-Methods: POST');
header('Access-Control-Allow-Headers: Content-Type');

// Only allow POST requests
if ($_SERVER['REQUEST_METHOD'] !== 'POST') {
    http_response_code(405);
    echo json_encode(['error' => 'Method not allowed']);
    exit;
}

// Get JSON input
$input = file_get_contents('php://input');
if (empty($input)) {
    http_response_code(400);
    echo json_encode(['error' => 'No data received']);
    exit;
}

// Decode JSON
$data = json_decode($input, true);
if (json_last_error() !== JSON_ERROR_NONE) {
    http_response_code(400);
    echo json_encode(['error' => 'Invalid JSON format']);
    exit;
}

// Validate required fields
$required_fields = ['tank_id', 'level', 'api_key'];
foreach ($required_fields as $field) {
    if (!isset($data[$field]) || $data[$field] === '') {
        http_response_code(400);
        echo json_encode(['error' => "Missing required field: $field"]);
        exit;
    }
}

// Sanitize input
$tank_id = filter_var($data['tank_id'], FILTER_VALIDATE_INT);
$level = filter_var($data['level'], FILTER_VALIDATE_INT);
$percentage = isset($data['percentage']) ? filter_var($data['percentage'], FILTER_VALIDATE_INT) : null;
$api_key = trim($data['api_key']);

// Validate ranges
if ($tank_id === false || $tank_id < 1) {
    http_response_code(400);
    echo json_encode(['error' => 'Invalid tank_id']);
    exit;
}

if ($level === false || $level < 0 || $level > 500) { // Max 500cm for safety
    http_response_code(400);
    echo json_encode(['error' => 'Invalid level value']);
    exit;
}

if ($percentage !== null && ($percentage === false || $percentage < 0 || $percentage > 100)) {
    http_response_code(400);
    echo json_encode(['error' => 'Invalid percentage value']);
    exit;
}

// Verify API key
if ($api_key !== API_KEY) {
    http_response_code(401);
    echo json_encode(['error' => 'Invalid API key']);
    exit;
}

// Rate limiting (simple implementation)
$rate_limit_file = sys_get_temp_dir() . '/tank_api_rate_limit_' . md5($api_key);
$rate_limit_data = [];

if (file_exists($rate_limit_file)) {
    $rate_limit_data = json_decode(file_get_contents($rate_limit_file), true);
    $current_time = time();

    // Clean old requests (older than 1 minute)
    $rate_limit_data = array_filter($rate_limit_data, function($timestamp) use ($current_time) {
        return ($current_time - $timestamp) < 60;
    });

    if (count($rate_limit_data) >= MAX_REQUESTS_PER_MINUTE) {
        http_response_code(429);
        echo json_encode(['error' => 'Rate limit exceeded']);
        exit;
    }
}

// Add current request to rate limit
$rate_limit_data[] = time();
file_put_contents($rate_limit_file, json_encode($rate_limit_data));

// Database connection
try {
    $pdo = new PDO(
        "mysql:host=" . DB_HOST . ";dbname=" . DB_NAME,
        DB_USER,
        DB_PASS,
        array(PDO::MYSQL_ATTR_INIT_COMMAND => "SET NAMES utf8")
    );
    $pdo->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
    $pdo->setAttribute(PDO::ATTR_EMULATE_PREPARES, false);

} catch(PDOException $e) {
    error_log("Database connection error: " . $e->getMessage());
    http_response_code(500);
    echo json_encode(['error' => 'Database connection failed']);
    exit;
}

// Calculate percentage if not provided
if ($percentage === null) {
    // Assuming max depth is 200cm (can be made configurable)
    $max_depth = 200;
    $percentage = round(($level / $max_depth) * 100);
    $percentage = max(0, min(100, $percentage));
}

// Insert data
try {
    $stmt = $pdo->prepare("
        INSERT INTO tank_data (tank_id, level, percentage, timestamp)
        VALUES (?, ?, ?, NOW())
    ");

    $result = $stmt->execute([$tank_id, $level, $percentage]);

    if ($result) {
        echo json_encode([
            'status' => 'OK',
            'message' => 'Data stored successfully',
            'data' => [
                'tank_id' => $tank_id,
                'level' => $level,
                'percentage' => $percentage,
                'timestamp' => date('Y-m-d H:i:s')
            ]
        ]);
    } else {
        throw new PDOException('Insert failed');
    }

} catch(PDOException $e) {
    error_log("Database insert error: " . $e->getMessage());
    http_response_code(500);
    echo json_encode(['error' => 'Failed to store data']);
    exit;
}
?>