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

const int START_Y = 48;
const int CARD_HEIGHT = 34;
const int CARD_SPACING = 6;

// Clean Minimalist Light Theme Palette
const uint32_t COLOR_BG        = 0xF8FAFC; // Soft Slate Off-White
const uint32_t COLOR_CARD      = 0xFFFFFF; // Pure White Card Fill
const uint32_t COLOR_BORDER    = 0xE2E8F0; // Delicate 1px Border
const uint32_t COLOR_TEXT_PRI  = 0x0F172A; // Deep Slate Charcoal
const uint32_t COLOR_TEXT_SEC  = 0x334155; // Slate Secondary
const uint32_t COLOR_TEXT_MUTED= 0x64748B; // Slate Muted Label
const uint32_t COLOR_SAPPHIRE  = 0x2563EB; // Sapphire Brand Accent
const uint32_t COLOR_GREEN     = 0x16A34A; // Success Green

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

void drawMenuCard(int i, bool isActive) {
    int cardY = START_Y + i * (CARD_HEIGHT + CARD_SPACING);

    if (isActive) {
        k10.canvas->canvasRectangle(10, cardY, 220, CARD_HEIGHT, COLOR_SAPPHIRE, COLOR_CARD, true);
        k10.canvas->canvasText(">", 20, cardY + 9, COLOR_SAPPHIRE, k10.canvas->eCNAndENFont16, 5, false);
        k10.canvas->canvasText(menuItems[i], 36, cardY + 9, COLOR_SAPPHIRE, k10.canvas->eCNAndENFont16, 22, false);
    } else {
        k10.canvas->canvasRectangle(10, cardY, 220, CARD_HEIGHT, COLOR_BORDER, COLOR_CARD, true);
        k10.canvas->canvasText(menuItems[i], 24, cardY + 9, COLOR_TEXT_SEC, k10.canvas->eCNAndENFont16, 22, false);
    }
}

void updateSelectedStatus(const String& msg) {
    // Clear only inner selection card (x: 12..228, y: 250..276)
    k10.canvas->canvasRectangle(12, 252, 216, 24, COLOR_CARD, COLOR_CARD, true);

    String statusText = "Selected: " + msg;
    k10.canvas->canvasText(statusText, 20, 256, COLOR_SAPPHIRE, k10.canvas->eCNAndENFont16, 22, false);
    k10.canvas->updateCanvas();
}

void drawStaticChrome() {
    k10.canvas->canvasClear();
    k10.canvas->canvasRectangle(0, 0, 240, 320, COLOR_BG, COLOR_BG, true);

    // App Header Bar (y: 0 to 40)
    k10.canvas->canvasRectangle(0, 0, 240, 40, COLOR_CARD, COLOR_CARD, true);
    k10.canvas->canvasLine(0, 40, 240, 40, COLOR_BORDER);
    k10.canvas->canvasText("Interactive Menu", 14, 12, COLOR_TEXT_PRI, k10.canvas->eCNAndENFont16, 50, false);
    // Sapphire brand dot
    k10.canvas->canvasCircle(224, 20, 4, COLOR_SAPPHIRE, COLOR_SAPPHIRE, true);

    // Selection Feedback Card (y: 248 to 280)
    k10.canvas->canvasRectangle(10, 248, 220, 32, COLOR_BORDER, COLOR_CARD, true);

    // Footer Bar (y: 286 to 320)
    k10.canvas->canvasRectangle(0, 286, 240, 34, COLOR_CARD, COLOR_CARD, true);
    k10.canvas->canvasLine(0, 286, 240, 286, COLOR_BORDER);
    k10.canvas->canvasText("[A] Next Item    [B] Select", 14, 294, COLOR_SAPPHIRE, k10.canvas->eCNAndENFont16, 50, false);
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
        drawMenuCard(i, i == activeIndex);
    }
    updateSelectedStatus(selectedMessage);

    k10.canvas->updateCanvas();
}

void loop() {
    if (checkButtonAPressed()) {
        int oldIndex = activeIndex;
        activeIndex = (activeIndex + 1) % totalItems;

        drawMenuCard(oldIndex, false);
        drawMenuCard(activeIndex, true);
        k10.canvas->updateCanvas();
    }

    if (checkButtonBPressed()) {
        selectedMessage = menuItems[activeIndex];
        updateSelectedStatus(selectedMessage);
    }

    delay(10);
}
