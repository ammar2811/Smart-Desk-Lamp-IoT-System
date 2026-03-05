/*
 * Smart Desk Lamp — Google Home (via Sinric Pro) + WiFiManager
 * 
 * Hardware:
 *   - ESP32 DevKit
 *   - JQC-3FF-S-Z relay module on GPIO 13
 *   - 9W light bulb via relay (240 V AC)
 *   - Optional: push-button on GPIO 14 (INPUT_PULLUP) for manual toggle
 *
 * First boot:
 *   The ESP32 creates a WiFi access point called "DeskLamp-Setup".
 *   Connect to it with your phone and enter your home WiFi credentials.
 *   Credentials are saved to flash — subsequent boots connect automatically.
 *
 * Cloud / Voice control:
 *   Sinric Pro bridges the ESP32 to Google Home. Create a free account at
 *   https://sinric.pro, add a "Switch" device, then paste your APP_KEY,
 *   APP_SECRET, and DEVICE_ID below.
 *
 * Required Arduino libraries (install via Library Manager):
 *   - SinricPro           (by Boris Jaeger)
 *   - WiFiManager         (by tzapu)
 *   - ArduinoJson         (by Benoit Blanchon — dependency of SinricPro)
 */

// ─── Includes ────────────────────────────────────────────────────────────────
#include <WiFi.h>
#include <WiFiManager.h>        // Captive-portal WiFi provisioning
#include <SinricPro.h>
#include <SinricProSwitch.h>

// ─── Sinric Pro credentials (paste yours from the dashboard) ─────────────────
#define APP_KEY    "YOUR_SINRIC_PRO_APP_KEY"
#define APP_SECRET "YOUR_SINRIC_PRO_APP_SECRET"
#define DEVICE_ID  "YOUR_SINRIC_PRO_DEVICE_ID"

// ─── Hardware pins ───────────────────────────────────────────────────────────
#define RELAY_PIN  13     // Relay IN (active HIGH)
#define BTN_PIN    14     // Optional physical push-button (active LOW)

// ─── State ───────────────────────────────────────────────────────────────────
bool lampState = false;   // Tracks current lamp state for cloud sync

// ─── Sinric Pro callback ─────────────────────────────────────────────────────
// Called whenever Google Home (or the Sinric Pro app) sends an ON/OFF command.
bool onPowerState(const String &deviceId, bool &state) {
  lampState = state;
  digitalWrite(RELAY_PIN, lampState ? HIGH : LOW);
  Serial.printf("[SinricPro] Device %s turned %s\n",
                deviceId.c_str(), lampState ? "ON" : "OFF");
  return true;  // Acknowledge success to the cloud
}

// ─── WiFi provisioning via WiFiManager ───────────────────────────────────────
void connectToWiFi() {
  WiFiManager wm;

  // Uncomment the next line to erase saved credentials (useful for testing):
  // wm.resetSettings();

  // Timeout: if no one configures WiFi within 3 minutes, reboot and try again.
  wm.setConfigPortalTimeout(180);

  Serial.println("[WiFi] Starting WiFiManager...");
  if (!wm.autoConnect("DeskLamp-Setup")) {
    Serial.println("[WiFi] Failed to connect — restarting...");
    delay(3000);
    ESP.restart();
  }
  Serial.printf("[WiFi] Connected! IP: %s\n", WiFi.localIP().toString().c_str());
}

// ─── Sinric Pro setup ────────────────────────────────────────────────────────
void setupSinricPro() {
  SinricProSwitch &mySwitch = SinricPro[DEVICE_ID];
  mySwitch.onPowerState(onPowerState);

  SinricPro.onConnected([]() { Serial.println("[SinricPro] Connected to cloud"); });
  SinricPro.onDisconnected([]() { Serial.println("[SinricPro] Disconnected from cloud"); });

  SinricPro.begin(APP_KEY, APP_SECRET);
  Serial.println("[SinricPro] Waiting for cloud connection...");
}

// ─── Physical button handling ────────────────────────────────────────────────
// Simple debounced toggle via a push-button wired between BTN_PIN and GND.
void handleButton() {
  static unsigned long lastPress = 0;
  const unsigned long debounceMs = 250;

  if (digitalRead(BTN_PIN) == LOW && (millis() - lastPress > debounceMs)) {
    lastPress = millis();

    lampState = !lampState;
    digitalWrite(RELAY_PIN, lampState ? HIGH : LOW);
    Serial.printf("[Button] Lamp toggled %s\n", lampState ? "ON" : "OFF");

    // Notify the cloud so Google Home stays in sync
    SinricProSwitch &mySwitch = SinricPro[DEVICE_ID];
    mySwitch.sendPowerStateEvent(lampState);
  }
}

// ─── Arduino entry points ────────────────────────────────────────────────────
void setup() {
  Serial.begin(115200);
  Serial.println("\n=== Smart Desk Lamp ===");

  // Relay — OFF by default (fail-safe)
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);

  // Optional push-button with internal pull-up
  pinMode(BTN_PIN, INPUT_PULLUP);

  // 1. Connect to WiFi (or launch captive portal on first boot)
  connectToWiFi();

  // 2. Initialise Sinric Pro
  setupSinricPro();
}

void loop() {
  SinricPro.handle();   // Maintain WebSocket connection & dispatch callbacks
  handleButton();        // Poll the physical button
}
