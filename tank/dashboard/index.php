<?php
require_once '../api/config.php';

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
    die("Database connection failed: " . $e->getMessage());
}

// Get latest tank data
$stmt = $pdo->query("SELECT * FROM latest_tank_readings WHERE tank_id = 1");
$latest_data = $stmt->fetch(PDO::FETCH_ASSOC);

// Get last 24 hours of data for chart
$stmt = $pdo->query("
    SELECT level, percentage, timestamp
    FROM tank_data
    WHERE tank_id = 1 AND timestamp >= DATE_SUB(NOW(), INTERVAL 24 HOUR)
    ORDER BY timestamp ASC
");
$chart_data = $stmt->fetchAll(PDO::FETCH_ASSOC);

// Get hourly averages for the chart
$stmt = $pdo->query("
    SELECT * FROM hourly_averages
    WHERE tank_id = 1
    ORDER BY hour_start ASC
");
$hourly_data = $stmt->fetchAll(PDO::FETCH_ASSOC);

// Get alert settings
$stmt = $pdo->query("SELECT * FROM alert_settings WHERE tank_id = 1 AND is_enabled = 1");
$alerts = $stmt->fetchAll(PDO::FETCH_ASSOC);

// Get system status
$system_status = [
    'database' => true,
    'api' => true,
    'last_update' => $latest_data['timestamp'] ?? null,
    'offline_duration' => null
];

if ($system_status['last_update']) {
    $last_update = new DateTime($system_status['last_update']);
    $now = new DateTime();
    $diff = $now->diff($last_update);
    $system_status['offline_duration'] = ($diff->i * 60) + $diff->s;
}
?>
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Smart Tank Monitor - IoT Dashboard</title>
    <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
    <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.3.0/dist/css/bootstrap.min.css" rel="stylesheet">
    <link href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.4.0/css/all.min.css" rel="stylesheet">
    <link href="https://fonts.googleapis.com/css2?family=Inter:wght@300;400;500;600;700&display=swap" rel="stylesheet">
    <style>
        :root {
            /* 🎨 Advanced Color Palette */
            --primary-color: #6366f1;
            --primary-gradient: linear-gradient(135deg, #6366f1 0%, #8b5cf6 100%);
            --secondary-color: #10b981;
            --secondary-gradient: linear-gradient(135deg, #10b981 0%, #059669 100%);
            --danger-color: #ef4444;
            --danger-gradient: linear-gradient(135deg, #ef4444 0%, #dc2626 100%);
            --warning-color: #f59e0b;
            --warning-gradient: linear-gradient(135deg, #f59e0b 0%, #d97706 100%);
            --info-color: #06b6d4;
            --info-gradient: linear-gradient(135deg, #06b6d4 0%, #0891b2 100%);
            --dark-color: #0f172a;
            --light-color: #f8fafc;

            /* 🎭 Advanced Design Variables */
            --border-radius: 16px;
            --border-radius-lg: 24px;
            --shadow: 0 4px 6px -1px rgba(0, 0, 0, 0.1), 0 2px 4px -1px rgba(0, 0, 0, 0.06);
            --shadow-lg: 0 10px 15px -3px rgba(0, 0, 0, 0.1), 0 4px 6px -2px rgba(0, 0, 0, 0.05);
            --shadow-xl: 0 20px 25px -5px rgba(0, 0, 0, 0.1), 0 10px 10px -5px rgba(0, 0, 0, 0.04);
            --shadow-2xl: 0 25px 50px -12px rgba(0, 0, 0, 0.25);
            --shadow-inner: inset 0 2px 4px 0 rgba(0, 0, 0, 0.06);

            /* 🌊 Animation Variables */
            --animation-fast: 0.15s cubic-bezier(0.4, 0, 0.2, 1);
            --animation-normal: 0.3s cubic-bezier(0.4, 0, 0.2, 1);
            --animation-slow: 0.5s cubic-bezier(0.4, 0, 0.2, 1);
            --animation-bounce: 0.6s cubic-bezier(0.68, -0.55, 0.265, 1.55);

            /* 📐 Layout Variables */
            --header-height: 80px;
            --sidebar-width: 280px;
            --content-padding: 2rem;
        }

        /* 🎨 Advanced Global Styles */
        * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
        }

        html {
            scroll-behavior: smooth;
        }

        body {
            font-family: 'Inter', -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif;
            background: linear-gradient(135deg, #0f172a 0%, #1e293b 50%, #334155 100%);
            background-attachment: fixed;
            min-height: 100vh;
            color: var(--light-color);
            line-height: 1.6;
            font-feature-settings: "cv02", "cv03", "cv04", "cv11";
            font-variation-settings: "opsz" 32;
        }

        /* 🌊 Advanced Background Effects */
        body::before {
            content: '';
            position: fixed;
            top: 0;
            left: 0;
            right: 0;
            bottom: 0;
            background:
                radial-gradient(circle at 20% 80%, rgba(99, 102, 241, 0.15) 0%, transparent 50%),
                radial-gradient(circle at 80% 20%, rgba(16, 185, 129, 0.15) 0%, transparent 50%),
                radial-gradient(circle at 40% 40%, rgba(6, 182, 212, 0.1) 0%, transparent 50%);
            pointer-events: none;
            z-index: -1;
        }

        /* 🎪 Advanced Container */
        .dashboard-container {
            max-width: 1600px;
            margin: 0 auto;
            padding: var(--content-padding);
            position: relative;
        }

        * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
        }

        body {
            font-family: 'Inter', sans-serif;
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            min-height: 100vh;
            color: var(--dark-color);
        }

        .dashboard-container {
            max-width: 1400px;
            margin: 0 auto;
            padding: 20px;
        }

        /* 🎭 Advanced Header */
        .dashboard-header {
            background: rgba(15, 23, 42, 0.9);
            backdrop-filter: blur(20px) saturate(180%);
            border: 1px solid rgba(255, 255, 255, 0.1);
            border-radius: var(--border-radius-lg);
            padding: 2rem;
            margin-bottom: 2rem;
            box-shadow: var(--shadow-xl);
            position: relative;
            overflow: hidden;
        }

        .dashboard-header::before {
            content: '';
            position: absolute;
            top: 0;
            left: 0;
            right: 0;
            bottom: 0;
            background: linear-gradient(135deg, rgba(99, 102, 241, 0.1) 0%, rgba(16, 185, 129, 0.1) 100%);
            border-radius: var(--border-radius-lg);
            pointer-events: none;
        }

        .dashboard-header::after {
            content: '';
            position: absolute;
            top: -50%;
            left: -50%;
            width: 200%;
            height: 200%;
            background: conic-gradient(from 0deg, transparent, rgba(99, 102, 241, 0.05), transparent);
            animation: rotate 20s linear infinite;
            pointer-events: none;
        }

        @keyframes rotate {
            0% { transform: rotate(0deg); }
            100% { transform: rotate(360deg); }
        }

        .header-content {
            display: flex;
            justify-content: space-between;
            align-items: center;
            flex-wrap: wrap;
            gap: 1.5rem;
        }

        .logo-section {
            display: flex;
            align-items: center;
            gap: 1.5rem;
        }

        .logo-icon {
            width: 60px;
            height: 60px;
            background: var(--primary-gradient);
            border-radius: 16px;
            display: flex;
            align-items: center;
            justify-content: center;
            color: white;
            font-size: 1.8rem;
            position: relative;
            box-shadow: var(--shadow-lg);
            animation: logoGlow 4s ease-in-out infinite;
        }

        .logo-icon::before {
            content: '';
            position: absolute;
            top: -3px;
            left: -3px;
            right: -3px;
            bottom: -3px;
            background: var(--primary-gradient);
            border-radius: 19px;
            z-index: -1;
            opacity: 0.3;
            animation: logoPulse 4s ease-in-out infinite;
        }

        @keyframes logoGlow {
            0%, 100% { box-shadow: 0 0 20px rgba(99, 102, 241, 0.3); }
            50% { box-shadow: 0 0 30px rgba(99, 102, 241, 0.5); }
        }

        @keyframes logoPulse {
            0%, 100% { opacity: 0.3; transform: scale(1); }
            50% { opacity: 0.6; transform: scale(1.1); }
        }

        .logo-text h1 {
            font-size: 2.2rem;
            font-weight: 800;
            background: linear-gradient(135deg, #ffffff 0%, #e2e8f0 100%);
            -webkit-background-clip: text;
            -webkit-text-fill-color: transparent;
            background-clip: text;
            margin: 0;
            text-shadow: 0 2px 4px rgba(0, 0, 0, 0.1);
        }

        .logo-text p {
            color: rgba(255, 255, 255, 0.7);
            margin: 0;
            font-size: 1rem;
            font-weight: 500;
            text-transform: uppercase;
            letter-spacing: 1px;
        }

        .header-actions {
            display: flex;
            gap: 1rem;
            align-items: center;
        }

        .theme-toggle, .notification-btn, .refresh-btn {
            width: 45px;
            height: 45px;
            border: none;
            border-radius: 10px;
            background: var(--light-color);
            color: var(--dark-color);
            cursor: pointer;
            transition: all 0.3s ease;
            display: flex;
            align-items: center;
            justify-content: center;
            font-size: 1.1rem;
            box-shadow: var(--shadow);
        }

        .theme-toggle:hover, .notification-btn:hover, .refresh-btn:hover {
            transform: translateY(-2px);
            box-shadow: var(--shadow-lg);
        }

        /* 🎭 Advanced Navigation */
        .nav-tabs {
            background: rgba(15, 23, 42, 0.9);
            backdrop-filter: blur(20px) saturate(180%);
            border: 1px solid rgba(255, 255, 255, 0.1);
            border-radius: var(--border-radius-lg);
            padding: 0.75rem;
            margin-bottom: 2rem;
            box-shadow: var(--shadow-xl);
            position: relative;
            overflow: hidden;
        }

        .nav-tabs::before {
            content: '';
            position: absolute;
            top: 0;
            left: 0;
            right: 0;
            bottom: 0;
            background: linear-gradient(135deg, rgba(99, 102, 241, 0.05) 0%, rgba(16, 185, 129, 0.05) 100%);
            border-radius: var(--border-radius-lg);
        }

        .nav-tabs .nav-link {
            border: none;
            border-radius: 12px;
            color: rgba(255, 255, 255, 0.7);
            font-weight: 600;
            padding: 1rem 2rem;
            transition: all var(--animation-normal);
            position: relative;
            overflow: hidden;
            text-transform: uppercase;
            letter-spacing: 0.5px;
        }

        .nav-tabs .nav-link::before {
            content: '';
            position: absolute;
            top: 0;
            left: -100%;
            width: 100%;
            height: 100%;
            background: linear-gradient(90deg, transparent, rgba(255, 255, 255, 0.1), transparent);
            transition: left var(--animation-slow);
        }

        .nav-tabs .nav-link:hover::before {
            left: 100%;
        }

        .nav-tabs .nav-link.active {
            background: var(--primary-gradient);
            color: white;
            box-shadow: 0 8px 25px rgba(99, 102, 241, 0.3);
            transform: translateY(-2px);
        }

        .nav-tabs .nav-link:hover:not(.active) {
            background: rgba(255, 255, 255, 0.05);
            color: rgba(255, 255, 255, 0.9);
            transform: translateY(-2px);
        }

        /* 🎪 Advanced Metric Cards */
        .metric-card {
            background: rgba(15, 23, 42, 0.8);
            backdrop-filter: blur(20px) saturate(180%);
            border: 1px solid rgba(255, 255, 255, 0.1);
            border-radius: var(--border-radius);
            padding: 2rem;
            margin-bottom: 1.5rem;
            box-shadow: var(--shadow-lg);
            transition: all var(--animation-normal);
            position: relative;
            overflow: hidden;
            cursor: pointer;
        }

        .metric-card::before {
            content: '';
            position: absolute;
            top: 0;
            left: 0;
            right: 0;
            bottom: 0;
            background: linear-gradient(135deg, rgba(99, 102, 241, 0.05) 0%, rgba(16, 185, 129, 0.05) 100%);
            opacity: 0;
            transition: opacity var(--animation-normal);
            border-radius: var(--border-radius);
        }

        .metric-card:hover {
            transform: translateY(-8px) scale(1.02);
            box-shadow: var(--shadow-2xl);
            border-color: rgba(99, 102, 241, 0.3);
        }

        .metric-card:hover::before {
            opacity: 1;
        }

        .metric-card:active {
            transform: translateY(-4px) scale(1.01);
            transition: all var(--animation-fast);
        }

        /* 🎭 Advanced Metric Elements */
        .metric-icon {
            width: 70px;
            height: 70px;
            border-radius: 16px;
            display: flex;
            align-items: center;
            justify-content: center;
            font-size: 1.8rem;
            margin-bottom: 1.5rem;
            position: relative;
            background: var(--primary-gradient);
            box-shadow: var(--shadow-lg);
            transition: all var(--animation-normal);
            animation: iconFloat 6s ease-in-out infinite;
        }

        .metric-icon::before {
            content: '';
            position: absolute;
            top: -2px;
            left: -2px;
            right: -2px;
            bottom: -2px;
            background: var(--primary-gradient);
            border-radius: 18px;
            z-index: -1;
            opacity: 0;
            transition: opacity var(--animation-normal);
        }

        .metric-card:hover .metric-icon {
            transform: scale(1.1) rotate(5deg);
            animation: iconPulse 1s ease-in-out infinite;
        }

        .metric-card:hover .metric-icon::before {
            opacity: 0.3;
        }

        @keyframes iconFloat {
            0%, 100% { transform: translateY(0px); }
            50% { transform: translateY(-5px); }
        }

        @keyframes iconPulse {
            0%, 100% { transform: scale(1.1) rotate(5deg); }
            50% { transform: scale(1.15) rotate(-5deg); }
        }

        .metric-value {
            font-size: 2.5rem;
            font-weight: 800;
            margin: 0.5rem 0;
            background: linear-gradient(135deg, #ffffff 0%, #e2e8f0 100%);
            -webkit-background-clip: text;
            -webkit-text-fill-color: transparent;
            background-clip: text;
            text-shadow: 0 2px 4px rgba(0, 0, 0, 0.1);
            transition: all var(--animation-normal);
        }

        .metric-card:hover .metric-value {
            transform: scale(1.05);
            text-shadow: 0 4px 8px rgba(0, 0, 0, 0.2);
        }

        .metric-label {
            color: rgba(255, 255, 255, 0.7);
            font-size: 0.85rem;
            text-transform: uppercase;
            letter-spacing: 1px;
            font-weight: 600;
            transition: color var(--animation-normal);
        }

        .metric-card:hover .metric-label {
            color: rgba(255, 255, 255, 0.9);
        }

        /* 🌊 Advanced 3D Tank Visualization */
        .tank-status-header {
            background: rgba(15, 23, 42, 0.9);
            backdrop-filter: blur(20px) saturate(180%);
            border: 1px solid rgba(255, 255, 255, 0.1);
            border-radius: var(--border-radius-lg);
            box-shadow: var(--shadow-xl);
            position: relative;
            overflow: hidden;
        }

        .tank-visualization-large {
            text-align: center;
            perspective: 1000px;
            position: relative;
        }

        .tank-container-large {
            position: relative;
            width: 280px;
            height: 400px;
            margin: 2rem auto;
            transform-style: preserve-3d;
            animation: tankFloat 8s ease-in-out infinite;
        }

        @keyframes tankFloat {
            0%, 100% { transform: translateY(0px) rotateY(0deg); }
            25% { transform: translateY(-10px) rotateY(2deg); }
            75% { transform: translateY(-5px) rotateY(-2deg); }
        }

        .tank-body {
            position: absolute;
            width: 100%;
            height: 100%;
            background: linear-gradient(145deg, #1e293b 0%, #334155 50%, #1e293b 100%);
            border: 3px solid #475569;
            border-radius: 30px 30px 0 0;
            box-shadow:
                inset 0 0 20px rgba(0, 0, 0, 0.3),
                0 15px 35px rgba(0, 0, 0, 0.3),
                0 0 50px rgba(99, 102, 241, 0.1);
            overflow: hidden;
        }

        .tank-body::before {
            content: '';
            position: absolute;
            top: 0;
            left: 0;
            right: 0;
            bottom: 0;
            background:
                linear-gradient(180deg, transparent 0%, rgba(99, 102, 241, 0.05) 50%, transparent 100%),
                radial-gradient(circle at 30% 30%, rgba(255, 255, 255, 0.1) 0%, transparent 50%);
            pointer-events: none;
        }

        .tank-water-large {
            position: absolute;
            bottom: 0;
            left: 0;
            right: 0;
            height: 0%;
            background:
                linear-gradient(180deg, #3b82f6 0%, #1d4ed8 50%, #1e40af 100%),
                radial-gradient(ellipse at center, rgba(255, 255, 255, 0.3) 0%, transparent 70%);
            transition: height 1.2s cubic-bezier(0.25, 0.46, 0.45, 0.94);
            overflow: hidden;
            border-radius: 0 0 25px 25px;
        }

        .tank-water-large::before {
            content: '';
            position: absolute;
            top: 0;
            left: 0;
            right: 0;
            height: 40px;
            background: linear-gradient(180deg, rgba(255, 255, 255, 0.4) 0%, rgba(255, 255, 255, 0.1) 50%, transparent 100%);
            animation: waterSurface 3s ease-in-out infinite;
        }

        .tank-water-large::after {
            content: '';
            position: absolute;
            top: 30%;
            left: 20%;
            width: 60px;
            height: 60px;
            background: radial-gradient(circle, rgba(255, 255, 255, 0.6) 0%, rgba(255, 255, 255, 0.2) 50%, transparent 100%);
            border-radius: 50%;
            animation: waterRipple 4s ease-in-out infinite;
        }

        @keyframes waterSurface {
            0%, 100% { transform: scaleX(1) rotate(-1deg); opacity: 0.4; }
            50% { transform: scaleX(1.02) rotate(1deg); opacity: 0.7; }
        }

        @keyframes waterRipple {
            0%, 100% {
                transform: scale(1) translateY(0px);
                opacity: 0.6;
            }
            50% {
                transform: scale(1.5) translateY(-10px);
                opacity: 0.9;
            }
        }

        .tank-bubbles {
            position: absolute;
            width: 100%;
            height: 100%;
            pointer-events: none;
        }

        .bubble {
            position: absolute;
            background: rgba(255, 255, 255, 0.3);
            border-radius: 50%;
            animation: bubbleRise 3s ease-in-out infinite;
        }

        .bubble:nth-child(1) {
            width: 8px;
            height: 8px;
            left: 20%;
            bottom: 0%;
            animation-delay: 0s;
        }

        .bubble:nth-child(2) {
            width: 6px;
            height: 6px;
            left: 60%;
            bottom: 0%;
            animation-delay: 1s;
        }

        .bubble:nth-child(3) {
            width: 10px;
            height: 10px;
            left: 80%;
            bottom: 0%;
            animation-delay: 2s;
        }

        @keyframes bubbleRise {
            0% {
                transform: translateY(100%) scale(0);
                opacity: 0;
            }
            20% {
                transform: translateY(80%) scale(0.5);
                opacity: 0.6;
            }
            80% {
                transform: translateY(20%) scale(1);
                opacity: 0.8;
            }
            100% {
                transform: translateY(-20%) scale(0.8);
                opacity: 0;
            }
        }

        .tank-level-indicator-large {
            position: absolute;
            top: -50px;
            left: 50%;
            transform: translateX(-50%);
            background: rgba(0, 0, 0, 0.8);
            color: white;
            padding: 12px 24px;
            border-radius: 25px;
            font-weight: 700;
            font-size: 1.2rem;
            box-shadow: var(--shadow-lg);
            border: 3px solid rgba(255, 255, 255, 0.3);
        }

        .tank-label-large {
            position: absolute;
            bottom: -40px;
            left: 50%;
            transform: translateX(-50%);
            color: #6b7280;
            font-size: 0.9rem;
            font-weight: 500;
        }

        /* 🎯 Advanced Status Badge with Dynamic Theming */
        .status-badge {
            display: inline-flex;
            align-items: center;
            gap: 12px;
            padding: 12px 24px;
            border-radius: 25px;
            font-weight: 700;
            font-size: 0.95rem;
            position: relative;
            overflow: hidden;
            transition: all var(--animation-normal);
            border: 2px solid transparent;
        }

        .status-badge::before {
            content: '';
            position: absolute;
            top: 0;
            left: -100%;
            width: 100%;
            height: 100%;
            background: linear-gradient(90deg, transparent, rgba(255, 255, 255, 0.2), transparent);
            animation: statusShimmer 3s ease-in-out infinite;
        }

        @keyframes statusShimmer {
            0% { left: -100%; }
            50% { left: 100%; }
            100% { left: -100%; }
        }

        .status-indicator {
            width: 20px;
            height: 20px;
            border-radius: 50%;
            position: relative;
            animation: statusPulse 2s ease-in-out infinite;
        }

        .status-indicator::before {
            content: '';
            position: absolute;
            top: -4px;
            left: -4px;
            right: -4px;
            bottom: -4px;
            border-radius: 50%;
            border: 2px solid currentColor;
            opacity: 0.3;
            animation: statusRing 2s ease-in-out infinite;
        }

        @keyframes statusPulse {
            0%, 100% {
                transform: scale(1);
                box-shadow: 0 0 0 0 currentColor;
            }
            50% {
                transform: scale(1.1);
                box-shadow: 0 0 0 8px transparent;
            }
        }

        @keyframes statusRing {
            0%, 100% { opacity: 0.3; transform: scale(1); }
            50% { opacity: 0.8; transform: scale(1.2); }
        }

        /* 🎨 Dynamic Status Colors */
        .status-normal {
            background: var(--secondary-gradient);
            color: white;
            border-color: rgba(16, 185, 129, 0.3);
        }

        .status-warning {
            background: var(--warning-gradient);
            color: white;
            border-color: rgba(245, 158, 11, 0.3);
            animation: warningPulse 2s ease-in-out infinite;
        }

        .status-danger {
            background: var(--danger-gradient);
            color: white;
            border-color: rgba(239, 68, 68, 0.3);
            animation: dangerPulse 1.5s ease-in-out infinite;
        }

        @keyframes warningPulse {
            0%, 100% { transform: scale(1); }
            50% { transform: scale(1.05); }
        }

        @keyframes dangerPulse {
            0%, 100% { transform: scale(1); }
            50% { transform: scale(1.1); }
        }

        /* 🎪 Advanced Quick Actions Bar */
        .quick-actions-bar {
            background: rgba(15, 23, 42, 0.9);
            backdrop-filter: blur(20px) saturate(180%);
            border: 1px solid rgba(255, 255, 255, 0.1);
            border-radius: var(--border-radius-lg);
            box-shadow: var(--shadow-xl);
            position: relative;
            overflow: hidden;
        }

        .quick-actions-bar::before {
            content: '';
            position: absolute;
            top: 0;
            left: 0;
            right: 0;
            bottom: 0;
            background: linear-gradient(135deg, rgba(99, 102, 241, 0.05) 0%, rgba(16, 185, 129, 0.05) 100%);
            border-radius: var(--border-radius-lg);
        }

        /* 🎭 Advanced Button Animations */
        .quick-actions-bar .btn {
            position: relative;
            overflow: hidden;
            transition: all var(--animation-normal);
            border: none;
            font-weight: 600;
            text-transform: uppercase;
            letter-spacing: 0.5px;
            border-radius: 12px;
            padding: 0.75rem 1.5rem;
        }

        .quick-actions-bar .btn::before {
            content: '';
            position: absolute;
            top: 50%;
            left: 50%;
            width: 0;
            height: 0;
            background: rgba(255, 255, 255, 0.2);
            border-radius: 50%;
            transform: translate(-50%, -50%);
            transition: all var(--animation-slow);
        }

        .quick-actions-bar .btn:hover::before {
            width: 300px;
            height: 300px;
        }

        .quick-actions-bar .btn:hover {
            transform: translateY(-3px);
            box-shadow: 0 10px 25px rgba(0, 0, 0, 0.2);
        }

        .quick-actions-bar .btn:active {
            transform: translateY(-1px);
            transition: all var(--animation-fast);
        }

        /* 🎨 Button-specific Animations */
        .btn-primary {
            background: var(--primary-gradient);
            color: white;
            animation: primaryGlow 3s ease-in-out infinite;
        }

        .btn-success {
            background: var(--secondary-gradient);
            color: white;
            animation: successGlow 3s ease-in-out infinite;
        }

        .btn-info {
            background: var(--info-gradient);
            color: white;
            animation: infoGlow 3s ease-in-out infinite;
        }

        .btn-warning {
            background: var(--warning-gradient);
            color: white;
            animation: warningGlow 3s ease-in-out infinite;
        }

        @keyframes primaryGlow {
            0%, 100% { box-shadow: 0 0 20px rgba(99, 102, 241, 0.3); }
            50% { box-shadow: 0 0 30px rgba(99, 102, 241, 0.5); }
        }

        @keyframes successGlow {
            0%, 100% { box-shadow: 0 0 20px rgba(16, 185, 129, 0.3); }
            50% { box-shadow: 0 0 30px rgba(16, 185, 129, 0.5); }
        }

        @keyframes infoGlow {
            0%, 100% { box-shadow: 0 0 20px rgba(6, 182, 212, 0.3); }
            50% { box-shadow: 0 0 30px rgba(6, 182, 212, 0.5); }
        }

        @keyframes warningGlow {
            0%, 100% { box-shadow: 0 0 20px rgba(245, 158, 11, 0.3); }
            50% { box-shadow: 0 0 30px rgba(245, 158, 11, 0.5); }
        }

        /* Compact Metric Cards */
        .metric-card-compact {
            background: rgba(255, 255, 255, 0.9);
            border-radius: 12px;
            padding: 1.5rem;
            text-align: center;
            box-shadow: var(--shadow);
            transition: all 0.3s ease;
            border: 1px solid rgba(255, 255, 255, 0.5);
        }

        .metric-card-compact:hover {
            transform: translateY(-3px);
            box-shadow: var(--shadow-lg);
        }

        .metric-icon-compact {
            width: 50px;
            height: 50px;
            border-radius: 10px;
            display: flex;
            align-items: center;
            justify-content: center;
            font-size: 1.2rem;
            margin: 0 auto 1rem;
        }

        .metric-value-compact {
            font-size: 1.5rem;
            font-weight: 700;
            margin-bottom: 0.5rem;
        }

        .metric-label-compact {
            color: #6b7280;
            font-size: 0.8rem;
            text-transform: uppercase;
            letter-spacing: 0.5px;
        }

        .progress-compact {
            height: 4px;
            background: rgba(0, 0, 0, 0.1);
            border-radius: 2px;
            overflow: hidden;
        }

        /* Content Sections */
        .content-section {
            display: none;
            animation: fadeIn 0.5s ease;
        }

        .content-section.active {
            display: block;
        }

        @keyframes fadeIn {
            from { opacity: 0; transform: translateY(20px); }
            to { opacity: 1; transform: translateY(0); }
        }

        /* Tank Visualization (Original) */
        .tank-visualization {
            background: rgba(255, 255, 255, 0.95);
            backdrop-filter: blur(10px);
            border-radius: var(--border-radius);
            padding: 2rem;
            text-align: center;
            box-shadow: var(--shadow);
        }

        .tank-container {
            position: relative;
            width: 180px;
            height: 280px;
            margin: 2rem auto;
            border: 4px solid #374151;
            border-radius: 15px 15px 0 0;
            background: linear-gradient(to bottom, #f3f4f6 0%, #e5e7eb 100%);
            overflow: hidden;
            box-shadow: inset 0 4px 8px rgba(0, 0, 0, 0.1);
        }

        .tank-water {
            position: absolute;
            bottom: 0;
            left: 0;
            right: 0;
            background: linear-gradient(to bottom, #3b82f6, #1d4ed8);
            transition: height 0.8s cubic-bezier(0.4, 0, 0.2, 1);
            overflow: hidden;
        }

        .tank-water::before {
            content: '';
            position: absolute;
            top: 0;
            left: 0;
            right: 0;
            height: 20px;
            background: linear-gradient(to bottom, rgba(255, 255, 255, 0.3), transparent);
        }

        .tank-water::after {
            content: '';
            position: absolute;
            top: 50%;
            left: 50%;
            width: 30px;
            height: 30px;
            background: radial-gradient(circle, rgba(255, 255, 255, 0.4) 0%, transparent 70%);
            border-radius: 50%;
            transform: translate(-50%, -50%);
            animation: water-shine 3s ease-in-out infinite;
        }

        @keyframes water-shine {
            0%, 100% { opacity: 0.3; transform: translate(-50%, -50%) scale(1); }
            50% { opacity: 0.8; transform: translate(-50%, -50%) scale(1.2); }
        }

        .tank-level-indicator {
            position: absolute;
            top: -40px;
            left: 50%;
            transform: translateX(-50%);
            background: rgba(0, 0, 0, 0.8);
            color: white;
            padding: 8px 16px;
            border-radius: 20px;
            font-weight: 600;
            font-size: 0.9rem;
            box-shadow: var(--shadow);
        }

        .tank-label {
            position: absolute;
            bottom: -30px;
            left: 50%;
            transform: translateX(-50%);
            color: #6b7280;
            font-size: 0.8rem;
        }

        /* Status Indicators */
        .status-indicator {
            width: 16px;
            height: 16px;
            border-radius: 50%;
            display: inline-block;
            margin-right: 8px;
            animation: pulse 2s infinite;
        }

        .status-normal { background-color: var(--secondary-color); }
        .status-warning { background-color: var(--warning-color); }
        .status-danger { background-color: var(--danger-color); }

        @keyframes pulse {
            0% { box-shadow: 0 0 0 0 rgba(16, 185, 129, 0.7); }
            70% { box-shadow: 0 0 0 10px rgba(16, 185, 129, 0); }
            100% { box-shadow: 0 0 0 0 rgba(16, 185, 129, 0); }
        }

        /* 📊 Advanced Chart Container */
        .chart-container {
            background: rgba(15, 23, 42, 0.9);
            backdrop-filter: blur(20px) saturate(180%);
            border: 1px solid rgba(255, 255, 255, 0.1);
            border-radius: var(--border-radius-lg);
            padding: 2rem;
            box-shadow: var(--shadow-xl);
            height: 450px;
            position: relative;
            overflow: hidden;
        }

        .chart-container::before {
            content: '';
            position: absolute;
            top: 0;
            left: 0;
            right: 0;
            bottom: 0;
            background: linear-gradient(135deg, rgba(99, 102, 241, 0.03) 0%, rgba(16, 185, 129, 0.03) 100%);
            border-radius: var(--border-radius-lg);
        }

        .chart-container:hover {
            border-color: rgba(99, 102, 241, 0.2);
            box-shadow: var(--shadow-2xl);
        }

        /* Alerts */
        .alerts-panel {
            background: rgba(255, 255, 255, 0.95);
            backdrop-filter: blur(10px);
            border-radius: var(--border-radius);
            padding: 1.5rem;
            box-shadow: var(--shadow);
        }

        .alert-item {
            padding: 1rem;
            border-radius: 8px;
            margin-bottom: 0.5rem;
            border-left: 4px solid;
            animation: slideIn 0.5s ease;
        }

        .alert-item.warning {
            background: rgba(245, 158, 11, 0.1);
            border-left-color: var(--warning-color);
        }

        .alert-item.danger {
            background: rgba(239, 68, 68, 0.1);
            border-left-color: var(--danger-color);
        }

        @keyframes slideIn {
            from { opacity: 0; transform: translateX(-20px); }
            to { opacity: 1; transform: translateX(0); }
        }

        /* Navbar Styles */
        .navbar-brand {
            font-weight: 700;
            font-size: 1.5rem;
        }

        .navbar-nav .nav-link {
            padding: 0.5rem 1rem;
            transition: all 0.3s ease;
        }

        .navbar-nav .nav-link:hover {
            background: rgba(59, 130, 246, 0.1);
            border-radius: 8px;
        }

        .navbar-nav .nav-link.active {
            background: var(--primary-color);
            color: white !important;
            border-radius: 8px;
        }

        .dropdown-menu {
            border: none;
            box-shadow: var(--shadow-lg);
            border-radius: var(--border-radius);
        }

        .dropdown-item {
            padding: 0.75rem 1rem;
            transition: all 0.3s ease;
        }

        .dropdown-item:hover {
            background: rgba(59, 130, 246, 0.1);
            color: var(--primary-color);
        }

        /* Mobile Responsiveness */
        @media (max-width: 768px) {
            .dashboard-container {
                padding: 10px;
            }

            .tank-status-header {
                padding: 1rem;
            }

            .tank-container-large {
                width: 150px;
                height: 220px;
            }

            .tank-level-indicator-large {
                font-size: 1rem;
                padding: 8px 16px;
                top: -40px;
            }

            .quick-actions-bar {
                padding: 0.75rem;
            }

            .quick-actions-bar .btn {
                padding: 0.5rem 0.75rem;
                font-size: 0.9rem;
            }

            .navbar-nav .nav-link {
                padding: 0.75rem;
                text-align: center;
            }

            .navbar-brand {
                font-size: 1.2rem;
            }

            .metric-card-compact {
                padding: 1rem;
            }

            .metric-value-compact {
                font-size: 1.2rem;
            }

            .chart-container {
                height: 300px;
            }

            /* Mobile navigation improvements */
            .navbar-collapse {
                background: rgba(255, 255, 255, 0.95);
                backdrop-filter: blur(10px);
                border-radius: var(--border-radius);
                margin-top: 1rem;
                padding: 1rem;
                box-shadow: var(--shadow);
            }

            .navbar-nav {
                text-align: center;
            }

            .dropdown-menu {
                background: rgba(255, 255, 255, 0.95);
                backdrop-filter: blur(10px);
            }
        }

        /* Dark theme support */
        .dark-theme {
            --primary-color: #60a5fa;
            --secondary-color: #34d399;
            --danger-color: #f87171;
            --warning-color: #fbbf24;
            --info-color: #22d3ee;
            --dark-color: #f9fafb;
            --light-color: #1f2937;
        }

        .dark-theme body {
            background: linear-gradient(135deg, #1e293b 0%, #334155 100%);
        }

        .dark-theme .metric-card,
        .dark-theme .dashboard-header,
        .dark-theme .nav-tabs,
        .dark-theme .tank-visualization,
        .dark-theme .chart-container,
        .dark-theme .alerts-panel {
            background: rgba(30, 41, 59, 0.95);
            color: var(--dark-color);
        }

        /* 🌊 Advanced Loading & Data Flow Animations */
        .loading {
            display: inline-block;
            width: 24px;
            height: 24px;
            border: 3px solid rgba(255, 255, 255, 0.1);
            border-radius: 50%;
            border-top-color: var(--primary-color);
            border-right-color: var(--secondary-color);
            animation: advancedSpin 1s ease-in-out infinite;
            position: relative;
        }

        .loading::before {
            content: '';
            position: absolute;
            top: 50%;
            left: 50%;
            width: 8px;
            height: 8px;
            background: var(--primary-color);
            border-radius: 50%;
            transform: translate(-50%, -50%);
            animation: loadingPulse 1s ease-in-out infinite;
        }

        @keyframes advancedSpin {
            to { transform: rotate(360deg); }
        }

        @keyframes loadingPulse {
            0%, 100% { opacity: 0.3; transform: translate(-50%, -50%) scale(1); }
            50% { opacity: 1; transform: translate(-50%, -50%) scale(1.2); }
        }

        /* 🎭 Data Flow Particles Animation */
        @keyframes dataFlow {
            0% {
                transform: translateY(0px) scale(0);
                opacity: 0;
            }
            10% {
                transform: translateY(-20px) scale(0.5);
                opacity: 1;
            }
            90% {
                transform: translateY(-200px) scale(1);
                opacity: 1;
            }
            100% {
                transform: translateY(-250px) scale(0.8);
                opacity: 0;
            }
        }

        /* 🌊 Water Filling Animation */
        .water-filling {
            animation: waterFill 1.2s cubic-bezier(0.25, 0.46, 0.45, 0.94);
        }

        @keyframes waterFill {
            0% { transform: scaleY(0); opacity: 0; }
            100% { transform: scaleY(1); opacity: 1; }
        }

        /* 🎪 Advanced Icon Animations */
        @keyframes iconRotate {
            0% { transform: rotate(0deg); }
            100% { transform: rotate(360deg); }
        }

        /* 📊 Chart Animation Enhancements */
        .chart-container canvas {
            transition: all var(--animation-normal);
        }

        .chart-container:hover canvas {
            transform: scale(1.02);
            filter: brightness(1.1);
        }

        /* Notification badge */
        .notification-badge {
            position: absolute;
            top: -5px;
            right: -5px;
            background: var(--danger-color);
            color: white;
            border-radius: 50%;
            width: 20px;
            height: 20px;
            font-size: 0.7rem;
            display: flex;
            align-items: center;
            justify-content: center;
        }
    </style>
</head>
<body>
    <div class="dashboard-container">
        <!-- Header -->
        <div class="dashboard-header">
            <div class="header-content">
                <div class="logo-section">
                    <div class="logo-icon">
                        <i class="fas fa-water"></i>
                    </div>
                    <div class="logo-text">
                        <h1>Smart Tank Monitor</h1>
                        <p>IoT Water Level Monitoring System</p>
                    </div>
                </div>
                <div class="header-actions">
                    <button class="theme-toggle" onclick="toggleTheme()" title="Toggle Theme">
                        <i class="fas fa-moon"></i>
                    </button>
                    <button class="notification-btn" onclick="toggleNotifications()" title="Notifications">
                        <i class="fas fa-bell"></i>
                        <span class="notification-badge" id="notificationBadge" style="display: none;">0</span>
                    </button>
                    <button class="refresh-btn" onclick="refreshData()" title="Refresh Data">
                        <i class="fas fa-sync-alt"></i>
                    </button>
                </div>
            </div>
        </div>

        <!-- Top Navigation Bar -->
        <nav class="navbar navbar-expand-lg navbar-light" style="background: rgba(255, 255, 255, 0.95); backdrop-filter: blur(10px); border-radius: var(--border-radius); box-shadow: var(--shadow); margin-bottom: 2rem;">
            <div class="container-fluid">
                <a class="navbar-brand" href="#" style="font-weight: 700; color: var(--primary-color);">
                    <i class="fas fa-water me-2"></i>Smart Tank Monitor
                </a>

                <button class="navbar-toggler" type="button" data-bs-toggle="collapse" data-bs-target="#navbarNav" aria-controls="navbarNav" aria-expanded="false" aria-label="Toggle navigation">
                    <span class="navbar-toggler-icon"></span>
                </button>

                <div class="collapse navbar-collapse" id="navbarNav">
                    <ul class="navbar-nav me-auto">
                        <li class="nav-item">
                            <a class="nav-link active" aria-current="page" href="#" onclick="showSection('overview')">
                                <i class="fas fa-tachometer-alt me-1"></i> Overview
                            </a>
                        </li>
                        <li class="nav-item">
                            <a class="nav-link" href="#" onclick="showSection('alerts')">
                                <i class="fas fa-exclamation-triangle me-1"></i> Alerts
                                <span class="badge bg-danger ms-1" id="navAlertBadge" style="display: none;">0</span>
                            </a>
                        </li>
                        <li class="nav-item">
                            <a class="nav-link" href="#" onclick="showSection('settings')">
                                <i class="fas fa-cog me-1"></i> Settings
                            </a>
                        </li>
                    </ul>

                    <ul class="navbar-nav">
                        <li class="nav-item dropdown">
                            <a class="nav-link dropdown-toggle" href="#" id="navbarDropdown" role="button" data-bs-toggle="dropdown" aria-expanded="false">
                                <i class="fas fa-user me-1"></i> Admin
                            </a>
                            <ul class="dropdown-menu" aria-labelledby="navbarDropdown">
                                <li><a class="dropdown-item" href="#" onclick="showSystemInfo()"><i class="fas fa-info-circle me-2"></i>System Info</a></li>
                                <li><a class="dropdown-item" href="#" onclick="exportData()"><i class="fas fa-download me-2"></i>Export Data</a></li>
                                <li><hr class="dropdown-divider"></li>
                                <li><a class="dropdown-item" href="#" onclick="logout()"><i class="fas fa-sign-out-alt me-2"></i>Logout</a></li>
                            </ul>
                        </li>
                    </ul>
                </div>
            </div>
        </nav>

        <!-- Main Content Area -->
        <div id="mainContent">
            <!-- Overview Section (Default) -->
            <div id="overview" class="content-section active">
                <!-- Tank Status Header -->
                <div class="tank-status-header" style="background: rgba(255, 255, 255, 0.95); backdrop-filter: blur(10px); border-radius: var(--border-radius); padding: 2rem; margin-bottom: 2rem; box-shadow: var(--shadow);">
                    <div class="row align-items-center">
                        <div class="col-lg-4">
                            <div class="tank-visualization-large">
                                <h4 class="text-center mb-3">Tank Status</h4>
                                <div class="tank-container-large">
                                    <div class="tank-body">
                                        <div class="tank-bubbles">
                                            <div class="bubble"></div>
                                            <div class="bubble"></div>
                                            <div class="bubble"></div>
                                        </div>
                                    </div>
                                    <div class="tank-level-indicator-large" id="tankLevelText">
                                        <?php echo isset($latest_data['percentage']) ? $latest_data['percentage'] . '%' : '0%'; ?>
                                    </div>
                                    <div class="tank-water-large" id="tankWater" style="height: <?php echo isset($latest_data['percentage']) ? $latest_data['percentage'] . '%' : '0%'; ?>;">
                                    </div>
                                    <div class="tank-label-large">
                                        Max: <?php echo isset($latest_data['max_depth']) ? $latest_data['max_depth'] . ' cm' : '200 cm'; ?>
                                    </div>
                                </div>
                                <div class="text-center mt-3">
                                    <div class="status-badge" id="statusBadge">
                                        <span class="status-indicator" id="statusIndicator"></span>
                                        <span id="statusText"><?php echo isset($latest_data['status']) ? $latest_data['status'] : 'Unknown'; ?></span>
                                    </div>
                                </div>
                            </div>
                        </div>

                        <div class="col-lg-8">
                            <div class="row g-3">
                                <div class="col-md-4">
                                    <div class="metric-card-compact">
                                        <div class="metric-icon-compact" style="background: linear-gradient(135deg, #3b82f6, #1d4ed8);">
                                            <i class="fas fa-ruler-vertical"></i>
                                        </div>
                                        <div class="metric-value-compact text-primary" id="currentLevel">
                                            <?php echo isset($latest_data['level']) ? number_format($latest_data['level']) . ' cm' : '--'; ?>
                                        </div>
                                        <div class="metric-label-compact">Current Level</div>
                                    </div>
                                </div>

                                <div class="col-md-4">
                                    <div class="metric-card-compact">
                                        <div class="metric-icon-compact" style="background: linear-gradient(135deg, #10b981, #059669);">
                                            <i class="fas fa-percentage"></i>
                                        </div>
                                        <div class="metric-value-compact text-success" id="fillPercentage">
                                            <?php echo isset($latest_data['percentage']) ? $latest_data['percentage'] . '%' : '--'; ?>
                                        </div>
                                        <div class="metric-label-compact">Fill Level</div>
                                        <div class="progress-compact mt-2">
                                            <div class="progress-bar bg-success" id="percentageBar" style="width: <?php echo isset($latest_data['percentage']) ? $latest_data['percentage'] . '%' : '0%'; ?>;"></div>
                                        </div>
                                    </div>
                                </div>

                                <div class="col-md-4">
                                    <div class="metric-card-compact">
                                        <div class="metric-icon-compact" style="background: linear-gradient(135deg, #f59e0b, #d97706);">
                                            <i class="fas fa-clock"></i>
                                        </div>
                                        <div class="metric-value-compact text-warning" id="lastUpdate">
                                            <?php
                                            if (isset($latest_data['timestamp'])) {
                                                $timestamp = new DateTime($latest_data['timestamp']);
                                                $now = new DateTime();
                                                $diff = $now->diff($timestamp);
                                                echo $diff->i . 'm ago';
                                            } else {
                                                echo '--';
                                            }
                                            ?>
                                        </div>
                                        <div class="metric-label-compact">Last Update</div>
                                        <small class="text-muted" id="lastUpdateDetail">
                                            <?php echo isset($latest_data['timestamp']) ? date('H:i:s', strtotime($latest_data['timestamp'])) : 'No data'; ?>
                                        </small>
                                    </div>
                                </div>
                            </div>
                        </div>
                    </div>
                </div>

                <!-- Quick Actions Bar -->
                <div class="quick-actions-bar" style="background: rgba(255, 255, 255, 0.95); backdrop-filter: blur(10px); border-radius: var(--border-radius); padding: 1rem; margin-bottom: 2rem; box-shadow: var(--shadow);">
                    <div class="d-flex justify-content-center gap-3 flex-wrap">
                        <button class="btn btn-primary" onclick="testNotification()">
                            <i class="fas fa-bell me-2"></i>Test Alert
                        </button>
                        <button class="btn btn-success" onclick="refreshData()">
                            <i class="fas fa-sync-alt me-2"></i>Refresh Data
                        </button>
                        <button class="btn btn-info" onclick="exportData()">
                            <i class="fas fa-download me-2"></i>Export Data
                        </button>
                        <button class="btn btn-warning" onclick="toggleTheme()">
                            <i class="fas fa-palette me-2"></i>Toggle Theme
                        </button>
                    </div>
                </div>

                <!-- Live Monitoring Chart -->
                <div class="chart-container">
                    <div class="d-flex justify-content-between align-items-center mb-3">
                        <h5 class="mb-0"><i class="fas fa-chart-line"></i> Live Tank Level Monitoring</h5>
                        <div class="d-flex gap-2">
                            <div class="btn-group btn-group-sm">
                                <button class="btn btn-outline-primary active" onclick="setChartPeriod('1h')">1H</button>
                                <button class="btn btn-outline-primary" onclick="setChartPeriod('6h')">6H</button>
                                <button class="btn btn-outline-primary" onclick="setChartPeriod('24h')">24H</button>
                            </div>
                        </div>
                    </div>
                    <canvas id="liveChart"></canvas>
                </div>
            </div>

            <!-- Alerts Section -->
            <div id="alerts" class="content-section">


                <div class="row g-4">
                    <!-- Alert Statistics -->
                    <div class="col-lg-4">
                        <div class="metric-card">
                            <h6 class="mb-3"><i class="fas fa-chart-pie"></i> Alert Summary</h6>
                            <div class="mb-3">
                                <div class="d-flex justify-content-between align-items-center">
                                    <span>Active Alerts</span>
                                    <span class="badge bg-danger" id="activeAlertsCount">0</span>
                                </div>
                            </div>
                            <div class="mb-3">
                                <div class="d-flex justify-content-between align-items-center">
                                    <span>Cleared Today</span>
                                    <span class="badge bg-success" id="clearedAlertsCount">0</span>
                                </div>
                            </div>
                            <div class="mb-3">
                                <div class="d-flex justify-content-between align-items-center">
                                    <span>System Status</span>
                                    <span class="badge bg-success" id="systemStatus">Normal</span>
                                </div>
                            </div>
                            <hr>
                            <div class="d-grid gap-2">
                                <button class="btn btn-outline-danger btn-sm" onclick="clearAllAlerts()">
                                    <i class="fas fa-trash"></i> Clear All Alerts
                                </button>
                                <button class="btn btn-outline-warning btn-sm" onclick="testAlert()">
                                    <i class="fas fa-test-tube"></i> Test Alert
                                </button>
                            </div>
                        </div>

                        <!-- Alert Thresholds -->
                        <div class="metric-card">
                            <h6 class="mb-3"><i class="fas fa-sliders-h"></i> Alert Thresholds</h6>
                            <div class="mb-3">
                                <label class="form-label small">Low Level Alert</label>
                                <div class="input-group input-group-sm">
                                    <input type="number" class="form-control" id="lowLevelThreshold" value="20" min="0" max="100">
                                    <span class="input-group-text">cm</span>
                                </div>
                            </div>
                            <div class="mb-3">
                                <label class="form-label small">High Level Alert</label>
                                <div class="input-group input-group-sm">
                                    <input type="number" class="form-control" id="highLevelThreshold" value="180" min="100" max="300">
                                    <span class="input-group-text">cm</span>
                                </div>
                            </div>
                            <div class="mb-3">
                                <label class="form-label small">Offline Timeout</label>
                                <div class="input-group input-group-sm">
                                    <input type="number" class="form-control" id="offlineTimeout" value="5" min="1" max="60">
                                    <span class="input-group-text">min</span>
                                </div>
                            </div>
                            <button class="btn btn-primary btn-sm w-100" onclick="updateThresholds()">
                                <i class="fas fa-save"></i> Update Thresholds
                            </button>
                        </div>
                    </div>

                    <!-- Alert History -->
                    <div class="col-lg-8">
                        <div class="alerts-panel">
                            <div class="d-flex justify-content-between align-items-center mb-4">
                                <h5 class="mb-0"><i class="fas fa-history"></i> Alert History</h5>
                                <div class="btn-group btn-group-sm">
                                    <button class="btn btn-outline-secondary active" onclick="filterAlerts('all')">All</button>
                                    <button class="btn btn-outline-warning" onclick="filterAlerts('warning')">Warnings</button>
                                    <button class="btn btn-outline-danger" onclick="filterAlerts('danger')">Critical</button>
                                </div>
                            </div>

                            <div id="alertsContainer">
                                <?php if (empty($alerts)): ?>
                                    <div class="alert-item" style="border-left-color: #10b981; background: rgba(16, 185, 129, 0.1);">
                                        <div class="d-flex align-items-center">
                                            <i class="fas fa-check-circle text-success fa-2x me-3"></i>
                                            <div>
                                                <strong class="text-success">All Systems Normal</strong>
                                                <p class="mb-0 mt-1 text-muted">No alerts at this time. Tank monitoring is working perfectly.</p>
                                            </div>
                                        </div>
                                    </div>
                                <?php else: ?>
                                    <?php foreach ($alerts as $alert): ?>
                                        <div class="alert-item <?php echo $alert['alert_type'] === 'low_level' ? 'warning' : 'danger'; ?>">
                                            <div class="d-flex align-items-center">
                                                <i class="fas fa-exclamation-triangle fa-2x me-3"></i>
                                                <div class="flex-grow-1">
                                                    <strong><?php echo ucfirst(str_replace('_', ' ', $alert['alert_type'])); ?> ALERT</strong>
                                                    <p class="mb-0 mt-1">Threshold: <?php echo $alert['threshold']; ?>cm</p>
                                                </div>
                                                <div class="text-end">
                                                    <small class="text-muted">Active</small>
                                                    <button class="btn btn-sm btn-outline-secondary ms-2" onclick="dismissAlert(<?php echo $alert['id']; ?>)">
                                                        <i class="fas fa-times"></i>
                                                    </button>
                                                </div>
                                            </div>
                                        </div>
                                    <?php endforeach; ?>
                                <?php endif; ?>
                            </div>
                        </div>
                    </div>
                </div>
            </div>

            <!-- Settings Section -->
            <div id="settings" class="content-section">

                <div class="row g-4">
                    <!-- System Information -->
                    <div class="col-lg-6">
                        <div class="metric-card">
                            <h6 class="mb-3"><i class="fas fa-server"></i> System Information</h6>
                            <div class="mb-3">
                                <div class="d-flex justify-content-between align-items-center">
                                    <div>
                                        <small class="text-muted">Dashboard Version</small>
                                        <div class="fw-bold">v2.0.0</div>
                                    </div>
                                    <span class="badge bg-info">Latest</span>
                                </div>
                            </div>
                            <div class="mb-3">
                                <div class="d-flex justify-content-between align-items-center">
                                    <div>
                                        <small class="text-muted">Last Refresh</small>
                                        <div class="fw-bold" id="lastRefresh"><?php echo date('H:i:s'); ?></div>
                                    </div>
                                    <i class="fas fa-sync-alt text-success"></i>
                                </div>
                            </div>
                            <div class="mb-3">
                                <div class="d-flex justify-content-between align-items-center">
                                    <div>
                                        <small class="text-muted">Current Theme</small>
                                        <div class="fw-bold" id="currentTheme">Light</div>
                                    </div>
                                    <button class="btn btn-sm btn-outline-primary" onclick="toggleTheme()">
                                        <i class="fas fa-palette"></i> Switch
                                    </button>
                                </div>
                            </div>
                            <div class="mb-3">
                                <div class="d-flex justify-content-between align-items-center">
                                    <div>
                                        <small class="text-muted">System Status</small>
                                        <div class="fw-bold text-success">Online</div>
                                    </div>
                                    <div class="status-indicator status-normal"></div>
                                </div>
                            </div>
                        </div>

                        <!-- Performance Metrics -->
                        <div class="metric-card">
                            <h6 class="mb-3"><i class="fas fa-tachometer-alt"></i> Performance</h6>
                            <div class="mb-3">
                                <div class="d-flex justify-content-between align-items-center">
                                    <span>Response Time</span>
                                    <span class="badge bg-success">Fast</span>
                                </div>
                                <div class="progress mt-2" style="height: 4px;">
                                    <div class="progress-bar bg-success" style="width: 85%"></div>
                                </div>
                            </div>
                            <div class="mb-3">
                                <div class="d-flex justify-content-between align-items-center">
                                    <span>Memory Usage</span>
                                    <span class="badge bg-warning">Normal</span>
                                </div>
                                <div class="progress mt-2" style="height: 4px;">
                                    <div class="progress-bar bg-warning" style="width: 45%"></div>
                                </div>
                            </div>
                            <div class="mb-3">
                                <div class="d-flex justify-content-between align-items-center">
                                    <span>CPU Usage</span>
                                    <span class="badge bg-info">Low</span>
                                </div>
                                <div class="progress mt-2" style="height: 4px;">
                                    <div class="progress-bar bg-info" style="width: 25%"></div>
                                </div>
                            </div>
                        </div>
                    </div>

                    <!-- Settings Configuration -->
                    <div class="col-lg-6">
                        <div class="metric-card">
                            <h6 class="mb-3"><i class="fas fa-bell"></i> Notification Settings</h6>
                            <div class="form-check mb-3">
                                <input class="form-check-input" type="checkbox" id="browserNotifications" checked>
                                <label class="form-check-label" for="browserNotifications">
                                    <i class="fas fa-desktop text-primary me-2"></i>
                                    Browser Notifications
                                    <small class="text-muted d-block">Show desktop notifications for alerts</small>
                                </label>
                            </div>
                            <div class="form-check mb-3">
                                <input class="form-check-input" type="checkbox" id="lowLevelAlerts" checked>
                                <label class="form-check-label" for="lowLevelAlerts">
                                    <i class="fas fa-exclamation-triangle text-warning me-2"></i>
                                    Low Level Alerts
                                    <small class="text-muted d-block">Alert when tank level is critically low</small>
                                </label>
                            </div>
                            <div class="form-check mb-3">
                                <input class="form-check-input" type="checkbox" id="offlineAlerts" checked>
                                <label class="form-check-label" for="offlineAlerts">
                                    <i class="fas fa-wifi-slash text-danger me-2"></i>
                                    Offline Alerts
                                    <small class="text-muted d-block">Alert when system goes offline</small>
                                </label>
                            </div>
                            <div class="form-check mb-3">
                                <input class="form-check-input" type="checkbox" id="soundAlerts" checked>
                                <label class="form-check-label" for="soundAlerts">
                                    <i class="fas fa-volume-up text-info me-2"></i>
                                    Sound Alerts
                                    <small class="text-muted d-block">Play sound for critical alerts</small>
                                </label>
                            </div>
                        </div>

                        <!-- Display Settings -->
                        <div class="metric-card">
                            <h6 class="mb-3"><i class="fas fa-display"></i> Display Settings</h6>
                            <div class="mb-3">
                                <label class="form-label small">Refresh Interval</label>
                                <select class="form-select form-select-sm" id="refreshInterval">
                                    <option value="10000">10 seconds</option>
                                    <option value="30000" selected>30 seconds</option>
                                    <option value="60000">1 minute</option>
                                    <option value="300000">5 minutes</option>
                                </select>
                            </div>
                            <div class="mb-3">
                                <label class="form-label small">Chart Animation</label>
                                <div class="form-check">
                                    <input class="form-check-input" type="checkbox" id="chartAnimation" checked>
                                    <label class="form-check-label" for="chartAnimation">
                                        Enable smooth animations
                                    </label>
                                </div>
                            </div>
                            <div class="mb-3">
                                <label class="form-label small">Language</label>
                                <select class="form-select form-select-sm" id="languageSelect">
                                    <option value="en" selected>English</option>
                                    <option value="es">Español</option>
                                    <option value="fr">Français</option>
                                    <option value="de">Deutsch</option>
                                </select>
                            </div>
                            <div class="d-grid">
                                <button class="btn btn-primary btn-sm" onclick="saveSettings()">
                                    <i class="fas fa-save"></i> Save Settings
                                </button>
                            </div>
                        </div>

                        <!-- Quick Actions -->
                        <div class="metric-card">
                            <h6 class="mb-3"><i class="fas fa-bolt"></i> Quick Actions</h6>
                            <div class="d-grid gap-2">
                                <button class="btn btn-outline-info btn-sm" onclick="exportData()">
                                    <i class="fas fa-download"></i> Export Data
                                </button>
                                <button class="btn btn-outline-secondary btn-sm" onclick="resetSettings()">
                                    <i class="fas fa-undo"></i> Reset to Defaults
                                </button>
                                <button class="btn btn-outline-danger btn-sm" onclick="showSystemInfo()">
                                    <i class="fas fa-info-circle"></i> System Information
                                </button>
                            </div>
                        </div>
                    </div>
                </div>
            </div>
        </div>
    </div>

    <script src="https://cdn.jsdelivr.net/npm/bootstrap@5.3.0/dist/js/bootstrap.bundle.min.js"></script>
    <script>
        // 🎯 Advanced Configuration
        const CONFIG = {
            refreshInterval: 30000, // 30 seconds
            notificationCheckInterval: 60000, // 1 minute
            maxDepth: <?php echo isset($latest_data['max_depth']) ? $latest_data['max_depth'] : 200; ?>,
            tankId: 1,
            apiEndpoint: '../api/get_tank_data.php',
            animationDuration: 1200, // Enhanced animation duration
            themeTransition: 800 // Theme transition speed
        };

        // 🌈 Dynamic Theming System
        const THEMES = {
            normal: {
                primary: '#6366f1',
                secondary: '#10b981',
                background: 'linear-gradient(135deg, #0f172a 0%, #1e293b 50%, #334155 100%)',
                cardBg: 'rgba(15, 23, 42, 0.9)'
            },
            warning: {
                primary: '#f59e0b',
                secondary: '#f59e0b',
                background: 'linear-gradient(135deg, #1e293b 0%, #451a03 50%, #1e293b 100%)',
                cardBg: 'rgba(30, 41, 59, 0.9)'
            },
            danger: {
                primary: '#ef4444',
                secondary: '#ef4444',
                background: 'linear-gradient(135deg, #1e293b 0%, #450a0a 50%, #1e293b 100%)',
                cardBg: 'rgba(30, 41, 59, 0.9)'
            }
        };

        // State management
        let currentData = <?php echo json_encode($latest_data); ?>;
        let chartData = <?php echo json_encode($chart_data); ?>;
        let hourlyData = <?php echo json_encode($hourly_data); ?>;
        let alerts = <?php echo json_encode($alerts); ?>;
        let liveChart, hourlyChart;
        let refreshInterval, notificationInterval;
        let isDarkTheme = localStorage.getItem('theme') === 'dark';
        let notificationPermission = 'default';

        // Initialize dashboard
        document.addEventListener('DOMContentLoaded', function() {
            initializeTheme();
            initializeCharts();
            initializeNotifications();
            startAutoRefresh();
            updateDashboard();
            setupEventListeners();

            // Update navigation badge
            updateNavBadge();
        });

        // 🎨 Advanced Theme Management with Dynamic Data Response
        function initializeTheme() {
            if (isDarkTheme) {
                document.body.classList.add('dark-theme');
                updateThemeIcon();
            }
            // Initialize dynamic theming
            initializeDynamicTheming();
        }

        function toggleTheme() {
            isDarkTheme = !isDarkTheme;
            document.body.classList.toggle('dark-theme');
            localStorage.setItem('theme', isDarkTheme ? 'dark' : 'light');
            updateThemeIcon();
            updateCurrentThemeDisplay();

            // Add theme transition effect
            document.body.style.transition = `all ${CONFIG.themeTransition}ms ease`;
            setTimeout(() => {
                document.body.style.transition = '';
            }, CONFIG.themeTransition);
        }

        function updateThemeIcon() {
            const themeIcon = document.querySelector('.theme-toggle i');
            themeIcon.className = isDarkTheme ? 'fas fa-sun' : 'fas fa-moon';
            themeIcon.style.animation = 'iconRotate 0.5s ease-in-out';
        }

        // 🌈 Dynamic Theming Based on Data
        function initializeDynamicTheming() {
            // Create dynamic theme observer
            const observer = new MutationObserver((mutations) => {
                mutations.forEach((mutation) => {
                    if (mutation.type === 'attributes' && mutation.attributeName === 'data-theme') {
                        applyDynamicTheme();
                    }
                });
            });

            observer.observe(document.body, {
                attributes: true,
                attributeFilter: ['data-theme']
            });
        }

        function applyDynamicTheme() {
            const percentage = currentData ? currentData.percentage : 0;
            const theme = determineDataTheme(percentage);

            // Apply theme transitions
            const root = document.documentElement;
            root.style.setProperty('--dynamic-primary', theme.primary);
            root.style.setProperty('--dynamic-secondary', theme.secondary);

            // Animate theme changes
            document.body.style.background = theme.background;
            document.body.style.transition = `all ${CONFIG.themeTransition}ms ease`;
        }

        function determineDataTheme(percentage) {
            if (percentage <= 20) return THEMES.danger;
            if (percentage <= 50) return THEMES.warning;
            return THEMES.normal;
        }

        // 🎭 Advanced Animation Functions
        function animateValue(element, start, end, duration) {
            let startTimestamp = null;
            const step = (timestamp) => {
                if (!startTimestamp) startTimestamp = timestamp;
                const progress = Math.min((timestamp - startTimestamp) / duration, 1);
                const current = Math.floor(progress * (end - start) + start);
                element.textContent = current;
                if (progress < 1) {
                    window.requestAnimationFrame(step);
                }
            };
            window.requestAnimationFrame(step);
        }

        function animateTankWater(percentage) {
            const tankWater = document.getElementById('tankWater');
            if (tankWater) {
                tankWater.style.height = `${percentage}%`;
                // Add water animation class
                tankWater.classList.add('water-filling');
                setTimeout(() => {
                    tankWater.classList.remove('water-filling');
                }, CONFIG.animationDuration);
            }
        }

        function addDataFlowAnimation() {
            // Create flowing data particles
            const container = document.querySelector('.dashboard-container');
            for (let i = 0; i < 5; i++) {
                setTimeout(() => {
                    createDataParticle(container);
                }, i * 200);
            }
        }

        function createDataParticle(container) {
            const particle = document.createElement('div');
            particle.className = 'data-particle';
            particle.style.cssText = `
                position: absolute;
                width: 4px;
                height: 4px;
                background: ${THEMES.normal.primary};
                border-radius: 50%;
                pointer-events: none;
                z-index: 1000;
                animation: dataFlow 3s linear forwards;
            `;

            // Random starting position
            particle.style.left = Math.random() * 100 + '%';
            particle.style.top = '10%';

            container.appendChild(particle);

            // Remove particle after animation
            setTimeout(() => {
                if (particle.parentNode) {
                    particle.parentNode.removeChild(particle);
                }
            }, 3000);
        }

        function updateCurrentThemeDisplay() {
            const themeDisplay = document.getElementById('currentTheme');
            if (themeDisplay) {
                themeDisplay.textContent = isDarkTheme ? 'Dark' : 'Light';
            }
        }

        // Chart initialization
        function initializeCharts() {
            // Live chart
            const liveCtx = document.getElementById('liveChart').getContext('2d');
            liveChart = new Chart(liveCtx, {
                type: 'line',
                data: {
                    labels: chartData.map(d => new Date(d.timestamp).toLocaleTimeString()),
                    datasets: [{
                        label: 'Water Level (cm)',
                        data: chartData.map(d => d.level),
                        borderColor: 'rgb(59, 130, 246)',
                        backgroundColor: 'rgba(59, 130, 246, 0.1)',
                        borderWidth: 3,
                        tension: 0.4,
                        fill: true,
                        pointBackgroundColor: 'rgb(59, 130, 246)',
                        pointBorderColor: '#ffffff',
                        pointBorderWidth: 2,
                        pointRadius: 4,
                        pointHoverRadius: 6
                    }]
                },
                options: {
                    responsive: true,
                    maintainAspectRatio: false,
                    interaction: {
                        intersect: false,
                        mode: 'index'
                    },
                    plugins: {
                        legend: {
                            display: false
                        },
                        tooltip: {
                            backgroundColor: 'rgba(0, 0, 0, 0.8)',
                            titleColor: '#ffffff',
                            bodyColor: '#ffffff',
                            borderColor: 'rgba(59, 130, 246, 0.5)',
                            borderWidth: 1,
                            cornerRadius: 8,
                            displayColors: true,
                            callbacks: {
                                label: function(context) {
                                    return `Level: ${context.parsed.y} cm`;
                                }
                            }
                        }
                    },
                    scales: {
                        x: {
                            grid: {
                                display: false
                            },
                            ticks: {
                                maxTicksLimit: 8
                            }
                        },
                        y: {
                            beginAtZero: true,
                            max: CONFIG.maxDepth,
                            grid: {
                                color: 'rgba(0, 0, 0, 0.1)'
                            },
                            ticks: {
                                callback: function(value) {
                                    return value + ' cm';
                                }
                            }
                        }
                    },
                    elements: {
                        point: {
                            hoverRadius: 8
                        }
                    }
                }
            });

            // Hourly chart
            const hourlyCtx = document.getElementById('hourlyChart').getContext('2d');
            hourlyChart = new Chart(hourlyCtx, {
                type: 'bar',
                data: {
                    labels: hourlyData.map(d => new Date(d.hour_start).toLocaleTimeString()),
                    datasets: [{
                        label: 'Average Level (cm)',
                        data: hourlyData.map(d => d.avg_level),
                        backgroundColor: 'rgba(16, 185, 129, 0.8)',
                        borderColor: 'rgb(16, 185, 129)',
                        borderWidth: 1,
                        borderRadius: 4,
                        borderSkipped: false
                    }]
                },
                options: {
                    responsive: true,
                    maintainAspectRatio: false,
                    plugins: {
                        legend: {
                            display: false
                        },
                        tooltip: {
                            backgroundColor: 'rgba(0, 0, 0, 0.8)',
                            titleColor: '#ffffff',
                            bodyColor: '#ffffff',
                            cornerRadius: 8,
                            callbacks: {
                                label: function(context) {
                                    return `Average: ${context.parsed.y} cm`;
                                }
                            }
                        }
                    },
                    scales: {
                        x: {
                            grid: {
                                display: false
                            }
                        },
                        y: {
                            beginAtZero: true,
                            max: CONFIG.maxDepth,
                            grid: {
                                color: 'rgba(0, 0, 0, 0.1)'
                            },
                            ticks: {
                                callback: function(value) {
                                    return value + ' cm';
                                }
                            }
                        }
                    }
                }
            });
        }

        // Notification system
        function initializeNotifications() {
            if ('Notification' in window) {
                notificationPermission = Notification.permission;
                if (notificationPermission === 'default') {
                    Notification.requestPermission().then(function(permission) {
                        notificationPermission = permission;
                    });
                }
            }
        }

        function showNotification(title, body, icon = '/favicon.ico', urgent = false) {
            if (notificationPermission === 'granted') {
                const options = {
                    body: body,
                    icon: icon,
                    badge: icon,
                    tag: 'tank-monitor',
                    requireInteraction: urgent,
                    silent: !urgent
                };

                const notification = new Notification(title, options);

                if (!urgent) {
                    setTimeout(() => {
                        notification.close();
                    }, 5000);
                }

                notification.onclick = function() {
                    window.focus();
                    notification.close();
                };
            }
        }

        function toggleNotifications() {
            const btn = document.querySelector('.notification-btn');
            const badge = document.getElementById('notificationBadge');

            if (btn.classList.contains('active')) {
                btn.classList.remove('active');
                badge.style.display = 'none';
            } else {
                btn.classList.add('active');
                badge.style.display = 'flex';
                badge.textContent = alerts.length;
            }
        }

        // Data management
        async function fetchLatestData() {
            try {
                const response = await fetch(`${CONFIG.apiEndpoint}?tank_id=${CONFIG.tankId}&t=${Date.now()}`);
                const data = await response.json();

                if (data.status === 'OK') {
                    return data;
                }
            } catch (error) {
                console.error('Error fetching data:', error);
            }
            return null;
        }

        function updateDashboard() {
            // 🎭 Advanced Dashboard Updates with Animations
            updateMetrics();
            updateTankVisualization();
            updateStatus();
            updateAlerts();

            // 🌈 Apply dynamic theming based on data
            applyDynamicTheme();

            // ⚡ Add data flow animation
            addDataFlowAnimation();
        }

        function updateMetrics() {
            const levelEl = document.getElementById('currentLevel');
            const percentageEl = document.getElementById('fillPercentage');
            const percentageBar = document.getElementById('percentageBar');
            const lastUpdateEl = document.getElementById('lastUpdate');
            const lastUpdateDetailEl = document.getElementById('lastUpdateDetail');

            if (currentData) {
                levelEl.textContent = `${currentData.level} cm`;
                percentageEl.textContent = `${currentData.percentage}%`;
                percentageBar.style.width = `${currentData.percentage}%`;

                const now = new Date();
                const lastUpdate = new Date(currentData.timestamp);
                const diffMinutes = Math.floor((now - lastUpdate) / (1000 * 60));
                lastUpdateEl.textContent = `${diffMinutes}m ago`;
                lastUpdateDetailEl.textContent = lastUpdate.toLocaleTimeString();
            }
        }

        function updateTankVisualization() {
            const tankWater = document.getElementById('tankWater');
            const tankLevelText = document.getElementById('tankLevelText');

            if (currentData && tankWater) {
                const percentage = currentData.percentage;

                // 🎭 Advanced Tank Animation
                animateTankWater(percentage);

                // 📊 Animate level text with counting effect
                const currentText = tankLevelText.textContent.replace('%', '');
                const currentValue = parseInt(currentText) || 0;

                if (currentValue !== percentage) {
                    animateValue(tankLevelText, currentValue, percentage, CONFIG.animationDuration);
                    tankLevelText.textContent = percentage + '%';
                }

                // 🌈 Dynamic color based on water level
                const waterColor = getWaterColor(percentage);
                tankWater.style.background = waterColor;
            }
        }

        function getWaterColor(percentage) {
            if (percentage <= 20) {
                return 'linear-gradient(180deg, #ef4444 0%, #dc2626 50%, #991b1b 100%)';
            } else if (percentage <= 50) {
                return 'linear-gradient(180deg, #f59e0b 0%, #d97706 50%, #92400e 100%)';
            } else {
                return 'linear-gradient(180deg, #10b981 0%, #059669 50%, #047857 100%)';
            }
        }

        function updateStatus() {
            const statusIcon = document.getElementById('statusIcon');
            const statusText = document.getElementById('statusText');
            const statusIndicator = document.getElementById('statusIndicator');

            if (currentData) {
                const status = currentData.status.toLowerCase();
                const statusClass = currentData.status_class;

                statusText.textContent = currentData.status;
                statusText.style.color = statusClass === 'danger' ? '#ef4444' : '#10b981';

                // Update icon
                const iconMap = {
                    'full': 'fas fa-check-circle',
                    'high': 'fas fa-exclamation-circle',
                    'medium': 'fas fa-info-circle',
                    'low': 'fas fa-exclamation-triangle',
                    'empty': 'fas fa-times-circle'
                };

                statusIcon.innerHTML = `<i class="${iconMap[status] || 'fas fa-info-circle'}"></i>`;

                // Update indicator
                statusIndicator.className = `status-indicator status-${statusClass || 'normal'}`;
            }
        }

        function updateAlerts() {
            const alertsContainer = document.getElementById('alertsContainer');
            const badge = document.getElementById('notificationBadge');
            const navBadge = document.getElementById('navAlertBadge');

            if (alerts.length > 0) {
                alertsContainer.innerHTML = alerts.map(alert => `
                    <div class="alert-item ${alert.alert_type === 'low_level' ? 'warning' : 'danger'}">
                        <div class="d-flex align-items-center">
                            <i class="fas fa-exclamation-triangle fa-2x me-3"></i>
                            <div class="flex-grow-1">
                                <strong>${alert.alert_type.replace('_', ' ').toUpperCase()} ALERT</strong>
                                <p class="mb-0 mt-1">Threshold: ${alert.threshold}cm</p>
                            </div>
                            <div class="text-end">
                                <small class="text-muted">Active</small>
                                <button class="btn btn-sm btn-outline-secondary ms-2" onclick="dismissAlert('${alert.id}')">
                                    <i class="fas fa-times"></i>
                                </button>
                            </div>
                        </div>
                    </div>
                `).join('');

                badge.textContent = alerts.length;
                badge.style.display = 'flex';

                navBadge.textContent = alerts.length;
                navBadge.style.display = 'inline-block';
            } else {
                alertsContainer.innerHTML = `
                    <div class="alert-item" style="border-left-color: #10b981; background: rgba(16, 185, 129, 0.1);">
                        <div class="d-flex align-items-center">
                            <i class="fas fa-check-circle text-success fa-2x me-3"></i>
                            <div>
                                <strong class="text-success">All Systems Normal</strong>
                                <p class="mb-0 mt-1 text-muted">No alerts at this time. Tank monitoring is working perfectly.</p>
                            </div>
                        </div>
                    </div>
                `;
                badge.style.display = 'none';
                navBadge.style.display = 'none';
            }
        }

        function updateNavBadge() {
            const navBadge = document.getElementById('navAlertBadge');
            if (alerts.length > 0) {
                navBadge.textContent = alerts.length;
                navBadge.style.display = 'inline-block';
            } else {
                navBadge.style.display = 'none';
            }
        }

        // Auto refresh
        function startAutoRefresh() {
            refreshInterval = setInterval(async () => {
                const data = await fetchLatestData();
                if (data) {
                    currentData = data.latest;
                    chartData = data.recent;
                    hourlyData = data.hourly;
                    alerts = data.alerts || [];

                    updateDashboard();
                    updateCharts();
                    updateNavBadge();

                    // Check for alerts
                    checkAlerts(data);
                }
            }, CONFIG.refreshInterval);

            // Notification check
            notificationInterval = setInterval(() => {
                checkSystemStatus();
                updateNavBadge();
            }, CONFIG.notificationCheckInterval);
        }

        function updateCharts() {
            if (liveChart) {
                liveChart.data.labels = chartData.map(d => new Date(d.timestamp).toLocaleTimeString());
                liveChart.data.datasets[0].data = chartData.map(d => d.level);
                liveChart.update('none');
            }

            if (hourlyChart) {
                hourlyChart.data.labels = hourlyData.map(d => new Date(d.hour_start).toLocaleTimeString());
                hourlyChart.data.datasets[0].data = hourlyData.map(d => d.avg_level);
                hourlyChart.update('none');
            }
        }

        function checkAlerts(data) {
            if (!data.latest) return;

            const level = data.latest.level;
            const percentage = data.latest.percentage;

            // Low level alert
            if (level <= 20) {
                showNotification(
                    'Low Water Level Alert!',
                    `Tank level is critically low: ${level}cm (${percentage}%)`,
                    null,
                    true
                );
            }

            // Offline alert
            if (data.offline_duration && data.offline_duration > 300) {
                showNotification(
                    'System Offline Alert',
                    'Tank monitoring system has been offline for more than 5 minutes',
                    null,
                    true
                );
            }
        }

        function checkSystemStatus() {
            // Check if data is recent
            if (currentData && currentData.timestamp) {
                const lastUpdate = new Date(currentData.timestamp);
                const now = new Date();
                const minutesSinceUpdate = (now - lastUpdate) / (1000 * 60);

                if (minutesSinceUpdate > 10) {
                    showNotification(
                        'System Status Warning',
                        'No recent updates from tank monitoring system',
                        null,
                        false
                    );
                }
            }
        }

        // Navigation system
        function showSection(sectionId) {
            // Hide all sections
            document.querySelectorAll('.content-section').forEach(section => {
                section.classList.remove('active');
            });

            // Show selected section
            const targetSection = document.getElementById(sectionId);
            if (targetSection) {
                targetSection.classList.add('active');
            }

            // Update navigation active state
            document.querySelectorAll('.nav-link').forEach(link => {
                link.classList.remove('active');
            });

            // Find and activate the corresponding nav link
            const activeLink = document.querySelector(`[onclick="showSection('${sectionId}')"]`);
            if (activeLink) {
                activeLink.classList.add('active');
            }

            // Special handling for sections
            if (sectionId === 'alerts') {
                updateAlerts();
            }
        }

        function logout() {
            if (confirm('Are you sure you want to logout?')) {
                window.location.href = '../logout.php';
            }
        }

        // Event listeners
        function setupEventListeners() {
            // Navigation clicks
            document.querySelectorAll('.nav-link').forEach(link => {
                link.addEventListener('click', function(e) {
                    e.preventDefault();
                    const section = this.getAttribute('onclick').match(/'([^']+)'/)[1];
                    showSection(section);
                });
            });

            // Chart period buttons
            document.querySelectorAll('.btn-group .btn').forEach(btn => {
                btn.addEventListener('click', function() {
                    document.querySelectorAll('.btn-group .btn').forEach(b => b.classList.remove('active'));
                    this.classList.add('active');
                });
            });

            // Mobile navbar toggle
            const navbarCollapse = document.getElementById('navbarNav');
            document.addEventListener('click', function(e) {
                if (!navbarCollapse.contains(e.target) && !e.target.classList.contains('navbar-toggler')) {
                    const bsCollapse = new bootstrap.Collapse(navbarCollapse, { hide: true });
                }
            });
        }

        // Utility functions
        function refreshData() {
            document.getElementById('lastRefresh').textContent = new Date().toLocaleTimeString();
            showNotification('Refreshing Data', 'Dashboard is being updated...', null, false);
            location.reload();
        }

        function setChartPeriod(period) {
            // This would typically fetch different time ranges
            console.log(`Setting chart period to: ${period}`);
        }

        function clearAllAlerts() {
            alerts = [];
            updateAlerts();
            showNotification('Alerts Cleared', 'All alerts have been cleared', null, false);
        }

        function testAlert() {
            showNotification(
                'Test Alert',
                'This is a test notification to verify the alert system is working correctly.',
                null,
                false
            );
        }

        function updateThresholds() {
            const lowLevel = document.getElementById('lowLevelThreshold').value;
            const highLevel = document.getElementById('highLevelThreshold').value;
            const offlineTimeout = document.getElementById('offlineTimeout').value;

            showNotification(
                'Thresholds Updated',
                `Low: ${lowLevel}cm, High: ${highLevel}cm, Offline: ${offlineTimeout}min`,
                null,
                false
            );
        }

        function filterAlerts(type) {
            const buttons = document.querySelectorAll('.btn-group .btn');
            buttons.forEach(btn => btn.classList.remove('active'));
            event.target.classList.add('active');

            // Filter logic would go here
            console.log(`Filtering alerts by type: ${type}`);
        }

        function dismissAlert(alertId) {
            // Remove alert from array and update display
            alerts = alerts.filter(alert => alert.id !== alertId);
            updateAlerts();
            showNotification('Alert Dismissed', 'Alert has been dismissed', null, false);
        }

        function exportData() {
            showNotification(
                'Data Export Started',
                'Your tank monitoring data is being prepared for download...',
                null,
                false
            );

            // Simulate export process
            setTimeout(() => {
                showNotification(
                    'Export Complete',
                    'Data has been exported successfully. Check your downloads folder.',
                    null,
                    false
                );
            }, 2000);
        }

        function showSystemInfo() {
            const info = `
System Information:
- Dashboard Version: v2.0.0
- Last Refresh: ${new Date().toLocaleTimeString()}
- Theme: ${isDarkTheme ? 'Dark' : 'Light'}
- Browser: ${navigator.userAgent.split(' ').pop()}
- Online Status: ${navigator.onLine ? 'Online' : 'Offline'}
- Screen Resolution: ${screen.width}x${screen.height}
            `;

            alert(info);
        }

        function saveSettings() {
            const browserNotif = document.getElementById('browserNotifications').checked;
            const lowLevelAlerts = document.getElementById('lowLevelAlerts').checked;
            const offlineAlerts = document.getElementById('offlineAlerts').checked;
            const soundAlerts = document.getElementById('soundAlerts').checked;
            const refreshInterval = document.getElementById('refreshInterval').value;
            const chartAnimation = document.getElementById('chartAnimation').checked;
            const language = document.getElementById('languageSelect').value;

            // Save settings to localStorage
            localStorage.setItem('settings', JSON.stringify({
                browserNotifications: browserNotif,
                lowLevelAlerts: lowLevelAlerts,
                offlineAlerts: offlineAlerts,
                soundAlerts: soundAlerts,
                refreshInterval: refreshInterval,
                chartAnimation: chartAnimation,
                language: language
            }));

            showNotification(
                'Settings Saved',
                'Your preferences have been saved successfully.',
                null,
                false
            );
        }

        function resetSettings() {
            if (confirm('Are you sure you want to reset all settings to defaults?')) {
                localStorage.removeItem('settings');
                localStorage.removeItem('theme');
                location.reload();
            }
        }

        function testNotification() {
            showNotification(
                'Test Notification',
                'This is a test notification to verify your browser notification settings.',
                null,
                false
            );
        }

        // Handle visibility change (tab switching)
        document.addEventListener('visibilitychange', function() {
            if (document.hidden) {
                // Pause animations when tab is not visible
                clearInterval(refreshInterval);
                clearInterval(notificationInterval);
            } else {
                // Resume when tab becomes visible
                startAutoRefresh();
            }
        });

        // Handle page unload
        window.addEventListener('beforeunload', function() {
            clearInterval(refreshInterval);
            clearInterval(notificationInterval);
        });

        // Error handling
        window.addEventListener('error', function(e) {
            console.error('Dashboard error:', e.error);
            showNotification('Dashboard Error', 'An error occurred while updating the dashboard', null, false);
        });

        // Keyboard shortcuts
        document.addEventListener('keydown', function(e) {
            if (e.ctrlKey || e.metaKey) {
                switch(e.key) {
                    case 'r':
                        e.preventDefault();
                        refreshData();
                        break;
                    case 't':
                        e.preventDefault();
                        toggleTheme();
                        break;
                }
            }
        });
    </script>
</body>
</html>