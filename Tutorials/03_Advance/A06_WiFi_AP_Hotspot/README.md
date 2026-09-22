## What this tutorial is??

This tutorial demonstrates configuring the UNIHIKER K10 in **SoftAP (Access Point) Mode**, hosting its own standalone Wi-Fi hotspot (`UNIHIKER_K10_AP`) with an onboard DHCP server.

Features:
- **Autonomous Wi-Fi Hotspot**: Broadcasts an independent 802.11 b/g/n network with configurable SSID and WPA2 security passphrase.
- **Client Station Monitor**: Real-time connected client node counter (`WiFi.softAPgetStationNum()`) tracking connecting phones/laptops.
- **Sunset Amber & Orange Theme**: Warm sunset aesthetic with black-crimson background (`0x140602`), amber panels (`0x240F05`), sunset orange (`0xF97316`), and golden yellow (`0xFDE047`).
- **Pulsing Radio Waveform**: Animated concentric radio broadcast waves radiating from the antenna icon.

---

## How it works

1. **SoftAP Initialization**:
   - `WiFi.mode(WIFI_AP)` initializes the ESP32-S3 radio in Access Point mode.
   - `WiFi.softAP(apSsid, apPassword)` starts the network and activates the internal DHCP server (assigning IPs starting from `192.168.4.2`).
2. **Dynamic Partial Refresh**:
   - Static chrome (header, credentials card, telemetry shell, antenna base dot, and footer) is drawn once during `setup()`.
   - In `loop()`, only the radio broadcast wave rings and the connected client counter card are refreshed without full-screen canvas clears.

---

## API and Functions detailes

| API / Method | Arguments | Return Type | Description |
| :--- | :--- | :--- | :--- |
| `WiFi.mode(WIFI_AP)` | Mode | `bool` | Sets radio to Access Point mode. |
| `WiFi.softAP(ssid, pass)` | SSID, Password | `bool` | Starts SoftAP broadcast and DHCP server. |
| `WiFi.softAPIP()` | None | `IPAddress` | Returns SoftAP gateway IP (`192.168.4.1`). |
| `WiFi.softAPgetStationNum()` | None | `int` | Returns count of currently connected client devices. |
| `k10.canvas->canvasCircle(...)` | x, y, r, border, fill, bool | `void` | Renders radio wave pulses. |
| `k10.canvas->updateCanvas()` | None | `void` | Flushes canvas buffer to display. |

---

## Full Code

