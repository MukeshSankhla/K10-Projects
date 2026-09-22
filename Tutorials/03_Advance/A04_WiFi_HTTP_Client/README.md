## What this tutorial is??

This tutorial demonstrates making **HTTP REST API requests** from the UNIHIKER K10 to cloud endpoints, calculating round-trip network latency, and visualizing response payloads on screen—with a built-in **Wi-Fi AP Configuration Portal** to dynamically configure wireless credentials without re-flashing.

Features:
- **Cloud REST Client**: Performs HTTP GET requests to `worldtimeapi.org` over TCP port 80.
- **Latency & Status Meter**: Displays real-time HTTP response status code (e.g. `200 OK`) and latency in milliseconds.
- **Wi-Fi Setup Access Point**: If unconfigured or requested via **Button B**, launches `UNIHIKER-HTTP-Setup` on `192.168.4.1` with a web portal to scan and save local Wi-Fi credentials to NVS storage.
- **Minimalist Light Theme**: Soft slate off-white canvas (`#F8FAFC`), pure white cards (`#FFFFFF`), delicate 1px borders (`#E2E8F0`), and clean emerald accents (`#059669`).
- **Interactive Buttons**:
  - **Button A**: Fetch API data from `worldtimeapi.org`.
  - **Button B**: Toggle Wi-Fi AP Setup Portal.

---

## How it works

1. **Dual Operating Modes**:
   - **Station Mode (`MODE_STA`)**: Connects to the saved Wi-Fi network and enables HTTP client operations.
   - **Access Point Portal Mode (`MODE_ACP`)**: Broadcasts a SoftAP hotspot `UNIHIKER-HTTP-Setup` on `192.168.4.1` with an embedded WebServer to scan and configure Wi-Fi credentials.
2. **NVS Persistent Storage**:
   - Uses ESP32 `Preferences` in namespace `"wifi_cfg"` to persist credentials across reboots.
3. **Dynamic Partial Refresh**:
   - Headers and card containers are drawn once in `setup()`.
   - On request execution or status change, only the status pill (`200 OK`), latency, and payload lines are updated without full-screen flicker.

---

## API and Functions details

| API / Method | Arguments | Return Type | Description |
| :--- | :--- | :--- | :--- |
| `WiFi.softAP(ssid)` | `const char*` | `bool` | Starts SoftAP hotspot for web setup. |
| `server.on(path, method, handler)` | Path, HTTPMethod, Function | `void` | Registers web route handlers. |
| `client.connect(host, port)` | `const char*, uint16_t` | `int` | Opens TCP connection to remote HTTP server. |
| `preferences.getString(key, def)` | `const char*, const char*` | `String` | Retrieves stored Wi-Fi credentials. |
| `k10.buttonA->isPressed()` | None | `bool` | Sends HTTP GET request. |
| `k10.buttonB->isPressed()` | None | `bool` | Toggles Wi-Fi AP Setup mode. |

---

## Factory Binary

The precompiled factory binary includes the complete bootloader, partition table, and application firmware:
- `A04_WiFi_HTTP_Client.bin` (Flash at `0x00000000`, 16MB DIO 80MHz)
