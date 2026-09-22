#include "unihiker_k10.h"

UNIHIKER_K10 k10;
uint8_t screen_dir = 2; // Portrait orientation (240x320)

// Menu structure definition
const char* menuItems[] = {
    "1. System Status",
    "2. Sensor Monitor",
    "3. RGB Light Show",
    "4. Audio Settings",
    "5. SD File Browser"
};
const int totalItems = sizeof(menuItems) / sizeof(menuItems[0]);
int highlightedIndex = 1; // Active item

const int START_Y = 52;
const int CARD_HEIGHT = 38;
const int CARD_SPACING = 8;

// Clean Minimalist Light Theme Palette
const uint32_t COLOR_BG        = 0xF8FAFC; // Soft Slate Off-White
const uint32_t COLOR_CARD      = 0xFFFFFF; // Pure White Card Fill
const uint32_t COLOR_BORDER    = 0xE2E8F0; // Delicate 1px Border
const uint32_t COLOR_TEXT_PRI  = 0x0F172A; // Deep Slate Charcoal
const uint32_t COLOR_TEXT_SEC  = 0x334155; // Slate Secondary
const uint32_t COLOR_TEXT_MUTED= 0x64748B; // Slate Muted Label
const uint32_t COLOR_SAPPHIRE  = 0x2563EB; // Sapphire Brand Accent

void drawMenuCard(int i, bool isActive) {
    int cardY = START_Y + i * (CARD_HEIGHT + CARD_SPACING);

    if (isActive) {
        // Active Card: Sapphire border outline with sapphire text
        k10.canvas->canvasRectangle(10, cardY, 220, CARD_HEIGHT, COLOR_SAPPHIRE, COLOR_CARD, true);
        k10.canvas->canvasText(">", 20, cardY + 11, COLOR_SAPPHIRE, k10.canvas->eCNAndENFont16, 5, false);
        k10.canvas->canvasText(menuItems[i], 36, cardY + 11, COLOR_SAPPHIRE, k10.canvas->eCNAndENFont16, 22, false);
    } else {
        // Inactive Card: Delicate 1px border with dark slate text
        k10.canvas->canvasRectangle(10, cardY, 220, CARD_HEIGHT, COLOR_BORDER, COLOR_CARD, true);
        k10.canvas->canvasText(menuItems[i], 24, cardY + 11, COLOR_TEXT_SEC, k10.canvas->eCNAndENFont16, 22, false);
    }
}

void drawStaticChrome() {
    k10.canvas->canvasClear();
    k10.canvas->canvasRectangle(0, 0, 240, 320, COLOR_BG, COLOR_BG, true);

    // App Header Bar (y: 0 to 40)
    k10.canvas->canvasRectangle(0, 0, 240, 40, COLOR_CARD, COLOR_CARD, true);
    k10.canvas->canvasLine(0, 40, 240, 40, COLOR_BORDER);
    k10.canvas->canvasText("Main Menu", 14, 12, COLOR_TEXT_PRI, k10.canvas->eCNAndENFont16, 50, false);
    // Sapphire brand dot
    k10.canvas->canvasCircle(224, 20, 4, COLOR_SAPPHIRE, COLOR_SAPPHIRE, true);

    // Footer Navigation Guide (y: 284 to 320)
    k10.canvas->canvasRectangle(0, 284, 240, 36, COLOR_CARD, COLOR_CARD, true);
    k10.canvas->canvasLine(0, 284, 240, 284, COLOR_BORDER);
    k10.canvas->canvasText("System Settings Menu", 14, 294, COLOR_TEXT_MUTED, k10.canvas->eCNAndENFont16, 50, false);
}

void setup() {
    k10.begin();
    k10.initScreen(screen_dir);
    k10.creatCanvas();
    k10.setScreenBackground(COLOR_BG);

    k10.rgb->brightness(5);
    k10.rgb->write(-1, 0x2563EB);

    drawStaticChrome();

    for (int i = 0; i < totalItems; i++) {
        drawMenuCard(i, i == highlightedIndex);
    }

    k10.canvas->updateCanvas();
}

void loop() {
    delay(200);
}