```cpp
#include "unihiker_k10.h"
#include <WiFi.h>

UNIHIKER_K10 k10;
uint8_t screen_dir = 2; // Portrait (240x320)

// SoftAP Hotspot Credentials
const char* apSsid = "UNIHIKER_K10_AP";
const char* apPassword = "password123";

// Sunset Amber & Orange Hotspot Theme
const uint32_t COLOR_BG        = 0x140602; // Deep Sunset Black-Crimson
const uint32_t COLOR_PANEL     = 0x240F05; // Sunset Amber Panel
const uint32_t COLOR_BORDER    = 0x4E220D; // Amber Border
const uint32_t COLOR_ORANGE    = 0xF97316; // Vibrant Sunset Orange
const uint32_t COLOR_AMBER     = 0xFBBF24; // Warm Golden Amber
const uint32_t COLOR_YELLOW    = 0xFDE047; // Bright Yellow
const uint32_t COLOR_GREEN     = 0x22C55E; // Green Connected
const uint32_t COLOR_WHITE     = 0xFFFFFF;
const uint32_t COLOR_MUTED     = 0xBA7A58;

int lastStationCount = -1;
int waveRadius = 12;
unsigned long lastWaveTick = 0;

void drawStaticChrome() {
    k10.canvas->canvasClear();
    k10.canvas->canvasRectangle(0, 0, 240, 320, COLOR_BG, COLOR_BG, true);

    // Header Bar
    k10.canvas->canvasRectangle(0, 0, 240, 36, COLOR_PANEL, COLOR_PANEL, true);
    k10.canvas->canvasLine(0, 36, 240, 36, COLOR_ORANGE);
    k10.canvas->canvasText("K10 WI-FI ACCESS POINT", 12, 10, COLOR_ORANGE, k10.canvas->eCNAndENFont16, 50, false);

    // Hotspot Credentials Pod (y: 44 to 154)
    k10.canvas->canvasRectangle(8, 44, 224, 110, COLOR_BORDER, COLOR_PANEL, true);
    k10.canvas->canvasText("HOTSPOT CREDENTIALS", 16, 52, COLOR_MUTED, k10.canvas->eCNAndENFont16, 50, false);
    k10.canvas->canvasLine(16, 68, 224, 68, COLOR_BORDER);

    k10.canvas->canvasText("SSID: ", 16, 76, COLOR_AMBER, k10.canvas->eCNAndENFont16, 50, false);
    k10.canvas->canvasText(apSsid, 70, 76, COLOR_WHITE, k10.canvas->eCNAndENFont16, 50, false);

    k10.canvas->canvasText("PASS: ", 16, 96, COLOR_AMBER, k10.canvas->eCNAndENFont16, 50, false);
    k10.canvas->canvasText(apPassword, 70, 96, COLOR_WHITE, k10.canvas->eCNAndENFont16, 50, false);

    k10.canvas->canvasText("GATEWAY: ", 16, 118, COLOR_AMBER, k10.canvas->eCNAndENFont16, 50, false);
    k10.canvas->canvasText(WiFi.softAPIP().toString(), 92, 118, COLOR_YELLOW, k10.canvas->eCNAndENFont16, 50, false);

    // Station Telemetry Pod (y: 162 to 276)
    k10.canvas->canvasRectangle(8, 162, 224, 114, COLOR_BORDER, COLOR_PANEL, true);
    k10.canvas->canvasText("CONNECTED CLIENTS", 16, 170, COLOR_MUTED, k10.canvas->eCNAndENFont16, 50, false);
    k10.canvas->canvasLine(16, 188, 224, 188, COLOR_BORDER);

    // Static Antenna Base Dot
    k10.canvas->canvasCircle(60, 232, 6, COLOR_ORANGE, COLOR_ORANGE, true);

    // Footer Control Prompt (y: 284 to 320)
    k10.canvas->canvasRectangle(0, 284, 240, 36, COLOR_PANEL, COLOR_PANEL, true);
    k10.canvas->canvasLine(0, 284, 240, 284, COLOR_BORDER);
    k10.canvas->canvasText("BROADCASTING CH 1 // 802.11", 16, 292, COLOR_ORANGE, k10.canvas->eCNAndENFont16, 50, false);

    k10.canvas->updateCanvas();
}

// Partial refresh for connected client count badge
void updateStationDisplay(int count) {
    k10.canvas->canvasRectangle(12, 200, 104, 66, COLOR_PANEL, COLOR_PANEL, true);

    String countStr = String(count) + " NODES";
    uint32_t countColor = (count > 0) ? COLOR_GREEN : COLOR_AMBER;
    k10.canvas->canvasText(countStr, 126, 214, countColor, k10.canvas->eCNAndENFont24, 50, false);

    String statusStr = (count > 0) ? "Clients Active" : "Waiting Clients";
    k10.canvas->canvasText(statusStr, 126, 244, COLOR_MUTED, k10.canvas->eCNAndENFont16, 50, false);

    if (count > 0) {
        k10.rgb->write(-1, 0x00FF88);
    } else {
        k10.rgb->write(-1, 0xFF6600);
    }

    k10.canvas->updateCanvas();
}

// Partial refresh for concentric radio beacon wave animation
void updateBeaconWave() {
    k10.canvas->canvasRectangle(18, 194, 84, 76, COLOR_PANEL, COLOR_PANEL, true);

    // Center antenna dot
    k10.canvas->canvasCircle(60, 232, 4, COLOR_YELLOW, COLOR_YELLOW, true);

    // Draw active wave ring
    k10.canvas->canvasCircle(60, 232, waveRadius, COLOR_ORANGE, COLOR_PANEL, false);
    if (waveRadius > 18) {
        k10.canvas->canvasCircle(60, 232, waveRadius - 12, COLOR_AMBER, COLOR_PANEL, false);
    }

    waveRadius += 3;
    if (waveRadius > 34) {
        waveRadius = 10;
    }

    k10.canvas->updateCanvas();
}

void setup() {
    Serial.begin(115200);
    k10.begin();
    k10.initScreen(screen_dir);
    k10.creatCanvas();
    k10.rgb->brightness(6);

    WiFi.mode(WIFI_AP);
    WiFi.softAP(apSsid, apPassword);

    drawStaticChrome();
    updateStationDisplay(0);
}

void loop() {
    unsigned long now = millis();

    if (now - lastWaveTick >= 120) {
        lastWaveTick = now;
        updateBeaconWave();
    }

    static unsigned long lastStationCheck = 0;
    if (now - lastStationCheck >= 1000) {
        lastStationCheck = now;
        int currentCount = WiFi.softAPgetStationNum();
        if (currentCount != lastStationCount) {
            lastStationCount = currentCount;
            updateStationDisplay(currentCount);
        }
    }

    delay(20);
}
```
