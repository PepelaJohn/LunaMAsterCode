

# MasterCode — Myosa Sensor Drone Telemetry

This Arduino sketch connects a drone’s onboard **Myosa sensor suite** to Wi-Fi, cycles through multiple environmental and motion sensors, collects telemetry, and securely sends the data as JSON to a remote server over HTTPS.

---

## Features
- Reads data from:
  - Accelerometer & Gyroscope
  - Air Quality
  - Barometric Pressure (pressure & altitude)
  - Light, Proximity, and Gesture sensors
  - Temperature and Humidity
- Cycles through sensor displays in 1.5-second intervals.
- Packages sensor readings into a JSON payload.
- Sends the data to a secure HTTPS endpoint (`https://track.lunadrone.com/api/telemetry`) using TLS.
- Uses **root CA certificate pinning** for secure server connection.

---

##  Requirements

- **Arduino IDE 2.x** (or 1.8.x)
- **Board:** ESP32 (WiFi capable)
- **Libraries:**
  - [Myosa Arduino Libraries](https://github.com/myosa-sensors/arduino-libraries)
  - [ArduinoJson](https://arduinojson.org/)
  - WiFi, WiFiClientSecure, HTTPClient (bundled with ESP32 Arduino core)

---

## Installing Myosa Libraries

### Option 1 — From Myosa GitHub
1. Download the Myosa libraries from:  
   [https://github.com/myosa-sensors/arduino-libraries](https://github.com/myosa-sensors/arduino-libraries)
2. In Arduino IDE, go to **Sketch → Include Library → Add .ZIP Library…**
3. Select the downloaded ZIP file.

### Option 2 — From Myosa Documentation  
Follow the official guide here:  
[Adding Myosa Libraries into Arduino IDE](https://wiki.myosa-sensors.org/docs/getting-started/software-setup/adding-myosa-libraries-into-arduino-ide)

---

## Setup & Usage

1. **Open the Sketch**  
   Open `mastercode.ino` in Arduino IDE.

2. **Set Wi-Fi Credentials**  
   Update these lines in the code:
   ```cpp
   const char* ssid = "YOUR_WIFI_SSID";
   const char* password = "YOUR_WIFI_PASSWORD";
   ````

3. **Set Drone ID**
   Update:

   ```cpp
   const char* droneId = "LNDR-001";
   ```

4. **Compile & Upload**
   Select your ESP32 board and port, then click **Upload**.

5. **View Serial Output**
   Open the Serial Monitor at `115200 baud` to see sensor readings and server responses.

---

## How It Works

1. **Initialization** (`setup()`):

   * Starts serial communication.
   * Initializes I²C (Wire) bus at 100 kHz.
   * Initializes Myosa sensors via `myosa.begin()`.
   * Connects to the configured Wi-Fi network.

2. **Loop Execution** (`loop()`):

   * Runs a **1.5 second timer** per sensor display mode.
   * Cycles through:

     ```
     0-3: Accelerometer & Gyroscope
     4: Air Quality
     5-6: Barometric Pressure
     7-8: Light, Proximity, Gesture
     9-10: Temperature & Humidity
     ```
   * Reads latest sensor values for:

     * `temperature` (°C)
     * `pressure` (hPa)
     * `altitude` (m, based on sea-level reference)
     * `speed` (from accelerometer X-axis)
   * Formats data as JSON using `ArduinoJson`.
   * Calls `sendToServer()` to transmit telemetry over HTTPS.

3. **Secure Transmission**:

   * Uses `WiFiClientSecure` with a pinned **root CA certificate**.
   * Sends a POST request to the server with `Content-Type: application/json`.
   * Logs HTTP response code and payload to Serial Monitor.

---

## Security

* **TLS with Root CA Pinning** — prevents MITM attacks by ensuring connection only to trusted server.
* No plaintext credentials are sent after connection is established.

---

##  Potential Improvements

### Switch to MQTT over TLS
Instead of sending JSON via HTTP, use MQTT for lightweight, low-latency, and reliable telemetry.

- **Broker:** `mqtts://broker.lunadrone.com:8883`
- Use **TLS 1.2** with CA certificate pinning, or even **mutual TLS** (client certificate + server certificate) for strong authentication.
- Retained messages for last known status.
- QoS 1 or 2 for guaranteed delivery.

### Non-blocking Wi-Fi reconnect
Automatically detect disconnection and reconnect without blocking sensor loops.

### Async communication
Use libraries like `AsyncMqttClient` or `ESPAsyncTCP` to avoid blocking during network I/O.

### Dynamic interval configuration
Allow changing `perModuleInterval` remotely via MQTT topic  
(e.g., `lunadrone/LNDR-001/config/interval`).

### Encrypted config storage
Store Wi-Fi credentials and keys securely in ESP32 NVS with encryption enabled.

### Failover to LoRa / 4G
If Wi-Fi is unavailable, fall back to cellular or LoRa for data relay.

### OTA updates
Secure over-the-air firmware updates using signed binaries.


---

## 📄 License

MIT License — feel free to modify and use.

---



