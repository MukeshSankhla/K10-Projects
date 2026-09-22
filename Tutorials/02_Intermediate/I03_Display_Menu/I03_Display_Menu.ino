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
    // Executive Obsidian theme background
    k10.setScreenBackground(0x0A0A14);

    // 1. Header Banner
    k10.canvas->canvasRectangle(0, 0, 240, 44, 0x141424, 0x141424, true);
    k10.canvas->canvasLine(0, 44, 240, 44, 0x312E81); // Royal indigo divider
    k10.canvas->canvasText("MAIN MENU", 62, 10, 0xF59E0B,
                           k10.canvas->eCNAndENFont24, 15, false);

    // 2. Render Menu Item Cards
    int startY = 56;
    int cardHeight = 36;
    int spacing = 8;

    for (int i = 0; i < totalItems; i++) {
        int cardY = startY + i * (cardHeight + spacing);

        if (i == activeIndex) {
            // Highlighted Active Card: Royal violet border with warm gold indicator
            k10.canvas->canvasRectangle(12, cardY, 216, cardHeight, 0x818CF8, 0x1E1B4B, true);
            k10.canvas->canvasText(">", 20, cardY + 9, 0xF59E0B,
                                   k10.canvas->eCNAndENFont16, 5, false);
            k10.canvas->canvasText(menuItems[i], 36, cardY + 9, 0xE0E7FF,
                                   k10.canvas->eCNAndENFont16, 22, false);
        } else {
            // Inactive Cards: Muted borders and slate text
            k10.canvas->canvasRectangle(12, cardY, 216, cardHeight, 0x27273A, 0x12121E, true);
            k10.canvas->canvasText(menuItems[i], 28, cardY + 9, 0x94A3B8,
                                   k10.canvas->eCNAndENFont16, 22, false);
        }
    }

    // 3. Footer Navigation Guide (Properly sized and centered)
    k10.canvas->canvasLine(15, 282, 225, 282, 0x312E81);
    k10.canvas->canvasText("Static Menu Preview", 46, 294, 0x818CF8,
                           k10.canvas->eCNAndENFont16, 22, false);

    k10.canvas->updateCanvas();
}

void setup() {
    k10.begin();
    k10.initScreen(screen_dir);
    k10.creatCanvas();

    k10.rgb->brightness(5);
    k10.rgb->write(-1, 0x818CF8); // Indigo glow

    drawMenu(highlightedIndex);
}

void loop() {
    // Static layout demonstration
    delay(100);
}
