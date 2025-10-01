// 🔔 NOTIFICATIONS - Handles toast notifications

const Notifications = {
    container: null,

    // 🚀 Initialize notification container
    init() {
        const { log } = window.APP_CONFIG;
        
        // Create container if it doesn't exist
        if (!this.container) {
            this.container = document.createElement('div');
            this.container.className = 'toast-container';
            this.container.id = 'toastContainer';
            document.body.appendChild(this.container);
        }
        
        log('Notifications initialized');
    },

    // 📢 Show notification
    show(type, title, message, duration = 5000) {
        const { log } = window.APP_CONFIG;
        
        if (!this.container) {
            this.init();
        }

        // Icon mapping
        const icons = {
            success: 'fa-check-circle',
            error: 'fa-exclamation-circle',
            warning: 'fa-exclamation-triangle',
            info: 'fa-info-circle'
        };

        // Create toast element
        const toast = document.createElement('div');
        toast.className = `toast toast-${type} animate-slide-in-right`;
        
        const toastId = 'toast-' + Date.now();
        toast.id = toastId;

        toast.innerHTML = `
            <div class="toast-icon">
                <i class="fas ${icons[type] || icons.info}"></i>
            </div>
            <div class="toast-content">
                <div class="toast-title">${this.escapeHtml(title)}</div>
                <div class="toast-message">${this.escapeHtml(message)}</div>
            </div>
            <button class="toast-close" onclick="window.Notifications.remove('${toastId}')">
                <i class="fas fa-times"></i>
            </button>
        `;

        // Add to container
        this.container.appendChild(toast);

        // Auto-remove after duration
        if (duration > 0) {
            setTimeout(() => {
                this.remove(toastId);
            }, duration);
        }

        log(`Notification shown: ${type} - ${title}`);

        return toastId;
    },

    // 🗑️ Remove notification
    remove(toastId) {
        const toast = document.getElementById(toastId);
        if (toast) {
            toast.classList.add('toast-hide');
            setTimeout(() => {
                if (toast.parentNode) {
                    toast.parentNode.removeChild(toast);
                }
            }, 300);
        }
    },

    // 🧹 Clear all notifications
    clearAll() {
        if (this.container) {
            while (this.container.firstChild) {
                this.container.removeChild(this.container.firstChild);
            }
        }
    },

    // ✅ Success notification
    success(title, message, duration) {
        return this.show('success', title, message, duration);
    },

    // ❌ Error notification
    error(title, message, duration) {
        return this.show('error', title, message, duration);
    },

    // ⚠️ Warning notification
    warning(title, message, duration) {
        return this.show('warning', title, message, duration);
    },

    // ℹ️ Info notification
    info(title, message, duration) {
        return this.show('info', title, message, duration);
    },

    // 🔒 Escape HTML to prevent XSS
    escapeHtml(unsafe) {
        if (typeof unsafe !== 'string') {
            return unsafe;
        }
        return unsafe
            .replace(/&/g, "&amp;")
            .replace(/</g, "&lt;")
            .replace(/>/g, "&gt;")
            .replace(/"/g, "&quot;")
            .replace(/'/g, "&#039;");
    }
};

// Export to window
window.Notifications = Notifications;
