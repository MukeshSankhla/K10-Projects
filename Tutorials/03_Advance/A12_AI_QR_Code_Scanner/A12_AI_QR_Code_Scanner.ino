#include "unihiker_k10.h"
#include "AIRecognition.h"

UNIHIKER_K10 k10;
uint8_t screen_dir = 2; // Portrait (240x320)
AIRecognition ai;

String lastPayload = "";
unsigned long lastCodeTime = 0;

// Clean Minimalist Light Theme Palette
const uint32_t COLOR_CARD      = 0xFFFFFF; // Pure White Card Fill
const uint32_t COLOR_BORDER    = 0xE2E8F0; // Delicate 1px Border
const uint32_t COLOR_TEXT_PRI  = 0x0F172A; // Deep Slate Charcoal
const uint32_t COLOR_TEXT_SEC  = 0x334155; // Slate Secondary
const uint32_t COLOR_TEXT_MUTED= 0x64748B; // Slate Muted Label
const uint32_t COLOR_EMERALD   = 0x059669; // Clean Emerald Brand Accent
const uint32_t COLOR_GREEN     = 0x16A34A; // Success Green
const uint32_t COLOR_WHITE     = 0xFFFFFF;

void drawStaticChrome() {
    // Clean Header Overlay Bar (y: 8 to 44)
    k10.canvas->canvasRectangle(10, 8, 220, 36, COLOR_BORDER, COLOR_CARD, true);
    k10.canvas->canvasText("QR Code Scanner", 20, 16, COLOR_TEXT_PRI, k10.canvas->eCNAndENFont16, 50, false);
    // Emerald brand accent dot
    k10.canvas->canvasCircle(216, 26, 4, COLOR_EMERALD, COLOR_EMERALD, true);

    // Viewfinder 1px Corner Markers (x: 50..190, y: 80..220)
    // Top-Left
    k10.canvas->canvasLine(50, 80, 70, 80, COLOR_WHITE);
    k10.canvas->canvasLine(50, 80, 50, 100, COLOR_WHITE);
    // Top-Right
    k10.canvas->canvasLine(190, 80, 170, 80, COLOR_WHITE);
    k10.canvas->canvasLine(190, 80, 190, 100, COLOR_WHITE);
    // Bottom-Left
    k10.canvas->canvasLine(50, 220, 70, 220, COLOR_WHITE);
    k10.canvas->canvasLine(50, 220, 50, 200, COLOR_WHITE);
    // Bottom-Right
    k10.canvas->canvasLine(190, 220, 170, 220, COLOR_WHITE);
    k10.canvas->canvasLine(190, 220, 190, 200, COLOR_WHITE);

    // Bottom Result Card (y: 234 to 312)
    k10.canvas->canvasRectangle(10, 234, 220, 78, COLOR_BORDER, COLOR_CARD, true);
    k10.canvas->canvasText("SCANNER STANDBY", 20, 242, COLOR_TEXT_MUTED, k10.canvas->eCNAndENFont16, 50, false);
    k10.canvas->canvasText("Point at any QR code", 20, 266, COLOR_TEXT_SEC, k10.canvas->eCNAndENFont16, 50, false);

    k10.canvas->updateCanvas();
}

// Partial refresh for decoded QR payload
void updateCodePayload(const String& payload) {
    k10.canvas->canvasRectangle(12, 236, 216, 74, COLOR_CARD, COLOR_CARD, true);

    // Status Pill
    k10.canvas->canvasRectangle(20, 242, 104, 18, COLOR_GREEN, COLOR_GREEN, true);
    k10.canvas->canvasText("CODE DECODED", 24, 243, COLOR_WHITE, k10.canvas->eCNAndENFont16, 50, false);

    // Truncate payload display gracefully if needed
    String p1 = payload.substring(0, 22);
    String p2 = (payload.length() > 22) ? payload.substring(22, 44) : "";

    k10.canvas->canvasText(p1, 20, 268, COLOR_TEXT_PRI, k10.canvas->eCNAndENFont16, 50, false);
    if (p2.length() > 0) {
        k10.canvas->canvasText(p2, 20, 288, COLOR_TEXT_SEC, k10.canvas->eCNAndENFont16, 50, false);
    }

    k10.canvas->updateCanvas();
    k10.rgb->write(-1, 0x16A34A);
}

void clearCodePayload() {
    k10.canvas->canvasRectangle(12, 236, 216, 74, COLOR_CARD, COLOR_CARD, true);
    k10.canvas->canvasText("SCANNER STANDBY", 20, 242, COLOR_TEXT_MUTED, k10.canvas->eCNAndENFont16, 50, false);
    k10.canvas->canvasText("Point at any QR code", 20, 266, COLOR_TEXT_SEC, k10.canvas->eCNAndENFont16, 50, false);
    k10.canvas->updateCanvas();
    k10.rgb->write(-1, 0x059669);
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
    ai.switchAiMode(ai.Code);

    k10.rgb->brightness(5);
    drawStaticChrome();
}

void loop() {
    if (ai.isDetectContent(AIRecognition::Code)) {
        String content = ai.getQrCodeContent();
        if (content.length() > 0 && content != lastPayload) {
            lastPayload = content;
            lastCodeTime = millis();
            updateCodePayload(content);
        }
    } else {
        if (lastPayload.length() > 0 && (millis() - lastCodeTime > 3000)) {
            lastPayload = "";
            clearCodePayload();
        }
    }
    delay(40);
}