<?php
// Tank Monitoring System - Main Index
// Redirects to dashboard or shows system status

header('Location: dashboard/');
exit();

// Alternative: Show a simple landing page
/*
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Tank Monitoring System</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            max-width: 800px;
            margin: 50px auto;
            padding: 20px;
            background-color: #f5f5f5;
        }
        .container {
            background: white;
            padding: 30px;
            border-radius: 10px;
            box-shadow: 0 2px 10px rgba(0,0,0,0.1);
        }
        .btn {
            display: inline-block;
            padding: 10px 20px;
            background-color: #007bff;
            color: white;
            text-decoration: none;
            border-radius: 5px;
            margin: 10px 10px 10px 0;
        }
        .btn:hover {
            background-color: #0056b3;
        }
        .status {
            padding: 15px;
            margin: 20px 0;
            border-radius: 5px;
            background-color: #d4edda;
            border: 1px solid #c3e6cb;
            color: #155724;
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>🚰 Tank Monitoring System</h1>

        <div class="status">
            <h3>✅ System Status: Operational</h3>
            <p>Database: Connected | API: Active | Data Collection: Running</p>
        </div>

        <h2>Available Interfaces:</h2>

        <a href="dashboard/" class="btn">📊 View Dashboard</a>
        <a href="dashboard/" class="btn">📊 Live Dashboard</a>

        <h3>API Endpoints:</h3>
        <ul>
            <li><strong>Update Tank Data:</strong> <code>POST /tank/api/tank_update.php</code></li>
            <li><strong>Get Tank Data:</strong> <code>GET /tank/api/get_tank_data.php</code></li>
            <li><strong>Live Dashboard:</strong> <code>GET /tank/dashboard/</code></li>
        </ul>

        <h3>System Information:</h3>
        <ul>
            <li>Server: <?php echo $_SERVER['SERVER_NAME']; ?></li>
            <li>PHP Version: <?php echo PHP_VERSION; ?></li>
            <li>Current Time: <?php echo date('Y-m-d H:i:s T'); ?></li>
        </ul>

        <p><small>Proudly powered by LiteSpeed Web Server</small></p>
    </div>
</body>
</html>
*/
?>