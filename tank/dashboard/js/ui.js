// 🎨 UI MODULE - Handles all DOM manipulations

const UI = {
    // 🌊 Update tank water level visualization (3D Tank)
    updateTankLevel(percentage) {
        const { log } = window.APP_CONFIG;
        
        // Use 3D Tank if available
        if (window.Tank3D && window.Tank3D.waterElement) {
            window.Tank3D.updateWaterLevel(percentage);
            log(`3D Tank level updated to ${percentage}%`);
        } else {
            // Fallback to old tank (if exists)
            const tankWater = document.getElementById('tankWater');
            const tankLevelText = document.getElementById('tankLevelText');
            
            if (tankWater && tankLevelText) {
                tankWater.style.height = `${percentage}%`;
                tankLevelText.textContent = `${percentage}%`;
                log(`Tank level updated to ${percentage}%`);
            }
        }
    },

    // 📊 Update metric cards
    updateMetrics(data) {
        const { log } = window.APP_CONFIG;
        
        if (!data || !data.latest) {
            log('No data available to update metrics');
            return;
        }

        const latest = data.latest;

        // Update level
        const levelEl = document.getElementById('metricLevel');
        if (levelEl) {
            levelEl.textContent = `${latest.level} cm`;
        }

        // Update percentage
        const percentEl = document.getElementById('metricPercent');
        if (percentEl) {
            percentEl.textContent = `${latest.percentage}%`;
        }

        // Update progress bar
        const progressBar = document.getElementById('percentProgress');
        if (progressBar) {
            progressBar.style.width = `${latest.percentage}%`;
            progressBar.className = `progress-bar ${this.getProgressClass(latest.percentage)}`;
        }

        // Update last update time
        this.updateLastUpdateTime(latest.timestamp);

        // Update status badge
        this.updateStatusBadge(latest.percentage, latest.level);

        log('Metrics updated successfully');
    },

    // ⏰ Update last update time
    updateLastUpdateTime(timestamp) {
        const timeEl = document.getElementById('lastUpdateTime');
        if (!timeEl) return;

        const date = new Date(timestamp);
        const now = new Date();
        const diffMs = now - date;
        const diffMins = Math.floor(diffMs / 60000);

        let timeText;
        if (diffMins < 1) {
            timeText = 'Just now';
        } else if (diffMins === 1) {
            timeText = '1 min ago';
        } else if (diffMins < 60) {
            timeText = `${diffMins} mins ago`;
        } else {
            const hours = Math.floor(diffMins / 60);
            timeText = hours === 1 ? '1 hour ago' : `${hours} hours ago`;
        }

        timeEl.textContent = timeText;
        timeEl.title = date.toLocaleString();
    },

    // 🏷️ Update status badge
    updateStatusBadge(percentage, level) {
        const { STATUS_CONFIG } = window.APP_CONFIG;
        const badge = document.getElementById('statusBadge');
        if (!badge) return;

        let status = STATUS_CONFIG.LOW;
        if (percentage >= STATUS_CONFIG.FULL.min) {
            status = STATUS_CONFIG.FULL;
        } else if (percentage >= STATUS_CONFIG.HIGH.min) {
            status = STATUS_CONFIG.HIGH;
        } else if (percentage >= STATUS_CONFIG.MEDIUM.min) {
            status = STATUS_CONFIG.MEDIUM;
        }

        badge.className = `tank-status-badge badge-${status.label.toLowerCase()}`;
        badge.style.background = status.color;
        badge.style.color = 'white';
        badge.innerHTML = `
            <span class="status-indicator status-online"></span>
            <span>${status.label}</span>
        `;
    },

    // 🎨 Get progress bar class based on percentage
    getProgressClass(percentage) {
        if (percentage >= 60) return 'progress-bar-success';
        if (percentage >= 30) return 'progress-bar-warning';
        return 'progress-bar-danger';
    },

    // ⏳ Show loading state
    showLoading() {
        const { log } = window.APP_CONFIG;
        document.body.classList.add('loading');
        log('Loading state activated');
    },

    // ✅ Hide loading state
    hideLoading() {
        const { log } = window.APP_CONFIG;
        document.body.classList.remove('loading');
        log('Loading state deactivated');
    },

    // ❌ Show error message
    showError(message) {
        window.Notifications.show('error', 'Error', message);
    },

    // ✅ Show success message
    showSuccess(message) {
        window.Notifications.show('success', 'Success', message);
    },

    // ℹ️ Show info message
    showInfo(message) {
        window.Notifications.show('info', 'Info', message);
    },

    // 🔄 Update connection status indicator
    updateConnectionStatus(isOnline) {
        const statusDot = document.querySelector('.header-btn .status-indicator');
        if (statusDot) {
            statusDot.className = isOnline ? 'status-indicator status-online' : 'status-indicator status-offline';
        }
    },

    // 🔄 Refresh button animation
    animateRefreshButton() {
        const refreshBtn = document.getElementById('refreshBtn');
        if (refreshBtn) {
            const icon = refreshBtn.querySelector('i');
            if (icon) {
                icon.classList.add('animate-spin');
                setTimeout(() => icon.classList.remove('animate-spin'), 1000);
            }
        }
    }
};

// Export to window
window.UI = UI;
