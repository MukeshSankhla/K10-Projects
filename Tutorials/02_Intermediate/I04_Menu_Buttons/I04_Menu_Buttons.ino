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

// Non-blocking button edge-detection state trackers
bool checkButtonAPressed() {
    static bool lastState = false;
    static unsigned long lastDebounceTime = 0;
    bool reading = k10.buttonA->isPressed();
    bool pressedEvent = false;

    if (reading != lastState) {
        lastDebounceTime = millis();
    }
    if ((millis() - lastDebounceTime) > 35) {
        static bool stableState = false;
        if (reading != stableState) {
            stableState = reading;
            if (stableState) {
                pressedEvent = true;
            }
        }
    }
    lastState = reading;
    return pressedEvent;
}

bool checkButtonBPressed() {
    static bool lastState = false;
    static unsigned long lastDebounceTime = 0;
    bool reading = k10.buttonB->isPressed();
    bool pressedEvent = false;

    if (reading != lastState) {
        lastDebounceTime = millis();
    }
    if ((millis() - lastDebounceTime) > 35) {
        static bool stableState = false;
        if (reading != stableState) {
            stableState = reading;
            if (stableState) {
                pressedEvent = true;
            }
        }
    }
    lastState = reading;
    return pressedEvent;
}

void renderMenu() {
    k10.canvas->canvasClear();
    // Arctic Night background
    k10.setScreenBackground(0x08101E);

    // 1. Header Banner
    k10.canvas->canvasRectangle(0, 0, 240, 44, 0x0E1A2E, 0x0E1A2E, true);
    k10.canvas->canvasLine(0, 44, 240, 44, 0x0284C7);
    k10.canvas->canvasText("MENU NAVIGATOR", 36, 10, 0x38BDF8,
                           k10.canvas->eCNAndENFont24, 16, false);

    // 2. Menu Item Cards
    int startY = 54;
    int cardHeight = 34;
    int spacing = 6;

    for (int i = 0; i < totalItems; i++) {
        int cardY = startY + i * (cardHeight + spacing);

        if (i == activeIndex) {
            // Selected/Focused Card: Glacial Ice cyan highlight
            k10.canvas->canvasRectangle(12, cardY, 216, cardHeight, 0x00E5FF, 0x0F2942, true);
            k10.canvas->canvasText(">", 20, cardY + 8, 0x00E5FF,
                                   k10.canvas->eCNAndENFont16, 5, false);
            k10.canvas->canvasText(menuItems[i], 36, cardY + 8, 0xE0FBFC,
                                   k10.canvas->eCNAndENFont16, 22, false);
        } else {
            // Unfocused Cards
            k10.canvas->canvasRectangle(12, cardY, 216, cardHeight, 0x1E3A5F, 0x0B1626, true);
            k10.canvas->canvasText(menuItems[i], 28, cardY + 8, 0x94A3B8,
                                   k10.canvas->eCNAndENFont16, 22, false);
        }
    }

    // 3. Selection Status Feedback Box
    k10.canvas->canvasRectangle(12, 226, 216, 34, 0x0284C7, 0x0E1A2E, true);
    String statusStr = "Active: " + selectedMessage;
    k10.canvas->canvasText(statusStr, 22, 234, 0x38BDF8,
                           k10.canvas->eCNAndENFont16, 24, false);

    // 4. Navigation Controls Bar (Zero-overflow 2-column layout)
    k10.canvas->canvasLine(12, 272, 228, 272, 0x1E3A5F);
    k10.canvas->canvasRectangle(12, 276, 216, 34, 0x1E3A5F, 0x0E1A2E, true);
    k10.canvas->canvasText("[A] Next", 22, 285, 0x00E5FF,
                           k10.canvas->eCNAndENFont16, 12, false);
    k10.canvas->canvasText("[B] Select", 136, 285, 0x38BDF8,
                           k10.canvas->eCNAndENFont16, 12, false);

    k10.canvas->updateCanvas();
}

void setup() {
    k10.begin();
    k10.initScreen(screen_dir);
    k10.creatCanvas();

    k10.rgb->brightness(5);
    k10.rgb->write(-1, 0x00E5FF);

    renderMenu();
}

void loop() {
    // Non-blocking Button A: Next Menu Item (Cyclic)
    if (checkButtonAPressed()) {
        activeIndex = (activeIndex + 1) % totalItems;
        renderMenu();
    }

    // Non-blocking Button B: Confirm / Execute Selected Option
    if (checkButtonBPressed()) {
        selectedMessage = String(menuItems[activeIndex]).substring(3); // Strip number prefix
        renderMenu();
    }

    delay(20); // Responsive loop tick
}
