#include "unihiker_k10.h"

UNIHIKER_K10 k10;
uint8_t screen_dir = 2; // Portrait orientation (240x320)

int stepCount = 0;
bool isHigh = false;
unsigned long lastStepTime = 0;
const int STEP_THRESHOLD = 1150; // Threshold above standard 1000mg 1G gravity
const unsigned long MIN_STEP_INTERVAL = 300; // Minimum 300ms between human steps

// Non-blocking button edge-detection tracker
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

void renderPedometerUI() {
    k10.canvas->canvasClear();
    // Pitch Black Athletic theme background
    k10.setScreenBackground(0x000000);

    // 1. Fitness Header Banner
    k10.canvas->canvasRectangle(0, 0, 240, 42, 0x111317, 0x111317, true);
    k10.canvas->canvasLine(0, 42, 240, 42, 0xA6FF00); // Activity neon lime line
    k10.canvas->canvasText("SMART PEDOMETER", 22, 10, 0xA6FF00,
                           k10.canvas->eCNAndENFont24, 16, false);

    // 2. Large Central Step Count Pod
    k10.canvas->canvasRectangle(14, 52, 212, 105, 0x1E222A, 0x0F1115, true);
    k10.canvas->canvasText("TOTAL STEPS", 72, 64, 0x94A3B8,
                           k10.canvas->eCNAndENFont16, 15, false);

    String stepStr = String(stepCount);
    int textX = 120 - (int)(stepStr.length() * 7);
    k10.canvas->canvasText(stepStr, textX, 98, 0xA6FF00,
                           k10.canvas->eCNAndENFont24, 12, false);

    // 3. Symmetrical Secondary Metric Pods (w=100, Left=14, Right=126)
    // Distance (0.75m per step average)
    float distanceKm = (stepCount * 0.75) / 1000.0;
    String distStr = String(distanceKm, 2) + " km";
    k10.canvas->canvasRectangle(14, 166, 100, 64, 0x1E222A, 0x0F1115, true);
    k10.canvas->canvasText("Distance", 34, 174, 0x94A3B8,
                           k10.canvas->eCNAndENFont16, 10, false);
    int distX = 64 - (int)(distStr.length() * 4);
    k10.canvas->canvasText(distStr, distX, 200, 0x00D2FF,
                           k10.canvas->eCNAndENFont16, 12, false);

    // Estimated Calories (0.04 kcal per step)
    float calories = stepCount * 0.04;
    String calStr = String((int)calories) + " kcal";
    k10.canvas->canvasRectangle(126, 166, 100, 64, 0x1E222A, 0x0F1115, true);
    k10.canvas->canvasText("Calories", 146, 174, 0x94A3B8,
                           k10.canvas->eCNAndENFont16, 10, false);
    int calX = 176 - (int)(calStr.length() * 4);
    k10.canvas->canvasText(calStr, calX, 200, 0xFF334B,
                           k10.canvas->eCNAndENFont16, 12, false);

    // 4. Live G-Force Motion Bar
    int strength = k10.getStrength();
    int barWidth = map(constrain(strength, 800, 1600), 800, 1600, 0, 208);
    k10.canvas->canvasRectangle(14, 240, 212, 14, 0x1E222A, 0x0A0B0D, true);
    uint32_t barColor = (strength > STEP_THRESHOLD) ? 0xA6FF00 : 0x00D2FF;
    if (barWidth > 0) {
        k10.canvas->canvasRectangle(16, 242, barWidth, 10, barColor, barColor, true);
    }

    // 5. Centered Reset Button Prompt (Zero-overflow)
    k10.canvas->canvasLine(15, 274, 225, 274, 0x1E222A);
    k10.canvas->canvasRectangle(20, 280, 200, 32, 0x1E222A, 0x111317, true);
    k10.canvas->canvasText("[A] Reset Counter", 48, 288, 0xA6FF00,
                           k10.canvas->eCNAndENFont16, 20, false);

    k10.canvas->updateCanvas();
}

void setup() {
    k10.begin();
    k10.initScreen(screen_dir);
    k10.creatCanvas();

    k10.rgb->brightness(5);
    k10.rgb->write(-1, 0xA6FF00); // Activity Lime glow

    renderPedometerUI();
}

void loop() {
    int strength = k10.getStrength();
    unsigned long now = millis();

    // Step detection state machine with time debounce
    if (strength > STEP_THRESHOLD && !isHigh) {
        if (now - lastStepTime > MIN_STEP_INTERVAL) {
            stepCount++;
            lastStepTime = now;
            isHigh = true;
            renderPedometerUI();
        }
    } else if (strength < STEP_THRESHOLD - 50 && isHigh) {
        isHigh = false;
    }

    // Non-blocking Button A: Reset Step Counter
    if (checkButtonAPressed()) {
        stepCount = 0;
        renderPedometerUI();
    }

    delay(20); // Responsive loop tick
}
