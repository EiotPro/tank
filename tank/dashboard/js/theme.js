// 🌓 THEME MANAGER - Handles dark/light theme switching

const Theme = {
    // 🚀 Initialize theme
    init() {
        const { log } = window.APP_CONFIG;
        
        // Get saved theme or default to light
        const savedTheme = localStorage.getItem('dashboard-theme') || 'light';
        this.setTheme(savedTheme, false);
        
        log(`Theme initialized: ${savedTheme}`);
    },

    // 🔄 Toggle between light and dark
    toggle() {
        const current = document.documentElement.getAttribute('data-theme') || 'light';
        const newTheme = current === 'light' ? 'dark' : 'light';
        this.setTheme(newTheme, true);
    },

    // 🎨 Set theme
    setTheme(theme, save = true) {
        const { log } = window.APP_CONFIG;
        
        // Validate theme
        if (!['light', 'dark'].includes(theme)) {
            log('Invalid theme:', theme);
            return;
        }

        // Apply theme to document
        document.documentElement.setAttribute('data-theme', theme);
        
        // Save to localStorage if requested
        if (save) {
            localStorage.setItem('dashboard-theme', theme);
            log(`Theme saved: ${theme}`);
        }

        // Update theme toggle button icon
        this.updateToggleIcon(theme);

        // Update chart colors if chart exists
        if (window.ChartManager && window.ChartManager.chart) {
            window.ChartManager.updateTheme(theme === 'dark');
        }

        // Show notification
        if (save) {
            window.Notifications.show(
                'info', 
                'Theme Changed', 
                `Switched to ${theme} mode`,
                2000
            );
        }

        log(`Theme applied: ${theme}`);
    },

    // 🔘 Update toggle button icon
    updateToggleIcon(theme) {
        const icon = document.querySelector('#themeToggle i');
        if (icon) {
            // Sun icon for dark mode (to switch to light)
            // Moon icon for light mode (to switch to dark)
            icon.className = theme === 'dark' ? 'fas fa-sun' : 'fas fa-moon';
        }
    },

    // 📱 Get current theme
    getCurrentTheme() {
        return document.documentElement.getAttribute('data-theme') || 'light';
    },

    // 🎨 Check if dark mode
    isDark() {
        return this.getCurrentTheme() === 'dark';
    }
};

// Export to window
window.Theme = Theme;
