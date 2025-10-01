// 🌊 3D TANK CONTROLLER - Advanced Water Visualization

const Tank3D = {
    waterElement: null,
    currentLevel: 0,
    targetLevel: 0,
    isAnimating: false,
    isCritical: false,
    beepInterval: null,
    audioContext: null,

    // 🚀 Initialize 3D tank
    init() {
        const { log } = window.APP_CONFIG;
        
        this.waterElement = document.getElementById('tank3dWater');
        
        if (!this.waterElement) {
            log('3D Tank element not found');
            return;
        }

        // Initialize audio context for beep sound
        this.initAudio();

        // Add entrance animation
        const container = document.querySelector('.tank-3d-container');
        if (container) {
            container.classList.add('animate-in');
        }

        // Set initial level from HTML
        const initialLevel = parseFloat(this.waterElement.style.height) || 0;
        this.currentLevel = initialLevel;
        this.targetLevel = initialLevel;
        
        // Check if initially critical
        if (initialLevel <= 10) {
            this.startCriticalAlert();
        }

        log('3D Tank initialized successfully');
    },

    // 🔊 Initialize Audio Context for beep sound
    initAudio() {
        try {
            // Create audio context (cross-browser)
            this.audioContext = new (window.AudioContext || window.webkitAudioContext)();
        } catch (e) {
            console.warn('Audio context not supported:', e);
        }
    },

    // 🔊 Play beep sound
    playBeep() {
        if (!this.audioContext) return;

        try {
            const oscillator = this.audioContext.createOscillator();
            const gainNode = this.audioContext.createGain();

            oscillator.connect(gainNode);
            gainNode.connect(this.audioContext.destination);

            // Beep settings
            oscillator.frequency.value = 800; // Hz - alert frequency
            oscillator.type = 'sine';
            
            // Volume envelope (fade in/out)
            gainNode.gain.setValueAtTime(0, this.audioContext.currentTime);
            gainNode.gain.linearRampToValueAtTime(0.3, this.audioContext.currentTime + 0.05);
            gainNode.gain.exponentialRampToValueAtTime(0.01, this.audioContext.currentTime + 0.5);

            // Play beep
            oscillator.start(this.audioContext.currentTime);
            oscillator.stop(this.audioContext.currentTime + 0.5);
        } catch (e) {
            console.warn('Beep sound error:', e);
        }
    },

    // ⚠️ Start critical level alert
    startCriticalAlert() {
        if (this.beepInterval) return; // Already running

        const { log } = window.APP_CONFIG;
        log('⚠️ CRITICAL ALERT STARTED - Beeping every 3 seconds');

        this.isCritical = true;
        
        // Play immediate beep
        this.playBeep();

        // Set interval to beep every 3 seconds
        this.beepInterval = setInterval(() => {
            this.playBeep();
        }, 3000);

        // Add visual pulsing effect
        if (this.waterElement) {
            this.waterElement.style.animation = 'criticalPulse 1s ease-in-out infinite';
        }
    },

    // ✅ Stop critical level alert
    stopCriticalAlert() {
        if (!this.beepInterval) return; // Not running

        const { log } = window.APP_CONFIG;
        log('✅ CRITICAL ALERT STOPPED');

        this.isCritical = false;
        
        // Clear beep interval
        clearInterval(this.beepInterval);
        this.beepInterval = null;

        // Remove visual pulsing effect
        if (this.waterElement) {
            this.waterElement.style.animation = '';
        }
    },

    // 💧 Update water level with smooth animation
    updateWaterLevel(percentage) {
        const { log } = window.APP_CONFIG;
        
        if (!this.waterElement) {
            log('Water element not available');
            return;
        }

        // Determine if filling or draining
        const previousLevel = this.targetLevel;
        this.targetLevel = Math.max(0, Math.min(100, percentage));
        const isFilling = this.targetLevel > previousLevel;
        const isDraining = this.targetLevel < previousLevel;

        // Add visual effects based on direction
        if (isFilling) {
            this.addFillingEffect();
        } else if (isDraining) {
            this.addDrainingEffect();
        }

        // Animate to target level
        this.animateWaterLevel();

        // Update percentage display
        this.updatePercentageDisplay(percentage);

        // Update water color based on level
        this.updateWaterColor(percentage);

        // Update level marks
        this.updateLevelMarks(percentage);

        // Handle critical level alert
        if (percentage <= 10 && !this.isCritical) {
            this.startCriticalAlert();
        } else if (percentage > 10 && this.isCritical) {
            this.stopCriticalAlert();
        }

        log(`3D Tank level ${isFilling ? 'FILLING' : isDraining ? 'DRAINING' : 'updated'} to ${percentage}%`);
    },

    // 🎬 Smooth animation to target level
    animateWaterLevel() {
        if (this.isAnimating) return;
        
        this.isAnimating = true;
        const startLevel = this.currentLevel;
        const endLevel = this.targetLevel;
        const duration = 1500; // 1.5 seconds
        const startTime = performance.now();

        const animate = (currentTime) => {
            const elapsed = currentTime - startTime;
            const progress = Math.min(elapsed / duration, 1);

            // Easing function for smooth animation
            const easeProgress = this.easeInOutCubic(progress);

            // Calculate current level
            this.currentLevel = startLevel + (endLevel - startLevel) * easeProgress;

            // Update water height
            this.waterElement.style.height = `${this.currentLevel}%`;

            // Continue animation if not complete
            if (progress < 1) {
                requestAnimationFrame(animate);
            } else {
                this.isAnimating = false;
            }
        };

        requestAnimationFrame(animate);
    },

    // 📊 Easing function for smooth animation
    easeInOutCubic(t) {
        return t < 0.5 
            ? 4 * t * t * t 
            : 1 - Math.pow(-2 * t + 2, 3) / 2;
    },

    // 🎨 Update water color based on level
    updateWaterColor(percentage) {
        if (!this.waterElement) return;

        // Remove all level classes
        this.waterElement.removeAttribute('data-level');

        // Set appropriate level class
        if (percentage <= 10) {
            this.waterElement.setAttribute('data-level', 'critical');
        } else if (percentage <= 30) {
            this.waterElement.setAttribute('data-level', 'low');
        } else if (percentage <= 60) {
            this.waterElement.setAttribute('data-level', 'medium');
        } else {
            this.waterElement.setAttribute('data-level', 'high');
        }
    },

    // 📊 Update percentage display
    updatePercentageDisplay(percentage) {
        const percentageDisplay = document.getElementById('tank3dPercentage');
        if (percentageDisplay) {
            // Animate number change
            this.animateNumber(percentageDisplay, percentage);
        }
    },

    // 🔢 Animate number change
    animateNumber(element, targetNumber) {
        const currentNumber = parseInt(element.textContent) || 0;
        const duration = 1000;
        const startTime = performance.now();

        const animate = (currentTime) => {
            const elapsed = currentTime - startTime;
            const progress = Math.min(elapsed / duration, 1);

            const current = Math.round(currentNumber + (targetNumber - currentNumber) * progress);
            element.textContent = `${current}%`;

            if (progress < 1) {
                requestAnimationFrame(animate);
            }
        };

        requestAnimationFrame(animate);
    },

    // 📏 Update level indicator marks
    updateLevelMarks(percentage) {
        const marks = document.querySelectorAll('.level-mark');
        marks.forEach((mark, index) => {
            const markLevel = 100 - (index * 25);
            if (percentage >= markLevel) {
                mark.style.background = 'rgba(79, 195, 247, 0.8)';
                mark.style.boxShadow = '0 0 10px rgba(79, 195, 247, 0.5)';
            } else {
                mark.style.background = 'rgba(100, 140, 180, 0.4)';
                mark.style.boxShadow = 'none';
            }
        });
    },

    // 🌊 FILLING EFFECT - Water rushing in from bottom
    addFillingEffect() {
        const container = this.waterElement.parentElement;
        if (!container) return;

        // Create multiple splash effects at water surface
        for (let i = 0; i < 3; i++) {
            setTimeout(() => {
                const splash = document.createElement('div');
                splash.className = 'water-filling-splash';
                splash.style.cssText = `
                    position: absolute;
                    width: ${60 + Math.random() * 40}px;
                    height: ${20 + Math.random() * 15}px;
                    background: radial-gradient(ellipse, rgba(79, 195, 247, 0.7), transparent);
                    left: ${20 + Math.random() * 60}%;
                    bottom: ${this.currentLevel}%;
                    transform: translateX(-50%);
                    animation: splashUp 0.6s ease-out forwards;
                    pointer-events: none;
                    border-radius: 50%;
                `;
                container.appendChild(splash);
                setTimeout(() => splash.remove(), 600);
            }, i * 150);
        }

        // Add rising bubbles during filling
        this.addFillingBubbles();

        // Add water stream effect from bottom
        const stream = document.createElement('div');
        stream.className = 'water-stream';
        stream.style.cssText = `
            position: absolute;
            width: 40px;
            height: 80px;
            background: linear-gradient(0deg, 
                rgba(79, 195, 247, 0.8) 0%, 
                rgba(79, 195, 247, 0.4) 50%,
                transparent 100%);
            left: 50%;
            bottom: 0;
            transform: translateX(-50%);
            animation: streamFlow 1.2s ease-out forwards;
            pointer-events: none;
            filter: blur(3px);
        `;
        container.appendChild(stream);
        setTimeout(() => stream.remove(), 1200);
    },

    // 💧 DRAINING EFFECT - Water draining from bottom
    addDrainingEffect() {
        const container = this.waterElement.parentElement;
        if (!container) return;

        // Create multiple drips falling down
        for (let i = 0; i < 5; i++) {
            setTimeout(() => {
                const drip = document.createElement('div');
                drip.className = 'water-drip';
                drip.style.cssText = `
                    position: absolute;
                    width: ${4 + Math.random() * 4}px;
                    height: ${15 + Math.random() * 10}px;
                    background: linear-gradient(180deg, 
                        rgba(79, 195, 247, 0.8), 
                        rgba(79, 195, 247, 0.4),
                        transparent);
                    border-radius: 50% 50% 50% 50% / 60% 60% 40% 40%;
                    left: ${20 + Math.random() * 60}%;
                    bottom: ${this.currentLevel - 5}%;
                    animation: dripDown 1.5s ease-in forwards;
                    pointer-events: none;
                `;
                container.appendChild(drip);
                setTimeout(() => drip.remove(), 1500);
            }, i * 100);
        }

        // Add swirl/vortex effect at water surface
        const vortex = document.createElement('div');
        vortex.className = 'water-vortex';
        vortex.style.cssText = `
            position: absolute;
            width: 60px;
            height: 60px;
            border: 3px solid rgba(79, 195, 247, 0.5);
            border-radius: 50%;
            left: 50%;
            bottom: ${this.currentLevel}%;
            transform: translateX(-50%) translateY(50%);
            animation: vortexSpin 1s ease-out forwards;
            pointer-events: none;
        `;
        container.appendChild(vortex);
        setTimeout(() => vortex.remove(), 1000);

        // Add downward stream effect
        const drain = document.createElement('div');
        drain.className = 'water-drain-stream';
        drain.style.cssText = `
            position: absolute;
            width: 30px;
            height: ${this.currentLevel}%;
            background: linear-gradient(180deg, 
                transparent 0%,
                rgba(79, 195, 247, 0.3) 40%,
                rgba(79, 195, 247, 0.6) 100%);
            left: 50%;
            bottom: 0;
            transform: translateX(-50%);
            animation: drainFlow 1s ease-in forwards;
            pointer-events: none;
            filter: blur(2px);
        `;
        container.appendChild(drain);
        setTimeout(() => drain.remove(), 1000);
    },

    // 🫧 Add bubbles during filling
    addFillingBubbles() {
        const container = this.waterElement.parentElement;
        if (!container) return;

        for (let i = 0; i < 8; i++) {
            setTimeout(() => {
                const bubble = document.createElement('div');
                bubble.className = 'filling-bubble';
                const size = 5 + Math.random() * 8;
                bubble.style.cssText = `
                    position: absolute;
                    width: ${size}px;
                    height: ${size}px;
                    background: radial-gradient(circle at 30% 30%, 
                        rgba(255, 255, 255, 0.9), 
                        rgba(79, 195, 247, 0.6));
                    border-radius: 50%;
                    left: ${20 + Math.random() * 60}%;
                    bottom: 0;
                    animation: bubbleRiseQuick ${1 + Math.random()}s ease-out forwards;
                    pointer-events: none;
                    box-shadow: inset 0 0 5px rgba(255, 255, 255, 0.8);
                `;
                container.appendChild(bubble);
                setTimeout(() => bubble.remove(), 2000);
            }, i * 100);
        }
    },

    // 🎭 Get current water status
    getWaterStatus(percentage) {
        if (percentage <= 10) return { label: 'Critical', color: '#ef5350', icon: 'exclamation-triangle' };
        if (percentage <= 30) return { label: 'Low', color: '#ffa726', icon: 'exclamation-circle' };
        if (percentage <= 60) return { label: 'Medium', color: '#66bb6a', icon: 'check-circle' };
        return { label: 'High', color: '#4fc3f7', icon: 'check-circle' };
    },

    // 🌟 Add particle effects based on water level
    updateParticleEffects(percentage) {
        // Remove existing particles
        document.querySelectorAll('.water-particle').forEach(p => p.remove());

        if (percentage > 20 && this.waterElement) {
            // Add particles based on water level
            const particleCount = Math.floor(percentage / 20);
            
            for (let i = 0; i < particleCount; i++) {
                const particle = document.createElement('div');
                particle.className = 'water-particle';
                particle.style.left = `${Math.random() * 80 + 10}%`;
                particle.style.bottom = `${Math.random() * 50}%`;
                particle.style.animationDelay = `${Math.random() * 8}s`;
                
                this.waterElement.appendChild(particle);
            }
        }
    }
};

