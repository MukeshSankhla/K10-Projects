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

// Minimalist Clean Light Theme Palette
const uint32_t COLOR_BG        = 0xF8FAFC; // Soft Slate Off-White
const uint32_t COLOR_CARD      = 0xFFFFFF; // Pure White Card Fill
const uint32_t COLOR_BORDER    = 0xE2E8F0; // Delicate 1px Border
const uint32_t COLOR_DIVIDER   = 0xF1F5F9; // Soft Inner Divider
const uint32_t COLOR_TEXT_PRI  = 0x0F172A; // Deep Slate Charcoal
const uint32_t COLOR_TEXT_SEC  = 0x334155; // Slate Secondary
const uint32_t COLOR_TEXT_MUTED= 0x64748B; // Slate Muted Label
const uint32_t COLOR_BLUE      = 0x2563EB; // Sapphire Brand Accent
const uint32_t COLOR_AMBER     = 0xD97706; // Warm Amber
const uint32_t COLOR_GREEN     = 0x16A34A; // Clean Emerald
const uint32_t COLOR_RED       = 0xDC2626; // Soft Crimson Red
const uint32_t COLOR_WHITE     = 0xFFFFFF;

// Forward declarations
void startStationMode();
void startACPMode();
void drawStaticChrome();
void updateNetworkStatus(bool connected);
void updateSignalMeter(int rssi, bool connected);
void updateACPTelemetry(int clients);

