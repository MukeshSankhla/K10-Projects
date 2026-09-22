## What this tutorial is??

This tutorial demonstrates running a **Self-Hosted Real-Time Web Dashboard** on the UNIHIKER K10. The device hosts a Wi-Fi Access Point (`K10_SENSOR_HUB`) and an embedded HTTP Web Server serving a glassmorphic web application.

Features:
- **Embedded Web Application**: Self-contained HTML5/CSS3/JavaScript responsive web app stored in Flash PROGMEM.
- **Live Sensor Telemetry**: Streams real-time temperature, humidity, and ambient light readings to web browser cards every 1000ms.
- **Remote RGB Actuation**: Control the K10's onboard RGB LED colors (Blue, Green, Off) with interactive web buttons.
- **Dual Visualizer**: Displays server IP (`http://192.168.4.1`), total HTTP request count, and onboard live metrics on the K10 screen simultaneously.
- **Dark Sapphire Cockpit Theme**: Oceanic sapphire background (`0x030814`), glass panel fill (`0x0B162B`), sky blue (`0x38BDF8`), and neon cyan (`0x22D3EE`).

---

## How it works

1. **Dual Network Server Architecture**:
   - SoftAP operates at `192.168.4.1`.
   - `WebServer server(80)` routes:
     - `GET /`: Returns responsive single-page application (`INDEX_HTML`).
     - `GET /api/sensors`: Returns JSON payload with temperature, humidity, and light.
     - `GET /api/led?color=RRGGBB`: Sets onboard RGB LED.
2. **Dynamic Partial Refresh**:
   - The shell, URL card, titles, and labels are drawn once in `setup()`.
   - In `loop()`, only the numeric values and HTTP hit counter are erased and updated without display blanking.

---

## API and Functions detailes

| API / Method | Arguments | Return Type | Description |
| :--- | :--- | :--- | :--- |
| `server.on(path, handler)` | Path, function pointer | `void` | Registers HTTP endpoint route. |
| `server.send(code, type, body)` | Status code, MIME, Content | `void` | Replies to HTTP client request. |
| `server.handleClient()` | None | `void` | Processes pending web requests. |
| `aht20.getData(...)` | Metric enum | `float` | Reads temperature / humidity. |
| `k10.readALS()` | None | `uint16_t` | Reads ambient light sensor. |
| `k10.canvas->updateCanvas()` | None | `void` | Flushes canvas to display. |

---

## Full Code

