# Smart Desk Lamp IoT System (ESP32 + Google Home)

## 📌 Overview
Control a desk lamp with **voice commands via Google Home** (or the Google Home app) using an ESP32 microcontroller and a relay module.  
The ESP32 connects to your WiFi network and communicates with **Sinric Pro**, a free cloud service that bridges it to Google Home.  
On first boot, a **WiFiManager captive portal** lets you enter WiFi credentials without hardcoding them.

## ⚙️ Features
- **Google Home voice control** — *"Hey Google, turn on Desk Lamp"*
- **Sinric Pro cloud bridge** — WebSocket-based, low-latency, free tier
- **WiFiManager captive portal** — no hardcoded WiFi credentials
- **Optional physical push-button** — manual toggle on GPIO 14, with bidirectional cloud sync
- **Relay module switching** for AC lamp control (240 V, 9 W bulb)
- **Fail-safe startup** — relay defaults to OFF
- **Serial monitor logging** — connection status and lamp events at 115200 baud

## 🧩 Hardware

| Component | Details |
|---|---|
| ESP32 DevKit V1 | Standard dual-core (WiFi + BT) |
| Relay module | JQC-3FF-S-Z (SRD-05VDC-SL-C), signal on **GPIO 13** |
| Light bulb | 9 W bulb in socket, switched via relay NO/C terminals |
| Power supply | 240 V AC mains (exercise extreme caution!) |
| Push-button *(optional)* | Momentary, wired between **GPIO 14** and **GND** |

> **Wiring summary** (from Fritzing diagram):
> - **Relay IN** → GPIO 13 (yellow wire)
> - **Relay VCC** → 3.3 V rail (red wire)
> - **Relay GND** → GND rail (black wire)
> - **Relay NO + C** → in series with the live wire of the lamp socket
> - **Bulb neutral** → directly to mains neutral

## 🧠 System Architecture

```
Google Home ──► Sinric Pro Cloud ──► (WebSocket) ──► ESP32
                                                       │
                                                   GPIO 13
                                                       │
                                                  Relay ──► Lamp
```

1. Google Home sends an ON/OFF intent to Sinric Pro's cloud.
2. Sinric Pro pushes the command over a persistent WebSocket to the ESP32.
3. The ESP32's `onPowerState` callback toggles the relay on GPIO 13.
4. If the physical button is pressed, the ESP32 toggles locally **and** sends a state event back to Sinric Pro so Google Home stays in sync.

## 🚀 Setup

### Prerequisites
1. **Arduino IDE** with ESP32 board support installed.
2. **Arduino libraries** (install via *Sketch → Include Library → Manage Libraries*):
   - `SinricPro` (by Boris Jaeger)
   - `WiFiManager` (by tzapu)
   - `ArduinoJson` (by Benoit Blanchon) — installed automatically as a SinricPro dependency
3. A free **Sinric Pro** account — sign up at <https://sinric.pro>.

### Sinric Pro Configuration
1. Log in to the Sinric Pro dashboard.
2. Go to **Devices → Add Device** → choose type **Switch** → name it (e.g. *Desk Lamp*).
3. Copy your **APP_KEY**, **APP_SECRET**, and the new **DEVICE_ID**.
4. Open `esp32BluetoothLamp.ino` and replace the three placeholder strings:
   ```cpp
   #define APP_KEY    "YOUR_SINRIC_PRO_APP_KEY"
   #define APP_SECRET "YOUR_SINRIC_PRO_APP_SECRET"
   #define DEVICE_ID  "YOUR_SINRIC_PRO_DEVICE_ID"
   ```

### Google Home Linking
1. Open the **Google Home** app on your phone.
2. Tap **+** → **Set up device** → **Works with Google**.
3. Search for **Sinric Pro** and link your account.
4. Your *Desk Lamp* device will appear automatically.

### Flash & First Boot
1. Connect the ESP32 via USB and select the correct board/port in Arduino IDE.
2. Upload the sketch.
3. On first boot the ESP32 creates a WiFi AP called **`DeskLamp-Setup`**.
4. Connect to it with your phone — a captive portal will open.
5. Enter your home WiFi SSID and password → the ESP32 saves them and reboots.
6. Open the Serial Monitor (115200 baud) to confirm:
   ```
   [WiFi] Connected! IP: 192.168.x.x
   [SinricPro] Connected to cloud
   ```
7. Say **"Hey Google, turn on Desk Lamp"** — the relay should click and the lamp turns on. 🎉

### Resetting WiFi Credentials
Uncomment this line in `connectToWiFi()` and re-flash:
```cpp
// wm.resetSettings();
```
The captive portal will reappear on the next boot.

## 📂 File Structure
```
├── esp32BluetoothLamp.ino   # Main Arduino sketch
└── README.md                # This file
```

## 💡 Future Enhancements
- Add **Alexa** integration (Sinric Pro supports it natively — just link in the Alexa app)
- Add an **ambient light sensor** (LDR / BH1750) for automatic lamp control
- Introduce **dimming** using a TRIAC/SSR and Sinric Pro's "Dimmable Switch" device type
- Build a **companion web dashboard** using Sinric Pro's REST API
- Add **OTA updates** for wireless firmware flashing

## ⚠️ Safety Warning
This project involves **240 V AC mains wiring**. Incorrect wiring can cause electric shock, fire, or death.  
- Never work on the circuit while it is connected to mains power.
- Use properly rated wires and connectors.
- If you are unsure, consult a qualified electrician.

## 📜 License
This project is licensed under the MIT License.
