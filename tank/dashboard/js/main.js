// 🚀 MAIN APPLICATION - Orchestrates all modules

let refreshInterval;
let lastUpdateTime = null;

// 🔄 Refresh all data
async function refreshData() {
    const { log, logError } = window.APP_CONFIG;
    
    try {
        log('Refreshing data...');
        
        // Animate refresh button
        window.UI.animateRefreshButton();
        
        // Fetch latest data
        const data = await window.API.getLatestData();
        
        if (data && data.latest) {
            // Update tank visualization
            window.UI.updateTankLevel(data.latest.percentage);
            
            // Update metric cards
            window.UI.updateMetrics(data);
            
            // Update chart
            window.ChartManager.update(data);
            
            // Update connection status
            window.UI.updateConnectionStatus(true);
            
            // Store last update time
            lastUpdateTime = new Date();
            
            log('Data refresh successful');
        } else {
            throw new Error('No data received from API');
        }
        
    } catch (error) {
        logError('Data refresh failed:', error);
        window.UI.updateConnectionStatus(false);
        window.Notifications.error('Connection Error', 'Failed to fetch tank data');
    }
}

// 📊 Initialize chart period buttons
function initChartPeriodButtons() {
    const { log } = window.APP_CONFIG;
    
    document.querySelectorAll('.chart-btn').forEach(btn => {
        btn.addEventListener('click', function() {
            // Remove active class from all buttons
            document.querySelectorAll('.chart-btn').forEach(b => b.classList.remove('active'));
            
            // Add active class to clicked button
            this.classList.add('active');
            
            // Get period from data attribute
            const period = this.dataset.period;
            
            // Change chart period
            window.ChartManager.changePeriod(period);
            
            // Refresh data to update chart
            refreshData();
            
            log(`Chart period changed to: ${period}`);
        });
    });
}

// 🌓 Initialize theme toggle
function initThemeToggle() {
    const { log } = window.APP_CONFIG;
    
    const themeBtn = document.getElementById('themeToggle');
    if (themeBtn) {
        themeBtn.addEventListener('click', () => {
            window.Theme.toggle();
        });
        log('Theme toggle initialized');
    }
}

// 🔄 Initialize refresh button
function initRefreshButton() {
    const { log } = window.APP_CONFIG;
    
    const refreshBtn = document.getElementById('refreshBtn');
    if (refreshBtn) {
        refreshBtn.addEventListener('click', () => {
            refreshData();
        });
        log('Refresh button initialized');
    }
}

// ⏰ Update relative time display
function updateRelativeTime() {
    if (lastUpdateTime) {
        const now = new Date();
        const diffMs = now - lastUpdateTime;
        const diffSecs = Math.floor(diffMs / 1000);
        
        const timeEl = document.getElementById('lastUpdateTime');
        if (timeEl) {
            if (diffSecs < 60) {
                timeEl.textContent = 'Just now';
            } else {
                const diffMins = Math.floor(diffSecs / 60);
                timeEl.textContent = diffMins === 1 ? '1 min ago' : `${diffMins} mins ago`;
            }
        }
    }
}

// 🏁 Initialize application
async function initializeApp() {
    const { log, REFRESH_INTERVAL } = window.APP_CONFIG;
    
    try {
        log('Initializing Tank Monitoring Dashboard...');
        
        // Initialize modules
        window.Notifications.init();
        log('✓ Notifications initialized');
        
        window.Theme.init();
        log('✓ Theme initialized');
        
        // Initialize 3D Tank
        if (window.Tank3D) {
            window.Tank3D.init();
            log('✓ 3D Tank initialized with effects & critical alert');
        }
        
        window.ChartManager.init();
        log('✓ Chart initialized');
        
        // Initialize event listeners
        initChartPeriodButtons();
        initThemeToggle();
        initRefreshButton();
        log('✓ Event listeners initialized');
        
        // Show loading state
        window.UI.showLoading();
        
        // Initial data load
        await refreshData();
        
        // Hide loading state
        window.UI.hideLoading();
        
        // Start auto-refresh
        refreshInterval = setInterval(refreshData, REFRESH_INTERVAL);
        log(`✓ Auto-refresh started (${REFRESH_INTERVAL / 1000}s interval)`);
        
        // Update relative time every 30 seconds
        setInterval(updateRelativeTime, 30000);
        
        // Show success notification
        window.Notifications.success(
            'Dashboard Ready', 
            'Connected and monitoring tank data', 
            3000
        );
        
        log('✅ Dashboard initialized successfully!');
        
    } catch (error) {
        window.APP_CONFIG.logError('Initialization failed:', error);
        window.UI.hideLoading();
        window.Notifications.error(
            'Initialization Failed', 
            'Please refresh the page or check your connection'
        );
    }
}

// 🛑 Cleanup on page unload
window.addEventListener('beforeunload', () => {
    if (refreshInterval) {
        clearInterval(refreshInterval);
    }
});

// 🚀 Start app when DOM is ready
if (document.readyState === 'loading') {
    document.addEventListener('DOMContentLoaded', initializeApp);
} else {
    // DOM already loaded
    initializeApp();
}

// Export for debugging
window.App = {
    refreshData,
    refreshInterval,
    lastUpdateTime
};
