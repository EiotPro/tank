#ifndef WIFI_CREDENTIALS_H
#define WIFI_CREDENTIALS_H

#include <Arduino.h>
#include <LittleFS.h>
#include <ArduinoJson.h>

// 💾 WiFi Credentials Storage for Pico W
class WiFiCredentials {
private:
    static const char* CONFIG_FILE;
    
public:
    struct Config {
        char ssid[64];
        char password[128];
        char api_host[128];
        char api_endpoint[128];
        char api_key[64];
        uint16_t api_port;
        bool configured;
    };
    
    Config config;
    
    // 🚀 Initialize storage
    bool begin() {
        Serial.println("📁 Initializing LittleFS...");
        
        if (!LittleFS.begin()) {
            Serial.println("❌ LittleFS mount failed");
            return false;
        }
        
        Serial.println("✅ LittleFS mounted successfully");
        return true;
    }
    
    // 💾 Load credentials from file
    bool load() {
        if (!LittleFS.exists(CONFIG_FILE)) {
            Serial.println("⚠️  No config file found - first boot");
            config.configured = false;
            return false;
        }
        
        File file = LittleFS.open(CONFIG_FILE, "r");
        if (!file) {
            Serial.println("❌ Failed to open config file");
            return false;
        }
        
        StaticJsonDocument<512> doc;
        DeserializationError error = deserializeJson(doc, file);
        file.close();
        
        if (error) {
            Serial.println("❌ Failed to parse config file");
            return false;
        }
        
        // Load configuration
        strlcpy(config.ssid, doc["ssid"] | "", sizeof(config.ssid));
        strlcpy(config.password, doc["password"] | "", sizeof(config.password));
        strlcpy(config.api_host, doc["api_host"] | "10.97.116.57", sizeof(config.api_host));
        strlcpy(config.api_endpoint, doc["api_endpoint"] | "/Tank/tank/api/tank_update.php", sizeof(config.api_endpoint));
        strlcpy(config.api_key, doc["api_key"] | "iotlogic", sizeof(config.api_key));
        config.api_port = doc["api_port"] | 80;
        config.configured = doc["configured"] | false;
        
        Serial.println("✅ Configuration loaded:");
        Serial.println("   SSID: " + String(config.ssid));
        Serial.println("   API Host: " + String(config.api_host));
        Serial.println("   Configured: " + String(config.configured ? "YES" : "NO"));
        
        return config.configured;
    }
    
    // 💾 Save credentials to file
    bool save(const char* ssid, const char* password, 
              const char* api_host, const char* api_endpoint, 
              const char* api_key, uint16_t api_port) {
        
        StaticJsonDocument<512> doc;
        
        doc["ssid"] = ssid;
        doc["password"] = password;
        doc["api_host"] = api_host;
        doc["api_endpoint"] = api_endpoint;
        doc["api_key"] = api_key;
        doc["api_port"] = api_port;
        doc["configured"] = true;
        
        File file = LittleFS.open(CONFIG_FILE, "w");
        if (!file) {
            Serial.println("❌ Failed to create config file");
            return false;
        }
        
        if (serializeJson(doc, file) == 0) {
            Serial.println("❌ Failed to write config file");
            file.close();
            return false;
        }
        
        file.close();
        
        Serial.println("✅ Configuration saved successfully");
        return true;
    }
    
    // 🗑️ Clear all credentials
    bool clear() {
        if (LittleFS.exists(CONFIG_FILE)) {
            LittleFS.remove(CONFIG_FILE);
            Serial.println("🗑️  Configuration cleared");
        }
        config.configured = false;
        return true;
    }
    
    // 📊 Get current configuration
    Config getConfig() {
        return config;
    }
    
    // Helper methods for easier access
    bool hasCredentials() {
        return config.configured;
    }
    
    String getSSID() {
        return String(config.ssid);
    }
    
    String getPassword() {
        return String(config.password);
    }
    
    String getAPIEndpoint() {
        return String(config.api_endpoint);
    }
    
    String getAPIKey() {
        return String(config.api_key);
    }
    
    String getAPIHost() {
        return String(config.api_host);
    }
    
    uint16_t getAPIPort() {
        return config.api_port;
    }
};

const char* WiFiCredentials::CONFIG_FILE = "/wifi_config.json";

#endif // WIFI_CREDENTIALS_H
