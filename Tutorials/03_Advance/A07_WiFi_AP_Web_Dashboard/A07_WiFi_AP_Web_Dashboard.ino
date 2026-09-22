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

// Clean Minimalist Light Theme Palette
const uint32_t COLOR_BG        = 0xF8FAFC; // Soft Slate Off-White
const uint32_t COLOR_CARD      = 0xFFFFFF; // Pure White Card Fill
const uint32_t COLOR_BORDER    = 0xE2E8F0; // Delicate 1px Border
const uint32_t COLOR_TEXT_PRI  = 0x0F172A; // Deep Slate Charcoal
const uint32_t COLOR_TEXT_SEC  = 0x334155; // Slate Secondary
const uint32_t COLOR_TEXT_MUTED= 0x64748B; // Slate Muted Label
const uint32_t COLOR_SKY       = 0x0284C7; // Sky Blue Brand Accent
const uint32_t COLOR_CORAL     = 0xE11D48; // Coral Rose
const uint32_t COLOR_TEAL      = 0x0D9488; // Teal
const uint32_t COLOR_AMBER     = 0xD97706; // Amber
const uint32_t COLOR_GREEN     = 0x16A34A; // Success Green
const uint32_t COLOR_WHITE     = 0xFFFFFF;

// Embedded Clean Modern Light Web Dashboard
const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>UNIHIKER K10 Dashboard</title>
<style>
  body { font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, sans-serif; background: #F8FAFC; color: #0F172A; margin: 0; padding: 24px 16px; }
  .container { max-width: 480px; margin: 0 auto; }
  h1 { font-size: 1.35rem; color: #0F172A; text-align: center; margin: 0 0 6px; }
  p.sub { text-align: center; color: #64748B; font-size: 0.85rem; margin: 0 0 20px; }
  .grid { display: grid; grid-template-columns: 1fr 1fr; gap: 12px; margin-bottom: 16px; }
  .card { background: #FFFFFF; border-radius: 12px; padding: 18px; border: 1px solid #E2E8F0; text-align: center; box-shadow: 0 2px 8px rgba(15,23,42,0.04); }
  .card.wide { grid-column: span 2; }
  .label { font-size: 0.75rem; color: #64748B; text-transform: uppercase; letter-spacing: 0.05em; font-weight: 600; }
  .value { font-size: 1.7rem; font-weight: 700; margin: 8px 0; color: #0284C7; }
  .unit { font-size: 0.85rem; color: #64748B; font-weight: 400; }
  .btn-group { display: flex; gap: 8px; justify-content: center; margin-top: 10px; }
  button { padding: 10px 18px; border-radius: 8px; border: none; font-weight: 600; cursor: pointer; color: #FFFFFF; font-size: 0.85rem; }
  .btn-blue { background: #0284C7; }
  .btn-green { background: #16A34A; }
  .btn-off { background: #DC2626; }
</style>
</head>
<body>
  <div class="container">
    <h1>Sensor Dashboard</h1>
    <p class="sub">SoftAP Live Telemetry &bull; http://192.168.4.1</p>
    <div class="grid">
      <div class="card">
        <div class="label">Temperature</div>
        <div class="value" id="temp">-- <span class="unit">&deg;C</span></div>
      </div>
      <div class="card">
        <div class="label">Humidity</div>
        <div class="value" id="hum">-- <span class="unit">%</span></div>
      </div>
      <div class="card wide">
        <div class="label">Ambient Light</div>
        <div class="value" id="light">-- <span class="unit">Lux</span></div>
      </div>
      <div class="card wide">
        <div class="label">Onboard RGB Light</div>
        <div class="btn-group">
          <button class="btn-blue" onclick="fetch('/rgb?c=blue')">Blue</button>
          <button class="btn-green" onclick="fetch('/rgb?c=green')">Green</button>
          <button class="btn-off" onclick="fetch('/rgb?c=off')">Off</button>
        </div>
      </div>
    </div>
  </div>
<script>
  function updateData() {
    fetch('/api/data').then(r => r.json()).then(d => {
      document.getElementById('temp').innerHTML = d.temp.toFixed(1) + ' <span class="unit">&deg;C</span>';
      document.getElementById('hum').innerHTML = d.hum.toFixed(1) + ' <span class="unit">%</span>';
      document.getElementById('light').innerHTML = d.light + ' <span class="unit">Lux</span>';
    });
  }
  setInterval(updateData, 1000);
  updateData();
</script>
</body>
</html>
)rawliteral";

void handleRoot() {
    httpHits++;
    server.send_P(200, "text/html", INDEX_HTML);
}

void handleData() {
    httpHits++;
    String json = "{";
    json += "\"temp\":" + String(currentTemp, 1) + ",";
    json += "\"hum\":" + String(currentHum, 1) + ",";
    json += "\"light\":" + String(currentLight) + ",";
    json += "\"hits\":" + String(httpHits);
    json += "}";
    server.send(200, "application/json", json);
}

void handleRGB() {
    httpHits++;
    if (server.hasArg("c")) {
        String c = server.arg("c");
        if (c == "blue") {
            k10.rgb->write(-1, 0x0284C7);
        } else if (c == "green") {
            k10.rgb->write(-1, 0x16A34A);
        } else {
            k10.rgb->write(-1, 0x000000);
        }
    }
    server.send(200, "text/plain", "OK");
}

void drawStaticChrome() {
    k10.canvas->canvasClear();
    k10.canvas->canvasRectangle(0, 0, 240, 320, COLOR_BG, COLOR_BG, true);

    // App Header Bar (y: 0 to 40)
    k10.canvas->canvasRectangle(0, 0, 240, 40, COLOR_CARD, COLOR_CARD, true);
    k10.canvas->canvasLine(0, 40, 240, 40, COLOR_BORDER);
    k10.canvas->canvasText("Sensor Dashboard", 14, 12, COLOR_TEXT_PRI, k10.canvas->eCNAndENFont16, 50, false);
    // Sky Blue brand dot
    k10.canvas->canvasCircle(224, 20, 4, COLOR_SKY, COLOR_SKY, true);

    // Hotspot Info Card (y: 48 to 102)
    k10.canvas->canvasRectangle(10, 48, 220, 54, COLOR_BORDER, COLOR_CARD, true);
    k10.canvas->canvasText("HOTSPOT / WEB URL", 20, 56, COLOR_TEXT_MUTED, k10.canvas->eCNAndENFont16, 50, false);
    k10.canvas->canvasText("http://192.168.4.1", 20, 74, COLOR_SKY, k10.canvas->eCNAndENFont16, 50, false);

    // Temperature Pod (y: 110 to 164)
    k10.canvas->canvasRectangle(10, 110, 220, 54, COLOR_BORDER, COLOR_CARD, true);
    k10.canvas->canvasText("TEMPERATURE", 20, 118, COLOR_TEXT_MUTED, k10.canvas->eCNAndENFont16, 50, false);

    // Humidity Pod (y: 172 to 226)
    k10.canvas->canvasRectangle(10, 172, 220, 54, COLOR_BORDER, COLOR_CARD, true);
    k10.canvas->canvasText("HUMIDITY", 20, 180, COLOR_TEXT_MUTED, k10.canvas->eCNAndENFont16, 50, false);

    // Light Sensor Pod (y: 234 to 284)
    k10.canvas->canvasRectangle(10, 234, 220, 50, COLOR_BORDER, COLOR_CARD, true);
    k10.canvas->canvasText("AMBIENT LIGHT", 20, 242, COLOR_TEXT_MUTED, k10.canvas->eCNAndENFont16, 50, false);

    // Footer Info Bar (y: 290 to 320)
    k10.canvas->canvasRectangle(0, 290, 240, 30, COLOR_CARD, COLOR_CARD, true);
    k10.canvas->canvasLine(0, 290, 240, 290, COLOR_BORDER);
    k10.canvas->canvasText("Embedded HTTP Web Server", 14, 296, COLOR_TEXT_MUTED, k10.canvas->eCNAndENFont16, 50, false);

    k10.canvas->updateCanvas();
}

// Partial refresh for on-screen sensor readouts
void updateScreenTelemetry(float temp, float hum, int light, uint32_t hits) {
    // Clear temp value
    k10.canvas->canvasRectangle(20, 136, 190, 20, COLOR_CARD, COLOR_CARD, true);
    String tStr = String(temp, 1) + "  deg C";
    k10.canvas->canvasText(tStr, 20, 136, COLOR_CORAL, k10.canvas->eCNAndENFont16, 50, false);

    // Clear hum value
    k10.canvas->canvasRectangle(20, 198, 190, 20, COLOR_CARD, COLOR_CARD, true);
    String hStr = String(hum, 1) + "  % RH";
    k10.canvas->canvasText(hStr, 20, 198, COLOR_TEAL, k10.canvas->eCNAndENFont16, 50, false);

    // Clear light value
    k10.canvas->canvasRectangle(20, 258, 190, 20, COLOR_CARD, COLOR_CARD, true);
    String lStr = String(light) + "  Lux";
    k10.canvas->canvasText(lStr, 20, 258, COLOR_AMBER, k10.canvas->eCNAndENFont16, 50, false);

    // Clear hit counter in header
    k10.canvas->canvasRectangle(140, 56, 80, 16, COLOR_CARD, COLOR_CARD, true);
    String reqStr = "Hits: " + String(hits);
    k10.canvas->canvasText(reqStr, 150, 56, COLOR_GREEN, k10.canvas->eCNAndENFont16, 50, false);

    k10.canvas->updateCanvas();
}

void setup() {
    Serial.begin(115200);
    k10.begin();
    k10.initScreen(screen_dir);
    k10.creatCanvas();
    k10.rgb->brightness(5);

    WiFi.mode(WIFI_AP);
    WiFi.softAP(apSsid, apPass);

    server.on("/", handleRoot);
    server.on("/api/data", handleData);
    server.on("/rgb", handleRGB);
    server.begin();

    drawStaticChrome();
    updateScreenTelemetry(0.0, 0.0, 0, 0);
}

void loop() {
    server.handleClient();

    unsigned long now = millis();
    if (now - lastSensorRead >= 1000) {
        lastSensorRead = now;
        currentTemp = aht20.getData(AHT20::eAHT20TempC);
        currentHum = aht20.getData(AHT20::eAHT20HumiRH);
        currentLight = k10.readALS();

        updateScreenTelemetry(currentTemp, currentHum, currentLight, httpHits);
    }
    delay(20);
}