// Web Server Handlers (Modern Clean Light Webpage)
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
                  "<title>UNIHIKER K10 - Wi-Fi Setup</title>"
                  "<style>"
                  "body{background:#F8FAFC;color:#0F172A;font-family:-apple-system,BlinkMacSystemFont,'Segoe UI',Roboto,sans-serif;margin:0;padding:24px 16px;}"
                  ".card{background:#FFFFFF;border:1px solid #E2E8F0;border-radius:16px;padding:28px 24px;max-width:380px;margin:auto;box-shadow:0 4px 16px rgba(15,23,42,0.06);}"
                  "h2{color:#0F172A;margin:0 0 8px;font-size:1.35rem;font-weight:700;}"
                  "p{color:#64748B;font-size:0.9rem;line-height:1.5;margin:0 0 20px;}"
                  "label{display:block;margin:14px 0 6px;color:#334155;font-size:0.82rem;font-weight:600;text-transform:uppercase;letter-spacing:0.5px;}"
                  "input,select{width:100%;box-sizing:border-box;padding:12px 14px;background:#F8FAFC;border:1px solid #CBD5E1;border-radius:10px;color:#0F172A;font-size:0.95rem;outline:none;transition:border-color 0.2s;}"
                  "input:focus,select:focus{border-color:#2563EB;background:#FFFFFF;}"
                  ".btn{width:100%;margin-top:24px;padding:14px;background:#2563EB;color:#FFFFFF;font-weight:600;font-size:1rem;border:none;border-radius:10px;cursor:pointer;transition:background 0.2s;}"
                  ".btn:hover{background:#1D4ED8;}"
                  ".footer{text-align:center;color:#94A3B8;font-size:0.75rem;margin-top:20px;}"
                  "</style></head><body>"
                  "<div class='card'>"
                  "<h2>Wi-Fi Setup</h2>"
                  "<p>Select your wireless network or enter details below to connect your UNIHIKER K10.</p>"
                  "<form method='POST' action='/save'>"
                  "<label>Available Networks</label>"
                  "<select id='netList' onchange='document.getElementById(\"ssidInput\").value = this.value;'>"
                  "<option value=''>-- Select Scanned Network --</option>" + options + "</select>"
                  "<label>Network SSID</label>"
                  "<input type='text' id='ssidInput' name='ssid' value='" + savedSSID + "' placeholder='Enter Wi-Fi name' required>"
                  "<label>Password</label>"
                  "<input type='password' name='pass' placeholder='Enter Wi-Fi password'>"
                  "<button type='submit' class='btn'>Connect Device</button>"
                  "</form>"
                  "<div class='footer'>UNIHIKER K10 Device Portal</div>"
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
                       "<title>Settings Saved</title>"
                       "<style>body{background:#F8FAFC;color:#0F172A;font-family:sans-serif;padding:36px 16px;text-align:center;}"
                       ".box{background:#FFFFFF;border:1px solid #E2E8F0;border-radius:16px;padding:32px 24px;max-width:360px;margin:auto;box-shadow:0 4px 16px rgba(15,23,42,0.06);}"
                       "h2{color:#16A34A;margin-top:0;}p{color:#334155;line-height:1.5;}</style></head><body>"
                       "<div class='box'><h2>Settings Saved</h2>"
                       "<p>Connecting UNIHIKER K10 to <b>" + savedSSID + "</b>...</p>"
                       "<p style='color:#64748B;font-size:0.85rem;'>The setup hotspot is closing now.</p>"
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

    // Clean App Header Bar (y: 0 to 40)
    k10.canvas->canvasRectangle(0, 0, 240, 40, COLOR_CARD, COLOR_CARD, true);
    k10.canvas->canvasLine(0, 40, 240, 40, COLOR_BORDER);

    if (currentMode == MODE_STA) {
        k10.canvas->canvasText("Wi-Fi Settings", 14, 12, COLOR_TEXT_PRI, k10.canvas->eCNAndENFont16, 50, false);
        // Small Sapphire brand dot
        k10.canvas->canvasCircle(224, 20, 4, COLOR_BLUE, COLOR_BLUE, true);

        // Main Card (y: 48 to 226)
        k10.canvas->canvasRectangle(10, 48, 220, 178, COLOR_BORDER, COLOR_CARD, true);

        k10.canvas->canvasText("CONNECTED NETWORK", 20, 58, COLOR_TEXT_MUTED, k10.canvas->eCNAndENFont16, 50, false);
        String displaySSID = (savedSSID.length() > 0) ? savedSSID : "Not Configured";
        k10.canvas->canvasText(displaySSID, 20, 76, COLOR_TEXT_PRI, k10.canvas->eCNAndENFont16, 50, false);

        k10.canvas->canvasLine(20, 98, 220, 98, COLOR_BORDER);
        k10.canvas->canvasText("STATUS", 20, 106, COLOR_TEXT_MUTED, k10.canvas->eCNAndENFont16, 50, false);
        k10.canvas->canvasText("IP ADDRESS", 20, 144, COLOR_TEXT_MUTED, k10.canvas->eCNAndENFont16, 50, false);
        k10.canvas->canvasText("MAC ADDRESS", 20, 182, COLOR_TEXT_MUTED, k10.canvas->eCNAndENFont16, 50, false);

        String mac = WiFi.macAddress();
        k10.canvas->canvasText(mac, 20, 198, COLOR_TEXT_SEC, k10.canvas->eCNAndENFont16, 50, false);

        // Signal Card (y: 234 to 280)
        k10.canvas->canvasRectangle(10, 234, 220, 46, COLOR_BORDER, COLOR_CARD, true);
        k10.canvas->canvasText("SIGNAL STRENGTH", 20, 248, COLOR_TEXT_MUTED, k10.canvas->eCNAndENFont16, 50, false);

        // Footer Bar (y: 288 to 320)
        k10.canvas->canvasRectangle(0, 288, 240, 32, COLOR_CARD, COLOR_CARD, true);
        k10.canvas->canvasLine(0, 288, 240, 288, COLOR_BORDER);
        k10.canvas->canvasText("[A] Setup Portal   [B] Reconnect", 14, 296, COLOR_BLUE, k10.canvas->eCNAndENFont16, 50, false);
    } else {
        k10.canvas->canvasText("Hotspot Setup", 14, 12, COLOR_TEXT_PRI, k10.canvas->eCNAndENFont16, 50, false);
        // Small Amber dot
        k10.canvas->canvasCircle(224, 20, 4, COLOR_AMBER, COLOR_AMBER, true);

        // Setup Card (y: 48 to 280)
        k10.canvas->canvasRectangle(10, 48, 220, 232, COLOR_BORDER, COLOR_CARD, true);

        // Active Badge
        k10.canvas->canvasRectangle(20, 58, 120, 22, COLOR_BORDER, COLOR_BG, true);
        k10.canvas->canvasText("PORTAL ACTIVE", 28, 62, COLOR_AMBER, k10.canvas->eCNAndENFont16, 50, false);

        k10.canvas->canvasText("HOTSPOT NAME", 20, 90, COLOR_TEXT_MUTED, k10.canvas->eCNAndENFont16, 50, false);
        k10.canvas->canvasText("UNIHIKER-K10-Setup", 20, 108, COLOR_TEXT_PRI, k10.canvas->eCNAndENFont16, 50, false);

        k10.canvas->canvasText("WEB ADDRESS", 20, 132, COLOR_TEXT_MUTED, k10.canvas->eCNAndENFont16, 50, false);
        k10.canvas->canvasText("http://192.168.4.1", 20, 150, COLOR_BLUE, k10.canvas->eCNAndENFont16, 50, false);

        k10.canvas->canvasLine(20, 174, 220, 174, COLOR_BORDER);
        k10.canvas->canvasText("HOW TO CONNECT:", 20, 182, COLOR_TEXT_PRI, k10.canvas->eCNAndENFont16, 50, false);
        k10.canvas->canvasText("1. Connect phone to hotspot", 20, 202, COLOR_TEXT_SEC, k10.canvas->eCNAndENFont16, 50, false);
        k10.canvas->canvasText("2. Open 192.168.4.1 in browser", 20, 222, COLOR_TEXT_SEC, k10.canvas->eCNAndENFont16, 50, false);
        k10.canvas->canvasText("3. Choose network & connect", 20, 242, COLOR_TEXT_SEC, k10.canvas->eCNAndENFont16, 50, false);

        // Footer Bar (y: 288 to 320)
        k10.canvas->canvasRectangle(0, 288, 240, 32, COLOR_CARD, COLOR_CARD, true);
        k10.canvas->canvasLine(0, 288, 240, 288, COLOR_BORDER);
        k10.canvas->canvasText("[B] Exit Setup Mode", 14, 296, COLOR_AMBER, k10.canvas->eCNAndENFont16, 50, false);
    }

    k10.canvas->updateCanvas();
}

