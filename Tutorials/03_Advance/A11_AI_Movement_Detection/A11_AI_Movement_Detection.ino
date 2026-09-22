#include "unihiker_k10.h"
#include "AIRecognition.h"

UNIHIKER_K10 k10;
uint8_t screen_dir = 2; // Portrait (240x320)
AIRecognition ai;

bool lastMotion = false;

// Clean Minimalist Light Theme Palette
const uint32_t COLOR_CARD      = 0xFFFFFF; // Pure White Card Fill
const uint32_t COLOR_BORDER    = 0xE2E8F0; // Delicate 1px Border
const uint32_t COLOR_TEXT_PRI  = 0x0F172A; // Deep Slate Charcoal
const uint32_t COLOR_TEXT_SEC  = 0x334155; // Slate Secondary
const uint32_t COLOR_TEXT_MUTED= 0x64748B; // Slate Muted Label
const uint32_t COLOR_RED       = 0xDC2626; // Alert Crimson Brand Accent
const uint32_t COLOR_GREEN     = 0x16A34A; // Clear Green
const uint32_t COLOR_WHITE     = 0xFFFFFF;

void drawStaticChrome() {
    // Clean Header Overlay Bar (y: 8 to 44)
    k10.canvas->canvasRectangle(10, 8, 220, 36, COLOR_BORDER, COLOR_CARD, true);
    k10.canvas->canvasText("Motion Monitor", 20, 16, COLOR_TEXT_PRI, k10.canvas->eCNAndENFont16, 50, false);
    // Alert Crimson brand accent dot
    k10.canvas->canvasCircle(216, 26, 4, COLOR_RED, COLOR_RED, true);

    // Bottom Status Card Shell (y: 236 to 312)
    k10.canvas->canvasRectangle(10, 236, 220, 76, COLOR_BORDER, COLOR_CARD, true);
    k10.canvas->canvasText("SURVEILLANCE STATUS", 20, 244, COLOR_TEXT_MUTED, k10.canvas->eCNAndENFont16, 50, false);
    k10.canvas->canvasText("Sensitivity: 50", 20, 290, COLOR_TEXT_MUTED, k10.canvas->eCNAndENFont16, 50, false);

    k10.canvas->updateCanvas();
}

// Partial refresh for motion alert status
void updateMotionState(bool detected) {
    // Clear status pill region
    k10.canvas->canvasRectangle(20, 262, 180, 24, COLOR_CARD, COLOR_CARD, true);

    if (detected) {
        k10.canvas->canvasRectangle(20, 264, 134, 20, COLOR_RED, COLOR_RED, true);
        k10.canvas->canvasText("MOTION DETECTED", 24, 266, COLOR_WHITE, k10.canvas->eCNAndENFont16, 50, false);
        k10.rgb->write(-1, 0xDC2626);
    } else {
        k10.canvas->canvasRectangle(20, 264, 96, 20, COLOR_GREEN, COLOR_GREEN, true);
        k10.canvas->canvasText("AREA CLEAR", 28, 266, COLOR_WHITE, k10.canvas->eCNAndENFont16, 50, false);
        k10.rgb->write(-1, 0x16A34A);
    }
    k10.canvas->updateCanvas();
}

void setup() {
    Serial.begin(115200);
    k10.begin();
    k10.initScreen(screen_dir);
    ai.initAi();
    k10.initBgCamerImage();
    k10.setBgCamerImage(false);
    k10.creatCanvas();
    ai.switchAiMode(ai.NoMode);
    k10.setBgCamerImage(true);
    ai.switchAiMode(ai.Move);
    ai.setMotinoThreshold(50);

    k10.rgb->brightness(5);
    drawStaticChrome();
    updateMotionState(false);
}

void loop() {
    bool hasMotion = ai.isDetectContent(AIRecognition::Move);
    if (hasMotion != lastMotion) {
        lastMotion = hasMotion;
        updateMotionState(hasMotion);
    }
    delay(40);
}