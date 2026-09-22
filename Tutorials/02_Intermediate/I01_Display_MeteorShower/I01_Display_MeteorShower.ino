#include "unihiker_k10.h"

UNIHIKER_K10 k10;
uint8_t screen_dir = 2; // Portrait orientation (240x320)

// Clean Minimalist Light Theme Palette
const uint32_t COLOR_BG        = 0xF8FAFC; // Soft Slate Off-White
const uint32_t COLOR_CARD      = 0xFFFFFF; // Pure White Card Fill
const uint32_t COLOR_SKY       = 0xF0F9FF; // Soft Sky Tint Fill
const uint32_t COLOR_BORDER    = 0xE2E8F0; // Delicate 1px Border
const uint32_t COLOR_BORDER_SKY= 0xBAE6FD; // Sky 1px Border
const uint32_t COLOR_TEXT_PRI  = 0x0F172A; // Deep Slate Charcoal
const uint32_t COLOR_TEXT_MUTED= 0x64748B; // Slate Muted Label
const uint32_t COLOR_SAPPHIRE  = 0x2563EB; // Sapphire Brand Accent

const uint32_t starColors[] = {
    0x2563EB, // Sapphire
    0x0284C7, // Sky Blue
    0x7C3AED, // Violet
    0x059669, // Emerald
    0xD97706, // Amber
    0xEA580C  // Tangerine
};
const int numColors = sizeof(starColors) / sizeof(starColors[0]);

void initStaticUI() {
    k10.canvas->canvasClear();
    k10.canvas->canvasRectangle(0, 0, 240, 320, COLOR_BG, COLOR_BG, true);

    // App Header Bar (y: 0 to 40)
    k10.canvas->canvasRectangle(0, 0, 240, 40, COLOR_CARD, COLOR_CARD, true);
    k10.canvas->canvasLine(0, 40, 240, 40, COLOR_BORDER);
    k10.canvas->canvasText("Sky Animation", 14, 12, COLOR_TEXT_PRI, k10.canvas->eCNAndENFont16, 50, false);
    // Sapphire brand dot
    k10.canvas->canvasCircle(224, 20, 4, COLOR_SAPPHIRE, COLOR_SAPPHIRE, true);

    // Sky Viewport Frame (y: 48 to 276)
    k10.canvas->canvasRectangle(10, 48, 220, 228, COLOR_BORDER_SKY, COLOR_SKY, true);

    // Footer Info Bar (y: 284 to 320)
    k10.canvas->canvasRectangle(0, 284, 240, 36, COLOR_CARD, COLOR_CARD, true);
    k10.canvas->canvasLine(0, 284, 240, 284, COLOR_BORDER);
    k10.canvas->canvasText("Dynamic Shooting Stars Animation", 14, 294, COLOR_TEXT_MUTED, k10.canvas->eCNAndENFont16, 50, false);

    k10.canvas->updateCanvas();
}

void setup() {
    k10.begin();
    k10.initScreen(screen_dir);
    k10.creatCanvas();
    k10.rgb->brightness(5);
    k10.rgb->write(-1, 0x2563EB);

    initStaticUI();
}

void loop() {
    // Dynamic Partial Refresh: clear ONLY the sky animation viewport (x: 11..228, y: 49..274)
    k10.canvas->canvasRectangle(11, 49, 218, 226, COLOR_SKY, COLOR_SKY, true);

    // Draw subtle star points
    for (int s = 0; s < 20; s++) {
        int sx = (s * 47) % 210 + 15;
        int sy = (s * 61) % 210 + 55;
        k10.canvas->canvasPoint(sx, sy, 0x94A3B8);
    }

    // Draw delicate 1px shooting star streaks
    k10.canvas->canvasSetLineWidth(1);
    for (int i = 0; i < 10; i++) {
        int startX = random(18, 190);
        int startY = random(55, 230);
        int length = random(15, 36);
        uint32_t color = starColors[random(0, numColors)];

        int endX = startX + length;
        int endY = startY + (length * 3 / 4);

        if (endX < 224 && endY < 270) {
            k10.canvas->canvasLine(startX, startY, endX, endY, color);
            k10.canvas->canvasPoint(endX, endY, 0x0F172A);
        }
    }

    k10.canvas->updateCanvas();
    delay(70);
}