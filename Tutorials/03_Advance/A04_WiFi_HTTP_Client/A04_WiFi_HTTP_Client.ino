#include "unihiker_k10.h"
#include <WiFi.h>
#include <WebServer.h>
#include <Preferences.h>

UNIHIKER_K10 k10;
uint8_t screen_dir = 2; // Portrait (240x320)

WebServer server(80);
Preferences preferences;

enum SystemMode {
    MODE_STA,
    MODE_ACP
};

SystemMode currentMode = MODE_STA;

String savedSSID = "";
String savedPass = "";
String savedApiKey = "";
String savedCity = "London";

bool switchRequested = false;
bool isConnecting = false;
unsigned long connectStartTime = 0;
unsigned long lastStatusCheck = 0;
int lastClientCount = -1;

// OpenWeatherMap API Target Host
const char* weatherHost = "api.openweathermap.org";
const int httpPort = 80;

// Clean Minimalist Light Theme Palette
const uint32_t COLOR_BG        = 0xF8FAFC; // Soft Slate Off-White
const uint32_t COLOR_CARD      = 0xFFFFFF; // Pure White Card Fill
const uint32_t COLOR_CONSOLE   = 0xF1F5F9; // Soft Paper Fill
const uint32_t COLOR_BORDER    = 0xE2E8F0; // Delicate 1px Border
const uint32_t COLOR_BORDER_CON= 0xCBD5E1; // Paper 1px Border
const uint32_t COLOR_TEXT_PRI  = 0x0F172A; // Deep Slate Charcoal
const uint32_t COLOR_TEXT_SEC  = 0x334155; // Slate Secondary
const uint32_t COLOR_TEXT_MUTED= 0x64748B; // Slate Muted Label
const uint32_t COLOR_EMERALD   = 0x059669; // Clean Emerald
const uint32_t COLOR_AMBER     = 0xD97706; // Warm Amber
const uint32_t COLOR_GREEN     = 0x16A34A; // Success Green
const uint32_t COLOR_RED       = 0xDC2626; // Error Red
const uint32_t COLOR_WHITE     = 0xFFFFFF;

// Forward declarations
void startStationMode();
void startACPMode();
void drawStationChrome();
void drawACPChrome();
void updateMetrics(int code, unsigned long latencyMs);
void updatePayload(const String& line1, const String& line2);
void updateWiFiStatusBadge(bool connected, const String& info);

String urlEncode(const String& str) {
    String encoded = "";
    for (int i = 0; i < str.length(); i++) {
        char c = str.charAt(i);
        if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            encoded += c;
        } else if (c == ' ') {
            encoded += "%20";
        } else {
            char buf[4];
            sprintf(buf, "%%%02X", (unsigned char)c);
            encoded += buf;
        }
    }
    return encoded;
}

// Non-blocking button edge-detection trackers
bool checkButtonAPressed() {
    static bool lastState = false;
    static unsigned long lastDebounceTime = 0;
    bool reading = k10.buttonA->isPressed();
    bool pressedEvent = false;

    if (reading != lastState) lastDebounceTime = millis();
    if ((millis() - lastDebounceTime) > 35) {
        static bool stableState = false;
        if (reading != stableState) {
            stableState = reading;
            if (stableState) pressedEvent = true;
        }
    }
    lastState = reading;
    return pressedEvent;
}

bool checkButtonBPressed() {
    static bool lastState = false;
    static unsigned long lastDebounceTime = 0;
    bool reading = k10.buttonB->isPressed();
    bool pressedEvent = false;

    if (reading != lastState) lastDebounceTime = millis();
    if ((millis() - lastDebounceTime) > 35) {
        static bool stableState = false;
        if (reading != stableState) {
            stableState = reading;
            if (stableState) pressedEvent = true;
        }
    }
    lastState = reading;
    return pressedEvent;
}

