# LoRa Water Tank Monitoring System

A LoRa-based water tank monitoring system with transmitter (TX) and receiver (RX) nodes for real-time water level monitoring using Raspberry Pi Pico.

## System Overview

### Transmitter (TX Node)
- **Hardware**: Raspberry Pi Pico + RAK3172 LoRa module + water level sensor
- **Function**: Reads analog water level sensor, converts to 2-byte HEX payload, transmits via LoRa P2P
- **Transmission**: Every 10 seconds using configured LoRa parameters
- **Output**: Serial monitoring of raw sensor values, calculated water levels, and transmission status

### Receiver (RX Node)
- **Hardware**: Raspberry Pi Pico + RAK3172 LoRa module
- **Function**: Receives LoRa P2P packets, parses HEX payload, calculates water level percentage
- **Features**: Real-time Serial output, automatic RX re-enabling, packet validation
- **Output**: Serial display of received data, decoded water levels, and percentages

## Local Monitoring

### Serial Monitor Setup
The system provides real-time monitoring through serial connections:

1. **Connect TX Node to Computer**
   - Use USB cable to connect Pico TX to computer
   - Open Serial Monitor at 115200 baud rate
   - Monitor: Raw sensor readings, water level calculations, transmission status

2. **Connect RX Node to Computer**
   - Use USB cable to connect Pico RX to computer
   - Open Serial Monitor at 115200 baud rate
   - Monitor: Received LoRa packets, decoded water levels, percentages

### Serial Output Examples

**TX Node Output:**
```
=== LoRa Water Tank TX Node ===
Initializing...
--- Water Level Reading ---
Raw sensor value: 512
Water level: 100 cm
Tank fill: 50%
HEX payload: 0064
LoRa transmission successful
```

**RX Node Output:**
```
=== LoRa Water Tank RX Node ===
Initializing LoRa reception...
RX Node initialization complete
Ready to receive LoRa data
LoRa RX: +EVT:RXP2P:0064
Received HEX payload: 0064
--- Water Level Data ---
Water level: 100 cm
Percentage: 50%
```

## Current Features

### Core Functionality
- ✅ **LoRa P2P Communication**: Point-to-point LoRa transmission between TX and RX nodes
- ✅ **HEX Payload Processing**: 2-byte hexadecimal encoding/decoding for water level data
- ✅ **Analog Sensor Reading**: TX reads water level sensor (0-1023 range)
- ✅ **Water Level Calculation**: Maps sensor values to centimeters (0-200cm range)
- ✅ **Percentage Calculation**: Converts water level to tank fill percentage
- ✅ **Serial Monitoring**: Real-time output for debugging and monitoring
- ✅ **Automatic RX Re-enabling**: RX automatically re-enters receive mode after processing packets

### Technical Features
- ✅ **Packet Parsing**: Extracts payload from "+EVT:RXP2P" responses
- ✅ **Hex-to-Decimal Conversion**: Converts received HEX strings to integer values
- ✅ **Data Validation**: Constrains values within valid ranges
- ✅ **Error Handling**: Graceful handling of malformed packets
- ✅ **Serial Debugging**: Comprehensive logging for troubleshooting

### Hardware Integration
- ✅ **RAK3172 LoRa Module**: AT command interface via Serial1
- ✅ **Raspberry Pi Pico**: Arduino framework compatibility
- ✅ **Analog Sensor Input**: A0 pin for water level sensor
- ✅ **GPIO Control**: Reset pin control for LoRa module initialization

### Local Monitoring
- ✅ **Serial Monitor Support**: Real-time monitoring via USB serial connection
- ✅ **Live Data Display**: Instant water level and percentage updates
- ✅ **Debug Information**: Detailed logging for troubleshooting
- ✅ **No External Dependencies**: Works with just USB connection to computer

## Project Structure

```
/src              → Source code (TX/RX firmware)
  /tx/main.cpp    → Transmitter firmware
  /rx/main.cpp    → Receiver firmware
/include          → Header files
  /config.h       → Configuration constants
/assets           → Future images/visuals
README.md         → This documentation
platformio.ini    → PlatformIO configuration
```

## Hardware Requirements

### TX Node (Transmitter)
- Raspberry Pi Pico
- RAK3172 LoRa module (connected to Serial1)
- Water level sensor (analog output, connected to A0)
- Power supply (5V USB or battery)

### RX Node (Receiver)
- Raspberry Pi Pico
- RAK3172 LoRa module (connected to Serial1)
- Power supply (5V USB)

