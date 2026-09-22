#include "unihiker_k10.h"

UNIHIKER_K10 k10;

// Minimalist Clean Light Palette
#define COLOR_BG        0xF8FAFC
#define COLOR_CARD      0xFFFFFF
#define COLOR_BORDER    0xE2E8F0
#define COLOR_TEXT_PRI  0x0F172A
#define COLOR_TEXT_MUTED 0x64748B
#define COLOR_PRIMARY   0x2563EB // Sapphire Blue
#define COLOR_ACCENT    0x10B981 // Emerald Green

void drawStaticChrome() {
    k10.canvas->canvasClear();
    k10.setScreenBackground(COLOR_BG);

    k10.canvas->canvasSetLineWidth(1);

    // Main Content Card
    k10.canvas->canvasRectangle(16, 24, 208, 272, COLOR_BORDER, COLOR_CARD, true);

    // Header Badge
    k10.canvas->canvasRectangle(16, 24, 208, 44, COLOR_BORDER, 0xF1F5F9, true);
    k10.canvas->canvasCircle(36, 46, 5, COLOR_PRIMARY, COLOR_PRIMARY, true);
    k10.canvas->canvasText("UNIHIKER K10", 52, 38, COLOR_TEXT_PRI,
                           k10.canvas->eCNAndENFont16, 20, false);

    // Subtitle & Introduction
    k10.canvas->canvasText("Hello, World!", 30, 84, COLOR_PRIMARY,
                           k10.canvas->eCNAndENFont24, 20, false);

    // Level Pill
    k10.canvas->canvasRectangle(30, 118, 120, 20, COLOR_BORDER, 0xEEF2FF, true);
    k10.canvas->canvasText("Level 1: Beginner", 36, 120, 0x4F46E5,
                           k10.canvas->eCNAndENFont16, 20, false);

    // Explanatory Text
    k10.canvas->canvasText("Clean, natural layout", 30, 150, COLOR_TEXT_MUTED,
                           k10.canvas->eCNAndENFont16, 25, false);
    k10.canvas->canvasText("designed for clarity.", 30, 172, COLOR_TEXT_MUTED,
                           k10.canvas->eCNAndENFont16, 25, false);

    // System Status Card Area
    k10.canvas->canvasLine(30, 208, 210, 208, COLOR_BORDER);
    k10.canvas->canvasText("SYSTEM ACTIVE", 30, 218, COLOR_TEXT_MUTED,
                           k10.canvas->eCNAndENFont16, 20, false);

    k10.canvas->updateCanvas();
}

void updateUptime(int seconds) {
    // Dynamic partial refresh of the uptime counter pill
    k10.canvas->canvasRectangle(30, 238, 180, 28, COLOR_BORDER, 0xF8FAFC, true);

    k10.canvas->canvasCircle(42, 252, 4, COLOR_ACCENT, COLOR_ACCENT, true);
    String timeStr = "Uptime: " + String(seconds) + " s";
    k10.canvas->canvasText(timeStr.c_str(), 54, 244, COLOR_TEXT_PRI,
                           k10.canvas->eCNAndENFont16, 20, false);

    k10.canvas->updateCanvas();
}

void setup() {
    k10.begin();
    k10.initScreen(2); // Portrait orientation (240x320)
    k10.creatCanvas();

    drawStaticChrome();
    updateUptime(0);
}

void loop() {
    static int seconds = 0;
    delay(1000);
    seconds++;
    updateUptime(seconds);
}