// Web Server Handlers (Modern Clean Light Webpage with City & API Key)
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
                  "<title>UNIHIKER K10 - Weather & Wi-Fi Setup</title>"
                  "<style>"
                  "body{background:#F8FAFC;color:#0F172A;font-family:-apple-system,BlinkMacSystemFont,'Segoe UI',Roboto,sans-serif;margin:0;padding:24px 16px;}"
                  ".card{background:#FFFFFF;border:1px solid #E2E8F0;border-radius:16px;padding:28px 24px;max-width:400px;margin:auto;box-shadow:0 4px 16px rgba(15,23,42,0.06);}"
                  "h2{color:#0F172A;margin:0 0 8px;font-size:1.35rem;font-weight:700;}"
                  "p{color:#64748B;font-size:0.9rem;line-height:1.5;margin:0 0 18px;}"
                  "label{display:block;margin:14px 0 6px;color:#334155;font-size:0.82rem;font-weight:600;text-transform:uppercase;letter-spacing:0.5px;}"
                  "input,select{width:100%;box-sizing:border-box;padding:12px 14px;background:#F8FAFC;border:1px solid #CBD5E1;border-radius:10px;color:#0F172A;font-size:0.95rem;outline:none;transition:border-color 0.2s;}"
                  "input:focus,select:focus{border-color:#059669;background:#FFFFFF;}"
                  ".btn{width:100%;margin-top:24px;padding:14px;background:#059669;color:#FFFFFF;font-weight:600;font-size:1rem;border:none;border-radius:10px;cursor:pointer;transition:background 0.2s;}"
                  ".btn:hover{background:#047857;}"
                  ".hint{color:#94A3B8;font-size:0.78rem;margin-top:4px;display:block;}"
                  ".footer{text-align:center;color:#94A3B8;font-size:0.75rem;margin-top:20px;}"
                  "</style></head><body>"
                  "<div class='card'>"
                  "<h2>OpenWeather & Wi-Fi Setup</h2>"
                  "<p>Configure wireless connection and weather settings for your UNIHIKER K10.</p>"
                  "<form method='POST' action='/save'>"
                  "<label>Available Networks</label>"
                  "<select id='netList' onchange='document.getElementById(\"ssidInput\").value = this.value;'>"
                  "<option value=''>-- Select Scanned Network --</option>" + options + "</select>"
                  "<label>Network SSID</label>"
                  "<input type='text' id='ssidInput' name='ssid' value='" + savedSSID + "' placeholder='Enter Wi-Fi SSID' required>"
                  "<label>Password</label>"
                  "<input type='password' name='pass' placeholder='Enter Wi-Fi password'>"
                  "<label>Location / City</label>"
                  "<input type='text' name='city' value='" + savedCity + "' placeholder='e.g. London, Tokyo, New York' required>"
                  "<span class='hint'>City name used for weather queries</span>"
                  "<label>OpenWeatherMap API Key</label>"
                  "<input type='text' name='apikey' value='" + savedApiKey + "' placeholder='32-character API Key'>"
                  "<span class='hint'>Free API key from openweathermap.org</span>"
                  "<button type='submit' class='btn'>Save & Connect</button>"
                  "</form>"
                  "<div class='footer'>UNIHIKER K10 Weather Portal</div>"
                  "</div></body></html>";

    server.send(200, "text/html", html);
}

void handleSave() {
    if (server.hasArg("ssid")) {
        savedSSID = server.arg("ssid");
        savedPass = server.arg("pass");
        preferences.putString("ssid", savedSSID);
        preferences.putString("pass", savedPass);

        if (server.hasArg("city")) {
            savedCity = server.arg("city");
            savedCity.trim();
            if (savedCity.length() == 0) savedCity = "London";
            preferences.putString("city", savedCity);
        }

        if (server.hasArg("apikey")) {
            savedApiKey = server.arg("apikey");
            savedApiKey.trim();
            preferences.putString("apikey", savedApiKey);
        }

        String reply = "<!DOCTYPE html><html><head><meta name='viewport' content='width=device-width,initial-scale=1'>"
                       "<title>Settings Saved</title>"
                       "<style>body{background:#F8FAFC;color:#0F172A;font-family:sans-serif;padding:36px 16px;text-align:center;}"
                       ".box{background:#FFFFFF;border:1px solid #E2E8F0;border-radius:16px;padding:32px 24px;max-width:360px;margin:auto;box-shadow:0 4px 16px rgba(15,23,42,0.06);}"
                       "h2{color:#16A34A;margin-top:0;}p{color:#334155;line-height:1.5;}</style></head><body>"
                       "<div class='box'><h2>Configuration Saved</h2>"
                       "<p>Connecting UNIHIKER K10 to <b>" + savedSSID + "</b>...</p>"
                       "<p>Weather Location: <b>" + savedCity + "</b></p>"
                       "<p style='color:#64748B;font-size:0.85rem;'>Hotspot is closing now.</p>"
                       "</div></body></html>";
        server.send(200, "text/html", reply);
        delay(1000);
        switchRequested = true;
    } else {
        server.send(400, "text/plain", "Missing SSID parameter");
    }
}

