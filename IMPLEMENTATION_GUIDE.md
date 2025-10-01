# 🚀 TANK DASHBOARD IMPLEMENTATION GUIDE

## ✅ Files Created So Far

### CSS Files (All Complete)
- ✅ `tank/dashboard/css/variables.css` - Color scheme & design tokens
- ✅ `tank/dashboard/css/base.css` - Base styles & utilities
- ✅ `tank/dashboard/css/components.css` - Reusable components
- ✅ `tank/dashboard/css/dashboard.css` - Dashboard-specific styles
- ✅ `tank/dashboard/css/animations.css` - All animations

### JavaScript Files (Partially Complete)
- ✅ `tank/dashboard/js/config.js` - **MOST IMPORTANT** (LOCAL/PRODUCTION switch)
- ✅ `tank/dashboard/js/api.js` - API interactions
- ✅ `tank/dashboard/js/ui.js` - DOM manipulation
- ⏳ `tank/dashboard/js/chartManager.js` - NEEDED
- ⏳ `tank/dashboard/js/theme.js` - NEEDED
- ⏳ `tank/dashboard/js/notifications.js` - NEEDED
- ⏳ `tank/dashboard/js/main.js` - NEEDED

### PHP Components (Not Yet Created)
- ⏳ `tank/dashboard/components/header.php`
- ⏳ `tank/dashboard/components/tank-viz.php`
- ⏳ `tank/dashboard/components/metrics.php`
- ⏳ `tank/dashboard/components/chart-section.php`
- ⏳ `tank/dashboard/components/footer.php`
- ⏳ New `tank/dashboard/index.php`

---

## 📝 REMAINING FILES TO CREATE

### 1. chartManager.js (Priority: HIGH)
```javascript
// 📊 CHART MANAGER - Create at: tank/dashboard/js/chartManager.js

const ChartManager = {
    chart: null,
    currentPeriod: '24h',

    init() {
        const ctx = document.getElementById('tankChart');
        if (!ctx) return;

        this.chart = new Chart(ctx, {
            type: 'line',
            data: {
                labels: [],
                datasets: [{
                    label: 'Water Level (%)',
                    data: [],
                    borderColor: '#4F46E5',
                    backgroundColor: 'rgba(79, 70, 229, 0.1)',
                    tension: 0.4,
                    fill: true
                }]
            },
            options: {
                responsive: true,
                maintainAspectRatio: false,
                plugins: {
                    legend: { display: false },
                    tooltip: {
                        backgroundColor: 'rgba(0, 0, 0, 0.8)',
                        titleColor: '#fff',
                        bodyColor: '#fff',
                        padding: 12,
                        displayColors: false
                    }
                },
                scales: {
                    y: {
                        beginAtZero: true,
                        max: 100,
                        grid: { color: 'rgba(0, 0, 0, 0.05)' }
                    },
                    x: {
                        grid: { display: false }
                    }
                }
            }
        });
    },

    update(data) {
        if (!this.chart || !data || !data.recent) return;

        const filtered = window.API.getFilteredChartData(data.recent, this.currentPeriod);
        
        this.chart.data.labels = filtered.map(item => {
            const date = new Date(item.timestamp);
            return date.toLocaleTimeString('en-US', { hour: '2-digit', minute: '2-digit' });
        });

        this.chart.data.datasets[0].data = filtered.map(item => item.percentage);
        this.chart.update('none');
    },

    changePeriod(period) {
        this.currentPeriod = period;
        // Re-fetch and update will be called by main.js
    }
};

window.ChartManager = ChartManager;
```

### 2. theme.js (Priority: MEDIUM)
```javascript
// 🌓 THEME MANAGER - Create at: tank/dashboard/js/theme.js

const Theme = {
    init() {
        const savedTheme = localStorage.getItem('dashboard-theme') || 'light';
        this.setTheme(savedTheme, false);
    },

    toggle() {
        const current = document.documentElement.getAttribute('data-theme') || 'light';
        const newTheme = current === 'light' ? 'dark' : 'light';
        this.setTheme(newTheme, true);
    },

    setTheme(theme, save = true) {
        document.documentElement.setAttribute('data-theme', theme);
        
        if (save) {
            localStorage.setItem('dashboard-theme', theme);
        }

        // Update theme toggle icon
        const icon = document.querySelector('#themeToggle i');
        if (icon) {
            icon.className = theme === 'dark' ? 'fas fa-sun' : 'fas fa-moon';
        }

        window.APP_CONFIG.log(`Theme changed to: ${theme}`);
    }
};

window.Theme = Theme;
```

