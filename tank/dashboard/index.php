<?php
// 🚀 Modern Tank Monitoring Dashboard
// Version: 2.0 - Redesigned with modular architecture

require_once '../api/config.php';

// 💾 DATABASE: Fetch initial data
try {
    $pdo = new PDO(
        "mysql:host=" . DB_HOST . ";dbname=" . DB_NAME,
        DB_USER,
        DB_PASS,
        array(PDO::MYSQL_ATTR_INIT_COMMAND => "SET NAMES utf8")
    );
    $pdo->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
    
    // Get latest tank data
    $stmt = $pdo->query("SELECT * FROM latest_tank_readings WHERE tank_id = 1");
    $latest_data = $stmt->fetch(PDO::FETCH_ASSOC);
    
} catch(PDOException $e) {
    $latest_data = null;
    error_log("Database connection failed: " . $e->getMessage());
}

// Set defaults if no data
$level = $latest_data['level'] ?? 0;
$percentage = $latest_data['percentage'] ?? 0;
$status = $latest_data['status'] ?? 'Unknown';
$timestamp = $latest_data['timestamp'] ?? date('Y-m-d H:i:s');
?>
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <meta name="description" content="IoT Water Tank Monitoring Dashboard with Real-time Updates">
    <title>Tank Monitor - IoT Dashboard</title>
    
    <!-- 🎨 CSS Files -->
    <link rel="stylesheet" href="css/variables.css">
    <link rel="stylesheet" href="css/base.css">
    <link rel="stylesheet" href="css/components.css">
    <link rel="stylesheet" href="css/dashboard.css">
    <link rel="stylesheet" href="css/animations.css">
    <link rel="stylesheet" href="css/tank-3d.css">
    
    <!-- 📚 External Libraries -->
    <link href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.4.0/css/all.min.css" rel="stylesheet">
    <link href="https://fonts.googleapis.com/css2?family=Inter:wght@300;400;500;600;700&display=swap" rel="stylesheet">
    <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