// ----------------------------------------------------
// UI Rendering Functions
// ----------------------------------------------------

void drawStationChrome() {
    k10.canvas->canvasClear();
    k10.canvas->canvasSetLineWidth(1);
    k10.setScreenBackground(COLOR_BG);

    // App Header Bar (y: 0 to 40)
    k10.canvas->canvasRectangle(0, 0, 240, 40, COLOR_CARD, COLOR_CARD, true);
    k10.canvas->canvasLine(0, 40, 240, 40, COLOR_BORDER);
    k10.canvas->canvasText("Weather Client", 14, 12, COLOR_TEXT_PRI, k10.canvas->eCNAndENFont16, 50, false);
    // Emerald brand dot
    k10.canvas->canvasCircle(224, 20, 4, COLOR_EMERALD, COLOR_EMERALD, true);

    // Target Host & City Card (y: 46 to 108)
    k10.canvas->canvasRectangle(10, 46, 220, 62, COLOR_BORDER, COLOR_CARD, true);
    k10.canvas->canvasText("OPENWEATHER HOST", 18, 54, COLOR_TEXT_MUTED, k10.canvas->eCNAndENFont16, 50, false);
    String cityLine = "City: " + savedCity;
    k10.canvas->canvasText(cityLine.c_str(), 18, 70, COLOR_TEXT_PRI, k10.canvas->eCNAndENFont16, 50, false);

    // Status & Latency Pods (y: 114 to 176)
    k10.canvas->canvasRectangle(10, 114, 106, 62, COLOR_BORDER, COLOR_CARD, true);
    k10.canvas->canvasText("HTTP STATUS", 18, 122, COLOR_TEXT_MUTED, k10.canvas->eCNAndENFont16, 50, false);

    k10.canvas->canvasRectangle(124, 114, 106, 62, COLOR_BORDER, COLOR_CARD, true);
    k10.canvas->canvasText("LATENCY", 132, 122, COLOR_TEXT_MUTED, k10.canvas->eCNAndENFont16, 50, false);

    // Weather Response Card (y: 182 to 276)
    k10.canvas->canvasRectangle(10, 182, 220, 94, COLOR_BORDER, COLOR_CARD, true);
    k10.canvas->canvasText("LIVE WEATHER DATA", 20, 190, COLOR_TEXT_MUTED, k10.canvas->eCNAndENFont16, 50, false);

    // Inner paper box for weather metrics (y: 210 to 266)
    k10.canvas->canvasRectangle(18, 210, 204, 56, COLOR_BORDER_CON, COLOR_CONSOLE, true);

    // Footer Control Bar (y: 284 to 320)
    k10.canvas->canvasRectangle(0, 284, 240, 36, COLOR_CARD, COLOR_CARD, true);
    k10.canvas->canvasLine(0, 284, 240, 284, COLOR_BORDER);
    k10.canvas->canvasText("[A] Fetch Data", 16, 294, COLOR_EMERALD, k10.canvas->eCNAndENFont16, 12, false);
    k10.canvas->canvasLine(124, 290, 124, 314, COLOR_BORDER);
    k10.canvas->canvasText("[B] Wi-Fi / AP", 136, 294, COLOR_AMBER, k10.canvas->eCNAndENFont16, 12, false);

    k10.canvas->updateCanvas();
}