### Pin Connections
- **RAK3172 Reset**: GPIO 2 (both TX and RX)
- **Water Sensor**: A0 (TX only)
- **Serial1**: GPIO 4 (TX), GPIO 5 (RX) for LoRa communication

## LoRa Configuration

- **Frequency**: 865 MHz (India region)
- **Spreading Factor**: 7
- **Bandwidth**: 125 kHz
- **Coding Rate**: 4/5
- **Preamble Length**: 8
- **TX Power**: 14 dBm

## Installation & Setup

### Prerequisites
- PlatformIO IDE or VSCode with PlatformIO extension
- Raspberry Pi Pico boards (standard or Pico W)
- RAK3172 LoRa modules
- Water level sensor (analog type)

### Software Setup

1. **Clone and Setup Project**
   ```bash
   git clone <repository>
   cd lora-water-tank-monitor
   ```

2. **Open in PlatformIO**
   - Launch PlatformIO IDE
   - Open the project folder

3. **Configure Settings** (Optional)
   - Edit `include/config.h` to customize:
     - LoRa frequency and parameters
     - Tank maximum depth (default: 200cm)
     - Transmission interval (default: 10 seconds)
     - Serial baud rates

4. **Upload Firmware**
   - Connect TX Pico and upload: `pio run -e tx --upload`
   - Connect RX Pico and upload: `pio run -e rx --upload`

## Usage

### Starting the System

1. **Setup Serial Monitors**
   - Connect both TX and RX Pico boards to computer via USB
   - Open two Serial Monitor windows (115200 baud rate)
   - One for TX node, one for RX node

2. **Power On Sequence**
   - Power on TX Pico first (starts transmitting immediately)
   - Power on RX Pico second (starts listening for packets)
   - Both will initialize and begin operation automatically

### Real-Time Monitoring

**TX Node Monitoring:**
```
=== LoRa Water Tank TX Node ===
Initializing...
TX Node initialization complete
Starting water level monitoring...
Transmission interval: 10 seconds

--- Water Level Reading ---
Raw sensor value: 512
Water level: 100 cm
Tank fill: 50%
HEX payload: 0064
LoRa transmission successful
Data sent successfully
```

**RX Node Monitoring:**
```
=== LoRa Water Tank RX Node ===
Initializing LoRa reception...
RX Node initialization complete
Ready to receive LoRa data

LoRa RX: +EVT:RXP2P:0064
Received HEX payload: 0064
--- Water Level Data ---
Water level: 100 cm
Percentage: 50%
```

### Understanding the Data

- **Raw sensor value**: Direct analog reading (0-1023)
- **Water level**: Calculated depth in centimeters (0-200cm)
- **Tank fill**: Percentage of tank capacity (0-100%)
- **HEX payload**: 4-digit hexadecimal for LoRa transmission
- **Transmission status**: Success/failure confirmation

## Configuration Options

### LoRa Parameters (config.h)
```cpp
#define LORA_FREQUENCY 865000000      // 865 MHz
#define LORA_SPREADING_FACTOR 7       // SF7
#define LORA_BANDWIDTH 1              // 125 kHz
#define LORA_CODING_RATE 1            // 4/5
#define LORA_PREAMBLE_LENGTH 8        // 8 symbols
#define LORA_TX_POWER 14              // 14 dBm
```

### System Parameters
```cpp
#define TRANSMISSION_INTERVAL 10000   // 10 seconds
#define TANK_MAX_DEPTH 200            // 200 cm
#define SERIAL_BAUD_RATE 115200       // Serial baud rate
#define LORA_BAUD_RATE 115200         // LoRa module baud rate
```

## Development Status

### Current Implementation
- ✅ Basic LoRa TX/RX communication
- ✅ Analog sensor reading and conversion
- ✅ HEX payload encoding/decoding
- ✅ Serial monitoring and debugging
- ✅ PlatformIO compatibility
- ✅ Raspberry Pi Pico support

### Removed Features (Due to Platform Limitations)
- ❌ WiFi Access Point (not available on standard Pico)
- ❌ Web server interface (WebServer library unavailable)
- ❌ File system logging (LittleFS not compatible)
- ❌ NTP time synchronization (requires WiFi)
- ❌ Historical data storage (no file system)
- ❌ JSON API endpoints (no web server)

### Known Limitations
- No web interface for remote monitoring (WiFi libraries incompatible)
- No data persistence between power cycles
- No real-time clock without external hardware
- Serial monitoring only (no network connectivity)
- HTTP client functionality removed due to framework compatibility issues

