#include "unihiker_k10.h"
#include "asr.h"

UNIHIKER_K10 k10;
uint8_t screen_dir = 2; // Portrait (240x320)
ASR asr;

// Clean Minimalist Light Theme Palette
const uint32_t COLOR_BG        = 0xF8FAFC; // Soft Slate Off-White
const uint32_t COLOR_CARD      = 0xFFFFFF; // Pure White Card Fill
const uint32_t COLOR_CONSOLE   = 0xF1F5F9; // Soft Paper Fill
const uint32_t COLOR_BORDER    = 0xE2E8F0; // Delicate 1px Border
const uint32_t COLOR_TEXT_PRI  = 0x0F172A; // Deep Slate Charcoal
const uint32_t COLOR_TEXT_SEC  = 0x334155; // Slate Secondary
const uint32_t COLOR_TEXT_MUTED= 0x64748B; // Slate Muted Label
const uint32_t COLOR_VIOLET    = 0x7C3AED; // Clean Violet Brand Accent
const uint32_t COLOR_GREEN     = 0x16A34A; // Success Green
const uint32_t COLOR_RED       = 0xDC2626; // Alert Red
const uint32_t COLOR_WHITE     = 0xFFFFFF;

bool lastWakeState = false;
bool lightIsOn = false;
unsigned long lastEqTick = 0;

void drawStaticChrome() {
    k10.canvas->canvasClear();
    k10.canvas->canvasRectangle(0, 0, 240, 320, COLOR_BG, COLOR_BG, true);

    // App Header Bar (y: 0 to 40)
    k10.canvas->canvasRectangle(0, 0, 240, 40, COLOR_CARD, COLOR_CARD, true);
    k10.canvas->canvasLine(0, 40, 240, 40, COLOR_BORDER);
    k10.canvas->canvasText("Voice Assistant", 14, 12, COLOR_TEXT_PRI, k10.canvas->eCNAndENFont16, 50, false);
    // Violet brand dot
    k10.canvas->canvasCircle(224, 20, 4, COLOR_VIOLET, COLOR_VIOLET, true);

    // Wake Word Card (y: 48 to 112)
    k10.canvas->canvasRectangle(10, 48, 220, 64, COLOR_BORDER, COLOR_CARD, true);
    k10.canvas->canvasText("WAKE PHRASE", 20, 56, COLOR_TEXT_MUTED, k10.canvas->eCNAndENFont16, 50, false);
    k10.canvas->canvasText("\"Hi, Telly\"", 20, 74, COLOR_VIOLET, k10.canvas->eCNAndENFont16, 50, false);

    // Audio Visualizer Card (y: 120 to 192)
    k10.canvas->canvasRectangle(10, 120, 220, 72, COLOR_BORDER, COLOR_CARD, true);
    k10.canvas->canvasText("AUDIO SPECTRUM", 20, 128, COLOR_TEXT_MUTED, k10.canvas->eCNAndENFont16, 50, false);

    // Voice Command Card (y: 200 to 280)
    k10.canvas->canvasRectangle(10, 200, 220, 80, COLOR_BORDER, COLOR_CARD, true);
    k10.canvas->canvasText("VOICE COMMANDS", 20, 208, COLOR_TEXT_MUTED, k10.canvas->eCNAndENFont16, 50, false);
    k10.canvas->canvasText("Say: \"Light on\" / \"Light off\"", 20, 226, COLOR_TEXT_SEC, k10.canvas->eCNAndENFont16, 50, false);

    // Footer Info Bar (y: 288 to 320)
    k10.canvas->canvasRectangle(0, 288, 240, 32, COLOR_CARD, COLOR_CARD, true);
    k10.canvas->canvasLine(0, 288, 240, 288, COLOR_BORDER);
    k10.canvas->canvasText("Offline Neural ASR Engine", 14, 296, COLOR_TEXT_MUTED, k10.canvas->eCNAndENFont16, 50, false);

    k10.canvas->updateCanvas();
}

void drawBootStatus(const char* msg) {
    k10.canvas->canvasRectangle(110, 72, 114, 24, COLOR_CARD, COLOR_CARD, true);
    k10.canvas->canvasText(msg, 114, 76, COLOR_VIOLET, k10.canvas->eCNAndENFont16, 50, false);
    k10.canvas->updateCanvas();
}