void drawACPChrome() {
    k10.canvas->canvasClear();
    k10.canvas->canvasSetLineWidth(1);
    k10.setScreenBackground(COLOR_BG);

    // App Header Bar
    k10.canvas->canvasRectangle(0, 0, 240, 40, COLOR_CARD, COLOR_CARD, true);
    k10.canvas->canvasLine(0, 40, 240, 40, COLOR_BORDER);
    k10.canvas->canvasText("Weather Setup Portal", 14, 12, COLOR_TEXT_PRI, k10.canvas->eCNAndENFont16, 50, false);
    k10.canvas->canvasCircle(224, 20, 4, COLOR_AMBER, COLOR_AMBER, true);

    // AP Hotspot Details Card (y: 46 to 128)
    k10.canvas->canvasRectangle(10, 46, 220, 80, COLOR_BORDER, COLOR_CARD, true);
    k10.canvas->canvasText("SETUP HOTSPOT SSID", 20, 54, COLOR_TEXT_MUTED, k10.canvas->eCNAndENFont16, 50, false);
    k10.canvas->canvasText("UNIHIKER-Weather-Setup", 20, 72, COLOR_AMBER, k10.canvas->eCNAndENFont16, 24, false);
    k10.canvas->canvasText("Open hotspot (no password)", 20, 98, COLOR_TEXT_MUTED, k10.canvas->eCNAndENFont16, 50, false);

    // Portal Web Address Card (y: 134 to 206)
    k10.canvas->canvasRectangle(10, 134, 220, 72, COLOR_BORDER, COLOR_CARD, true);
    k10.canvas->canvasText("PORTAL IP ADDRESS", 20, 142, COLOR_TEXT_MUTED, k10.canvas->eCNAndENFont16, 50, false);
    k10.canvas->canvasText("http://192.168.4.1", 20, 160, COLOR_TEXT_PRI, k10.canvas->eCNAndENFont16, 24, false);
    k10.canvas->canvasText("Configure Wi-Fi, City & Key", 20, 184, COLOR_TEXT_MUTED, k10.canvas->eCNAndENFont16, 50, false);

    // Config Overview Card (y: 212 to 276)
    k10.canvas->canvasRectangle(10, 212, 220, 64, COLOR_BORDER, COLOR_CARD, true);
    String cfgCity = "City: " + savedCity;
    String cfgKey = "API Key: " + String((savedApiKey.length() > 0) ? "Configured" : "Not Set (Required)");
    k10.canvas->canvasText(cfgCity.c_str(), 20, 222, COLOR_TEXT_SEC, k10.canvas->eCNAndENFont16, 50, false);
    k10.canvas->canvasText(cfgKey.c_str(), 20, 246, (savedApiKey.length() > 0) ? COLOR_GREEN : COLOR_RED, k10.canvas->eCNAndENFont16, 50, false);

    // Footer Control Bar (y: 284 to 320)
    k10.canvas->canvasRectangle(0, 284, 240, 36, COLOR_CARD, COLOR_CARD, true);
    k10.canvas->canvasLine(0, 284, 240, 284, COLOR_BORDER);
    k10.canvas->canvasText("[B] Exit Wi-Fi Setup", 50, 294, COLOR_EMERALD, k10.canvas->eCNAndENFont16, 50, false);

    k10.canvas->updateCanvas();
}

void updateMetrics(int code, unsigned long latencyMs) {
    k10.canvas->canvasRectangle(16, 140, 94, 26, COLOR_CARD, COLOR_CARD, true);
    k10.canvas->canvasRectangle(130, 140, 94, 26, COLOR_CARD, COLOR_CARD, true);

    if (code == 200) {
        k10.canvas->canvasRectangle(18, 142, 66, 20, COLOR_GREEN, COLOR_GREEN, true);
        k10.canvas->canvasText("200 OK", 22, 144, COLOR_WHITE, k10.canvas->eCNAndENFont16, 50, false);
    } else if (code > 0) {
        k10.canvas->canvasRectangle(18, 142, 66, 20, COLOR_RED, COLOR_RED, true);
        k10.canvas->canvasText(String(code), 26, 144, COLOR_WHITE, k10.canvas->eCNAndENFont16, 50, false);
    } else {
        k10.canvas->canvasText("STANDBY", 18, 144, COLOR_TEXT_MUTED, k10.canvas->eCNAndENFont16, 50, false);
    }

    if (latencyMs > 0) {
        String latStr = String(latencyMs) + " ms";
        k10.canvas->canvasText(latStr, 132, 144, COLOR_TEXT_PRI, k10.canvas->eCNAndENFont16, 50, false);
    } else {
        k10.canvas->canvasText("-- ms", 132, 144, COLOR_TEXT_MUTED, k10.canvas->eCNAndENFont16, 50, false);
    }

    k10.canvas->updateCanvas();
}

