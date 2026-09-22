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
int highlightedIndex = 1; // Example active item (Sensor Monitor)

void drawMenu(int activeIndex) {
    k10.canvas->canvasClear();
    k10.setScreenBackground(0x0F172A); // Deep slate dark background

    // 1. Header Banner
    k10.canvas->canvasRectangle(0, 0, 240, 46, 0x1E293B, 0x1E293B, true);
    k10.canvas->canvasText("MAIN MENU", 60, 12, 0xFEE715,
                           k10.canvas->eCNAndENFont24, 20, false);
    k10.canvas->canvasLine(15, 46, 225, 46, 0x334155);

    // 2. Render Menu Item Cards
    int startY = 60;
    int cardHeight = 38;
    int spacing = 8;

    for (int i = 0; i < totalItems; i++) {
        int cardY = startY + i * (cardHeight + spacing);

        if (i == activeIndex) {
            // Highlighted Active Card: Bright blue border with cyan text & indicator
            k10.canvas->canvasRectangle(12, cardY, 216, cardHeight, 0x00E5FF, 0x1E293B, true);
            k10.canvas->canvasText(">", 20, cardY + 10, 0xFEE715,
                                   k10.canvas->eCNAndENFont16, 10, false);
            k10.canvas->canvasText(menuItems[i], 38, cardY + 10, 0x00E5FF,
                                   k10.canvas->eCNAndENFont16, 24, false);
        } else {
            // Inactive Cards: Subtle border and slate text
            k10.canvas->canvasRectangle(12, cardY, 216, cardHeight, 0x334155, 0x111827, true);
            k10.canvas->canvasText(menuItems[i], 30, cardY + 10, 0x94A3B8,
                                   k10.canvas->eCNAndENFont16, 24, false);
        }
    }

    // 3. Footer Navigation Guide
    k10.canvas->canvasLine(15, 290, 225, 290, 0x334155);
    k10.canvas->canvasText("Static Layout Demo: Menu Cards", 15, 298, 0x64748B,
                           k10.canvas->eCNAndENFont16, 30, false);

    k10.canvas->updateCanvas();
}

void setup() {
    k10.begin();
    k10.initScreen(screen_dir);
    k10.creatCanvas();

    k10.rgb->brightness(5);
    k10.rgb->write(-1, 0x00E5FF);

    drawMenu(highlightedIndex);
}

void loop() {
    // Static layout demonstration
    delay(100);
}
