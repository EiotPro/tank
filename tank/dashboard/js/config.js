// ⚙️ CONFIGURATION - Environment Settings
// 🔄 LOCAL/PRODUCTION: Change 'ENV' to switch between environments

// 🔄 CHANGE THIS TO SWITCH ENVIRONMENTS
const ENV = 'LOCAL'; // Options: 'LOCAL' or 'PRODUCTION'

// Environment-specific configurations
const CONFIG = {
    LOCAL: {
        // 🏠 Local development settings (XAMPP)
        API_BASE_URL: '../api',
        REFRESH_INTERVAL: 5000,      // 5 seconds (for testing)
        CHART_UPDATE_INTERVAL: 10000, // 10 seconds
        DEBUG: true,                  // Enable console logs
        ENABLE_NOTIFICATIONS: true,   // Browser notifications
        MAX_RETRY_ATTEMPTS: 3,
        RETRY_DELAY: 2000
    },
    
    PRODUCTION: {
        // 🌐 Production settings (iotlogic.in)
        API_BASE_URL: 'https://iotlogic.in/tank/api',
        REFRESH_INTERVAL: 30000,      // 30 seconds
        CHART_UPDATE_INTERVAL: 60000,  // 1 minute
        DEBUG: false,                  // Disable console logs
        ENABLE_NOTIFICATIONS: true,    // Browser notifications
        MAX_RETRY_ATTEMPTS: 5,
        RETRY_DELAY: 3000
    }
};

// Export current environment configuration
const CURRENT_CONFIG = CONFIG[ENV];

// API Endpoints
const API_ENDPOINTS = {
    GET_LATEST_DATA: `${CURRENT_CONFIG.API_BASE_URL}/get_tank_data.php`,
    UPDATE_TANK_DATA: `${CURRENT_CONFIG.API_BASE_URL}/tank_update.php`,
    CHECK_STATUS: `${CURRENT_CONFIG.API_BASE_URL}/check_endpoint.php`
};

// Chart Configuration
const CHART_CONFIG = {
    PERIODS: {
        '1h': {
            label: '1 Hour',
            hours: 1,
            pointRadius: 3
        },
        '6h': {
            label: '6 Hours',
            hours: 6,
            pointRadius: 2
        },
        '24h': {
            label: '24 Hours',
            hours: 24,
            pointRadius: 1
        }
    },
    DEFAULT_PERIOD: '24h'
};

// Tank Configuration
const TANK_CONFIG = {
    MAX_DEPTH: 200,        // cm
    MIN_DEPTH: 0,          // cm
    WARNING_LEVEL: 20,     // cm (Low level warning)
    CRITICAL_LEVEL: 10,    // cm (Critical level alert)
    HIGH_LEVEL: 180        // cm (High level warning)
};

// Status Configuration
const STATUS_CONFIG = {
    FULL: { min: 80, color: '#10B981', label: 'Full' },
    HIGH: { min: 50, color: '#06B6D4', label: 'High' },
    MEDIUM: { min: 20, color: '#F59E0B', label: 'Medium' },
    LOW: { min: 0, color: '#EF4444', label: 'Low' }
};

// Debug logger
const log = (...args) => {
    if (CURRENT_CONFIG.DEBUG) {
        console.log('[Tank Monitor]', ...args);
    }
};

const logError = (...args) => {
    if (CURRENT_CONFIG.DEBUG) {
        console.error('[Tank Monitor Error]', ...args);
    }
};

// Export configuration
window.APP_CONFIG = {
    ENV,
    ...CURRENT_CONFIG,
    API_ENDPOINTS,
    CHART_CONFIG,
    TANK_CONFIG,
    STATUS_CONFIG,
    log,
    logError
};

// Log current environment on load
if (CURRENT_CONFIG.DEBUG) {
    console.log('%c🚀 Tank Monitoring Dashboard', 'color: #4F46E5; font-size: 16px; font-weight: bold');
    console.log('%c📍 Environment: ' + ENV, 'color: #10B981; font-weight: bold');
    console.log('%c🔗 API Base URL: ' + CURRENT_CONFIG.API_BASE_URL, 'color: #06B6D4');
    console.log('%c⏱️  Refresh Interval: ' + (CURRENT_CONFIG.REFRESH_INTERVAL / 1000) + 's', 'color: #F59E0B');
}