void updatePayload(const String& line1, const String& line2) {
    k10.canvas->canvasRectangle(20, 212, 200, 52, COLOR_CONSOLE, COLOR_CONSOLE, true);
    k10.canvas->canvasText(line1, 24, 218, COLOR_TEXT_SEC, k10.canvas->eCNAndENFont16, 50, false);
    k10.canvas->canvasText(line2, 24, 240, COLOR_TEXT_MUTED, k10.canvas->eCNAndENFont16, 50, false);
    k10.canvas->updateCanvas();
}

void updateWiFiStatusBadge(bool connected, const String& info) {
    k10.canvas->canvasRectangle(18, 88, 204, 18, COLOR_CARD, COLOR_CARD, true);
    if (connected) {
        k10.canvas->canvasCircle(26, 96, 3, COLOR_GREEN, COLOR_GREEN, true);
        String label = "Wi-Fi: " + info;
        k10.canvas->canvasText(label, 36, 88, COLOR_TEXT_SEC, k10.canvas->eCNAndENFont16, 50, false);
    } else {
        k10.canvas->canvasCircle(26, 96, 3, COLOR_AMBER, COLOR_AMBER, true);
        k10.canvas->canvasText(info, 36, 88, COLOR_AMBER, k10.canvas->eCNAndENFont16, 50, false);
    }
    k10.canvas->updateCanvas();
}

// ----------------------------------------------------
// Core OpenWeatherMap HTTP Request Logic
// ----------------------------------------------------

void performWeatherRequest() {
    if (savedApiKey.length() == 0) {
        updateMetrics(401, 0);
        updatePayload("API Key Required", "Set key in [B] Setup AP");
        k10.rgb->write(-1, 0xDC2626);
        return;
    }

    WiFiClient client;
    updatePayload("Connecting to OpenWeather...", savedCity);
    k10.rgb->write(-1, 0x059669);

    unsigned long startTime = millis();
    if (!client.connect(weatherHost, httpPort)) {
        updateMetrics(503, 0);
        updatePayload("Connection Failed", "Cannot reach OpenWeather");
        k10.rgb->write(-1, 0xDC2626);
        return;
    }

    String path = "/data/2.5/weather?q=" + urlEncode(savedCity) + "&appid=" + savedApiKey + "&units=metric";

    client.print(String("GET ") + path + " HTTP/1.1\r\n" +
                 "Host: " + weatherHost + "\r\n" +
                 "User-Agent: UNIHIKER-K10\r\n" +
                 "Connection: close\r\n\r\n");

    unsigned long timeout = millis();
    while (client.available() == 0) {
        if (millis() - timeout > 6000) {
            client.stop();
            updateMetrics(408, 0);
            updatePayload("Request Timeout", "Server did not reply");
            k10.rgb->write(-1, 0xDC2626);
            return;
        }
    }

    unsigned long latency = millis() - startTime;
    String statusLine = client.readStringUntil('\r');
    int statusCode = 0;
    if (statusLine.indexOf(" ") != -1) {
        int firstSpace = statusLine.indexOf(" ");
        String codeStr = statusLine.substring(firstSpace + 1, firstSpace + 4);
        statusCode = codeStr.toInt();
    }
    updateMetrics(statusCode, latency);

    // Skip HTTP headers
    while (client.connected()) {
        String line = client.readStringUntil('\n');
        if (line == "\r") break;
    }

    // Read JSON body (up to 1200 characters)
    String body = "";
    while (client.available()) {
        body += (char)client.read();
        if (body.length() > 1200) break;
    }
    client.stop();

    if (statusCode == 200) {
        // Parse temperature: "temp":21.5
        String tempStr = "--";
        int tempIdx = body.indexOf("\"temp\":");
        if (tempIdx != -1) {
            int endIdx = body.indexOf(",", tempIdx);
            if (endIdx != -1) {
                tempStr = body.substring(tempIdx + 7, endIdx);
            }
        }

        // Parse humidity: "humidity":65
        String humStr = "--";
        int humIdx = body.indexOf("\"humidity\":");
        if (humIdx != -1) {
            int endIdx = body.indexOf(",", humIdx);
            if (endIdx == -1) endIdx = body.indexOf("}", humIdx);
            if (endIdx != -1) {
                humStr = body.substring(humIdx + 11, endIdx);
            }
        }

        // Parse weather description: "main":"Clear"
        String mainWeather = "Weather";
        int wIdx = body.indexOf("\"main\":\"");
        if (wIdx != -1) {
            int endIdx = body.indexOf("\"", wIdx + 8);
            if (endIdx != -1) {
                mainWeather = body.substring(wIdx + 8, endIdx);
            }
        }

        String line1 = savedCity + ": " + tempStr + " C";
        String line2 = mainWeather + " (Hum: " + humStr + "%)";
        updatePayload(line1, line2);
        k10.rgb->write(-1, 0x16A34A);
    } else if (statusCode == 401) {
        updatePayload("Invalid API Key (401)", "Set valid key in [B]");
        k10.rgb->write(-1, 0xDC2626);
    } else if (statusCode == 404) {
        updatePayload("City Not Found (404)", "Check name in [B] Setup");
        k10.rgb->write(-1, 0xDC2626);
    } else {
        updatePayload("HTTP Error: " + String(statusCode), "Check API configuration");
        k10.rgb->write(-1, 0xDC2626);
    }
}

