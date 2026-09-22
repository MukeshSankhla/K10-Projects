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

// 1. Render static layout (header, pod frames, metric titles, motion track frame, reset prompt) once
void drawScreenChrome() {
    // Fitness Header Banner
    k10.canvas->canvasRectangle(0, 0, 240, 42, 0x111317, 0x111317, true);
    k10.canvas->canvasLine(0, 42, 240, 42, 0xA6FF00); // Activity neon lime line
    k10.canvas->canvasText("SMART PEDOMETER", 22, 10, 0xA6FF00,
                           k10.canvas->eCNAndENFont24, 16, false);

    // Large Central Step Count Pod Frame & Title
    k10.canvas->canvasRectangle(14, 52, 212, 105, 0x1E222A, 0x0F1115, true);
    k10.canvas->canvasText("TOTAL STEPS", 72, 64, 0x94A3B8,
                           k10.canvas->eCNAndENFont16, 15, false);

    // Distance Pod Frame & Title
    k10.canvas->canvasRectangle(14, 166, 100, 64, 0x1E222A, 0x0F1115, true);
    k10.canvas->canvasText("Distance", 34, 174, 0x94A3B8,
                           k10.canvas->eCNAndENFont16, 10, false);

    // Calories Pod Frame & Title
    k10.canvas->canvasRectangle(126, 166, 100, 64, 0x1E222A, 0x0F1115, true);
    k10.canvas->canvasText("Calories", 146, 174, 0x94A3B8,
                           k10.canvas->eCNAndENFont16, 10, false);

    // Motion Bar Track Container
    k10.canvas->canvasRectangle(14, 240, 212, 14, 0x1E222A, 0x0A0B0D, true);

    // Reset Button Prompt (Zero-overflow)
    k10.canvas->canvasLine(15, 274, 225, 274, 0x1E222A);
    k10.canvas->canvasRectangle(20, 280, 200, 32, 0x1E222A, 0x111317, true);
    k10.canvas->canvasText("[A] Reset Counter", 48, 288, 0xA6FF00,
                           k10.canvas->eCNAndENFont16, 20, false);
}

// 2. Dynamic Partial Refresh: update ONLY step count, distance, and calorie numbers
void updateStepMetrics(int steps) {
    // Clear and redraw step count
    k10.canvas->canvasRectangle(20, 92, 200, 44, 0x0F1115, 0x0F1115, true);
    String stepStr = String(steps);
    int textX = 120 - (int)(stepStr.length() * 7);
    k10.canvas->canvasText(stepStr, textX, 98, 0xA6FF00,
                           k10.canvas->eCNAndENFont24, 12, false);

    // Clear and redraw distance
    k10.canvas->canvasRectangle(20, 194, 88, 26, 0x0F1115, 0x0F1115, true);
    float distanceKm = (steps * 0.75) / 1000.0;
    String distStr = String(distanceKm, 2) + " km";
    int distX = 64 - (int)(distStr.length() * 4);
    k10.canvas->canvasText(distStr, distX, 200, 0x00D2FF,
                           k10.canvas->eCNAndENFont16, 12, false);

    // Clear and redraw calories
    k10.canvas->canvasRectangle(132, 194, 88, 26, 0x0F1115, 0x0F1115, true);
    float calories = steps * 0.04;
    String calStr = String((int)calories) + " kcal";
    int calX = 176 - (int)(calStr.length() * 4);
    k10.canvas->canvasText(calStr, calX, 200, 0xFF334B,
                           k10.canvas->eCNAndENFont16, 12, false);
}

// 3. Dynamic Partial Refresh: update ONLY the live G-force motion bar fill
void updateMotionBar(int strength) {
    int barWidth = map(constrain(strength, 800, 1600), 800, 1600, 0, 208);
    k10.canvas->canvasRectangle(16, 242, 208, 10, 0x0A0B0D, 0x0A0B0D, true);
    uint32_t barColor = (strength > STEP_THRESHOLD) ? 0xA6FF00 : 0x00D2FF;
    if (barWidth > 0) {
        k10.canvas->canvasRectangle(16, 242, barWidth, 10, barColor, barColor, true);
    }
}

void setup() {
    k10.begin();
    k10.initScreen(screen_dir);
    k10.creatCanvas();
    // Pitch Black Athletic theme background
    k10.setScreenBackground(0x000000);

    k10.rgb->brightness(5);
    k10.rgb->write(-1, 0xA6FF00); // Activity Lime glow

    // Initial paint: static chrome + initial numbers + initial motion bar
    drawScreenChrome();
    updateStepMetrics(stepCount);
    updateMotionBar(k10.getStrength());
    k10.canvas->updateCanvas();
}

void loop() {
    int strength = k10.getStrength();
    unsigned long now = millis();
    bool metricsChanged = false;

    // Step detection state machine with time debounce
    if (strength > STEP_THRESHOLD && !isHigh) {
        if (now - lastStepTime > MIN_STEP_INTERVAL) {
            stepCount++;
            lastStepTime = now;
            isHigh = true;
            metricsChanged = true;
        }
    } else if (strength < STEP_THRESHOLD - 50 && isHigh) {
        isHigh = false;
    }

    // Non-blocking Button A: Reset Step Counter
    if (checkButtonAPressed()) {
        stepCount = 0;
        metricsChanged = true;
    }

    // Dynamic Partial Refresh: update numbers if changed, and always update motion bar
    if (metricsChanged) {
        updateStepMetrics(stepCount);
    }
    updateMotionBar(strength);

    // Flush canvas without full-screen flicker
    k10.canvas->updateCanvas();
    delay(20); // Responsive loop tick
}
