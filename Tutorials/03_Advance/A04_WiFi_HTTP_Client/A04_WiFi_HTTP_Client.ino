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

// Clean Minimalist Light Theme Palette
const uint32_t COLOR_BG        = 0xF8FAFC; // Soft Slate Off-White
const uint32_t COLOR_CARD      = 0xFFFFFF; // Pure White Card
const uint32_t COLOR_CONSOLE   = 0xF1F5F9; // Soft Paper Fill
const uint32_t COLOR_BORDER    = 0xE2E8F0; // Delicate 1px Border
const uint32_t COLOR_BORDER_CON= 0xCBD5E1; // Paper 1px Border
const uint32_t COLOR_TEXT_PRI  = 0x0F172A; // Deep Slate Charcoal
const uint32_t COLOR_TEXT_SEC  = 0x334155; // Slate Secondary
const uint32_t COLOR_TEXT_MUTED= 0x64748B; // Slate Muted Label
const uint32_t COLOR_EMERALD   = 0x059669; // Clean Emerald
const uint32_t COLOR_GREEN     = 0x16A34A; // Success Green
const uint32_t COLOR_RED       = 0xDC2626; // Error Red
const uint32_t COLOR_WHITE     = 0xFFFFFF;

void drawStaticChrome() {
    k10.canvas->canvasClear();
    k10.canvas->canvasRectangle(0, 0, 240, 320, COLOR_BG, COLOR_BG, true);

    // App Header Bar (y: 0 to 40)
    k10.canvas->canvasRectangle(0, 0, 240, 40, COLOR_CARD, COLOR_CARD, true);
    k10.canvas->canvasLine(0, 40, 240, 40, COLOR_BORDER);
    k10.canvas->canvasText("HTTP API Client", 14, 12, COLOR_TEXT_PRI, k10.canvas->eCNAndENFont16, 50, false);
    // Emerald brand dot
    k10.canvas->canvasCircle(224, 20, 4, COLOR_EMERALD, COLOR_EMERALD, true);

    // Endpoint Card (y: 48 to 102)
    k10.canvas->canvasRectangle(10, 48, 220, 54, COLOR_BORDER, COLOR_CARD, true);
    k10.canvas->canvasText("TARGET ENDPOINT", 20, 56, COLOR_TEXT_MUTED, k10.canvas->eCNAndENFont16, 50, false);
    k10.canvas->canvasText("worldtimeapi.org:80", 20, 74, COLOR_TEXT_PRI, k10.canvas->eCNAndENFont16, 50, false);

    // Status & Latency Pods (y: 110 to 174)
    k10.canvas->canvasRectangle(10, 110, 106, 64, COLOR_BORDER, COLOR_CARD, true);
    k10.canvas->canvasText("STATUS", 18, 118, COLOR_TEXT_MUTED, k10.canvas->eCNAndENFont16, 50, false);

    k10.canvas->canvasRectangle(124, 110, 106, 64, COLOR_BORDER, COLOR_CARD, true);
    k10.canvas->canvasText("LATENCY", 132, 118, COLOR_TEXT_MUTED, k10.canvas->eCNAndENFont16, 50, false);

    // Payload Card (y: 182 to 278)
    k10.canvas->canvasRectangle(10, 182, 220, 96, COLOR_BORDER, COLOR_CARD, true);
    k10.canvas->canvasText("RESPONSE DATA", 20, 190, COLOR_TEXT_MUTED, k10.canvas->eCNAndENFont16, 50, false);

    // Inner paper box for response (y: 212 to 268)
    k10.canvas->canvasRectangle(18, 212, 204, 56, COLOR_BORDER_CON, COLOR_CONSOLE, true);

    // Footer (y: 286 to 320)
    k10.canvas->canvasRectangle(0, 286, 240, 34, COLOR_CARD, COLOR_CARD, true);
    k10.canvas->canvasLine(0, 286, 240, 286, COLOR_BORDER);
    k10.canvas->canvasText("[A] Fetch API Data", 14, 294, COLOR_EMERALD, k10.canvas->eCNAndENFont16, 50, false);

    k10.canvas->updateCanvas();
}

