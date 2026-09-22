## What this tutorial is??

This tutorial demonstrates configuring the UNIHIKER K10 in **Wi-Fi Station (STA) mode** with an integrated **Access Point Configuration Portal (ACP Mode)**. Users can wirelessly configure Wi-Fi credentials from any smartphone or PC browser without hardcoding passwords in the sketch.

Features:
- **Dual Operating Modes**:
  - **Station Node (STA)**: Connects to local Wi-Fi, displays live IP, MAC address, and animated 5-tier RSSI signal gauge.
  - **ACP Config Portal (SoftAP)**: Broadcasts an open Wi-Fi hotspot (`UNIHIKER-K10-Setup`) and hosts a responsive Cyberpunk Web portal at `http://192.168.4.1` with active Wi-Fi scanning.
- **NVS Persistent Memory**: Credentials are saved permanently in ESP32-S3 Flash memory using `Preferences`.
- **Interactive Button Controls**:
  - **Button A**: Enter ACP Configuration Portal mode from Station mode.
  - **Button B**: Cancel ACP mode and return to Station mode / Reconnect Wi-Fi.
- **Cyberpunk Cyan & Neon Yellow Theme**: Sci-fi terminal styling (`0x030A14` deep navy background, `0x00E5FF` electric cyan, `0xFFD700` neon yellow, and `0x10B981` status green).
- **Zero-Flicker Partial Refresh**: All transitions and periodic status updates modify only changed UI regions.

---

## How it works

1. **Auto Boot Logic**:
   - On boot, the sketch checks `Preferences` for saved credentials (`ssid` and `pass`).
   - If no credentials exist, it immediately boots into **ACP Config Portal Mode**.
   - If credentials exist, it initializes **Station Mode** and attempts to connect.
2. **Access Point Configuration Portal (ACP Mode)**:
   - Starts SoftAP with SSID `UNIHIKER-K10-Setup` on IP `192.168.4.1`.
   - Starts an embedded HTTP WebServer on port 80.
   - When a user connects to `UNIHIKER-K10-Setup` and opens `http://192.168.4.1`, the page lists available Wi-Fi networks in a dropdown and provides a password field.
   - Submitting the form saves the credentials to flash, displays a confirmation page, shuts down the AP, and connects the K10 to the chosen network.
3. **Dynamic Component Partial Refresh**:
   - Static chrome is drawn once upon mode entry via `drawStaticChrome()`.
   - During STA mode, only the status pill (`ONLINE` / `CONNECTING...`), IP address string, and the 5-tier RSSI stair bars are updated.
   - During ACP mode, the connected client count badge refreshes dynamically when a phone associates or disconnects.

---

## API and Functions details

| API / Method | Arguments | Return Type | Description |
| :--- | :--- | :--- | :--- |
| `preferences.begin("k10wifi", false)` | Name, read-only flag | `bool` | Initializes NVS namespace for Wi-Fi storage. |
| `preferences.putString(key, value)` | Key string, value string | `size_t` | Writes credential to persistent flash memory. |
| `WiFi.softAP(ssid)` | `const char*` | `bool` | Starts SoftAP hotspot for ACP config portal. |
| `WiFi.softAPgetStationNum()` | None | `int` | Returns count of devices connected to K10 hotspot. |
| `server.on(path, handler)` | Path, function pointer | `void` | Binds HTTP web routes (`/` and `/save`). |
| `server.handleClient()` | None | `void` | Processes incoming HTTP configuration requests. |
| `WiFi.mode(WIFI_STA)` | `wifi_mode_t` | `bool` | Switches radio into client station mode. |
| `WiFi.begin(ssid, pass)` | `const char*, const char*` | `wl_status_t` | Connects to configured wireless network. |
| `k10.buttonA->isPressed()` | None | `bool` | Triggers ACP Portal mode from Station mode. |
| `k10.canvas->updateCanvas()` | None | `void` | Flushes graphics buffer to display. |

---

## Full Code

