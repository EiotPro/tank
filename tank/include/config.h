#ifndef CONFIG_H
#define CONFIG_H

// LoRa Configuration
#define LORA_FREQUENCY 865000000  // 865 MHz
#define LORA_SPREADING_FACTOR 7
#define LORA_BANDWIDTH 1          // 125 kHz
#define LORA_CODING_RATE 1        // 4/5
#define LORA_PREAMBLE_LENGTH 8
#define LORA_TX_POWER 14          // 14 dBm

// TX Node Configuration
#define TRANSMISSION_INTERVAL 10000  // 10 seconds
#define TANK_MAX_DEPTH 200           // Maximum tank depth in cm
#define TANK_MIN_DEPTH 20            // Minimum tank depth in cm

// RX Node Wi-Fi Configuration
#define WIFI_SSID "WaterTank_Monitor"
#define WIFI_PASSWORD "watertank123"
#define WEB_SERVER_PORT 80

// Data Logging Configuration
#define MAX_LOG_FILES 10
#define LOG_FILE_PREFIX "/logs/tank_level_"

// Sensor Configuration
#define WATER_SENSOR_PIN A0

// Hardware Pin Configuration
#define RAK3172_RESET 2

// System Configuration
#define SERIAL_BAUD_RATE 115200
#define LORA_BAUD_RATE 115200

#endif // CONFIG_H