```cpp
#include "unihiker_k10.h"
#include <WiFi.h>
#include <WebServer.h>

UNIHIKER_K10 k10;
AHT20 aht20;
uint8_t screen_dir = 2; // Portrait (240x320)

const char* apSsid = "K10_SENSOR_HUB";
const char* apPass = "12345678";

WebServer server(80);
uint32_t httpHits = 0;
unsigned long lastSensorRead = 0;

float currentTemp = 0.0;
float currentHum = 0.0;
int currentLight = 0;

// Dark Sapphire Glass Cockpit Theme
const uint32_t COLOR_BG        = 0x030814; // Deep Oceanic Sapphire
const uint32_t COLOR_PANEL     = 0x0B162B; // Glass Card Fill
const uint32_t COLOR_BORDER    = 0x1A335E; // Sapphire Border
const uint32_t COLOR_SKY       = 0x38BDF8; // Bright Sky Blue
const uint32_t COLOR_INDIGO    = 0x818CF8; // Electric Indigo
const uint32_t COLOR_CYAN      = 0x22D3EE; // Neon Cyan
const uint32_t COLOR_YELLOW    = 0xFBBF24;
const uint32_t COLOR_WHITE     = 0xFFFFFF;
const uint32_t COLOR_MUTED     = 0x64748B;

// Embedded HTML/CSS/JS Single-Page Web Dashboard
const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>UNIHIKER K10 Dashboard</title>
<style>
  body { font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, sans-serif; background: #0b1120; color: #f8fafc; margin: 0; padding: 20px; }
  h1 { font-size: 1.4rem; color: #38bdf8; text-align: center; margin-bottom: 5px; }
  p.sub { text-align: center; color: #94a3b8; font-size: 0.85rem; margin-top: 0; margin-bottom: 20px; }
  .grid { display: grid; grid-template-columns: 1fr 1fr; gap: 12px; max-width: 500px; margin: 0 auto; }
  .card { background: #1e293b; border-radius: 12px; padding: 15px; border: 1px solid #334155; text-align: center; box-shadow: 0 4px 6px -1px rgba(0,0,0,0.3); }
  .card.wide { grid-column: span 2; }
  .label { font-size: 0.75rem; color: #94a3b8; text-transform: uppercase; letter-spacing: 0.05em; }
  .value { font-size: 1.8rem; font-weight: 700; margin: 8px 0; color: #38bdf8; }
  .unit { font-size: 0.9rem; color: #64748b; font-weight: 400; }
  .btn-group { display: flex; gap: 8px; justify-content: center; margin-top: 10px; }
  button { padding: 8px 16px; border-radius: 6px; border: none; font-weight: 600; cursor: pointer; color: #fff; }
  .btn-blue { background: #0284c7; }
  .btn-green { background: #16a34a; }
  .btn-off { background: #dc2626; }
</style>
</head>
<body>
  <h1>UNIHIKER K10 SENSOR HUB</h1>
  <p class="sub">SoftAP Live Telemetry // http://192.168.4.1</p>
  <div class="grid">
    <div class="card">
      <div class="label">Temperature</div>
      <div class="value" id="temp">--<span class="unit"> &deg;C</span></div>
    </div>
    <div class="card">
      <div class="label">Humidity</div>
      <div class="value" id="hum">--<span class="unit"> %</span></div>
    </div>
    <div class="card wide">
      <div class="label">Ambient Light Intensity</div>
      <div class="value" id="light" style="color: #facc15;">--<span class="unit"> Lux</span></div>
    </div>
    <div class="card wide">
      <div class="label">Remote RGB Control</div>
      <div class="btn-group">
        <button class="btn-blue" onclick="setLed('0000FF')">Blue</button>
        <button class="btn-green" onclick="setLed('00FF00')">Green</button>
        <button class="btn-off" onclick="setLed('000000')">Off</button>
      </div>
    </div>
  </div>
<script>
  function fetchSensors() {
    fetch('/api/sensors')
      .then(r => r.json())
      .then(d => {
        document.getElementById('temp').innerHTML = d.temp.toFixed(1) + '<span class="unit"> &deg;C</span>';
        document.getElementById('hum').innerHTML = d.hum.toFixed(1) + '<span class="unit"> %</span>';
        document.getElementById('light').innerHTML = d.light + '<span class="unit"> Lux</span>';
      })
      .catch(e => console.log(e));
  }
  function setLed(color) {
    fetch('/api/led?color=' + color);
  }
  setInterval(fetchSensors, 1000);
  fetchSensors();
</script>
</body>
</html>
)rawliteral";

void handleRoot() {
    httpHits++;
    server.send(200, "text/html", INDEX_HTML);
}

void handleApiSensors() {
    httpHits++;
    String json = "{";
    json += "\"temp\":" + String(currentTemp, 2) + ",";
    json += "\"hum\":" + String(currentHum, 2) + ",";
    json += "\"light\":" + String(currentLight);
    json += "}";
    server.send(200, "application/json", json);
}

void handleApiLed() {
    httpHits++;
    if (server.hasArg("color")) {
        String colStr = server.arg("color");
        long col = strtol(colStr.c_str(), NULL, 16);
        k10.rgb->write(-1, col);
    }
    server.send(200, "text/plain", "OK");
}

void drawStaticChrome() {
    k10.canvas->canvasClear();
    k10.canvas->canvasRectangle(0, 0, 240, 320, COLOR_BG, COLOR_BG, true);

    // Sapphire Cockpit Header
    k10.canvas->canvasRectangle(0, 0, 240, 36, COLOR_PANEL, COLOR_PANEL, true);
    k10.canvas->canvasLine(0, 36, 240, 36, COLOR_SKY);
    k10.canvas->canvasText("K10 WEB SERVER DASHBOARD", 10, 10, COLOR_SKY, k10.canvas->eCNAndENFont16, 50, false);

    // Web Access URL Card (y: 44 to 102)
    k10.canvas->canvasRectangle(8, 44, 224, 58, COLOR_BORDER, COLOR_PANEL, true);
    k10.canvas->canvasText("CONNECT WIFI & BROWSE", 16, 52, COLOR_MUTED, k10.canvas->eCNAndENFont16, 50, false);
    k10.canvas->canvasText("http://192.168.4.1", 16, 72, COLOR_YELLOW, k10.canvas->eCNAndENFont24, 50, false);

    // Sensor Telemetry Mini Pods (y: 110 to 226)
    k10.canvas->canvasRectangle(8, 110, 224, 116, COLOR_BORDER, COLOR_PANEL, true);
    k10.canvas->canvasText("ONBOARD LIVE TELEMETRY", 16, 118, COLOR_MUTED, k10.canvas->eCNAndENFont16, 50, false);
    k10.canvas->canvasLine(16, 134, 224, 134, COLOR_BORDER);

    k10.canvas->canvasText("TEMP:", 16, 144, COLOR_SKY, k10.canvas->eCNAndENFont16, 50, false);
    k10.canvas->canvasText("HUMID:", 16, 170, COLOR_CYAN, k10.canvas->eCNAndENFont16, 50, false);
    k10.canvas->canvasText("LIGHT:", 16, 196, COLOR_YELLOW, k10.canvas->eCNAndENFont16, 50, false);

    // HTTP Traffic Card (y: 234 to 280)
    k10.canvas->canvasRectangle(8, 234, 224, 46, COLOR_BORDER, COLOR_PANEL, true);
    k10.canvas->canvasText("HTTP REQUESTS: ", 16, 248, COLOR_MUTED, k10.canvas->eCNAndENFont16, 50, false);

    // Footer Status Prompt (y: 288 to 320)
    k10.canvas->canvasRectangle(0, 288, 240, 32, COLOR_PANEL, COLOR_PANEL, true);
    k10.canvas->canvasLine(0, 288, 240, 288, COLOR_BORDER);
    k10.canvas->canvasText("HOTSPOT: K10_SENSOR_HUB", 16, 296, COLOR_SKY, k10.canvas->eCNAndENFont16, 50, false);

    k10.canvas->updateCanvas();
}

// Partial refresh for on-screen telemetry
void updateDashboardTelemetry(float t, float h, int l, uint32_t hits) {
    k10.canvas->canvasRectangle(80, 142, 140, 22, COLOR_PANEL, COLOR_PANEL, true);
    k10.canvas->canvasText(String(t, 1) + " C", 80, 144, COLOR_WHITE, k10.canvas->eCNAndENFont16, 50, false);

    k10.canvas->canvasRectangle(80, 168, 140, 22, COLOR_PANEL, COLOR_PANEL, true);
    k10.canvas->canvasText(String(h, 1) + " %", 80, 170, COLOR_WHITE, k10.canvas->eCNAndENFont16, 50, false);

    k10.canvas->canvasRectangle(80, 194, 140, 22, COLOR_PANEL, COLOR_PANEL, true);
    k10.canvas->canvasText(String(l) + " Lux", 80, 196, COLOR_WHITE, k10.canvas->eCNAndENFont16, 50, false);

    k10.canvas->canvasRectangle(140, 244, 84, 26, COLOR_PANEL, COLOR_PANEL, true);
    k10.canvas->canvasText(String(hits), 144, 246, COLOR_SKY, k10.canvas->eCNAndENFont16, 50, false);

    k10.canvas->updateCanvas();
}

void setup() {
    Serial.begin(115200);
    k10.begin();
    k10.initScreen(screen_dir);
    k10.creatCanvas();
    k10.rgb->brightness(6);

    WiFi.mode(WIFI_AP);
    WiFi.softAP(apSsid, apPass);

    server.on("/", handleRoot);
    server.on("/api/sensors", handleApiSensors);
    server.on("/api/led", handleApiLed);
    server.begin();

    drawStaticChrome();
}

void loop() {
    server.handleClient();

    unsigned long now = millis();
    if (now - lastSensorRead >= 800) {
        lastSensorRead = now;
        currentTemp = aht20.getData(AHT20::eAHT20TempC);
        currentHum = aht20.getData(AHT20::eAHT20HumiRH);
        currentLight = k10.readALS();
        updateDashboardTelemetry(currentTemp, currentHum, currentLight, httpHits);
    }

    delay(10);
}
```
