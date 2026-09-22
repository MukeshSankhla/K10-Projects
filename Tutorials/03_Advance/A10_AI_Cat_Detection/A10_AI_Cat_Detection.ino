#include "unihiker_k10.h"
#include "AIRecognition.h"

UNIHIKER_K10 k10;
uint8_t screen_dir = 2; // Portrait (240x320)
AIRecognition ai;

// Clean Minimalist Light Theme Palette
const uint32_t COLOR_CARD      = 0xFFFFFF; // Pure White Card Fill
const uint32_t COLOR_BORDER    = 0xE2E8F0; // Delicate 1px Border
const uint32_t COLOR_TEXT_PRI  = 0x0F172A; // Deep Slate Charcoal
const uint32_t COLOR_TEXT_SEC  = 0x334155; // Slate Secondary
const uint32_t COLOR_TEXT_MUTED= 0x64748B; // Slate Muted Label
const uint32_t COLOR_AMBER     = 0xD97706; // Warm Amber Brand Accent
const uint32_t COLOR_GREEN     = 0x16A34A; // Success Green
const uint32_t COLOR_WHITE     = 0xFFFFFF;

bool hadCat = false;
unsigned long lastNoCatTime = 0;

void drawStaticChrome() {
    // Clean Header Overlay Bar (y: 8 to 44)
    k10.canvas->canvasRectangle(10, 8, 220, 36, COLOR_BORDER, COLOR_CARD, true);
    k10.canvas->canvasText("Pet Detection", 20, 16, COLOR_TEXT_PRI, k10.canvas->eCNAndENFont16, 50, false);
    // Amber brand accent dot
    k10.canvas->canvasCircle(216, 26, 4, COLOR_AMBER, COLOR_AMBER, true);

    // Bottom Telemetry Card Shell (y: 236 to 312)
    k10.canvas->canvasRectangle(10, 236, 220, 76, COLOR_BORDER, COLOR_CARD, true);
    k10.canvas->canvasText("Scanning for pets...", 20, 252, COLOR_TEXT_MUTED, k10.canvas->eCNAndENFont16, 50, false);
    k10.canvas->canvasText("Point camera at cat or dog", 20, 276, COLOR_TEXT_SEC, k10.canvas->eCNAndENFont16, 50, false);

    k10.canvas->updateCanvas();
}

// Partial refresh for detected pet metrics
void updateCatDisplay(int cx, int cy, int len, int wid) {
    k10.canvas->canvasRectangle(12, 240, 216, 68, COLOR_CARD, COLOR_CARD, true);

    // Status Pill
    k10.canvas->canvasRectangle(20, 246, 100, 18, COLOR_GREEN, COLOR_GREEN, true);
    k10.canvas->canvasText("PET DETECTED", 24, 247, COLOR_WHITE, k10.canvas->eCNAndENFont16, 50, false);

    String posStr = "Center: (" + String(cx) + ", " + String(cy) + ")";
    k10.canvas->canvasText(posStr, 20, 270, COLOR_TEXT_PRI, k10.canvas->eCNAndENFont16, 50, false);

    String dimStr = "Size: " + String(wid) + " x " + String(len) + " px";
    k10.canvas->canvasText(dimStr, 20, 288, COLOR_TEXT_MUTED, k10.canvas->eCNAndENFont16, 50, false);

    k10.canvas->updateCanvas();
    k10.rgb->write(-1, 0x16A34A);
}

// Partial refresh when no pet is in frame
void clearCatDisplay() {
    k10.canvas->canvasRectangle(12, 240, 216, 68, COLOR_CARD, COLOR_CARD, true);
    k10.canvas->canvasText("Scanning for pets...", 20, 252, COLOR_TEXT_MUTED, k10.canvas->eCNAndENFont16, 50, false);
    k10.canvas->canvasText("Point camera at cat or dog", 20, 276, COLOR_TEXT_SEC, k10.canvas->eCNAndENFont16, 50, false);
    k10.canvas->updateCanvas();
    k10.rgb->write(-1, 0xD97706);
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
    ai.switchAiMode(ai.Cat);

    k10.rgb->brightness(5);
    drawStaticChrome();
}

void loop() {
    if (ai.isDetectContent(AIRecognition::Cat)) {
        hadCat = true;
        lastNoCatTime = millis();

        int len = ai.getCatData(AIRecognition::Length);
        int wid = ai.getCatData(AIRecognition::Width);
        int cx  = ai.getCatData(AIRecognition::CenterX);
        int cy  = ai.getCatData(AIRecognition::CenterY);

        updateCatDisplay(cx, cy, len, wid);
    } else {
        if (hadCat && (millis() - lastNoCatTime > 600)) {
            hadCat = false;
            clearCatDisplay();
        }
    }
    delay(40);
}