void drawModelMissingAlert() {
    k10.canvas->canvasRectangle(10, 48, 220, 232, 0xFEE2E2, COLOR_CARD, true);
    k10.canvas->canvasRectangle(20, 58, 200, 24, COLOR_RED, COLOR_RED, true);
    k10.canvas->canvasText("MODEL INIT TIMEOUT", 36, 62, COLOR_WHITE, k10.canvas->eCNAndENFont16, 50, false);

    k10.canvas->canvasText("Speech Model partition", 20, 96, COLOR_RED, k10.canvas->eCNAndENFont16, 50, false);
    k10.canvas->canvasText("at 0x510000 missing!", 20, 116, COLOR_RED, k10.canvas->eCNAndENFont16, 50, false);

    k10.canvas->canvasText("To fix:", 20, 146, COLOR_TEXT_PRI, k10.canvas->eCNAndENFont16, 50, false);
    k10.canvas->canvasText("1. Flash A13 merged bin", 20, 168, COLOR_VIOLET, k10.canvas->eCNAndENFont16, 50, false);
    k10.canvas->canvasText("2. Or in Arduino IDE:", 20, 192, COLOR_TEXT_SEC, k10.canvas->eCNAndENFont16, 50, false);
    k10.canvas->canvasText("   Tools -> Model -> EN", 20, 212, COLOR_TEXT_SEC, k10.canvas->eCNAndENFont16, 50, false);

    k10.canvas->updateCanvas();
}

// Partial refresh for wake state pill
void updateWakeState(bool awake) {
    k10.canvas->canvasRectangle(110, 70, 114, 26, COLOR_CARD, COLOR_CARD, true);

    if (awake) {
        k10.canvas->canvasRectangle(118, 72, 94, 20, COLOR_GREEN, COLOR_GREEN, true);
        k10.canvas->canvasText("LISTENING...", 124, 74, COLOR_WHITE, k10.canvas->eCNAndENFont16, 50, false);
    } else {
        k10.canvas->canvasRectangle(128, 72, 84, 20, COLOR_BORDER, COLOR_CONSOLE, true);
        k10.canvas->canvasText("STANDBY", 136, 74, COLOR_TEXT_MUTED, k10.canvas->eCNAndENFont16, 50, false);
    }

    k10.canvas->updateCanvas();
}

// Partial refresh for equalizer spectrum bars
void updateEqualizer(bool active) {
    k10.canvas->canvasRectangle(20, 148, 200, 36, COLOR_CARD, COLOR_CARD, true);

    for (int i = 0; i < 9; i++) {
        int barH = active ? random(6, 28) : 4;
        int barX = 24 + (i * 22);
        int barY = 180 - barH;
        uint32_t barColor = active ? COLOR_VIOLET : COLOR_BORDER;
        k10.canvas->canvasRectangle(barX, barY, 12, barH, barColor, barColor, true);
    }
    k10.canvas->updateCanvas();
}

// Partial refresh for light command state
void updateLightState(bool on) {
    k10.canvas->canvasRectangle(20, 248, 200, 26, COLOR_CARD, COLOR_CARD, true);

    if (on) {
        k10.canvas->canvasRectangle(20, 250, 180, 22, COLOR_RED, COLOR_RED, true);
        k10.canvas->canvasText("LIGHT STATE: ON (RED)", 28, 253, COLOR_WHITE, k10.canvas->eCNAndENFont16, 50, false);
    } else {
        k10.canvas->canvasRectangle(20, 250, 180, 22, COLOR_BORDER, COLOR_CONSOLE, true);
        k10.canvas->canvasText("LIGHT STATE: OFF", 48, 253, COLOR_TEXT_MUTED, k10.canvas->eCNAndENFont16, 50, false);
    }

    k10.canvas->updateCanvas();
}

void setup() {
    Serial.begin(115200);
    k10.begin();

    k10.initScreen(screen_dir);
    k10.creatCanvas();
    drawStaticChrome();
    drawBootStatus("Loading model...");

    asr.asrInit(CONTINUOUS, EN_MODE, 6000);

    unsigned long startInit = millis();
    bool initOk = true;
    while (asr._asrState == 0) {
        delay(100);
        if (millis() - startInit > 7000) {
            initOk = false;
            break;
        }
    }

    if (!initOk) {
        drawModelMissingAlert();
        while (1) {
            delay(1000);
        }
    }

    asr.addASRCommand(1, "Switch on");
    asr.addASRCommand(1, "light on");
    asr.addASRCommand(1, "lights on");

    asr.addASRCommand(2, "Switch off");
    asr.addASRCommand(2, "light off");
    asr.addASRCommand(2, "lights off");

    updateWakeState(false);
    updateEqualizer(false);
    updateLightState(false);
}

void loop() {
    bool awake = asr.isWakeUp();
    if (awake != lastWakeState) {
        lastWakeState = awake;
        updateWakeState(awake);
    }

    unsigned long now = millis();
    if (now - lastEqTick >= 100) {
        lastEqTick = now;
        updateEqualizer(awake);
    }

    if (asr.isDetectCmdID(1)) {
        lightIsOn = true;
        k10.rgb->brightness(9);
        k10.rgb->write(-1, 0xFF0000);
        updateLightState(true);
    } else if (asr.isDetectCmdID(2)) {
        lightIsOn = false;
        k10.rgb->brightness(0);
        k10.rgb->write(-1, 0x000000);
        updateLightState(false);
    }

    delay(20);
}
