/*
 * ESP32 Thermocouple-Based Relay Controller
 * IoT Temperature Control via Blynk App
 *
 * Author : BT22EEE110
 * Board  : ESP32 DevKit
 *
 * Hardware:
 *   MAX6675  → SCK:GPIO5  CS:GPIO23  SO:GPIO19  VCC:3.3V
 *   Relay 1  → GPIO26 (Induction)
 *   Relay 2  → GPIO27 (Spare)
 *
 * Blynk Virtual Pins:
 *   V0 Temperature (Gauge)
 *   V1 Relay Status (LED)
 *   V2 Low Threshold Slider  (20–300°C)
 *   V3 High Threshold Slider (20–300°C)
 *   V4 Mode (0=AUTO, 1=FORCE ON, 2=FORCE OFF)
 *   V5 Status Label
 */

// ── Blynk Config ──────────────────────────────
#define BLYNK_TEMPLATE_ID    "YOUR_TEMPLATE_ID"
#define BLYNK_TEMPLATE_NAME  "Induction Controller"
#define BLYNK_AUTH_TOKEN     "YOUR_AUTH_TOKEN"
#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <max6675.h>

// ── WiFi Credentials (use secrets.h or set here) ──
const char* WIFI_SSID     = "YOUR_SSID";
const char* WIFI_PASSWORD = "YOUR_PASSWORD";

const char* BLYNK_SERVER = "blynk.cloud";
const int   BLYNK_PORT   = 80;

// ── Pin Definitions ───────────────────────────
#define MAX6675_SCK  5
#define MAX6675_CS   23
#define MAX6675_SO   19

#define RELAY_INDUCTION 26
#define RELAY_SPARE     27
#define RELAY_ON  LOW
#define RELAY_OFF HIGH

// ── Thresholds ────────────────────────────────
float tempLow  = 60.0;
float tempHigh = 70.0;

// ── State ─────────────────────────────────────
float currentTemp     = 0.0;
bool  inductionActive = false;
bool  sensorError     = false;
int   controlMode     = 0;   // 0=AUTO  1=FORCE ON  2=FORCE OFF

MAX6675    thermocouple(MAX6675_SCK, MAX6675_CS, MAX6675_SO);
BlynkTimer timer;

// ── Relay Control Logic ───────────────────────
void applyControl() {
  if (controlMode == 1) {
    inductionActive = true;
  } else if (controlMode == 2) {
    inductionActive = false;
  } else {
    if (!sensorError) {
      if (currentTemp < tempLow)  inductionActive = true;
      if (currentTemp > tempHigh) inductionActive = false;
    }
  }
  digitalWrite(RELAY_INDUCTION, inductionActive ? RELAY_ON : RELAY_OFF);
}

// ── Send Data to Blynk ────────────────────────
void sendToBlynk() {
  Blynk.virtualWrite(V0, sensorError ? 0 : currentTemp);
  Blynk.virtualWrite(V1, inductionActive ? 255 : 0);

  String modeStr = (controlMode == 0) ? "AUTO" :
                   (controlMode == 1) ? "MANUAL ON" : "MANUAL OFF";
  String status = sensorError
    ? "SENSOR ERROR - Check Wiring!"
    : "T:" + String(currentTemp, 1) + "C | " + modeStr + " | Relay:" + (inductionActive ? "ON" : "OFF");

  Blynk.virtualWrite(V5, status);
}

// ── Read Thermocouple & Update State ──────────
void readSensor() {
  float reading = thermocouple.readCelsius();
  sensorError = isnan(reading);
  if (!sensorError) currentTemp = reading;

  applyControl();

  if (sensorError) {
    Serial.println("ERROR: Thermocouple not connected!");
  } else {
    Serial.printf("T:%.1f C | Relay:%s | Mode:%s | Low:%.0f High:%.0f\n",
      currentTemp,
      inductionActive ? "ON " : "OFF",
      controlMode == 0 ? "AUTO" : controlMode == 1 ? "FORCE_ON" : "FORCE_OFF",
      tempLow, tempHigh);
  }
}

// ── Blynk Virtual Pin Handlers ────────────────
BLYNK_WRITE(V2) {
  float val = param.asFloat();
  if (val >= 20 && val < tempHigh) { tempLow = val; applyControl(); }
  else Blynk.virtualWrite(V2, tempLow);
}

BLYNK_WRITE(V3) {
  float val = param.asFloat();
  if (val > tempLow && val <= 300) { tempHigh = val; applyControl(); }
  else Blynk.virtualWrite(V3, tempHigh);
}

BLYNK_WRITE(V4) {
  controlMode = param.asInt();
  applyControl();
}

BLYNK_CONNECTED() {
  Blynk.virtualWrite(V2, tempLow);
  Blynk.virtualWrite(V3, tempHigh);
  Blynk.virtualWrite(V4, controlMode);
  Serial.println("Blynk connected — widgets synced");
}

// ── Setup ─────────────────────────────────────
void setup() {
  Serial.begin(115200);
  delay(300);

  pinMode(RELAY_INDUCTION, OUTPUT);
  pinMode(RELAY_SPARE,     OUTPUT);
  digitalWrite(RELAY_INDUCTION, RELAY_OFF);
  digitalWrite(RELAY_SPARE,     RELAY_OFF);

  Serial.printf("Connecting to WiFi: %s\n", WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  int tries = 0;
  while (WiFi.status() != WL_CONNECTED && tries < 40) {
    delay(500); Serial.print("."); tries++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.printf("\nWiFi Connected | IP: %s\n", WiFi.localIP().toString().c_str());
  } else {
    Serial.println("\nWiFi Failed! Check credentials.");
  }

  Blynk.config(BLYNK_AUTH_TOKEN, BLYNK_SERVER, BLYNK_PORT);
  Blynk.connect();

  timer.setInterval(500L,  readSensor);
  timer.setInterval(1000L, sendToBlynk);

  Serial.println("Setup complete.");
}

// ── Loop ──────────────────────────────────────
void loop() {
  Blynk.run();
  timer.run();
}