// Partial refresh for HTTP status and latency metrics
void updateMetrics(int code, unsigned long latencyMs) {
    // Clear status pod value
    k10.canvas->canvasRectangle(16, 138, 94, 26, COLOR_CARD, COLOR_CARD, true);
    // Clear latency pod value
    k10.canvas->canvasRectangle(130, 138, 94, 26, COLOR_CARD, COLOR_CARD, true);

    if (code == 200) {
        k10.canvas->canvasRectangle(18, 140, 64, 20, COLOR_GREEN, COLOR_GREEN, true);
        k10.canvas->canvasText("200 OK", 22, 142, COLOR_WHITE, k10.canvas->eCNAndENFont16, 50, false);
    } else if (code > 0) {
        k10.canvas->canvasRectangle(18, 140, 64, 20, COLOR_RED, COLOR_RED, true);
        k10.canvas->canvasText(String(code), 26, 142, COLOR_WHITE, k10.canvas->eCNAndENFont16, 50, false);
    } else {
        k10.canvas->canvasText("STANDBY", 18, 142, COLOR_TEXT_MUTED, k10.canvas->eCNAndENFont16, 50, false);
    }

    if (latencyMs > 0) {
        String latStr = String(latencyMs) + " ms";
        k10.canvas->canvasText(latStr, 132, 142, COLOR_TEXT_PRI, k10.canvas->eCNAndENFont16, 50, false);
    } else {
        k10.canvas->canvasText("-- ms", 132, 142, COLOR_TEXT_MUTED, k10.canvas->eCNAndENFont16, 50, false);
    }

    k10.canvas->updateCanvas();
}

// Partial refresh for response payload text
void updatePayload(const String& line1, const String& line2) {
    k10.canvas->canvasRectangle(20, 214, 200, 52, COLOR_CONSOLE, COLOR_CONSOLE, true);
    k10.canvas->canvasText(line1, 24, 220, COLOR_TEXT_SEC, k10.canvas->eCNAndENFont16, 50, false);
    k10.canvas->canvasText(line2, 24, 242, COLOR_TEXT_MUTED, k10.canvas->eCNAndENFont16, 50, false);
    k10.canvas->updateCanvas();
}

void performRequest() {
    WiFiClient client;
    updatePayload("Connecting to host...", "");
    k10.rgb->write(-1, 0x059669);

    unsigned long startTime = millis();
    if (!client.connect(host, httpPort)) {
        updateMetrics(503, 0);
        updatePayload("Connection failed", "Cannot reach server");
        k10.rgb->write(-1, 0xDC2626);
        return;
    }

    client.print(String("GET ") + path + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" +
                 "User-Agent: UNIHIKER-K10\r\n" +
                 "Connection: close\r\n\r\n");

    unsigned long timeout = millis();
    while (client.available() == 0) {
        if (millis() - timeout > 5000) {
            client.stop();
            updateMetrics(408, 0);
            updatePayload("Request Timeout", "");
            k10.rgb->write(-1, 0xDC2626);
            return;
        }
    }

    unsigned long latency = millis() - startTime;
    String statusLine = client.readStringUntil('\r');
    int statusCode = 200;
    if (statusLine.indexOf(" ") != -1) {
        int firstSpace = statusLine.indexOf(" ");
        String codeStr = statusLine.substring(firstSpace + 1, firstSpace + 4);
        statusCode = codeStr.toInt();
    }
    updateMetrics(statusCode, latency);

    while (client.connected()) {
        String line = client.readStringUntil('\n');
        if (line == "\r") break;
    }

    String payload = "";
    while (client.available()) {
        payload += (char)client.read();
        if (payload.length() > 80) break;
    }
    client.stop();

    String pLine1 = payload.substring(0, 24);
    String pLine2 = (payload.length() > 24) ? payload.substring(24, 52) : "";
    updatePayload(pLine1, pLine2);
    k10.rgb->write(-1, 0x16A34A);
}

void setup() {
    Serial.begin(115200);
    k10.begin();
    k10.initScreen(screen_dir);
    k10.creatCanvas();
    k10.rgb->brightness(5);

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    drawStaticChrome();
    updateMetrics(0, 0);
    updatePayload("Ready to fetch", "Press Button A");
}

void loop() {
    if (k10.buttonA->isPressed()) {
        delay(200);
        if (WiFi.status() == WL_CONNECTED) {
            performRequest();
        } else {
            updateMetrics(0, 0);
            updatePayload("Wi-Fi Disconnected", "Check network target");
            k10.rgb->write(-1, 0xDC2626);
        }
    }
    delay(30);
}
