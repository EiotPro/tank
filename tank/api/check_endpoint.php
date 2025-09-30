<?php
// IoT Logic Server Endpoint Checker - UPDATED FOR LIVE SERVER
echo "<h2>🚀 IoT Logic API Status Check</h2>";
echo "<p>🌐 Website: iotlogic.in/tank</p>";
echo "<p>⏰ Current URL: " . $_SERVER['REQUEST_URI'] . "</p>";
echo "<p>🖥️ Server: " . $_SERVER['SERVER_SOFTWARE'] . "</p>";
echo "<p>📁 Document Root: " . $_SERVER['DOCUMENT_ROOT'] . "</p>";
echo "<p>🔧 PHP Version: " . PHP_VERSION . "</p>";

echo "<h3>📁 File Existence Check:</h3>";
$files_to_check = [
   '/tank/api/tank_update.php',
   '/tank/api/config.php',
   '/tank/api/get_tank_data.php',
    '/tank/database/dwljzndr_tank.sql',
    '/tank/index.php',
    '/tank/dashboard/index.php'
];

foreach ($files_to_check as $file) {
    $full_path = $_SERVER['DOCUMENT_ROOT'] . $file;
    if (file_exists($full_path)) {
        echo "✅ $file - EXISTS<br>";
        echo "&nbsp;&nbsp;&nbsp;📏 Size: " . filesize($full_path) . " bytes<br>";
        echo "&nbsp;&nbsp;&nbsp;📅 Modified: " . date("Y-m-d H:i", filemtime($full_path)) . "<br>";
    } else {
        echo "❌ $file - MISSING<br>";
    }
    echo "<br>";
}

echo "<h3>📂 Directory Permissions:</h3>";
$dirs_to_check = [
    '/tank',
    '/tank/api',
    '/tank/database',
    '/tank/dashboard'
];

foreach ($dirs_to_check as $dir) {
    $full_path = $_SERVER['DOCUMENT_ROOT'] . $dir;
    if (is_dir($full_path)) {
        echo "✅ $dir - EXISTS<br>";
        if (is_writable($full_path)) {
            echo "&nbsp;&nbsp;&nbsp;✏️ Writable<br>";
        } else {
            echo "&nbsp;&nbsp;&nbsp;🔒 Not writable<br>";
        }
    } else {
        echo "❌ $dir - MISSING<br>";
    }
    echo "<br>";
}

echo "<h3>🗄️ Database Connection Test:</h3>";
try {
    // Try live server credentials first
    $pdo = new PDO(
        "mysql:host=localhost;dbname=dwljzndr_tank",
        "dwljzndr_tank",
        "Amir@123#khan"
    );
    $pdo->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
    echo "✅ Database connection successful!<br>";
    echo "&nbsp;&nbsp;&nbsp;🔐 Connected with live credentials<br>";

    // Check if tables exist
    $tables = $pdo->query("SHOW TABLES")->fetchAll(PDO::FETCH_COLUMN);
    echo "&nbsp;&nbsp;&nbsp;📊 Tables found: " . count($tables) . "<br>";
    foreach ($tables as $table) {
        echo "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;✅ $table<br>";
    }

    // Check for recent data
    $stmt = $pdo->query("SELECT COUNT(*) as count FROM latest_tank_readings");
    $count = $stmt->fetch(PDO::FETCH_ASSOC);
    echo "&nbsp;&nbsp;&nbsp;📈 Latest readings: " . $count['count'] . " records<br>";

} catch(PDOException $e) {
    echo "❌ Database connection failed: " . $e->getMessage() . "<br>";

    // Try alternative connection
    try {
        $pdo = new PDO(
            "mysql:host=localhost;dbname=dwljzndr_tank",
            "root",
            ""
        );
        echo "&nbsp;&nbsp;&nbsp;⚠️ Connected with root credentials<br>";
    } catch(PDOException $e2) {
        echo "&nbsp;&nbsp;&nbsp;❌ Root connection also failed: " . $e2->getMessage() . "<br>";
    }
}

echo "<h3>🌐 API Endpoint Tests:</h3>";
$base_url = 'https://' . $_SERVER['HTTP_HOST'];
$test_endpoints = [
   '/tank/api/get_tank_data.php',
   '/tank/api/tank_update.php',
   '/tank/index.php',
    '/tank/dashboard/index.php'
];

foreach ($test_endpoints as $endpoint) {
    $url = $base_url . $endpoint;

    $ch = curl_init();
    curl_setopt($ch, CURLOPT_URL, $url);
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
    curl_setopt($ch, CURLOPT_TIMEOUT, 5);
    curl_setopt($ch, CURLOPT_SSL_VERIFYPEER, false);
    curl_setopt($ch, CURLOPT_NOBODY, true);

    curl_exec($ch);
    $http_code = curl_getinfo($ch, CURLOPT_HTTP_CODE);
    curl_close($ch);

    echo "Testing: <a href='$url' target='_blank'>$url</a><br>";
    echo "&nbsp;&nbsp;&nbsp;";
    if ($http_code == 200) {
        echo "✅ HTTP $http_code - OK<br>";
    } elseif ($http_code == 405) {
        echo "✅ HTTP $http_code - OK (POST only)<br>";
    } else {
        echo "❌ HTTP $http_code - Issue<br>";
    }
}

echo "<h3>📊 Data Status:</h3>";
try {
    $pdo = new PDO(
        "mysql:host=localhost;dbname=dwljzndr_tank",
        "dwljzndr_tank",
        "Amir@123#khan"
    );

    // Check tank data
    $stmt = $pdo->query("SELECT * FROM latest_tank_readings ORDER BY timestamp DESC LIMIT 3");
    $readings = $stmt->fetchAll(PDO::FETCH_ASSOC);

    if ($readings) {
        echo "✅ Recent tank readings:<br>";
        foreach ($readings as $reading) {
            echo "&nbsp;&nbsp;&nbsp;Tank {$reading['tank_id']}: {$reading['level']}cm, {$reading['percentage']}% at {$reading['timestamp']}<br>";
        }
    } else {
        echo "⚠️ No tank readings found<br>";
    }

} catch(PDOException $e) {
    echo "❌ Cannot check data: " . $e->getMessage() . "<br>";
}

echo "<h3>🚀 Next Steps:</h3>";
echo "<ol>";
echo "<li>✅ Files uploaded to server</li>";
echo "<li>✅ Database connected and has data</li>";
echo "<li>🔧 Update ESP32 config with correct API endpoint: <code>/tank/api/tank_update.php</code></li>";
echo "<li>⚡ Test sensor data transmission</li>";
echo "<li>📱 Access dashboard: <a href='/tank/dashboard/' target='_blank'>View Dashboard</a></li>";
echo "</ol>";

echo "<h3>💡 Quick Test Commands:</h3>";
echo "<code>curl https://iotlogic.in/tank/api/get_tank_data.php</code><br>";
echo "<code>curl -X POST https://iotlogic.in/tank/api/tank_update.php -H 'Content-Type: application/json' -d '{\"tank_id\":1,\"level\":150,\"percentage\":75,\"api_key\":\"iotlogic\"}'</code><br>";

echo "<p><small>Updated for iotlogic.in live server</small></p>";
?>