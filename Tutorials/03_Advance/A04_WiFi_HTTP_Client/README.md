## What this tutorial is??

This tutorial demonstrates making **HTTP REST API requests** from the UNIHIKER K10 to **OpenWeatherMap** (`api.openweathermap.org`), fetching live temperature, humidity, and atmospheric conditions for any configured city, calculating round-trip network latency, and visualizing weather payloads on screen—with a built-in **Wi-Fi & API Configuration Portal** to dynamically configure wireless credentials, location, and API key.

Features:
- **OpenWeatherMap REST Client**: Performs HTTP GET requests to `api.openweathermap.org/data/2.5/weather` to fetch live meteorological telemetry.
- **Configurable Location & API Key**:
  - Location/City name and OpenWeatherMap 32-character API key are configured via the web setup portal.
  - Settings are saved to non-volatile flash memory (`Preferences`).
- **Latency & Status Meter**: Displays real-time HTTP response status code (`200 OK`, `401 Unauthorized`, `404 Not Found`) and latency in milliseconds.
- **Wi-Fi Setup Access Point**: If unconfigured or requested via **Button B**, launches `UNIHIKER-Weather-Setup` on `192.168.4.1` with a web portal to scan Wi-Fi, enter password, and set location & API key.
- **Minimalist Light Theme**: Soft slate off-white canvas (`#F8FAFC`), pure white cards (`#FFFFFF`), delicate 1px borders (`#E2E8F0`), and clean emerald accents (`#059669`).
- **Interactive Buttons**:
  - **Button A**: Fetch live weather data for configured location.
  - **Button B**: Toggle Wi-Fi & Weather Setup Portal.

---

## How it works

1. **Dual Operating Modes**:
   - **Station Mode (`MODE_STA`)**: Connects to the saved Wi-Fi network and executes OpenWeatherMap REST queries.
   - **Access Point Portal Mode (`MODE_ACP`)**: Broadcasts a SoftAP hotspot `UNIHIKER-Weather-Setup` on `192.168.4.1` with an embedded WebServer to configure Wi-Fi, City, and API Key.
2. **NVS Persistent Storage**:
   - Uses ESP32 `Preferences` in namespace `"weather_cfg"` to persist SSID, password, city, and API key across reboots.
3. **OpenWeatherMap Query & Parsing**:
   - Sends: `GET /data/2.5/weather?q={city}&appid={key}&units=metric HTTP/1.1`
   - Parses temperature (`temp`), humidity (`humidity`), and condition (`main`) from JSON response stream.
4. **Dynamic Partial Refresh**:
   - Chrome and card structures are drawn once in `setup()`.
   - On request execution or status change, only the status pill (`200 OK`), latency, and weather data lines update without full-screen flicker.

---

## API and Functions details

| API / Method | Arguments | Return Type | Description |
| :--- | :--- | :--- | :--- |
| `WiFi.softAP(ssid)` | `const char*` | `bool` | Starts SoftAP hotspot for web setup. |
| `server.on(path, method, handler)` | Path, HTTPMethod, Function | `void` | Registers web route handlers. |
| `client.connect(host, port)` | `const char*, uint16_t` | `int` | Opens TCP connection to OpenWeatherMap. |
| `preferences.getString(key, def)` | `const char*, const char*` | `String` | Retrieves stored credentials and settings. |
| `k10.buttonA->isPressed()` | None | `bool` | Sends HTTP GET request for weather data. |
| `k10.buttonB->isPressed()` | None | `bool` | Toggles Wi-Fi & Weather Setup mode. |

---

## Factory Binary

The precompiled factory binary includes the complete bootloader, partition table, and application firmware:
- `A04_WiFi_HTTP_Client.bin` (Flash at `0x00000000`, 16MB DIO 80MHz)
