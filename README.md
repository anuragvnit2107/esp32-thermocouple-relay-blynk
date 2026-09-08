# ESP32 Thermocouple-Based Relay Controller

> IoT temperature control system using MAX6675 + ESP32 + Blynk App

**Author:** BT22EEE110  
**Platform:** ESP32 DevKit | Arduino Framework  
**Cloud:** Blynk IoT

---

## What It Does

Reads temperature from a **MAX6675 K-type thermocouple** and automatically switches a **relay ON/OFF** based on configurable thresholds — all controllable remotely via the **Blynk mobile app**.

- Relay turns **ON** when temperature drops below the low threshold  
- Relay turns **OFF** when temperature rises above the high threshold  
- Supports **manual override** (Force ON / Force OFF) from the app  
- Live status updates to the Blynk dashboard every second

---

## Hardware

| Component | Details |
|-----------|---------|
| Microcontroller | ESP32 DevKit v1 |
| Temperature Sensor | MAX6675 + K-Type Thermocouple |
| Relay Module | 2-Channel 5V Relay |
| Power | 5V USB / External Supply |

---

## Wiring

### MAX6675 → ESP32

| MAX6675 Pin | ESP32 GPIO |
|-------------|-----------|
| VCC | 3.3V |
| GND | GND |
| SCK | GPIO 5 |
| CS | GPIO 23 |
| SO | GPIO 19 |

### Relay Module → ESP32

| Relay Pin | ESP32 GPIO |
|-----------|-----------|
| IN1 (Induction Relay) | GPIO 26 |
| IN2 (Spare Relay) | GPIO 27 |
| VCC | 5V |
| GND | GND |

---

## Blynk App Setup

Create a new template in Blynk and add these virtual pins:

| Virtual Pin | Widget | Function |
|-------------|--------|----------|
| V0 | Gauge | Live Temperature (°C) |
| V1 | LED | Relay Status |
| V2 | Slider (20–300) | Low Threshold |
| V3 | Slider (20–300) | High Threshold |
| V4 | Segmented Switch | Mode: AUTO / FORCE ON / FORCE OFF |
| V5 | Label | Status String |

---

## Getting Started

### 1. Clone the repo

```bash
git clone https://github.com/YOUR_USERNAME/esp32-induction-controller.git
cd esp32-induction-controller
```

### 2. Install Libraries (Arduino IDE)

- `BlynkSimpleEsp32` — via Library Manager
- `max6675` by Adafruit — via Library Manager

### 3. Add Your Credentials

Open `esp32_induction_controller.ino` and replace the placeholders:

```cpp
#define BLYNK_TEMPLATE_ID   "YOUR_TEMPLATE_ID"
#define BLYNK_AUTH_TOKEN    "YOUR_AUTH_TOKEN"

const char* WIFI_SSID     = "YOUR_SSID";
const char* WIFI_PASSWORD = "YOUR_PASSWORD";
```



### 4. Upload

Select **ESP32 Dev Module** in Arduino IDE, set baud to `115200`, and upload.

---

## Control Modes

| Mode | Behavior |
|------|----------|
| AUTO | Relay controlled automatically by thresholds |
| FORCE ON | Relay always ON regardless of temperature |
| FORCE OFF | Relay always OFF regardless of temperature |

---

## Serial Monitor Output

```
Connecting to WiFi: MyNetwork
....
WiFi Connected | IP: 192.168.1.105
Blynk connected — widgets synced
T:65.2 C | Relay:ON  | Mode:AUTO | Low:60 High:70
T:71.0 C | Relay:OFF | Mode:AUTO | Low:60 High:70
```

---

## Project Structure

```
esp32-induction-controller/
├── esp32_induction_controller.ino   # Main source file
└── README.md
```

---

## License

MIT License — free to use, modify, and distribute with attribution.