// Partial refresh for STA Link State & IP
void updateNetworkStatus(bool connected) {
    if (currentMode != MODE_STA) return;

    // Clear status pill area
    k10.canvas->canvasRectangle(20, 122, 190, 18, COLOR_CARD, COLOR_CARD, true);
    // Clear IP area
    k10.canvas->canvasRectangle(20, 160, 190, 18, COLOR_CARD, COLOR_CARD, true);

    if (connected) {
        k10.canvas->canvasRectangle(20, 122, 64, 18, COLOR_GREEN, COLOR_GREEN, true);
        k10.canvas->canvasText("Online", 28, 123, COLOR_WHITE, k10.canvas->eCNAndENFont16, 50, false);

        String ipStr = WiFi.localIP().toString();
        k10.canvas->canvasText(ipStr, 20, 160, COLOR_BLUE, k10.canvas->eCNAndENFont16, 50, false);
        k10.rgb->write(-1, 0x16A34A);
    } else {
        k10.canvas->canvasRectangle(20, 122, 96, 18, COLOR_RED, COLOR_RED, true);
        k10.canvas->canvasText("Connecting...", 26, 123, COLOR_WHITE, k10.canvas->eCNAndENFont16, 50, false);

        k10.canvas->canvasText("Searching network...", 20, 160, COLOR_TEXT_MUTED, k10.canvas->eCNAndENFont16, 50, false);
        k10.rgb->write(-1, 0xEA580C);
    }
    k10.canvas->updateCanvas();
}

// Partial refresh for RSSI stair-bars and dBm readout
void updateSignalMeter(int rssi, bool connected) {
    if (currentMode != MODE_STA) return;

    // Clear RSSI meter area (x: 130..220, y: 240..274)
    k10.canvas->canvasRectangle(130, 240, 94, 34, COLOR_CARD, COLOR_CARD, true);

    if (!connected) {
        k10.canvas->canvasText("-- dBm", 160, 248, COLOR_TEXT_MUTED, k10.canvas->eCNAndENFont16, 50, false);
        k10.canvas->updateCanvas();
        return;
    }

    int bars = 1;
    if (rssi > -60) bars = 5;
    else if (rssi > -70) bars = 4;
    else if (rssi > -80) bars = 3;
    else if (rssi > -90) bars = 2;

    for (int i = 0; i < 5; i++) {
        int barH = 5 + (i * 3);
        int barX = 136 + (i * 8);
        int barY = 268 - barH;
        uint32_t c = (i < bars) ? COLOR_BLUE : COLOR_BORDER;
        k10.canvas->canvasRectangle(barX, barY, 5, barH, c, c, true);
    }

    String dbmStr = String(rssi) + "dBm";
    k10.canvas->canvasText(dbmStr, 178, 250, COLOR_TEXT_SEC, k10.canvas->eCNAndENFont16, 50, false);
    k10.canvas->updateCanvas();
}

// Partial refresh for ACP client count
void updateACPTelemetry(int clients) {
    if (currentMode != MODE_ACP) return;

    k10.canvas->canvasRectangle(20, 260, 190, 16, COLOR_CARD, COLOR_CARD, true);
    String status = "Connected Devices: " + String(clients);
    uint32_t c = (clients > 0) ? COLOR_GREEN : COLOR_TEXT_MUTED;
    k10.canvas->canvasText(status, 20, 260, c, k10.canvas->eCNAndENFont16, 50, false);
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
    k10.rgb->write(-1, 0x2563EB);
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
    k10.rgb->write(-1, 0xD97706);
}

void setup() {
    Serial.begin(115200);
    k10.begin();
    k10.initScreen(screen_dir);
    k10.creatCanvas();
    k10.rgb->brightness(5);

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
    if (currentMode == MODE_ACP) {
        server.handleClient();

        if (switchRequested) {
            switchRequested = false;
            startStationMode();
            return;
        }

        int clients = WiFi.softAPgetStationNum();
        if (clients != lastClientCount) {
            lastClientCount = clients;
            updateACPTelemetry(clients);
        }

        if (k10.buttonB->isPressed()) {
            delay(200);
            startStationMode();
            return;
        }
    } else {
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

        if (k10.buttonA->isPressed()) {
            delay(200);
            startACPMode();
            return;
        }

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