### 3. notifications.js (Priority: HIGH)
```javascript
// 🔔 NOTIFICATIONS - Create at: tank/dashboard/js/notifications.js

const Notifications = {
    container: null,

    init() {
        this.container = document.createElement('div');
        this.container.className = 'toast-container';
        document.body.appendChild(this.container);
    },

    show(type, title, message, duration = 5000) {
        const toast = document.createElement('div');
        toast.className = `toast toast-${type}`;
        
        const icons = {
            success: 'fa-check-circle',
            error: 'fa-exclamation-circle',
            warning: 'fa-exclamation-triangle',
            info: 'fa-info-circle'
        };

        toast.innerHTML = `
            <div class="toast-icon">
                <i class="fas ${icons[type] || icons.info}"></i>
            </div>
            <div class="toast-content">
                <div class="toast-title">${title}</div>
                <div class="toast-message">${message}</div>
            </div>
            <button class="toast-close" onclick="this.parentElement.remove()">
                <i class="fas fa-times"></i>
            </button>
        `;

        this.container.appendChild(toast);

        setTimeout(() => {
            toast.classList.add('toast-hide');
            setTimeout(() => toast.remove(), 300);
        }, duration);
    }
};

window.Notifications = Notifications;
```

### 4. main.js (Priority: CRITICAL)
```javascript
// 🚀 MAIN APP - Create at: tank/dashboard/js/main.js

let refreshInterval;

async function refreshData() {
    try {
        window.UI.animateRefreshButton();
        
        const data = await window.API.getLatestData();
        
        if (data && data.latest) {
            // Update UI
            window.UI.updateTankLevel(data.latest.percentage);
            window.UI.updateMetrics(data);
            
            // Update chart
            window.ChartManager.update(data);
            
            // Update connection status
            window.UI.updateConnectionStatus(true);
        }
    } catch (error) {
        window.APP_CONFIG.logError('Refresh failed:', error);
        window.UI.updateConnectionStatus(false);
        window.Notifications.show('error', 'Connection Error', 'Failed to fetch data');
    }
}

function initChartPeriodButtons() {
    document.querySelectorAll('.chart-btn').forEach(btn => {
        btn.addEventListener('click', function() {
            document.querySelectorAll('.chart-btn').forEach(b => b.classList.remove('active'));
            this.classList.add('active');
            
            const period = this.dataset.period;
            window.ChartManager.changePeriod(period);
            refreshData();
        });
    });
}

function initThemeToggle() {
    const themeBtn = document.getElementById('themeToggle');
    if (themeBtn) {
        themeBtn.addEventListener('click', () => window.Theme.toggle());
    }
}

function initRefreshButton() {
    const refreshBtn = document.getElementById('refreshBtn');
    if (refreshBtn) {
        refreshBtn.addEventListener('click', refreshData);
    }
}

// Initialize on DOM load
document.addEventListener('DOMContentLoaded', async () => {
    // Initialize modules
    window.Notifications.init();
    window.Theme.init();
    window.ChartManager.init();
    
    // Initialize event listeners
    initChartPeriodButtons();
    initThemeToggle();
    initRefreshButton();
    
    // Initial data load
    await refreshData();
    
    // Start auto-refresh
    refreshInterval = setInterval(refreshData, window.APP_CONFIG.REFRESH_INTERVAL);
    
    window.APP_CONFIG.log('Dashboard initialized successfully');
    window.Notifications.show('success', 'Connected', 'Dashboard loaded successfully');
});
```

---

## 📦 QUICK START - Minimal Working Version

Create this simple test file to verify setup:

### tank/dashboard/test.html
```html
<!DOCTYPE html>
<html>
<head>
    <title>Tank Dashboard Test</title>
    <link rel="stylesheet" href="css/variables.css">
    <link rel="stylesheet" href="css/base.css">
    <link rel="stylesheet" href="css/components.css">
    <link rel="stylesheet" href="css/dashboard.css">
    <link rel="stylesheet" href="css/animations.css">
    <link href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.4.0/css/all.min.css" rel="stylesheet">
</head>
<body>
    <div class="container-fluid">
        <div class="card">
            <h2>✅ CSS Files Loaded Successfully!</h2>
            <p>If you see this styled properly, all CSS is working.</p>
            <button class="btn btn-primary"><i class="fas fa-check"></i> Test Button</button>
        </div>

        <div class="tank-card">
            <div class="tank-container">
                <div class="tank-body">
                    <div class="tank-water" id="tankWater" style="height: 75%"></div>
                    <div class="tank-level-display">75%</div>
                </div>
            </div>
        </div>
    </div>

    <script src="js/config.js"></script>
    <script>
        console.log('Config loaded:', window.APP_CONFIG);
    </script>
</body>
</html>
```

---

## 🔄 LOCAL TO PRODUCTION CHECKLIST

### When Ready to Deploy:

1. **Change Environment** in `tank/dashboard/js/config.js`:
   ```javascript
   const ENV = 'PRODUCTION'; // Change from 'LOCAL'
   ```

2. **Update API Config** in `tank/api/config.php`:
   ```php
   define('DB_USER', 'dwljzndr_tank');
   define('DB_PASS', 'your_production_password');
   define('API_KEY', 'your_secure_production_key');
   ```

3. **Update Hardware** in `include/config.h`:
   ```cpp
   #define WEBSITE_HOST "iotlogic.in"
   #define WEBSITE_PORT 443  // HTTPS
   #define API_KEY "your_secure_production_key"
   ```

---

## ⚡ NEXT STEPS

1. Create the remaining JS files (chartManager, theme, notifications, main)
2. Test with `test.html`
3. Create PHP components
4. Create new index.php
5. Test locally
6. Deploy to production

**Need help with any specific file? Let me know!**