// ----------------------------------------------------
// Mode Switch Handlers
// ----------------------------------------------------

void startStationMode() {
    currentMode = MODE_STA;
    WiFi.softAPdisconnect(true);
    server.stop();

    WiFi.mode(WIFI_STA);

    drawStationChrome();
    updateMetrics(0, 0);

    if (savedSSID.length() > 0) {
        updatePayload("Connecting to Wi-Fi...", savedSSID);
        updateWiFiStatusBadge(false, "Connecting: " + savedSSID);
        WiFi.begin(savedSSID.c_str(), savedPass.c_str());
        isConnecting = true;
        connectStartTime = millis();
    } else {
        updatePayload("No Wi-Fi Configured", "Press [B] for Setup AP");
        updateWiFiStatusBadge(false, "Not Configured");
    }

    k10.rgb->write(-1, 0x059669);
}

void startACPMode() {
    currentMode = MODE_ACP;
    WiFi.disconnect(true);

    WiFi.mode(WIFI_AP);
    WiFi.softAP("UNIHIKER-Weather-Setup");

    server.on("/", HTTP_GET, handleRoot);
    server.on("/save", HTTP_POST, handleSave);
    server.begin();

    lastClientCount = -1;
    drawACPChrome();
    k10.rgb->write(-1, 0xD97706); // Amber setup light
}

void setup() {
    Serial.begin(115200);
    k10.begin();
    k10.initScreen(screen_dir);
    k10.creatCanvas();
    k10.rgb->brightness(5);

    preferences.begin("weather_cfg", false);
    savedSSID = preferences.getString("ssid", "");
    savedPass = preferences.getString("pass", "");
    savedApiKey = preferences.getString("apikey", "");
    savedCity = preferences.getString("city", "London");

    if (savedSSID.length() == 0) {
        startACPMode();
    } else {
        startStationMode();
    }
}

void loop() {
    if (switchRequested) {
        switchRequested = false;
        startStationMode();
        return;
    }

    if (currentMode == MODE_ACP) {
        server.handleClient();

        if (checkButtonBPressed()) {
            startStationMode();
        }
    } else {
        // Station Mode
        if (isConnecting) {
            if (WiFi.status() == WL_CONNECTED) {
                isConnecting = false;
                updateWiFiStatusBadge(true, savedSSID + " (" + WiFi.localIP().toString() + ")");
                updatePayload("Ready to fetch", "Press [A] for Weather");
                k10.rgb->write(-1, 0x16A34A);
            } else if (millis() - connectStartTime > 15000) {
                isConnecting = false;
                updateWiFiStatusBadge(false, "Connection Failed");
                updatePayload("Wi-Fi timeout", "Press [B] for Setup AP");
                k10.rgb->write(-1, 0xDC2626);
            }
        }

        // Button A: Trigger Weather API Request
        if (checkButtonAPressed()) {
            if (WiFi.status() == WL_CONNECTED) {
                performWeatherRequest();
            } else {
                updateMetrics(0, 0);
                updatePayload("Wi-Fi Disconnected", "Press [B] for Setup AP");
                k10.rgb->write(-1, 0xDC2626);
            }
        }

        // Button B: Enter Wi-Fi Setup Access Point
        if (checkButtonBPressed()) {
            startACPMode();
        }
    }

    delay(20);
}
