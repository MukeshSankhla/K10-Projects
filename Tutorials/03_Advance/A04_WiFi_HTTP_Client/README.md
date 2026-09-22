## What this tutorial is??

This tutorial demonstrates making **HTTP REST API requests** from the UNIHIKER K10 to cloud endpoints, calculating round-trip network latency, and visualizing response payloads on screen.

Features:
- **Cloud REST Client**: Performs HTTP GET requests to `worldtimeapi.org` over TCP port 80.
- **Latency & Status Meter**: Displays real-time HTTP response status code (e.g. `200 OK`) and latency in milliseconds.
- **Emerald Teal Cloud Theme**: Visual design with deep forest emerald background (`0x03140E`), bright emerald (`0x10B981`), mint teal (`0x34D399`), and gold metrics (`0xFACC15`).
- **Interactive Button Trigger**: Press Button A to immediately execute a new request and benchmark network performance.

---

## How it works

1. **Lightweight Socket-Level HTTP Client**:
   - Uses `WiFiClient` to establish a direct TCP connection to the host on port 80.
   - Transmits standard HTTP/1.1 headers (`GET /api/timezone/Etc/UTC HTTP/1.1\r\nHost: worldtimeapi.org\r\nConnection: close\r\n\r\n`).
   - Parses the HTTP status line and payload timestamp string.
2. **Dynamic Partial Refresh**:
   - The shell, target host banner, metric titles, and button legend are drawn once in `setup()`.
   - On request execution, only the status code pill (`200 OK`), latency text (`142 ms`), and payload terminal lines are erased and redrawn inside their bounding boxes.

---

## API and Functions detailes

| API / Method | Arguments | Return Type | Description |
| :--- | :--- | :--- | :--- |
| `client.connect(host, port)` | `const char*, uint16_t` | `int` | Opens TCP connection to remote HTTP server. |
| `client.print(str)` | `String` / `const char*` | `size_t` | Sends raw HTTP request headers. |
| `client.readStringUntil(char)` | Delimiter character | `String` | Reads response stream up to line break. |
| `k10.buttonA->isPressed()` | None | `bool` | Trigger button for re-sending API requests. |
| `k10.canvas->updateCanvas()` | None | `void` | Flushes buffer to display. |

---

## Full Code

