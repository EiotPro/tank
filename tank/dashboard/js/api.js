// 🚀 API MODULE - Handles all API interactions

const API = {
    // 📊 Fetch latest tank data
    async getLatestData(tankId = 1) {
        const { API_ENDPOINTS, MAX_RETRY_ATTEMPTS, RETRY_DELAY, log, logError } = window.APP_CONFIG;
        
        for (let attempt = 1; attempt <= MAX_RETRY_ATTEMPTS; attempt++) {
            try {
                log(`Fetching data (attempt ${attempt}/${MAX_RETRY_ATTEMPTS})...`);
                
                const response = await fetch(`${API_ENDPOINTS.GET_LATEST_DATA}?tank_id=${tankId}&_t=${Date.now()}`);
                
                if (!response.ok) {
                    throw new Error(`HTTP error! status: ${response.status}`);
                }
                
                const data = await response.json();
                log('Data fetched successfully:', data);
                return data;
                
            } catch (error) {
                logError(`Attempt ${attempt} failed:`, error);
                
                if (attempt === MAX_RETRY_ATTEMPTS) {
                    throw new Error('Failed to fetch data after multiple attempts');
                }
                
                // Wait before retrying
                await new Promise(resolve => setTimeout(resolve, RETRY_DELAY));
            }
        }
    },

    // 🔄 Check system status
    async checkStatus() {
        const { API_ENDPOINTS, log, logError } = window.APP_CONFIG;
        
        try {
            const response = await fetch(API_ENDPOINTS.CHECK_STATUS);
            
            if (!response.ok) {
                return { status: 'error', online: false };
            }
            
            const data = await response.json();
            return { ...data, online: true };
            
        } catch (error) {
            logError('Status check failed:', error);
            return { status: 'error', online: false };
        }
    },

    // 📤 Send test data (for testing purposes)
    async sendTestData(level, percentage) {
        const { API_ENDPOINTS, log, logError } = window.APP_CONFIG;
        
        try {
            const payload = {
                tank_id: 1,
                level: level,
                percentage: percentage,
                api_key: 'iotlogic'
            };
            
            const response = await fetch(API_ENDPOINTS.UPDATE_TANK_DATA, {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json'
                },
                body: JSON.stringify(payload)
            });
            
            if (!response.ok) {
                throw new Error(`HTTP error! status: ${response.status}`);
            }
            
            const data = await response.json();
            log('Test data sent successfully:', data);
            return data;
            
        } catch (error) {
            logError('Failed to send test data:', error);
            throw error;
        }
    },

    // 📊 Get filtered chart data by period
    getFilteredChartData(allData, period) {
        const { log } = window.APP_CONFIG;
        
        if (!allData || allData.length === 0) {
            log('No data available for filtering');
            return [];
        }

        const now = new Date();
        const hours = window.APP_CONFIG.CHART_CONFIG.PERIODS[period].hours;
        const cutoffTime = new Date(now.getTime() - (hours * 60 * 60 * 1000));

        const filtered = allData.filter(item => {
            const itemTime = new Date(item.timestamp);
            return itemTime >= cutoffTime;
        });

        log(`Filtered ${filtered.length} data points for ${period} period`);
        return filtered;
    }
};

// Export to window
window.API = API;
