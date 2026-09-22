#include "unihiker_k10.h"

UNIHIKER_K10 k10;
uint8_t screen_dir = 2; // Portrait orientation (240x320)

// Clean Minimalist Light Theme Palette
const uint32_t COLOR_BG        = 0xF8FAFC; // Soft Slate Off-White
const uint32_t COLOR_CARD      = 0xFFFFFF; // Pure White Card Fill
const uint32_t COLOR_TRACK     = 0xF1F5F9; // Soft Track Fill
const uint32_t COLOR_BORDER    = 0xE2E8F0; // Delicate 1px Border
const uint32_t COLOR_BORDER_TRK= 0xCBD5E1; // Track Border
const uint32_t COLOR_TEXT_PRI  = 0x0F172A; // Deep Slate Charcoal
const uint32_t COLOR_TEXT_MUTED= 0x64748B; // Slate Muted Label
const uint32_t COLOR_BLUE      = 0x0284C7; // Sky Blue
const uint32_t COLOR_GREEN     = 0x16A34A; // Emerald
const uint32_t COLOR_AMBER     = 0xD97706; // Amber

void drawGaugeCard(int cardY, const char* label) {
    // Card container (y: cardY to cardY + 68)
    k10.canvas->canvasRectangle(10, cardY, 220, 68, COLOR_BORDER, COLOR_CARD, true);

    // Label
    k10.canvas->canvasText(label, 20, cardY + 12, COLOR_TEXT_MUTED, k10.canvas->eCNAndENFont16, 18, false);

    // Track frame (x: 20..220, y: cardY + 36, w: 200, h: 18)
    k10.canvas->canvasRectangle(20, cardY + 36, 200, 18, COLOR_BORDER_TRK, COLOR_TRACK, true);
}

void updateGauge(int cardY, int value, uint32_t barColor) {
    // Clear numeric percentage area (x: 160..220, y: cardY + 10)
    k10.canvas->canvasRectangle(160, cardY + 10, 60, 20, COLOR_CARD, COLOR_CARD, true);

    String valStr = String(value) + "%";
    int readoutX = 220 - (int)(valStr.length() * 9);
    k10.canvas->canvasText(valStr, readoutX, cardY + 12, COLOR_TEXT_PRI, k10.canvas->eCNAndENFont16, 8, false);

    // Clear and redraw inner track bar
    k10.canvas->canvasRectangle(22, cardY + 38, 196, 14, COLOR_TRACK, COLOR_TRACK, true);

    int fillW = map(constrain(value, 0, 100), 0, 100, 0, 196);
    if (fillW > 0) {
        k10.canvas->canvasRectangle(22, cardY + 38, fillW, 14, barColor, barColor, true);
    }
}

void drawScreenChrome() {
    k10.canvas->canvasClear();
    k10.canvas->canvasRectangle(0, 0, 240, 320, COLOR_BG, COLOR_BG, true);

    // App Header Bar (y: 0 to 40)
    k10.canvas->canvasRectangle(0, 0, 240, 40, COLOR_CARD, COLOR_CARD, true);
    k10.canvas->canvasLine(0, 40, 240, 40, COLOR_BORDER);
    k10.canvas->canvasText("Linear Gauges", 14, 12, COLOR_TEXT_PRI, k10.canvas->eCNAndENFont16, 50, false);
    // Blue brand dot
    k10.canvas->canvasCircle(224, 20, 4, COLOR_BLUE, COLOR_BLUE, true);

    // 3 Gauge Cards
    drawGaugeCard(48, "System Load");
    drawGaugeCard(124, "Memory Usage");
    drawGaugeCard(200, "Storage Volume");

    // Footer Bar (y: 284 to 320)
    k10.canvas->canvasRectangle(0, 284, 240, 36, COLOR_CARD, COLOR_CARD, true);
    k10.canvas->canvasLine(0, 284, 240, 284, COLOR_BORDER);
    k10.canvas->canvasText("Dynamic Component Telemetry", 14, 294, COLOR_TEXT_MUTED, k10.canvas->eCNAndENFont16, 50, false);
}

void setup() {
    k10.begin();
    k10.initScreen(screen_dir);
    k10.creatCanvas();
    k10.setScreenBackground(COLOR_BG);

    k10.rgb->brightness(5);
    k10.rgb->write(-1, 0x0284C7);

    drawScreenChrome();
    k10.canvas->updateCanvas();
}

void loop() {
    static int wave = 0;
    wave = (wave + 2) % 360;

    int v1 = 50 + (int)(45.0 * sin(radians(wave)));
    int v2 = 50 + (int)(40.0 * sin(radians(wave + 120)));
    int v3 = 50 + (int)(35.0 * sin(radians(wave + 240)));

    updateGauge(48, v1, COLOR_BLUE);
    updateGauge(124, v2, COLOR_GREEN);
    updateGauge(200, v3, COLOR_AMBER);

    k10.canvas->updateCanvas();
    delay(50);
}
