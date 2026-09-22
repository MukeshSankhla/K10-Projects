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
String currentStatus = "Mode: System Ready";

const int START_Y = 54;
const int CARD_HEIGHT = 34;
const int CARD_SPACING = 6;

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

// Render individual menu item card component
void drawMenuCard(int i, bool isFocused) {
    int cardY = START_Y + i * (CARD_HEIGHT + CARD_SPACING);

    if (isFocused) {
        // Selected/Focused Card: Neon pink border with cyber turquoise text
        k10.canvas->canvasRectangle(12, cardY, 216, CARD_HEIGHT, 0xFF007F, 0x361152, true);
        k10.canvas->canvasText(">", 20, cardY + 8, 0xFFE600,
                               k10.canvas->eCNAndENFont16, 5, false);
        k10.canvas->canvasText(menuItems[i], 36, cardY + 8, 0x00F5D4,
                               k10.canvas->eCNAndENFont16, 22, false);
    } else {
        // Unfocused Cards
        k10.canvas->canvasRectangle(12, cardY, 216, CARD_HEIGHT, 0x3D185F, 0x1E0A32, true);
        k10.canvas->canvasText(menuItems[i], 28, cardY + 8, 0xA78BFA,
                               k10.canvas->eCNAndENFont16, 22, false);
    }
}

// Render dynamic feedback component only
void updateStatusBox(const String& status) {
    k10.canvas->canvasRectangle(12, 226, 216, 34, 0x00F5D4, 0x220C3A, true);
    k10.canvas->canvasText(status, 22, 234, 0x00F5D4,
                           k10.canvas->eCNAndENFont16, 24, false);
}

// Render static chrome (header and bottom navigation guide) once
void drawStaticChrome() {
    // 1. Header Banner
    k10.canvas->canvasRectangle(0, 0, 240, 44, 0x220C3A, 0x220C3A, true);
    k10.canvas->canvasLine(0, 44, 240, 44, 0xFF007F); // Neon magenta line
    k10.canvas->canvasText("ARCADE HARDWARE", 24, 10, 0xFFE600,
                           k10.canvas->eCNAndENFont24, 16, false);

    // 2. Navigation Controls Bar (Zero-overflow 2-column layout)
    k10.canvas->canvasLine(12, 272, 228, 272, 0xFF007F);
    k10.canvas->canvasRectangle(12, 276, 216, 34, 0x3D185F, 0x220C3A, true);
    k10.canvas->canvasText("[A] Next", 22, 285, 0x00F5D4,
                           k10.canvas->eCNAndENFont16, 12, false);
    k10.canvas->canvasText("[B] Trigger", 136, 285, 0xFFE600,
                           k10.canvas->eCNAndENFont16, 12, false);
}

void executeOption(int index) {
    switch (index) {
        case 0: // Red Alarm
            currentStatus = "Mode: Red Alarm";
            updateStatusBox(currentStatus);
            k10.canvas->updateCanvas();
            k10.rgb->write(-1, 0xFF0000);
            music.playTone(880, 200);
            break;

        case 1: // Emerald Calm
            currentStatus = "Mode: Emerald Calm";
            updateStatusBox(currentStatus);
            k10.canvas->updateCanvas();
            k10.rgb->write(-1, 0x00FF88);
            music.playTone(523, 300);
            break;

        case 2: // Cyber Cyan
            currentStatus = "Mode: Cyber Cyan";
            updateStatusBox(currentStatus);
            k10.canvas->updateCanvas();
            k10.rgb->write(-1, 0x00E5FF);
            music.playTone(659, 200);
            break;

        case 3: // Melodic Fanfare
            currentStatus = "Mode: Melodic";
            updateStatusBox(currentStatus);
            k10.canvas->updateCanvas();
            k10.rgb->write(0, 0xFF007F);
            k10.rgb->write(1, 0x00F5D4);
            k10.rgb->write(2, 0xFFE600);
            music.playTone(784, 150);
            music.playTone(1046, 300);
            break;

        case 4: // Mute & OFF
            currentStatus = "Mode: Systems OFF";
            updateStatusBox(currentStatus);
            k10.canvas->updateCanvas();
            k10.rgb->write(-1, 0x000000);
            music.stopPlayTone();
            break;
    }
}

void setup() {
    k10.begin();
    k10.initScreen(screen_dir);
    k10.creatCanvas();
    // Synthwave Neon Arcade background
    k10.setScreenBackground(0x160824);

    k10.rgb->brightness(5);
    k10.rgb->write(-1, 0x00F5D4);

    // Initial full paint: static chrome + all cards + status box
    drawStaticChrome();
    for (int i = 0; i < totalItems; i++) {
        drawMenuCard(i, i == activeIndex);
    }
    updateStatusBox(currentStatus);
    k10.canvas->updateCanvas();
}

void loop() {
    // Dynamic Partial Refresh on Button A: Update ONLY toggled menu cards
    if (checkButtonAPressed()) {
        int prevIndex = activeIndex;
        activeIndex = (activeIndex + 1) % totalItems;

        drawMenuCard(prevIndex, false); // Invalidate previous active card
        drawMenuCard(activeIndex, true); // Highlight newly active card
        k10.canvas->updateCanvas();
    }

    // Dynamic Partial Refresh on Button B: Update ONLY status box and execute hardware trigger
    if (checkButtonBPressed()) {
        executeOption(activeIndex);
    }

    delay(20); // Responsive loop tick
}
