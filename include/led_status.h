#ifndef LED_STATUS_H
#define LED_STATUS_H

#include <Arduino.h>

/**
 * LED Status Indicator System for Pico W
 * 
 * Provides visual feedback for different system states:
 * - AP Mode (Config): Fast blinking blue
 * - Connecting to WiFi: Slow pulsing blue
 * - WiFi Connected: Solid green
 * - Error/Failed: Fast blinking red
 * - Data Transmission: Quick green flash
 * 
 * Hardware:
 * - LED_PIN: GPIO pin for status LED (or use onboard LED)
 */

// LED Pin Configuration
#ifndef LED_PIN
#define LED_PIN LED_BUILTIN  // Use onboard LED by default (GP25 on Pico W)
#endif

// Alternative: Use external RGB LED
// #define LED_RED_PIN 15
// #define LED_GREEN_PIN 14
// #define LED_BLUE_PIN 13

// LED Status States
enum LEDStatus {
    LED_OFF,
    LED_AP_MODE,          // Fast blinking (config mode)
    LED_CONNECTING,       // Slow pulsing (connecting to WiFi)
    LED_CONNECTED,        // Solid on (WiFi connected)
    LED_ERROR,            // Fast blinking red (error state)
    LED_TRANSMITTING,     // Quick flash (data transmission)
    LED_CUSTOM            // Custom pattern
};

class LEDStatusManager {
private:
    int ledPin;
    LEDStatus currentStatus;
    unsigned long lastUpdate;
    bool ledState;
    int blinkCount;
    
    // Timing patterns (in milliseconds)
    const unsigned long FAST_BLINK_INTERVAL = 200;    // AP mode / Error
    const unsigned long SLOW_PULSE_INTERVAL = 1000;   // Connecting
    const unsigned long QUICK_FLASH_DURATION = 100;   // Data transmission
    
    // For smooth pulsing effect
    int brightness;
    bool increasing;

public:
    LEDStatusManager(int pin = LED_PIN) : 
        ledPin(pin), 
        currentStatus(LED_OFF),
        lastUpdate(0),
        ledState(false),
        blinkCount(0),
        brightness(0),
        increasing(true) {
    }
    
    /**
     * Initialize LED pin
     */
    void begin() {
        pinMode(ledPin, OUTPUT);
        digitalWrite(ledPin, LOW);
        Serial.println("✅ LED Status Manager initialized on pin " + String(ledPin));
    }
    
    /**
     * Set LED status mode
     */
    void setStatus(LEDStatus status) {
        if (currentStatus != status) {
            currentStatus = status;
            lastUpdate = 0;  // Reset timing
            blinkCount = 0;
            
            // Immediate visual feedback for certain states
            switch (status) {
                case LED_OFF:
                    digitalWrite(ledPin, LOW);
                    Serial.println("💡 LED: OFF");
                    break;
                case LED_CONNECTED:
                    digitalWrite(ledPin, HIGH);
                    Serial.println("💡 LED: CONNECTED (solid on)");
                    break;
                case LED_AP_MODE:
                    Serial.println("💡 LED: AP MODE (fast blink)");
                    break;
                case LED_CONNECTING:
                    Serial.println("💡 LED: CONNECTING (slow pulse)");
                    break;
                case LED_ERROR:
                    Serial.println("💡 LED: ERROR (fast blink red)");
                    break;
                default:
                    break;
            }
        }
    }
    
    /**
     * Update LED state based on current status
     * Call this in your main loop()
     */
    void update() {
        unsigned long currentMillis = millis();
        
        switch (currentStatus) {
            case LED_OFF:
                // LED stays off
                break;
                
            case LED_AP_MODE:
                // Fast blinking for AP/config mode
                if (currentMillis - lastUpdate >= FAST_BLINK_INTERVAL) {
                    ledState = !ledState;
                    digitalWrite(ledPin, ledState ? HIGH : LOW);
                    lastUpdate = currentMillis;
                }
                break;
                
            case LED_CONNECTING:
                // Slow pulsing effect
                if (currentMillis - lastUpdate >= 10) {  // Update every 10ms for smooth pulse
                    if (increasing) {
                        brightness += 5;
                        if (brightness >= 255) {
                            brightness = 255;
                            increasing = false;
                        }
                    } else {
                        brightness -= 5;
                        if (brightness <= 0) {
                            brightness = 0;
                            increasing = true;
                        }
                    }
                    analogWrite(ledPin, brightness);
                    lastUpdate = currentMillis;
                }
                break;
                
            case LED_CONNECTED:
                // Solid on (handled in setStatus)
                break;
                
            case LED_ERROR:
                // Fast blinking for errors (same as AP mode but semantically different)
                if (currentMillis - lastUpdate >= FAST_BLINK_INTERVAL) {
                    ledState = !ledState;
                    digitalWrite(ledPin, ledState ? HIGH : LOW);
                    lastUpdate = currentMillis;
                }
                break;
                
            case LED_TRANSMITTING:
                // Quick flash then return to connected state
                if (blinkCount < 2) {  // Flash twice
                    if (currentMillis - lastUpdate >= QUICK_FLASH_DURATION) {
                        ledState = !ledState;
                        digitalWrite(ledPin, ledState ? HIGH : LOW);
                        lastUpdate = currentMillis;
                        if (!ledState) blinkCount++;
                    }
                } else {
                    // Return to connected state after flash
                    setStatus(LED_CONNECTED);
                    blinkCount = 0;
                }
                break;
                
            case LED_CUSTOM:
                // User-defined pattern
                break;
        }
    }
    
    /**
     * Quick flash to indicate data transmission
     */
    void flashTransmit() {
        blinkCount = 0;
        setStatus(LED_TRANSMITTING);
    }
    
    /**
     * Get current status
     */
    LEDStatus getStatus() {
        return currentStatus;
    }
    
    /**
     * Manually control LED (for custom patterns)
     */
    void setLED(bool state) {
        digitalWrite(ledPin, state ? HIGH : LOW);
        ledState = state;
    }
    
    /**
     * Manually control LED brightness (PWM)
     */
    void setBrightness(int level) {
        analogWrite(ledPin, constrain(level, 0, 255));
    }
};

// Global LED manager instance (optional - can be instantiated in main.cpp instead)
// extern LEDStatusManager ledStatus;

#endif // LED_STATUS_H