// Add CSS for dynamic animations
const style = document.createElement('style');
style.textContent = `
    /* 🌊 FILLING EFFECTS */
    @keyframes splashUp {
        0% {
            transform: translateX(-50%) scale(0.3) translateY(0);
            opacity: 1;
        }
        50% {
            transform: translateX(-50%) scale(1.2) translateY(-15px);
            opacity: 0.8;
        }
        100% {
            transform: translateX(-50%) scale(1.8) translateY(-30px);
            opacity: 0;
        }
    }

    @keyframes streamFlow {
        0% {
            height: 0;
            opacity: 0;
        }
        30% {
            opacity: 1;
        }
        100% {
            height: 80px;
            opacity: 0.3;
        }
    }

    @keyframes bubbleRiseQuick {
        0% {
            bottom: 0;
            opacity: 0;
            transform: translateX(0) scale(0.5);
        }
        20% {
            opacity: 1;
        }
        100% {
            bottom: 100%;
            opacity: 0;
            transform: translateX(${Math.random() > 0.5 ? '' : '-'}20px) scale(1.2);
        }
    }

    /* 💧 DRAINING EFFECTS */
    @keyframes dripDown {
        0% {
            transform: translateY(0) scaleY(1);
            opacity: 1;
        }
        50% {
            opacity: 0.8;
        }
        100% {
            transform: translateY(100px) scaleY(2.5);
            opacity: 0;
        }
    }

    @keyframes vortexSpin {
        0% {
            transform: translateX(-50%) translateY(50%) rotate(0deg) scale(0.3);
            opacity: 0;
            border-width: 3px;
        }
        50% {
            opacity: 0.8;
            border-width: 2px;
        }
        100% {
            transform: translateX(-50%) translateY(50%) rotate(360deg) scale(1.5);
            opacity: 0;
            border-width: 1px;
        }
    }

    @keyframes drainFlow {
        0% {
            opacity: 0;
            transform: translateX(-50%) scaleY(0);
            transform-origin: bottom;
        }
        30% {
            opacity: 0.8;
        }
        100% {
            opacity: 0;
            transform: translateX(-50%) scaleY(1);
            transform-origin: bottom;
        }
    }

    /* 🎨 Water Effects Styling */
    .water-filling-splash {
        filter: blur(4px);
    }

    .water-drip {
        filter: blur(1px);
    }

    .filling-bubble {
        filter: blur(0.5px);
    }

    /* ⚠️ CRITICAL LEVEL PULSE ANIMATION */
    @keyframes criticalPulse {
        0%, 100% {
            box-shadow: 
                inset 0 10px 30px rgba(255, 255, 255, 0.3),
                inset 0 -10px 20px rgba(0, 0, 0, 0.2),
                0 0 20px rgba(239, 83, 80, 0.8);
        }
        50% {
            box-shadow: 
                inset 0 10px 30px rgba(255, 255, 255, 0.3),
                inset 0 -10px 20px rgba(0, 0, 0, 0.2),
                0 0 40px rgba(239, 83, 80, 1),
                0 0 60px rgba(239, 83, 80, 0.6);
        }
    }
`;
document.head.appendChild(style);

// Export to window
window.Tank3D = Tank3D;