```cpp
#include "unihiker_k10.h"
#include <WiFi.h>
#include <WebServer.h>
#include <Preferences.h>

UNIHIKER_K10 k10;
uint8_t screen_dir = 2; // Portrait (240x320)

WebServer server(80);
Preferences preferences;

// Operating Modes
enum SystemMode {
    MODE_STA,
    MODE_ACP
};

SystemMode currentMode = MODE_STA;

String savedSSID = "";
String savedPass = "";
bool switchRequested = false;
bool lastConnected = false;
int lastRssi = -999;
int lastClientCount = -1;
unsigned long lastStatusCheck = 0;
unsigned long connectStartTime = 0;

// Cyberpunk Cyan & Neon Yellow Color Palette
const uint32_t COLOR_BG        = 0x030A14; // Deep Cyber Navy
const uint32_t COLOR_PANEL     = 0x0A1828; // Panel Card Fill
const uint32_t COLOR_BORDER    = 0x143450; // Panel Border
const uint32_t COLOR_CYAN      = 0x00E5FF; // Electric Cyan
const uint32_t COLOR_YELLOW    = 0xFFD700; // Neon Yellow
const uint32_t COLOR_GREEN     = 0x10B981; // Connected Green
const uint32_t COLOR_RED       = 0xEF4444; // Disconnected Red
const uint32_t COLOR_TEXT_MUTED= 0x64748B; // Slate Muted Text
const uint32_t COLOR_WHITE     = 0xFFFFFF; // Pure White

// Forward declarations
void startStationMode();
void startACPMode();
void drawStaticChrome();
void updateNetworkStatus(bool connected);
void updateSignalMeter(int rssi, bool connected);
void updateACPTelemetry(int clients);

// Web Server Handlers
void handleRoot() {
    int n = WiFi.scanNetworks();
    String options = "";
    for (int i = 0; i < n; ++i) {
        String net = WiFi.SSID(i);
        int r = WiFi.RSSI(i);
        String sel = (net == savedSSID) ? " selected" : "";
        options += "<option value='" + net + "'" + sel + ">" + net + " (" + String(r) + " dBm)</option>";
    }

    String html = "<!DOCTYPE html><html><head><meta name='viewport' content='width=device-width,initial-scale=1'>"
                  "<title>UNIHIKER K10 // ACP Portal</title>"
                  "<style>"
                  "body{background:#030A14;color:#00E5FF;font-family:-apple-system,BlinkMacSystemFont,'Segoe UI',Roboto,sans-serif;margin:0;padding:20px;}"
                  ".card{background:#0A1828;border:1px solid #143450;border-radius:12px;padding:24px;max-width:380px;margin:auto;box-shadow:0 8px 24px rgba(0,0,0,0.5);}"
                  "h2{color:#FFD700;margin-top:0;font-size:1.3rem;letter-spacing:1px;}"
                  "p{color:#94A3B8;font-size:0.9rem;line-height:1.4;}"
                  "label{display:block;margin:14px 0 6px;color:#38BDF8;font-size:0.85rem;font-weight:600;text-transform:uppercase;}"
                  "input,select{width:100%;box-sizing:border-box;padding:12px;background:#030A14;border:1px solid #1E293B;border-radius:8px;color:#FFF;font-size:1rem;outline:none;}"
                  "input:focus,select:focus{border-color:#00E5FF;}"
                  ".btn{width:100%;margin-top:22px;padding:14px;background:#00E5FF;color:#030A14;font-weight:700;font-size:1rem;border:none;border-radius:8px;cursor:pointer;text-transform:uppercase;letter-spacing:1px;}"
                  ".btn:hover{background:#FFD700;}"
                  ".footer{text-align:center;color:#64748B;font-size:0.75rem;margin-top:16px;}"
                  "</style></head><body>"
                  "<div class='card'>"
                  "<h2>&#9889; UNIHIKER K10 ACP</h2>"
                  "<p>Select your local Wi-Fi access point or enter network details to configure station connectivity.</p>"
                  "<form method='POST' action='/save'>"
                  "<label>Nearby Wi-Fi Networks</label>"
                  "<select id='netList' onchange='document.getElementById(\"ssidInput\").value = this.value;'>"
                  "<option value=''>-- Select Scanned Network --</option>" + options + "</select>"
                  "<label>SSID Name</label>"
                  "<input type='text' id='ssidInput' name='ssid' value='" + savedSSID + "' placeholder='Network SSID' required>"
                  "<label>Password</label>"
                  "<input type='password' name='pass' placeholder='WPA2 Key'>"
                  "<button type='submit' class='btn'>Save & Connect</button>"
                  "</form>"
                  "<div class='footer'>UNIHIKER K10 &bull; Cyberpunk IoT Node</div>"
                  "</div></body></html>";

    server.send(200, "text/html", html);
}

void handleSave() {
    if (server.hasArg("ssid")) {
        savedSSID = server.arg("ssid");
        savedPass = server.arg("pass");

        preferences.putString("ssid", savedSSID);
        preferences.putString("pass", savedPass);

        String reply = "<!DOCTYPE html><html><head><meta name='viewport' content='width=device-width,initial-scale=1'>"
                       "<title>Credentials Saved</title>"
                       "<style>body{background:#030A14;color:#10B981;font-family:sans-serif;padding:30px;text-align:center;}"
                       ".box{background:#0A1828;border:1px solid #143450;border-radius:12px;padding:24px;max-width:360px;margin:auto;}"
                       "h2{color:#FFD700;}p{color:#FFF;}</style></head><body>"
                       "<div class='box'><h2>Credentials Saved!</h2>"
                       "<p>Connecting UNIHIKER K10 to <b>" + savedSSID + "</b>...</p>"
                       "<p style='color:#64748B;font-size:0.85rem;'>The configuration hotspot will close now.</p>"
                       "</div></body></html>";
        server.send(200, "text/html", reply);
        delay(1000);
        switchRequested = true;
    } else {
        server.send(400, "text/plain", "Missing SSID");
    }
}

// Draw static chrome layout
void drawStaticChrome() {
    k10.canvas->canvasClear();
    k10.canvas->canvasRectangle(0, 0, 240, 320, COLOR_BG, COLOR_BG, true);

    // Header Bar
    k10.canvas->canvasRectangle(0, 0, 240, 36, COLOR_PANEL, COLOR_PANEL, true);
    k10.canvas->canvasLine(0, 36, 240, 36, (currentMode == MODE_STA) ? COLOR_CYAN : COLOR_YELLOW);

    if (currentMode == MODE_STA) {
        k10.canvas->canvasText("CYBERNET // STA-NODE", 12, 10, COLOR_CYAN, k10.canvas->eCNAndENFont16, 50, false);

        // Network Telemetry Main Card (y: 44 to 228)
        k10.canvas->canvasRectangle(8, 44, 224, 184, COLOR_BORDER, COLOR_PANEL, true);

        k10.canvas->canvasText("NETWORK TARGET", 18, 54, COLOR_TEXT_MUTED, k10.canvas->eCNAndENFont16, 50, false);
        String displaySSID = (savedSSID.length() > 0) ? savedSSID : "[NO SSID CONFIGURED]";
        k10.canvas->canvasText(displaySSID, 18, 72, COLOR_WHITE, k10.canvas->eCNAndENFont16, 50, false);

        k10.canvas->canvasLine(18, 96, 222, 96, COLOR_BORDER);
        k10.canvas->canvasText("LINK STATE", 18, 104, COLOR_TEXT_MUTED, k10.canvas->eCNAndENFont16, 50, false);
        k10.canvas->canvasText("IP ADDRESS", 18, 144, COLOR_TEXT_MUTED, k10.canvas->eCNAndENFont16, 50, false);
        k10.canvas->canvasText("MAC ADDR", 18, 184, COLOR_TEXT_MUTED, k10.canvas->eCNAndENFont16, 50, false);

        String mac = WiFi.macAddress();
        k10.canvas->canvasText(mac, 18, 202, COLOR_YELLOW, k10.canvas->eCNAndENFont16, 50, false);

        // Signal Strength Gauge Box (y: 236 to 286)
        k10.canvas->canvasRectangle(8, 236, 224, 50, COLOR_BORDER, COLOR_PANEL, true);
        k10.canvas->canvasText("SIGNAL (RSSI)", 18, 246, COLOR_TEXT_MUTED, k10.canvas->eCNAndENFont16, 50, false);

        // Bottom Navigation Guide (y: 294 to 320)
        k10.canvas->canvasRectangle(0, 294, 240, 26, COLOR_PANEL, COLOR_PANEL, true);
        k10.canvas->canvasLine(0, 294, 240, 294, COLOR_BORDER);
        k10.canvas->canvasText("[A] ACP Config Portal | [B] Reconnect", 8, 299, COLOR_CYAN, k10.canvas->eCNAndENFont16, 50, false);
    } else {
        k10.canvas->canvasText("CYBERNET // ACP CONFIG", 12, 10, COLOR_YELLOW, k10.canvas->eCNAndENFont16, 50, false);

        // ACP Information Card (y: 44 to 286)
        k10.canvas->canvasRectangle(8, 44, 224, 242, COLOR_BORDER, COLOR_PANEL, true);

        // Mode Pill
        k10.canvas->canvasRectangle(18, 54, 204, 24, 0x78350F, 0x78350F, true);
        k10.canvas->canvasText("ACP PORTAL ACTIVE", 44, 58, COLOR_YELLOW, k10.canvas->eCNAndENFont16, 50, false);

        k10.canvas->canvasText("HOTSPOT SSID", 18, 90, COLOR_TEXT_MUTED, k10.canvas->eCNAndENFont16, 50, false);
        k10.canvas->canvasText("UNIHIKER-K10-Setup", 18, 108, COLOR_WHITE, k10.canvas->eCNAndENFont16, 50, false);

        k10.canvas->canvasText("PORTAL URL", 18, 134, COLOR_TEXT_MUTED, k10.canvas->eCNAndENFont16, 50, false);
        k10.canvas->canvasText("http://192.168.4.1", 18, 152, COLOR_CYAN, k10.canvas->eCNAndENFont16, 50, false);

        k10.canvas->canvasLine(18, 178, 222, 178, COLOR_BORDER);
        k10.canvas->canvasText("INSTRUCTIONS:", 18, 186, COLOR_YELLOW, k10.canvas->eCNAndENFont16, 50, false);
        k10.canvas->canvasText("1. Connect phone to AP", 18, 206, COLOR_WHITE, k10.canvas->eCNAndENFont16, 50, false);
        k10.canvas->canvasText("2. Open 192.168.4.1 in browser", 18, 226, COLOR_WHITE, k10.canvas->eCNAndENFont16, 50, false);
        k10.canvas->canvasText("3. Enter Wi-Fi credentials", 18, 246, COLOR_WHITE, k10.canvas->eCNAndENFont16, 50, false);

        // Bottom Navigation Guide (y: 294 to 320)
        k10.canvas->canvasRectangle(0, 294, 240, 26, COLOR_PANEL, COLOR_PANEL, true);
        k10.canvas->canvasLine(0, 294, 240, 294, COLOR_BORDER);
        k10.canvas->canvasText("[B] Exit / Return to STA Node", 16, 299, COLOR_YELLOW, k10.canvas->eCNAndENFont16, 50, false);
    }

    k10.canvas->updateCanvas();
}

// Partial refresh for STA Link State & IP
void updateNetworkStatus(bool connected) {
    if (currentMode != MODE_STA) return;

    k10.canvas->canvasRectangle(18, 122, 196, 20, COLOR_PANEL, COLOR_PANEL, true);
    k10.canvas->canvasRectangle(18, 162, 196, 20, COLOR_PANEL, COLOR_PANEL, true);

    if (connected) {
        k10.canvas->canvasRectangle(18, 122, 70, 18, COLOR_GREEN, COLOR_GREEN, true);
        k10.canvas->canvasText("ONLINE", 26, 123, COLOR_WHITE, k10.canvas->eCNAndENFont16, 50, false);

        String ipStr = WiFi.localIP().toString();
        k10.canvas->canvasText(ipStr, 18, 162, COLOR_CYAN, k10.canvas->eCNAndENFont16, 50, false);
        k10.rgb->write(-1, 0x00FF88);
    } else {
        k10.canvas->canvasRectangle(18, 122, 110, 18, COLOR_RED, COLOR_RED, true);
        k10.canvas->canvasText("CONNECTING...", 20, 123, COLOR_WHITE, k10.canvas->eCNAndENFont16, 50, false);

        k10.canvas->canvasText("0.0.0.0 (SEARCHING)", 18, 162, COLOR_TEXT_MUTED, k10.canvas->eCNAndENFont16, 50, false);
        k10.rgb->write(-1, 0xFF3300);
    }
    k10.canvas->updateCanvas();
}

// Partial refresh for RSSI stair-bars and dBm readout
void updateSignalMeter(int rssi, bool connected) {
    if (currentMode != MODE_STA) return;

    k10.canvas->canvasRectangle(110, 242, 114, 38, COLOR_PANEL, COLOR_PANEL, true);

    if (!connected) {
        k10.canvas->canvasText("-- dBm", 150, 252, COLOR_TEXT_MUTED, k10.canvas->eCNAndENFont16, 50, false);
        k10.canvas->updateCanvas();
        return;
    }

    int bars = 1;
    if (rssi > -60) bars = 5;
    else if (rssi > -70) bars = 4;
    else if (rssi > -80) bars = 3;
    else if (rssi > -90) bars = 2;

    for (int i = 0; i < 5; i++) {
        int barH = 6 + (i * 4);
        int barX = 116 + (i * 9);
        int barY = 274 - barH;
        uint32_t c = (i < bars) ? COLOR_CYAN : COLOR_BORDER;
        k10.canvas->canvasRectangle(barX, barY, 6, barH, c, c, true);
    }

    String dbmStr = String(rssi) + "dBm";
    k10.canvas->canvasText(dbmStr, 165, 254, COLOR_YELLOW, k10.canvas->eCNAndENFont16, 50, false);
    k10.canvas->updateCanvas();
}

// Partial refresh for ACP client count
void updateACPTelemetry(int clients) {
    if (currentMode != MODE_ACP) return;

    k10.canvas->canvasRectangle(18, 268, 204, 16, COLOR_PANEL, COLOR_PANEL, true);
    String status = "Connected Clients: " + String(clients);
    k10.canvas->canvasText(status, 18, 268, (clients > 0) ? COLOR_GREEN : COLOR_TEXT_MUTED, k10.canvas->eCNAndENFont16, 50, false);
    k10.canvas->updateCanvas();
}

// Start Station Mode
void startStationMode() {
    currentMode = MODE_STA;
    server.close();
    WiFi.softAPdisconnect(true);
    WiFi.mode(WIFI_STA);

    if (savedSSID.length() > 0) {
        WiFi.begin(savedSSID.c_str(), savedPass.c_str());
        connectStartTime = millis();
    }

    drawStaticChrome();
    updateNetworkStatus(false);
    updateSignalMeter(-100, false);
    k10.rgb->write(-1, 0x00E5FF);
}

// Start ACP (Access Point Config Portal) Mode
void startACPMode() {
    currentMode = MODE_ACP;
    WiFi.disconnect(true);
    WiFi.mode(WIFI_AP);
    WiFi.softAP("UNIHIKER-K10-Setup");

    server.on("/", handleRoot);
    server.on("/save", HTTP_POST, handleSave);
    server.begin();

    lastClientCount = -1;
    drawStaticChrome();
    updateACPTelemetry(0);
    k10.rgb->write(-1, 0xFFD700);
}

void setup() {
    Serial.begin(115200);
    k10.begin();
    k10.initScreen(screen_dir);
    k10.creatCanvas();
    k10.rgb->brightness(6);

    preferences.begin("k10wifi", false);
    savedSSID = preferences.getString("ssid", "");
    savedPass = preferences.getString("pass", "");

    // If no credentials saved, launch ACP mode directly
    if (savedSSID.length() == 0) {
        startACPMode();
    } else {
        startStationMode();
    }
}

void loop() {
    // Handle Web Server if in ACP mode
    if (currentMode == MODE_ACP) {
        server.handleClient();

        if (switchRequested) {
            switchRequested = false;
            startStationMode();
            return;
        }

        // Monitor connected stations
        int clients = WiFi.softAPgetStationNum();
        if (clients != lastClientCount) {
            lastClientCount = clients;
            updateACPTelemetry(clients);
        }

        // Button B: Cancel ACP and return to STA mode
        if (k10.buttonB->isPressed()) {
            delay(200);
            startStationMode();
            return;
        }
    } else {
        // In Station Mode
        unsigned long now = millis();
        if (now - lastStatusCheck >= 1000) {
            lastStatusCheck = now;
            bool isConnected = (WiFi.status() == WL_CONNECTED);
            int currentRssi = isConnected ? WiFi.RSSI() : -100;

            if (isConnected != lastConnected) {
                lastConnected = isConnected;
                updateNetworkStatus(isConnected);
                updateSignalMeter(currentRssi, isConnected);
            } else if (isConnected && (abs(currentRssi - lastRssi) >= 2)) {
                lastRssi = currentRssi;
                updateSignalMeter(currentRssi, isConnected);
            }
        }

        // Button A: Enter ACP Mode to reconfigure Wi-Fi
        if (k10.buttonA->isPressed()) {
            delay(200);
            startACPMode();
            return;
        }

        // Button B: Reconnect Wi-Fi
        if (k10.buttonB->isPressed()) {
            delay(200);
            if (savedSSID.length() > 0) {
                WiFi.disconnect();
                WiFi.begin(savedSSID.c_str(), savedPass.c_str());
                updateNetworkStatus(false);
            }
        }
    }

    delay(20);
}
```