```cpp
#include "unihiker_k10.h"
#include <WiFi.h>

UNIHIKER_K10 k10;
uint8_t screen_dir = 2; // Portrait (240x320)

// Wi-Fi Configuration
const char* ssid = "UNIHIKER_WIFI";
const char* password = "password123";

// Target HTTP Host & Path
const char* host = "worldtimeapi.org";
const int httpPort = 80;
const char* path = "/api/timezone/Etc/UTC";

// Emerald Teal Cloud Bridge Theme
const uint32_t COLOR_BG        = 0x03140E; // Deep Forest Emerald
const uint32_t COLOR_PANEL     = 0x082419; // Panel Fill
const uint32_t COLOR_BORDER    = 0x144A34; // Panel Border
const uint32_t COLOR_EMERALD   = 0x10B981; // Bright Emerald
const uint32_t COLOR_TEAL      = 0x34D399; // Mint Teal
const uint32_t COLOR_YELLOW    = 0xFACC15;
const uint32_t COLOR_RED       = 0xEF4444;
const uint32_t COLOR_WHITE     = 0xFFFFFF;
const uint32_t COLOR_MUTED     = 0x6EE7B7;

void drawStaticChrome() {
    k10.canvas->canvasClear();
    k10.canvas->canvasRectangle(0, 0, 240, 320, COLOR_BG, COLOR_BG, true);

    // Header Bar
    k10.canvas->canvasRectangle(0, 0, 240, 36, COLOR_PANEL, COLOR_PANEL, true);
    k10.canvas->canvasLine(0, 36, 240, 36, COLOR_EMERALD);
    k10.canvas->canvasText("CLOUD REST API CLIENT", 12, 10, COLOR_EMERALD, k10.canvas->eCNAndENFont16, 50, false);

    // Endpoint Card (y: 44 to 90)
    k10.canvas->canvasRectangle(8, 44, 224, 46, COLOR_BORDER, COLOR_PANEL, true);
    k10.canvas->canvasText("TARGET HOST", 16, 52, COLOR_MUTED, k10.canvas->eCNAndENFont16, 50, false);
    k10.canvas->canvasText("worldtimeapi.org:80", 16, 68, COLOR_WHITE, k10.canvas->eCNAndENFont16, 50, false);

    // Performance Metrics Pod (y: 96 to 156)
    k10.canvas->canvasRectangle(8, 96, 224, 60, COLOR_BORDER, COLOR_PANEL, true);
    k10.canvas->canvasText("HTTP STATUS", 16, 104, COLOR_MUTED, k10.canvas->eCNAndENFont16, 50, false);
    k10.canvas->canvasText("LATENCY", 136, 104, COLOR_MUTED, k10.canvas->eCNAndENFont16, 50, false);
    k10.canvas->canvasLine(124, 102, 124, 150, COLOR_BORDER);

    // Payload Terminal Window (y: 162 to 276)
    k10.canvas->canvasRectangle(8, 162, 224, 114, COLOR_BORDER, COLOR_PANEL, true);
    k10.canvas->canvasText("RESPONSE PAYLOAD", 16, 170, COLOR_MUTED, k10.canvas->eCNAndENFont16, 50, false);
    k10.canvas->canvasLine(16, 188, 224, 188, COLOR_BORDER);

    // Footer Control Prompt (y: 284 to 320)
    k10.canvas->canvasRectangle(0, 284, 240, 36, COLOR_PANEL, COLOR_PANEL, true);
    k10.canvas->canvasLine(0, 284, 240, 284, COLOR_BORDER);
    k10.canvas->canvasText("BTN A: Send API Request", 16, 292, COLOR_TEAL, k10.canvas->eCNAndENFont16, 50, false);

    k10.canvas->updateCanvas();
}

// Partial refresh for HTTP status and latency metrics
void updateMetrics(int code, unsigned long latencyMs) {
    k10.canvas->canvasRectangle(16, 122, 102, 28, COLOR_PANEL, COLOR_PANEL, true);
    k10.canvas->canvasRectangle(136, 122, 88, 28, COLOR_PANEL, COLOR_PANEL, true);

    if (code == 200) {
        k10.canvas->canvasRectangle(16, 124, 86, 22, COLOR_EMERALD, COLOR_EMERALD, true);
        k10.canvas->canvasText("200 OK", 24, 127, COLOR_WHITE, k10.canvas->eCNAndENFont16, 50, false);
        k10.canvas->canvasText(String(latencyMs) + " ms", 136, 126, COLOR_YELLOW, k10.canvas->eCNAndENFont16, 50, false);
        k10.rgb->write(-1, 0x00FF88);
    } else if (code > 0) {
        k10.canvas->canvasRectangle(16, 124, 86, 22, COLOR_YELLOW, COLOR_YELLOW, true);
        k10.canvas->canvasText(String(code), 24, 127, COLOR_BG, k10.canvas->eCNAndENFont16, 50, false);
        k10.canvas->canvasText(String(latencyMs) + " ms", 136, 126, COLOR_WHITE, k10.canvas->eCNAndENFont16, 50, false);
        k10.rgb->write(-1, 0xFFBB00);
    } else {
        k10.canvas->canvasRectangle(16, 124, 86, 22, COLOR_RED, COLOR_RED, true);
        k10.canvas->canvasText("OFFLINE", 22, 127, COLOR_WHITE, k10.canvas->eCNAndENFont16, 50, false);
        k10.canvas->canvasText("-- ms", 136, 126, COLOR_MUTED, k10.canvas->eCNAndENFont16, 50, false);
        k10.rgb->write(-1, 0xFF2200);
    }

    k10.canvas->updateCanvas();
}

// Partial refresh for response payload text lines
void updatePayloadDisplay(String line1, String line2, String line3) {
    k10.canvas->canvasRectangle(12, 194, 216, 76, COLOR_PANEL, COLOR_PANEL, true);

    k10.canvas->canvasText(line1, 16, 196, COLOR_WHITE, k10.canvas->eCNAndENFont16, 50, false);
    k10.canvas->canvasText(line2, 16, 218, COLOR_TEAL, k10.canvas->eCNAndENFont16, 50, false);
    k10.canvas->canvasText(line3, 16, 240, COLOR_MUTED, k10.canvas->eCNAndENFont16, 50, false);

    k10.canvas->updateCanvas();
}

void executeApiRequest() {
    if (WiFi.status() != WL_CONNECTED) {
        updateMetrics(-1, 0);
        updatePayloadDisplay("No Wi-Fi Connection!", "Configure SSID in sketch.", "Waiting for connection...");
        return;
    }

    updatePayloadDisplay("Connecting to Host...", host, "");

    WiFiClient client;
    unsigned long start = millis();

    if (!client.connect(host, httpPort)) {
        updateMetrics(0, 0);
        updatePayloadDisplay("Connection Failed!", "Host unreachable.", "");
        return;
    }

    // Send HTTP GET request
    client.print(String("GET ") + path + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" +
                 "User-Agent: UNIHIKER_K10\r\n" +
                 "Connection: close\r\n\r\n");

    unsigned long timeout = millis();
    while (client.available() == 0) {
        if (millis() - timeout > 3000) {
            client.stop();
            updateMetrics(408, 3000);
            updatePayloadDisplay("Request Timeout!", "No response from server.", "");
            return;
        }
    }

    unsigned long latency = millis() - start;

    String statusLine = client.readStringUntil('\r');
    int httpCode = 0;
    if (statusLine.indexOf("200") != -1) {
        httpCode = 200;
    } else {
        httpCode = 500;
    }

    updateMetrics(httpCode, latency);

    while (client.connected()) {
        String line = client.readStringUntil('\n');
        if (line == "\r") break;
    }

    String body = client.readString();
    int timeIdx = body.indexOf("\"datetime\":\"");
    String timeSnippet = "";
    if (timeIdx != -1) {
        timeSnippet = body.substring(timeIdx + 12, timeIdx + 31);
    } else {
        timeSnippet = body.substring(0, min((int)body.length(), 24));
    }

    updatePayloadDisplay("GET 200 Success (" + String(body.length()) + "B)", timeSnippet, "UTC Server Timestamp");
    client.stop();
}

void setup() {
    Serial.begin(115200);
    k10.begin();
    k10.initScreen(screen_dir);
    k10.creatCanvas();
    k10.rgb->brightness(6);

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    drawStaticChrome();
    updateMetrics(-1, 0);
    updatePayloadDisplay("Press BTN A to execute", "GET request to Cloud API.", "SSID: " + String(ssid));
}

void loop() {
    static bool lastBtnA = false;
    bool btnA = k10.buttonA->isPressed();
    if (btnA && !lastBtnA) {
        executeApiRequest();
    }
    lastBtnA = btnA;

    delay(40);
}
```
