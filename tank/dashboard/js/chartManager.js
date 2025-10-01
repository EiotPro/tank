// 📊 CHART MANAGER - Handles Chart.js visualization

const ChartManager = {
    chart: null,
    currentPeriod: '24h',

    // 🚀 Initialize chart
    init() {
        const { log } = window.APP_CONFIG;
        const ctx = document.getElementById('tankChart');
        
        if (!ctx) {
            log('Chart canvas not found');
            return;
        }

        // Chart.js configuration
        this.chart = new Chart(ctx, {
            type: 'line',
            data: {
                labels: [],
                datasets: [{
                    label: 'Water Level (%)',
                    data: [],
                    borderColor: '#4F46E5',
                    backgroundColor: 'rgba(79, 70, 229, 0.1)',
                    borderWidth: 2,
                    tension: 0.4,
                    fill: true,
                    pointRadius: 3,
                    pointHoverRadius: 6,
                    pointBackgroundColor: '#4F46E5',
                    pointBorderColor: '#fff',
                    pointBorderWidth: 2
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
                        titleColor: '#fff',
                        bodyColor: '#fff',
                        padding: 12,
                        displayColors: false,
                        cornerRadius: 8,
                        callbacks: {
                            label: function(context) {
                                return `Water Level: ${context.parsed.y}%`;
                            }
                        }
                    }
                },
                scales: {
                    y: {
                        beginAtZero: true,
                        max: 100,
                        ticks: {
                            color: '#64748b',
                            callback: function(value) {
                                return value + '%';
                            }
                        },
                        grid: {
                            color: 'rgba(0, 0, 0, 0.05)',
                            drawBorder: false
                        }
                    },
                    x: {
                        ticks: {
                            color: '#64748b',
                            maxRotation: 0,
                            autoSkipPadding: 20
                        },
                        grid: {
                            display: false,
                            drawBorder: false
                        }
                    }
                },
                animation: {
                    duration: 750,
                    easing: 'easeInOutQuart'
                }
            }
        });

        log('Chart initialized successfully');
    },

    // 🔄 Update chart with new data
    update(data) {
        const { log } = window.APP_CONFIG;
        
        if (!this.chart) {
            log('Chart not initialized');
            return;
        }

        if (!data || !data.recent) {
            log('No chart data available');
            return;
        }

        // Filter data by current period
        const filtered = window.API.getFilteredChartData(data.recent, this.currentPeriod);
        
        if (filtered.length === 0) {
            log('No data points for selected period');
            return;
        }

        // Update labels (timestamps)
        this.chart.data.labels = filtered.map(item => {
            const date = new Date(item.timestamp);
            const hours = this.currentPeriod === '1h' ? 1 : this.currentPeriod === '6h' ? 6 : 24;
            
            if (hours === 1) {
                return date.toLocaleTimeString('en-US', { 
                    hour: '2-digit', 
                    minute: '2-digit' 
                });
            } else if (hours === 6) {
                return date.toLocaleTimeString('en-US', { 
                    hour: '2-digit', 
                    minute: '2-digit' 
                });
            } else {
                return date.toLocaleTimeString('en-US', { 
                    hour: '2-digit' 
                });
            }
        });

        // Update data points
        this.chart.data.datasets[0].data = filtered.map(item => item.percentage);

        // Update point radius based on period
        const periodConfig = window.APP_CONFIG.CHART_CONFIG.PERIODS[this.currentPeriod];
        this.chart.data.datasets[0].pointRadius = periodConfig.pointRadius;

        // Update chart
        this.chart.update('none');

        log(`Chart updated with ${filtered.length} data points for ${this.currentPeriod} period`);
    },

    // 📅 Change chart period
    changePeriod(period) {
        const { log } = window.APP_CONFIG;
        
        if (!['1h', '6h', '24h'].includes(period)) {
            log('Invalid period:', period);
            return;
        }

        this.currentPeriod = period;
        log(`Chart period changed to: ${period}`);
    },

    // 🎨 Update chart theme
    updateTheme(isDark) {
        if (!this.chart) return;

        const gridColor = isDark ? 'rgba(255, 255, 255, 0.1)' : 'rgba(0, 0, 0, 0.05)';
        const textColor = isDark ? '#cbd5e1' : '#64748b';

        this.chart.options.scales.y.ticks.color = textColor;
        this.chart.options.scales.y.grid.color = gridColor;
        this.chart.options.scales.x.ticks.color = textColor;

        this.chart.update('none');
    }
};

// Export to window
window.ChartManager = ChartManager;
