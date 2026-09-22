#include "unihiker_k10.h"

UNIHIKER_K10 k10;

struct ColorTheme {
    uint32_t bg;
    uint32_t accent;
    const char* name;
    const char* hexCode;
};

ColorTheme themes[] = {
    {0xF0F9FF, 0x0284C7, "Sky Blue", "#F0F9FF"},
    {0xF0FDF4, 0x16A34A, "Emerald Mint", "#F0FDF4"},
    {0xFFFBEB, 0xD97706, "Warm Amber", "#FFFBEB"},
    {0xFAF5FF, 0x9333EA, "Soft Lavender", "#FAF5FF"}
};

const int THEME_COUNT = sizeof(themes) / sizeof(themes[0]);
int currentTheme = 0;

void showTheme(int idx) {
    k10.canvas->canvasClear();
    k10.setScreenBackground(themes[idx].bg);

    // Clean white central card with delicate 1px border
    k10.canvas->canvasSetLineWidth(1);
    k10.canvas->canvasRectangle(20, 40, 200, 240, 0xE2E8F0, 0xFFFFFF, true);

    // Card header bar
    k10.canvas->canvasRectangle(20, 40, 200, 42, 0xE2E8F0, 0xF8FAFC, true);
    k10.canvas->canvasCircle(40, 61, 5, themes[idx].accent, themes[idx].accent, true);
    k10.canvas->canvasText("Color Palette", 56, 52, 0x0F172A, k10.canvas->eCNAndENFont16, 20, false);

    // Swatch preview box
    k10.canvas->canvasRectangle(40, 100, 160, 64, 0xE2E8F0, themes[idx].bg, true);
    k10.canvas->canvasCircle(120, 132, 14, themes[idx].accent, themes[idx].accent, true);

    // Color label details
    k10.canvas->canvasText(themes[idx].name, 42, 180, 0x0F172A, k10.canvas->eCNAndENFont16, 20, false);
    k10.canvas->canvasText(themes[idx].hexCode, 42, 210, themes[idx].accent, k10.canvas->eCNAndENFont16, 20, false);

    // Footer indicator
    k10.canvas->canvasLine(35, 242, 205, 242, 0xF1F5F9);
    k10.canvas->canvasText("Cycling palettes...", 54, 252, 0x94A3B8, k10.canvas->eCNAndENFont16, 20, false);

    k10.canvas->updateCanvas();
}

void setup() {
    k10.begin();
    k10.initScreen(2); // Portrait orientation (240x320)
    k10.creatCanvas();
    showTheme(currentTheme);
}

void loop() {
    delay(2000);
    currentTheme = (currentTheme + 1) % THEME_COUNT;
    showTheme(currentTheme);
}
