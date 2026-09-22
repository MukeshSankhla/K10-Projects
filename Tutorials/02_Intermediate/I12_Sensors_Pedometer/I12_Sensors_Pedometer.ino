#include "unihiker_k10.h"

UNIHIKER_K10 k10;
uint8_t screen_dir = 2; // Portrait orientation (240x320)

int stepCount = 0;
bool isHigh = false;
unsigned long lastStepTime = 0;
const int STEP_THRESHOLD = 1150; // Threshold above standard 1000mg 1G gravity
const unsigned long MIN_STEP_INTERVAL = 300; // Minimum 300ms between human steps

// Clean Minimalist Light Theme Palette
const uint32_t COLOR_BG        = 0xF8FAFC; // Soft Slate Off-White
const uint32_t COLOR_CARD      = 0xFFFFFF; // Pure White Card Fill
const uint32_t COLOR_TRACK     = 0xF1F5F9; // Soft Track Fill
const uint32_t COLOR_BORDER    = 0xE2E8F0; // Delicate 1px Border
const uint32_t COLOR_BORDER_TRK= 0xCBD5E1; // Track Border
const uint32_t COLOR_TEXT_PRI  = 0x0F172A; // Deep Slate Charcoal
const uint32_t COLOR_TEXT_SEC  = 0x334155; // Slate Secondary
const uint32_t COLOR_TEXT_MUTED= 0x64748B; // Slate Muted Label
const uint32_t COLOR_GREEN     = 0x16A34A; // Activity Emerald
const uint32_t COLOR_BLUE      = 0x0284C7; // Sky Blue
const uint32_t COLOR_AMBER     = 0xD97706; // Amber

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

void drawScreenChrome() {
    k10.canvas->canvasClear();
    k10.canvas->canvasRectangle(0, 0, 240, 320, COLOR_BG, COLOR_BG, true);

    // App Header Bar (y: 0 to 40)
    k10.canvas->canvasRectangle(0, 0, 240, 40, COLOR_CARD, COLOR_CARD, true);
    k10.canvas->canvasLine(0, 40, 240, 40, COLOR_BORDER);
    k10.canvas->canvasText("Fitness Activity", 14, 12, COLOR_TEXT_PRI, k10.canvas->eCNAndENFont16, 50, false);
    // Green brand dot
    k10.canvas->canvasCircle(224, 20, 4, COLOR_GREEN, COLOR_GREEN, true);

    // Large Step Count Card (y: 48 to 154)
    k10.canvas->canvasRectangle(10, 48, 220, 106, COLOR_BORDER, COLOR_CARD, true);
    k10.canvas->canvasText("TOTAL STEPS TODAY", 20, 58, COLOR_TEXT_MUTED, k10.canvas->eCNAndENFont16, 20, false);

    // Distance Card (y: 162 to 226)
    k10.canvas->canvasRectangle(10, 162, 106, 64, COLOR_BORDER, COLOR_CARD, true);
    k10.canvas->canvasText("DISTANCE", 18, 170, COLOR_TEXT_MUTED, k10.canvas->eCNAndENFont16, 12, false);

    // Calories Card (y: 162 to 226)
    k10.canvas->canvasRectangle(124, 162, 106, 64, COLOR_BORDER, COLOR_CARD, true);
    k10.canvas->canvasText("CALORIES", 132, 170, COLOR_TEXT_MUTED, k10.canvas->eCNAndENFont16, 12, false);

    // Motion Bar Track (y: 236 to 254)
    k10.canvas->canvasRectangle(10, 236, 220, 18, COLOR_BORDER_TRK, COLOR_TRACK, true);

    // Footer Bar (y: 284 to 320)
    k10.canvas->canvasRectangle(0, 284, 240, 36, COLOR_CARD, COLOR_CARD, true);
    k10.canvas->canvasLine(0, 284, 240, 284, COLOR_BORDER);
    k10.canvas->canvasText("[A] Reset Step Counter", 14, 294, COLOR_GREEN, k10.canvas->eCNAndENFont16, 50, false);
}

void updateStepTelemetry(int steps) {
    // Clear step number area
    k10.canvas->canvasRectangle(18, 80, 204, 66, COLOR_CARD, COLOR_CARD, true);

    String sStr = String(steps);
    int stepX = 120 - (int)(sStr.length() * 11);
    k10.canvas->canvasText(sStr, stepX, 86, COLOR_TEXT_PRI, k10.canvas->eCNAndENFont24, 8, false);

    // Clear distance
    k10.canvas->canvasRectangle(16, 192, 94, 26, COLOR_CARD, COLOR_CARD, true);
    float km = steps * 0.00075;
    k10.canvas->canvasText(String(km, 2) + " km", 18, 196, COLOR_BLUE, k10.canvas->eCNAndENFont16, 10, false);

    // Clear calories
    k10.canvas->canvasRectangle(130, 192, 94, 26, COLOR_CARD, COLOR_CARD, true);
    int cal = (int)(steps * 0.04);
    k10.canvas->canvasText(String(cal) + " kcal", 132, 196, COLOR_AMBER, k10.canvas->eCNAndENFont16, 10, false);

    k10.canvas->updateCanvas();
}

void updateMotionBar(int mag) {
    k10.canvas->canvasRectangle(12, 238, 216, 14, COLOR_TRACK, COLOR_TRACK, true);

    int barW = map(constrain(mag, 800, 1600), 800, 1600, 0, 216);
    if (barW > 0) {
        uint32_t barCol = (mag >= STEP_THRESHOLD) ? COLOR_GREEN : COLOR_BORDER_TRK;
        k10.canvas->canvasRectangle(12, 238, barW, 14, barCol, barCol, true);
    }
}

void setup() {
    k10.begin();
    k10.initScreen(screen_dir);
    k10.creatCanvas();
    k10.setScreenBackground(COLOR_BG);

    k10.rgb->brightness(5);
    k10.rgb->write(-1, 0x16A34A);

    drawScreenChrome();
    updateStepTelemetry(0);
    k10.canvas->updateCanvas();
}

void loop() {
    float ax = k10.getAccelerometerX();
    float ay = k10.getAccelerometerY();
    float az = k10.getAccelerometerZ();

    int magnitude = (int)sqrt((ax * ax) + (ay * ay) + (az * az));
    unsigned long now = millis();

    if (magnitude > STEP_THRESHOLD && !isHigh) {
        if (now - lastStepTime > MIN_STEP_INTERVAL) {
            stepCount++;
            lastStepTime = now;
            isHigh = true;
            k10.rgb->write(-1, 0x16A34A);
            updateStepTelemetry(stepCount);
        }
    } else if (magnitude < 1050) {
        isHigh = false;
    }

    if (checkButtonAPressed()) {
        stepCount = 0;
        updateStepTelemetry(0);
        k10.rgb->write(-1, 0x0284C7);
    }

    updateMotionBar(magnitude);
    k10.canvas->updateCanvas();
    delay(20);
}
