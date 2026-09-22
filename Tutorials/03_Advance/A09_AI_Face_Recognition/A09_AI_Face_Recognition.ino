#include "unihiker_k10.h"
#include "AIRecognition.h"

UNIHIKER_K10 k10;
uint8_t screen_dir = 2; // Portrait (240x320)
AIRecognition ai;

String recognizedID = "";
String currentAction = "Ready to verify";

// Clean Minimalist Light Theme Palette
const uint32_t COLOR_CARD      = 0xFFFFFF; // Pure White Card Fill
const uint32_t COLOR_BORDER    = 0xE2E8F0; // Delicate 1px Border
const uint32_t COLOR_TEXT_PRI  = 0x0F172A; // Deep Slate Charcoal
const uint32_t COLOR_TEXT_SEC  = 0x334155; // Slate Secondary
const uint32_t COLOR_TEXT_MUTED= 0x64748B; // Slate Muted Label
const uint32_t COLOR_COBALT    = 0x1D4ED8; // Clean Cobalt Blue
const uint32_t COLOR_GREEN     = 0x16A34A; // Success Green
const uint32_t COLOR_RED       = 0xDC2626; // Alert Red
const uint32_t COLOR_WHITE     = 0xFFFFFF;

void drawStaticChrome() {
    // Clean Header Overlay Bar (y: 8 to 44)
    k10.canvas->canvasRectangle(10, 8, 220, 36, COLOR_BORDER, COLOR_CARD, true);
    k10.canvas->canvasText("Face Recognition", 20, 16, COLOR_TEXT_PRI, k10.canvas->eCNAndENFont16, 50, false);
    // Cobalt brand accent dot
    k10.canvas->canvasCircle(216, 26, 4, COLOR_COBALT, COLOR_COBALT, true);

    // Bottom Result Card (y: 232 to 312)
    k10.canvas->canvasRectangle(10, 232, 220, 80, COLOR_BORDER, COLOR_CARD, true);
    k10.canvas->canvasText("BIOMETRIC STATUS", 20, 240, COLOR_TEXT_MUTED, k10.canvas->eCNAndENFont16, 50, false);
    k10.canvas->canvasText("Ready - Align face", 20, 260, COLOR_TEXT_SEC, k10.canvas->eCNAndENFont16, 50, false);
    k10.canvas->canvasText("[A] Enroll Face   [B] Verify", 20, 286, COLOR_COBALT, k10.canvas->eCNAndENFont16, 50, false);

    k10.canvas->updateCanvas();
}

// Partial refresh for recognized ID and action
void updateRecognitionCard(const String& id, const String& statusMsg, uint32_t statusColor) {
    // Clear inner result card
    k10.canvas->canvasRectangle(12, 238, 216, 44, COLOR_CARD, COLOR_CARD, true);

    k10.canvas->canvasText(statusMsg, 20, 242, statusColor, k10.canvas->eCNAndENFont16, 50, false);

    if (id.length() > 0) {
        String idStr;
        if (id == "-1" || id.toInt() < 0) {
            idStr = "ID: Unknown";
        } else {
            idStr = "ID: " + id;
        }
        k10.canvas->canvasText(idStr, 20, 262, COLOR_TEXT_PRI, k10.canvas->eCNAndENFont16, 50, false);
    } else {
        k10.canvas->canvasText("Press [B] to Scan", 20, 262, COLOR_TEXT_MUTED, k10.canvas->eCNAndENFont16, 50, false);
    }

    k10.canvas->updateCanvas();
}

void onButtonAPressed() {
    k10.rgb->write(-1, 0x16A34A);
    ai.sendFaceCmd(ENROLL);
    updateRecognitionCard("", "Enrolling new face...", COLOR_GREEN);
}

void onButtonBPressed() {
    k10.rgb->write(-1, 0x1D4ED8);
    ai.sendFaceCmd(RECOGNIZE);
    updateRecognitionCard("", "Verifying face...", COLOR_COBALT);
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

    k10.buttonA->setPressedCallback(onButtonAPressed);
    k10.buttonB->setPressedCallback(onButtonBPressed);

    k10.setBgCamerImage(true);
    ai.switchAiMode(ai.Face);

    k10.rgb->brightness(5);
    drawStaticChrome();
}

void loop() {
    if (ai.isRecognized()) {
        recognizedID = ai.getRecognitionID();
        if (recognizedID == "-1" || recognizedID.toInt() < 0) {
            updateRecognitionCard(recognizedID, "Unknown Face", COLOR_RED);
            k10.rgb->write(-1, 0xDC2626); // Red LED alert for unrecognized face
        } else {
            updateRecognitionCard(recognizedID, "Face Verified", COLOR_GREEN);
            k10.rgb->write(-1, 0x16A34A); // Green LED for enrolled face
        }
    }
    delay(40);
}