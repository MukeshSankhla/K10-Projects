#include "unihiker_k10.h"

UNIHIKER_K10 k10;
uint8_t screen_dir = 2; // Portrait orientation (240x320)

const char* menuItems[] = {
    "1. Device Info",
    "2. Test Sensors",
    "3. Display Colors",
    "4. Buzzer Melody",
    "5. System Reboot"
};
const int totalItems = sizeof(menuItems) / sizeof(menuItems[0]);
int activeIndex = 0;
String selectedMessage = "None";

void renderMenu() {
    k10.canvas->canvasClear();
    k10.setScreenBackground(0x0F172A);

    // 1. Header Banner
    k10.canvas->canvasRectangle(0, 0, 240, 44, 0x1E293B, 0x1E293B, true);
    k10.canvas->canvasText("MENU + BUTTONS", 30, 12, 0xFEE715,
                           k10.canvas->eCNAndENFont24, 20, false);
    k10.canvas->canvasLine(15, 44, 225, 44, 0x334155);

    // 2. Menu Item Cards
    int startY = 56;
    int cardHeight = 36;
    int spacing = 6;

    for (int i = 0; i < totalItems; i++) {
        int cardY = startY + i * (cardHeight + spacing);

        if (i == activeIndex) {
            // Selected/Focused Card
            k10.canvas->canvasRectangle(12, cardY, 216, cardHeight, 0x00E5FF, 0x1E293B, true);
            k10.canvas->canvasText(">", 20, cardY + 9, 0xFEE715,
                                   k10.canvas->eCNAndENFont16, 10, false);
            k10.canvas->canvasText(menuItems[i], 38, cardY + 9, 0x00E5FF,
                                   k10.canvas->eCNAndENFont16, 24, false);
        } else {
            // Unfocused Cards
            k10.canvas->canvasRectangle(12, cardY, 216, cardHeight, 0x334155, 0x111827, true);
            k10.canvas->canvasText(menuItems[i], 30, cardY + 9, 0x94A3B8,
                                   k10.canvas->eCNAndENFont16, 24, false);
        }
    }

    // 3. Selection Status Feedback Box
    k10.canvas->canvasRectangle(12, 230, 216, 32, 0x38BDF8, 0x1E293B, true);
    String statusStr = "Selected: " + selectedMessage;
    k10.canvas->canvasText(statusStr, 20, 238, 0x00FF88,
                           k10.canvas->eCNAndENFont16, 26, false);

    // 4. Navigation Controls Bar
    k10.canvas->canvasLine(15, 270, 225, 270, 0x334155);
    k10.canvas->canvasText("[A] Next Item", 20, 278, 0x38BDF8,
                           k10.canvas->eCNAndENFont16, 26, false);
    k10.canvas->canvasText("[B] Select Item", 130, 278, 0xFEE715,
                           k10.canvas->eCNAndENFont16, 26, false);

    k10.canvas->updateCanvas();
}

void setup() {
    k10.begin();
    k10.initScreen(screen_dir);
    k10.creatCanvas();

    k10.rgb->brightness(5);
    k10.rgb->write(-1, 0x000000);

    renderMenu();
}

void loop() {
    // Button A: Next Menu Item (Cyclic)
    if (k10.buttonA->isPressed()) {
        activeIndex = (activeIndex + 1) % totalItems;
        renderMenu();

        while (k10.buttonA->isPressed()) {
            delay(50);
        }
    }
    // Button B: Confirm / Execute Selected Option
    else if (k10.buttonB->isPressed()) {
        selectedMessage = String(menuItems[activeIndex]).substring(3); // Strip number prefix
        renderMenu();

        while (k10.buttonB->isPressed()) {
            delay(50);
        }
    }

    delay(20);
}
