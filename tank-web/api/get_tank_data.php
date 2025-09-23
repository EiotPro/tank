<?php
require_once 'config.php';

// Set headers for JSON response
header('Content-Type: application/json');
header('Access-Control-Allow-Origin: *');

// Database connection
try {
    $pdo = new PDO(
        "mysql:host=" . DB_HOST . ";dbname=" . DB_NAME,
        DB_USER,
        DB_PASS,
        array(PDO::MYSQL_ATTR_INIT_COMMAND => "SET NAMES utf8")
    );
    $pdo->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
} catch(PDOException $e) {
    http_response_code(500);
    echo json_encode(['error' => 'Database connection failed']);
    exit;
}

// Get tank_id from query parameter (default to 1)
$tank_id = isset($_GET['tank_id']) ? (int)$_GET['tank_id'] : 1;

// Get latest tank data
$stmt = $pdo->prepare("SELECT * FROM latest_tank_readings WHERE tank_id = ?");
$stmt->execute([$tank_id]);
$latest_data = $stmt->fetch(PDO::FETCH_ASSOC);

// Get last 50 readings for live updates
$stmt = $pdo->prepare("
    SELECT level, percentage, timestamp
    FROM tank_data
    WHERE tank_id = ?
    ORDER BY timestamp DESC
    LIMIT 50
");
$stmt->execute([$tank_id]);
$recent_data = $stmt->fetchAll(PDO::FETCH_ASSOC);

// Reverse array to get chronological order
$recent_data = array_reverse($recent_data);

// Get hourly averages for the last 24 hours
$stmt = $pdo->prepare("
    SELECT * FROM hourly_averages
    WHERE tank_id = ?
    ORDER BY hour_start DESC
    LIMIT 24
");
$stmt->execute([$tank_id]);
$hourly_data = $stmt->fetchAll(PDO::FETCH_ASSOC);

// Reverse to get chronological order
$hourly_data = array_reverse($hourly_data);

$response = [
    'status' => 'OK',
    'tank_id' => $tank_id,
    'latest' => $latest_data ?: null,
    'recent' => $recent_data,
    'hourly' => $hourly_data,
    'timestamp' => date('Y-m-d H:i:s')
];

echo json_encode($response);
?>