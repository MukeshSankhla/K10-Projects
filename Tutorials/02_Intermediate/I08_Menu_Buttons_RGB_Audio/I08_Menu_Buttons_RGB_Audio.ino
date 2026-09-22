#include "unihiker_k10.h"

UNIHIKER_K10 k10;
uint8_t screen_dir = 2; // Portrait orientation (240x320)
Music music;

const char* menuItems[] = {
    "1. Red Alarm Strobe",
    "2. Emerald Calm Mode",
    "3. Cyber Cyan Pulse",
    "4. Melodic Fanfare",
    "5. Mute & Turn OFF"
};
const int totalItems = sizeof(menuItems) / sizeof(menuItems[0]);
int activeIndex = 0;
String currentStatus = "System Idle";

void renderMenu() {
    k10.canvas->canvasClear();
    k10.setScreenBackground(0x0F172A);

    // 1. Header Banner
    k10.canvas->canvasRectangle(0, 0, 240, 44, 0x1E293B, 0x1E293B, true);
    k10.canvas->canvasText("HARDWARE MENU", 32, 12, 0xFEE715,
                           k10.canvas->eCNAndENFont24, 20, false);
    k10.canvas->canvasLine(15, 44, 225, 44, 0x334155);

    // 2. Menu Item Cards
    int startY = 54;
    int cardHeight = 34;
    int spacing = 6;

    for (int i = 0; i < totalItems; i++) {
        int cardY = startY + i * (cardHeight + spacing);

        if (i == activeIndex) {
            k10.canvas->canvasRectangle(12, cardY, 216, cardHeight, 0x00E5FF, 0x1E293B, true);
            k10.canvas->canvasText(">", 20, cardY + 8, 0xFEE715,
                                   k10.canvas->eCNAndENFont16, 10, false);
            k10.canvas->canvasText(menuItems[i], 36, cardY + 8, 0x00E5FF,
                                   k10.canvas->eCNAndENFont16, 24, false);
        } else {
            k10.canvas->canvasRectangle(12, cardY, 216, cardHeight, 0x334155, 0x111827, true);
            k10.canvas->canvasText(menuItems[i], 30, cardY + 8, 0x94A3B8,
                                   k10.canvas->eCNAndENFont16, 24, false);
        }
    }

    // 3. Active Mode Feedback Card
    k10.canvas->canvasRectangle(12, 226, 216, 34, 0x38BDF8, 0x1E293B, true);
    k10.canvas->canvasText(currentStatus, 20, 234, 0x00FF88,
                           k10.canvas->eCNAndENFont16, 26, false);

    // 4. Navigation Controls Bar
    k10.canvas->canvasLine(15, 270, 225, 270, 0x334155);
    k10.canvas->canvasText("[A] Next Option", 20, 278, 0x38BDF8,
                           k10.canvas->eCNAndENFont16, 26, false);
    k10.canvas->canvasText("[B] Trigger Action", 130, 278, 0xFEE715,
                           k10.canvas->eCNAndENFont16, 26, false);

    k10.canvas->updateCanvas();
}

void executeOption(int index) {
    switch (index) {
        case 0: // Red Alarm
            currentStatus = "Active: Red Alarm";
            renderMenu();
            k10.rgb->write(-1, 0xFF0000);
            music.playTone(880, 200); // 880Hz beep
            break;

        case 1: // Emerald Calm
            currentStatus = "Active: Emerald Mode";
            renderMenu();
            k10.rgb->write(-1, 0x00FF88);
            music.playTone(523, 300); // C5 chime
            break;

        case 2: // Cyber Cyan
            currentStatus = "Active: Cyber Cyan";
            renderMenu();
            k10.rgb->write(-1, 0x00E5FF);
            music.playTone(659, 200); // E5
            delay(100);
            music.playTone(784, 200); // G5
            break;

        case 3: // Melodic Fanfare
            currentStatus = "Active: Melodic Fanfare";
            renderMenu();
            // Rainbow LEDs & tone sequence
            k10.rgb->write(0, 0xFF0000);
            k10.rgb->write(1, 0x00FF00);
            k10.rgb->write(2, 0x0000FF);
            music.playTone(523, 150);
            music.playTone(659, 150);
            music.playTone(784, 150);
            music.playTone(1046, 300);
            break;

        case 4: // Mute & OFF
            currentStatus = "Active: All Systems OFF";
            renderMenu();
            k10.rgb->write(-1, 0x000000);
            music.stopPlayTone();
            break;
    }
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
    // Button A: Navigate to Next Item
    if (k10.buttonA->isPressed()) {
        activeIndex = (activeIndex + 1) % totalItems;
        renderMenu();

        while (k10.buttonA->isPressed()) {
            delay(50);
        }
    }
    // Button B: Execute Action
    else if (k10.buttonB->isPressed()) {
        executeOption(activeIndex);

        while (k10.buttonB->isPressed()) {
            delay(50);
        }
    }

    delay(20);
}
