#include "unihiker_k10.h"
#include <WiFi.h>

UNIHIKER_K10 k10;
uint8_t screen_dir = 2; // Portrait (240x320)

// SoftAP Hotspot Credentials
const char* apSsid = "UNIHIKER_K10_AP";
const char* apPassword = "password123";

// Clean Minimalist Light Theme Palette
const uint32_t COLOR_BG        = 0xF8FAFC; // Soft Slate Off-White
const uint32_t COLOR_CARD      = 0xFFFFFF; // Pure White Card Fill
const uint32_t COLOR_BORDER    = 0xE2E8F0; // Delicate 1px Border
const uint32_t COLOR_DIVIDER   = 0xF1F5F9; // Inner Divider
const uint32_t COLOR_TEXT_PRI  = 0x0F172A; // Deep Slate Charcoal
const uint32_t COLOR_TEXT_SEC  = 0x334155; // Slate Secondary
const uint32_t COLOR_TEXT_MUTED= 0x64748B; // Slate Muted Label
const uint32_t COLOR_ORANGE    = 0xEA580C; // Warm Tangerine Brand Accent
const uint32_t COLOR_AMBER     = 0xD97706; // Amber
const uint32_t COLOR_GREEN     = 0x16A34A; // Connected Green
const uint32_t COLOR_WHITE     = 0xFFFFFF;

int lastStationCount = -1;
int waveRadius = 10;
unsigned long lastWaveTick = 0;

void drawStaticChrome() {
    k10.canvas->canvasClear();
    k10.canvas->canvasRectangle(0, 0, 240, 320, COLOR_BG, COLOR_BG, true);

    // App Header Bar (y: 0 to 40)
    k10.canvas->canvasRectangle(0, 0, 240, 40, COLOR_CARD, COLOR_CARD, true);
    k10.canvas->canvasLine(0, 40, 240, 40, COLOR_BORDER);
    k10.canvas->canvasText("Personal Hotspot", 14, 12, COLOR_TEXT_PRI, k10.canvas->eCNAndENFont16, 50, false);
    // Tangerine brand dot
    k10.canvas->canvasCircle(224, 20, 4, COLOR_ORANGE, COLOR_ORANGE, true);

    // Hotspot Credentials Card (y: 48 to 166)
    k10.canvas->canvasRectangle(10, 48, 220, 118, COLOR_BORDER, COLOR_CARD, true);
    k10.canvas->canvasText("HOTSPOT CREDENTIALS", 20, 56, COLOR_TEXT_MUTED, k10.canvas->eCNAndENFont16, 50, false);

    k10.canvas->canvasText("Network:", 20, 78, COLOR_TEXT_MUTED, k10.canvas->eCNAndENFont16, 50, false);
    k10.canvas->canvasText(apSsid, 84, 78, COLOR_TEXT_PRI, k10.canvas->eCNAndENFont16, 50, false);

    k10.canvas->canvasText("Password:", 20, 102, COLOR_TEXT_MUTED, k10.canvas->eCNAndENFont16, 50, false);
    k10.canvas->canvasText(apPassword, 96, 102, COLOR_TEXT_PRI, k10.canvas->eCNAndENFont16, 50, false);

    k10.canvas->canvasText("Router IP:", 20, 126, COLOR_TEXT_MUTED, k10.canvas->eCNAndENFont16, 50, false);
    k10.canvas->canvasText(WiFi.softAPIP().toString(), 96, 126, COLOR_ORANGE, k10.canvas->eCNAndENFont16, 50, false);

    // Connected Clients Card (y: 174 to 280)
    k10.canvas->canvasRectangle(10, 174, 220, 106, COLOR_BORDER, COLOR_CARD, true);
    k10.canvas->canvasText("CONNECTED CLIENTS", 20, 182, COLOR_TEXT_MUTED, k10.canvas->eCNAndENFont16, 50, false);

    // Subtle Antenna Base Dot
    k10.canvas->canvasCircle(56, 238, 4, COLOR_ORANGE, COLOR_ORANGE, true);

    // Footer Bar (y: 288 to 320)
    k10.canvas->canvasRectangle(0, 288, 240, 32, COLOR_CARD, COLOR_CARD, true);
    k10.canvas->canvasLine(0, 288, 240, 288, COLOR_BORDER);
    k10.canvas->canvasText("Broadcasting Wi-Fi Network", 14, 296, COLOR_TEXT_MUTED, k10.canvas->eCNAndENFont16, 50, false);

    k10.canvas->updateCanvas();
}

// Partial refresh for client count readout
void updateClientCount(int count) {
    k10.canvas->canvasRectangle(100, 218, 120, 34, COLOR_CARD, COLOR_CARD, true);

    String cntStr = String(count);
    uint32_t col = (count > 0) ? COLOR_GREEN : COLOR_TEXT_MUTED;
    k10.canvas->canvasText(cntStr, 104, 220, col, k10.canvas->eCNAndENFont24, 50, false);
    k10.canvas->canvasText("Devices", 144, 226, COLOR_TEXT_SEC, k10.canvas->eCNAndENFont16, 50, false);

    if (count > 0) {
        k10.rgb->write(-1, 0x16A34A);
    } else {
        k10.rgb->write(-1, 0xEA580C);
    }

    k10.canvas->updateCanvas();
}

// Partial refresh for subtle antenna radio ripple
void updateAntennaRipple() {
    // Clear antenna broadcast wave zone (x: 24..88, y: 206..270)
    k10.canvas->canvasRectangle(24, 206, 64, 64, COLOR_CARD, COLOR_CARD, true);

    // Draw central dot
    k10.canvas->canvasCircle(56, 238, 4, COLOR_ORANGE, COLOR_ORANGE, true);

    // Draw 1px subtle ripple ring
    k10.canvas->canvasCircle(56, 238, waveRadius, COLOR_BORDER, COLOR_CARD, false);

    waveRadius += 4;
    if (waveRadius > 24) waveRadius = 10;

    k10.canvas->updateCanvas();
}

void setup() {
    Serial.begin(115200);
    k10.begin();
    k10.initScreen(screen_dir);
    k10.creatCanvas();
    k10.rgb->brightness(5);

    WiFi.mode(WIFI_AP);
    WiFi.softAP(apSsid, apPassword);

    drawStaticChrome();
    updateClientCount(0);
}

void loop() {
    int currentClients = WiFi.softAPgetStationNum();
    if (currentClients != lastStationCount) {
        lastStationCount = currentClients;
        updateClientCount(currentClients);
    }

    unsigned long now = millis();
    if (now - lastWaveTick >= 250) {
        lastWaveTick = now;
        updateAntennaRipple();
    }

    delay(30);
}
