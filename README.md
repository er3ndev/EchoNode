# EchoNode // Wi-Fi Pentest Suite

<p align="center">
  <img src="https://img.shields.io/badge/Platform-ESP8266-blue?style=flat-square" />
  <img src="https://img.shields.io/badge/Framework-Arduino-teal?style=flat-square" />
  <img src="https://img.shields.io/badge/Build-PlatformIO-orange?style=flat-square" />
  <img src="https://img.shields.io/badge/Version-2.0-green?style=flat-square" />
  <img src="https://img.shields.io/badge/License-MIT-yellow?style=flat-square" />
</p>

**EchoNode** is a lightweight, self-contained Wi-Fi penetration testing toolkit built for the ESP8266 (Wemos D1 Mini). It provides a sleek, mobile-friendly web dashboard for real-time network reconnaissance and 802.11 frame injection — all from a device that fits in your pocket.

---

## ✨ Features

| Feature | Description |
|---|---|
| **Wi-Fi Scanner** | Discover nearby access points with SSID, BSSID, channel, RSSI, and encryption info |
| **Deauthentication** | Send deauth/disassociation frames to disconnect clients from a target network |
| **Beacon Flood** | Broadcast up to 20 fake SSIDs with randomized MACs to spam the airwaves |
| **Live Dashboard** | Real-time system metrics — chip ID, free RAM, flash size, CPU freq, uptime, connected clients |
| **Live Console** | Timestamped event log for monitoring all operations in real time |
| **Gzip-Compressed UI** | Dashboard served as a gzip-compressed payload for fast load times over AP |
| **Mobile-First Design** | Fully responsive — designed to be used from a phone connected to the AP |

---

## 🛠 Hardware

- **Wemos D1 Mini** (ESP8266) or any ESP8266-based board
- Micro-USB cable
- That's it.

---

## 🚀 Getting Started

### Prerequisites

- [PlatformIO](https://platformio.org/) (VSCode extension or CLI)

### Build & Flash

```bash
# Clone the repo
git clone https://github.com/er3ndev/EchoNode.git
cd EchoNode

# Build and upload
pio run -t upload

# Monitor serial output (optional)
pio device monitor -b 115200
```

### Connect

1. Power up the board
2. Connect to the **`EchoNode_Admin`** Wi-Fi network
3. Password: `StrongPassword123`
4. Open **`http://192.168.4.1`** in your browser

> **Tip:** Change the default AP credentials in `EchoNode.ino` before deploying.

---

## 📱 Dashboard

The web interface runs entirely on the ESP8266 — no internet connection required. It features a dark, terminal-inspired design with four tabs:

- **Dashboard** — System vitals and device info
- **Scanner** — Scan and list nearby Wi-Fi networks, select targets
- **Attacks** — Launch Deauth or Beacon Flood attacks
- **Console** — Live operation log with color-coded entries

---

## 📁 Project Structure

```
EchoNode/
├── EchoNode.ino        # Main firmware — AP setup, web server, attack logic
├── Dashboard.h         # Raw HTML/CSS/JS for the web UI
├── Dashboard_gz.h      # Gzip-compressed version served to clients
├── platformio.ini      # PlatformIO build configuration
└── README.md
```

---

## ⚙️ Configuration

Edit the top of `EchoNode.ino` to customize:

```cpp
const char* AP_SSID     = "EchoNode_Admin";    // Access point name
const char* AP_PASSWORD = "StrongPassword123";  // AP password
```

---

## 🔧 How It Works

### Scanning
The scanner uses synchronous `WiFi.scanNetworks()` with a delayed trigger to ensure the HTTP response reaches the client before the radio switches to scan mode. Includes retry logic for cold-start scenarios where the radio needs warm-up time.

### Deauthentication
Sends IEEE 802.11 deauthentication (subtype 0xC0) and disassociation (subtype 0xA0) management frames via `wifi_send_pkt_freedom()`. Targets are selected from scan results.

### Beacon Flood
Generates beacon frames with randomized source MACs and user-defined SSIDs. Channels are randomized per frame to maximize visibility across the spectrum.

---

## ⚠️ Disclaimer

> This project is built **strictly for educational purposes and authorized security testing**.
>
> Unauthorized use of deauthentication attacks or beacon flooding on networks you do not own or have explicit permission to test is **illegal** in most jurisdictions.
>
> The author takes **no responsibility** for any misuse of this tool. Use it responsibly and ethically.

---

## 📄 License

This project is licensed under the [MIT License](LICENSE).

---

## 🤝 Contributing

Pull requests are welcome. For major changes, open an issue first to discuss what you'd like to change.

---

<p align="center">
  <sub>Built with ☕ and mass amounts of frustration debugging ESP8266 Wi-Fi stacks.</sub>
</p>
