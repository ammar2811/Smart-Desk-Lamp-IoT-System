# Smart Desk Lamp IoT System (ESP32 + Bluetooth)

## 📌 Overview
This IoT project allows wireless control of a desk lamp using an ESP32 microcontroller and a relay module.  
Commands are sent via a Bluetooth mobile app (Arduino Bluetooth Controller) to toggle the lamp in real time.  
Planned extensions include integrating Google Home voice assistant support.

## ⚙️ Features
- ESP32 microcontroller-based control
- Bluetooth communication with Android Arduino Controller app
- Relay module switching for AC lamp control
- Real-time ON/OFF feedback over serial monitor
- Safe startup/fail-safe logic to prevent electrical faults

## 🧠 System Architecture
- Bluetooth commands are parsed by the ESP32 over UART
- GPIO pin output drives the relay module to toggle power to the lamp

## 🚀 Setup
1. Install Arduino IDE and ESP32 board support
2. Connect the ESP32 to a relay module and lamp wiring (exercise electrical safety!)
3. Load the provided `.ino` code and flash it to the ESP32
4. Pair your phone with the ESP32 over Bluetooth
5. Use the Arduino Bluetooth Controller app to send ON/OFF commands

## 💡 Future Enhancements
- Add Google Home and Alexa integration via MQTT/Webhooks
- Build a companion Android app with custom UI
- Add ambient light sensor for automatic lamp control

## 📜 License
This project is licensed under the MIT License.
