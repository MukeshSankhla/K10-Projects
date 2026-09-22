#include "unihiker_k10.h"

UNIHIKER_K10 k10;
uint8_t screen_dir = 2; // Portrait orientation (240x320)

int countA = 0;
int countB = 0;

// Non-blocking button edge-detection trackers
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

void renderButtonUI(bool heldA, bool heldB) {
    k10.canvas->canvasClear();
    // Deep Navy theme background
    k10.setScreenBackground(0x0B0F19);

    // 1. Header Banner
    k10.canvas->canvasRectangle(0, 0, 240, 42, 0x111928, 0x111928, true);
    k10.canvas->canvasLine(0, 42, 240, 42, 0x38BDF8);
    k10.canvas->canvasText("BUTTON MONITOR", 28, 10, 0x38BDF8,
                           k10.canvas->eCNAndENFont24, 16, false);

    // 2. Button A Visualizer Pod (Left: x=14, w=100)
    uint32_t borderA = heldA ? 0x10B981 : 0x1E293B;
    uint32_t bgA = heldA ? 0x064E3B : 0x0F172A;
    k10.canvas->canvasRectangle(14, 56, 100, 130, borderA, bgA, true);

    k10.canvas->canvasText("BUTTON [A]", 22, 68, 0x94A3B8,
                           k10.canvas->eCNAndENFont16, 12, false);

    k10.canvas->canvasCircle(64, 115, 22, borderA, heldA ? 0x10B981 : 0x1E293B, true);
    k10.canvas->canvasText(heldA ? "HELD" : "IDLE", heldA ? 48 : 50, 107, heldA ? 0xFFFFFF : 0x64748B,
                           k10.canvas->eCNAndENFont16, 6, false);

    String cntStrA = "Hits: " + String(countA);
    int cntXA = 64 - (int)(cntStrA.length() * 4);
    k10.canvas->canvasText(cntStrA, cntXA, 154, 0x10B981,
                           k10.canvas->eCNAndENFont16, 12, false);

    // 3. Button B Visualizer Pod (Right: x=126, w=100)
    uint32_t borderB = heldB ? 0x3B82F6 : 0x1E293B;
    uint32_t bgB = heldB ? 0x1E3A8A : 0x0F172A;
    k10.canvas->canvasRectangle(126, 56, 100, 130, borderB, bgB, true);

    k10.canvas->canvasText("BUTTON [B]", 134, 68, 0x94A3B8,
                           k10.canvas->eCNAndENFont16, 12, false);

    k10.canvas->canvasCircle(176, 115, 22, borderB, heldB ? 0x3B82F6 : 0x1E293B, true);
    k10.canvas->canvasText(heldB ? "HELD" : "IDLE", heldB ? 160 : 162, 107, heldB ? 0xFFFFFF : 0x64748B,
                           k10.canvas->eCNAndENFont16, 6, false);

    String cntStrB = "Hits: " + String(countB);
    int cntXB = 176 - (int)(cntStrB.length() * 4);
    k10.canvas->canvasText(cntStrB, cntXB, 154, 0x38BDF8,
                           k10.canvas->eCNAndENFont16, 12, false);

    // 4. Combined Status Card
    k10.canvas->canvasRectangle(14, 200, 212, 58, 0x1E293B, 0x0F172A, true);
    if (heldA && heldB) {
        k10.canvas->canvasText("DUAL COMBO TRIGGER!", 36, 218, 0xF59E0B,
                               k10.canvas->eCNAndENFont16, 22, false);
    } else if (heldA) {
        k10.canvas->canvasText("Button A Active (Green)", 32, 218, 0x10B981,
                               k10.canvas->eCNAndENFont16, 24, false);
    } else if (heldB) {
        k10.canvas->canvasText("Button B Active (Blue)", 36, 218, 0x38BDF8,
                               k10.canvas->eCNAndENFont16, 24, false);
    } else {
        k10.canvas->canvasText("Non-Blocking State: IDLE", 28, 218, 0x64748B,
                               k10.canvas->eCNAndENFont16, 24, false);
    }

    // 5. Centered Footer Bar (Zero-overflow)
    k10.canvas->canvasLine(15, 276, 225, 276, 0x1E293B);
    k10.canvas->canvasText("Press / Hold Buttons to Test", 18, 290, 0x64748B,
                           k10.canvas->eCNAndENFont16, 26, false);

    k10.canvas->updateCanvas();
}

void setup() {
    k10.begin();
    k10.initScreen(screen_dir);
    k10.creatCanvas();

    k10.rgb->brightness(5);
    k10.rgb->write(-1, 0x000000); // Start with LEDs OFF

    renderButtonUI(false, false);
}

void loop() {
    // Edge detection for press counts
    if (checkButtonAPressed()) {
        countA++;
    }
    if (checkButtonBPressed()) {
        countB++;
    }

    // Real-time held state polling
    bool heldA = k10.buttonA->isPressed();
    bool heldB = k10.buttonB->isPressed();

    // Mirror on onboard RGB LEDs
    if (heldA && heldB) {
        k10.rgb->write(-1, 0x00FFFF); // Cyan for both
    } else if (heldA) {
        k10.rgb->write(-1, 0x00FF00); // Green for A
    } else if (heldB) {
        k10.rgb->write(-1, 0x0000FF); // Blue for B
    } else {
        k10.rgb->write(-1, 0x000000); // OFF
    }

    renderButtonUI(heldA, heldB);
    delay(30); // Smooth 33 FPS non-blocking UI refresh
}