## Future Development Opportunities

### Phase 1: Enhanced Monitoring
- Add OLED display for local visualization
- Implement buzzer alerts for low water levels
- Add multiple sensor support
- Improve error handling and recovery

### Phase 2: Data Persistence
- Add SD card module for data logging
- Implement CSV file storage
- Add data export functionality
- Create historical trend analysis

### Phase 3: Connectivity Options
- Add ESP32-based gateway for WiFi connectivity
- Implement MQTT communication
- Create cloud dashboard integration
- Develop mobile app companion

### Phase 3.5: Local Monitoring Enhancements
- Add OLED display for local visualization
- Implement LED status indicators
- Add buzzer alerts for low water levels
- Create simple push-button interface

### Phase 4: Advanced Features
- Multiple tank monitoring
- Automated pump control
- Predictive maintenance alerts
- Machine learning for sensor calibration

## Troubleshooting

### Common Issues

**TX Node:**
- No sensor readings: Check A0 connection and sensor power
- Transmission failures: Verify LoRa module connections and antenna
- Serial output garbled: Check baud rate settings

**RX Node:**
- No packets received: Ensure TX is transmitting and antennas are clear
- Invalid payloads: Check LoRa parameter synchronization
- Serial not responding: Verify Serial1 pin connections

### Debug Tips
- Use Serial Monitor at 115200 baud
- Check LoRa module responses for AT command errors
- Verify power supply stability
- Test with known working configurations

### Local Monitoring Setup
- Connect both TX and RX Pico boards to computer via USB
- Open two Serial Monitor windows (115200 baud)
- Monitor TX for sensor readings and transmission status
- Monitor RX for received packets and decoded data
- Both monitors will show real-time updates every 10 seconds

## Contributing

This project is designed for easy extension and modification. Key areas for contribution:

- Adding new sensor types
- Implementing different communication protocols
- Creating visualization interfaces
- Optimizing power consumption
- Adding security features

## License

This project is open-source. Please refer to the license file for details.

## Support

For issues and questions:
- Check the troubleshooting section
- Review Serial output for error messages
- Verify hardware connections
- Test with minimal configurations

### Local Monitoring Support
- **Serial Monitor**: Primary method for monitoring system status
- **Real-time Updates**: Both TX and RX provide live data every 10 seconds
- **Debug Information**: Comprehensive logging for troubleshooting
- **No External Dependencies**: Works with just USB connection to computer

### Getting Results from Transmitter
1. Connect TX Pico to computer via USB
2. Open Serial Monitor at 115200 baud rate
3. Monitor the output for:
   - Raw sensor values (0-1023)
   - Calculated water level in cm
   - Tank fill percentage (0-100%)
   - HEX payload being transmitted
   - Transmission success/failure status

## Local Monitoring Guide

### How to Monitor Results Locally

1. **Connect Both Devices**
   - Connect TX Pico (with sensor) to computer via USB
   - Connect RX Pico to computer via USB
   - Use two separate USB ports or a USB hub

2. **Open Serial Monitors**
   - Open PlatformIO/VSCode
   - Open Serial Monitor for TX environment (115200 baud)
   - Open Serial Monitor for RX environment (115200 baud)

3. **Monitor TX Output**
   - Shows raw sensor readings
   - Displays calculated water levels
   - Shows transmission status
   - Updates every 10 seconds

4. **Monitor RX Output**
   - Shows received LoRa packets
   - Displays decoded water level data
   - Shows percentage calculations
   - Updates when packets are received

### Understanding the Output

**TX Node Example:**
```
Raw sensor value: 512
Water level: 100 cm
Tank fill: 50%
HEX payload: 0064
LoRa transmission successful
```

**RX Node Example:**
```
LoRa RX: +EVT:RXP2P:0064
Received HEX payload: 0064
Water level: 100 cm
Percentage: 50%
```

### Data Interpretation

- **Raw sensor value**: 0-1023 (higher = more water)
- **Water level**: 0-200 cm (calculated depth)
- **Tank fill**: 0-100% (percentage of capacity)
- **HEX payload**: 4-digit hex for LoRa transmission
- **Status messages**: Success/failure confirmations

---

**Note**: This documentation reflects the current implementation as of the latest update. The system has been simplified from its original ESP32-based design to work with Raspberry Pi Pico's capabilities and available libraries. Local serial monitoring provides real-time access to all system data without requiring network connectivity.