</head>
<body>
    <div class="container-fluid">
        
        <!-- 🎯 HEADER -->
        <header class="dashboard-header">
            <div class="header-brand">
                <div class="header-logo">
                    <i class="fas fa-water"></i>
                </div>
                <div class="header-title">
                    <h1>Tank Monitor</h1>
                    <p class="header-subtitle">IoT Water Level System</p>
                </div>
            </div>
            <div class="header-actions">
                <button class="header-btn" id="refreshBtn" title="Refresh Data">
                    <i class="fas fa-sync-alt"></i>
                </button>
                <button class="header-btn" id="themeToggle" title="Toggle Theme">
                    <i class="fas fa-moon"></i>
                </button>
                <button class="header-btn" title="Connection Status">
                    <span class="status-indicator status-online"></span>
                </button>
            </div>
        </header>

        <!-- 🎯 HERO SECTION -->
        <section class="hero-section">
            
            <!-- 🌊 3D Tank Visualization with Effects -->
            <div class="tank-card">
                <h3 class="text-center">Tank Status - 3D View</h3>
                
                <!-- 3D Tank Container -->
                <div class="tank-3d-container" id="tank3dContainer">
                    <!-- Percentage Display -->
                    <div class="tank-3d-percentage" id="tank3dPercentage"><?php echo $percentage; ?>%</div>

                    <!-- Tank Body -->
                    <div class="tank-3d-body">
                        <div class="tank-cylinder">
                            <!-- Water -->
                            <div class="tank-3d-water" id="tank3dWater" 
                                 data-level="<?php echo $percentage <= 10 ? 'critical' : ($percentage <= 30 ? 'low' : ($percentage <= 60 ? 'medium' : 'high')); ?>"
                                 style="height: <?php echo $percentage; ?>%;">
                                <!-- Bubbles -->
                                <div class="tank-bubble"></div>
                                <div class="tank-bubble"></div>
                                <div class="tank-bubble"></div>
                                <div class="tank-bubble"></div>
                                <div class="tank-bubble"></div>
                            </div>

                            <!-- Glass Reflection -->
                            <div class="tank-reflection"></div>
                        </div>

                        <!-- Level Marks -->
                        <div class="tank-level-marks">
                            <div class="level-mark" data-level="100%"></div>
                            <div class="level-mark" data-level="75%"></div>
                            <div class="level-mark" data-level="50%"></div>
                            <div class="level-mark" data-level="25%"></div>
                            <div class="level-mark" data-level="0%"></div>
                        </div>
                    </div>

                    <!-- Status Ring -->
                    <div class="tank-status-ring"></div>
                </div>

                <div class="tank-status">
                    <div class="tank-status-badge badge-success" id="statusBadge">
                        <span class="status-indicator status-online"></span>
                        <span><?php echo $status; ?></span>
                    </div>
                </div>
            </div>

            <!-- 📊 Metrics Grid -->
            <div class="metrics-grid">
                
                <!-- Metric: Current Level -->
                <div class="metric-card">
                    <div class="metric-icon" style="background: var(--gradient-primary);">
                        <i class="fas fa-ruler-vertical"></i>
                    </div>
                    <div class="metric-value" id="metricLevel"><?php echo $level; ?> cm</div>
                    <div class="metric-label">Water Level</div>
                </div>

                <!-- Metric: Fill Percentage -->
                <div class="metric-card">
                    <div class="metric-icon" style="background: var(--gradient-success);">
                        <i class="fas fa-percentage"></i>
                    </div>
                    <div class="metric-value" id="metricPercent"><?php echo $percentage; ?>%</div>
                    <div class="metric-label">Fill Level</div>
                    <div class="progress mt-2">
                        <div class="progress-bar progress-bar-success" id="percentProgress" style="width: <?php echo $percentage; ?>%;"></div>
                    </div>
                </div>

                <!-- Metric: Last Update -->
                <div class="metric-card">
                    <div class="metric-icon" style="background: var(--gradient-info);">
                        <i class="fas fa-clock"></i>
                    </div>
                    <div class="metric-value" id="lastUpdateTime">
                        <?php 
                        $time = new DateTime($timestamp);
                        $now = new DateTime();
                        $diff = $now->diff($time);
                        echo $diff->i . 'm ago';
                        ?>
                    </div>
                    <div class="metric-label">Last Update</div>
                    <small class="text-muted" style="font-size: 0.75rem; margin-top: 0.5rem; display: block;">
                        <?php echo date('H:i:s', strtotime($timestamp)); ?>
                    </small>
                </div>

                <!-- Metric: Tank Capacity -->
                <div class="metric-card">
                    <div class="metric-icon" style="background: var(--gradient-warning);">
                        <i class="fas fa-database"></i>
                    </div>
                    <div class="metric-value">200 cm</div>
                    <div class="metric-label">Max Capacity</div>
                </div>

            </div>
        </section>

        <!-- 📈 CHART SECTION -->
        <section class="chart-section">
            <div class="chart-header">
                <h3 class="chart-title">
                    <i class="fas fa-chart-line"></i>
                    Historical Data
                </h3>
                <div class="chart-controls">
                    <button class="chart-btn" data-period="1h">1H</button>
                    <button class="chart-btn" data-period="6h">6H</button>
                    <button class="chart-btn active" data-period="24h">24H</button>
                </div>
            </div>
            <div class="chart-wrapper">
                <canvas id="tankChart"></canvas>
            </div>
        </section>

        <!-- 👣 FOOTER -->
        <footer class="dashboard-footer">
            <div class="footer-links">
                <a href="#"><i class="fas fa-home"></i> Home</a>
                <a href="#"><i class="fas fa-cog"></i> Settings</a>
                <a href="#"><i class="fas fa-question-circle"></i> Help</a>
            </div>
            <p>&copy; <?php echo date('Y'); ?> Tank Monitoring System | Version 2.0</p>
            <small style="color: var(--text-muted);">
                Environment: <strong><?php echo defined('DB_HOST') ? 'Connected' : 'Disconnected'; ?></strong>
            </small>
        </footer>

    </div>

    <!-- ⚙️ JavaScript Modules (ORDER MATTERS!) -->
    <script src="js/config.js"></script>
    <script src="js/api.js"></script>
    <script src="js/ui.js"></script>
    <script src="js/tank3d.js"></script>
    <script src="js/chartManager.js"></script>
    <script src="js/theme.js"></script>
    <script src="js/notifications.js"></script>
    <script src="js/main.js"></script>

</body>
</html>
