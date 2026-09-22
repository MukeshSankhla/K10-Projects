#include "asr.h"
#include "unihiker_k10.h"

UNIHIKER_K10 k10;
ASR asr;
uint8_t screen_dir = 2; // Portrait (240x320)

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
int eqPhase = 0;

void drawStaticChrome() {
    k10.canvas->canvasClear();
    k10.canvas->canvasSetLineWidth(1);
    k10.setScreenBackground(COLOR_BG);

    // App Header Bar (y: 0 to 40)
    k10.canvas->canvasRectangle(0, 0, 240, 40, COLOR_CARD, COLOR_CARD, true);
    k10.canvas->canvasLine(0, 40, 240, 40, COLOR_BORDER);
    k10.canvas->canvasText("Voice Assistant", 14, 12, COLOR_TEXT_PRI, k10.canvas->eCNAndENFont16, 50, false);
    // Violet brand dot
    k10.canvas->canvasCircle(224, 20, 4, COLOR_VIOLET, COLOR_VIOLET, true);

    // Wake Phrase Card (y: 46 to 110)
    k10.canvas->canvasRectangle(10, 46, 220, 64, COLOR_BORDER, COLOR_CARD, true);
    k10.canvas->canvasText("WAKE PHRASE", 20, 54, COLOR_TEXT_MUTED, k10.canvas->eCNAndENFont16, 50, false);
    k10.canvas->canvasText("\"Hi Telly\" / \"Jarvis\"", 20, 74, COLOR_VIOLET, k10.canvas->eCNAndENFont16, 50, false);

    // Audio Visualizer Card (y: 116 to 192)
    k10.canvas->canvasRectangle(10, 116, 220, 76, COLOR_BORDER, COLOR_CARD, true);
    k10.canvas->canvasText("AUDIO SPECTRUM", 20, 124, COLOR_TEXT_MUTED, k10.canvas->eCNAndENFont16, 50, false);

    // Voice Command Card (y: 198 to 280)
    k10.canvas->canvasRectangle(10, 198, 220, 82, COLOR_BORDER, COLOR_CARD, true);
    k10.canvas->canvasText("VOICE COMMANDS", 20, 206, COLOR_TEXT_MUTED, k10.canvas->eCNAndENFont16, 50, false);
    k10.canvas->canvasText("Say: \"Switch on\" / \"Switch off\"", 20, 224, COLOR_TEXT_SEC, k10.canvas->eCNAndENFont16, 50, false);

    // Footer Info Bar (y: 286 to 320)
    k10.canvas->canvasRectangle(0, 286, 240, 34, COLOR_CARD, COLOR_CARD, true);
    k10.canvas->canvasLine(0, 286, 240, 286, COLOR_BORDER);
    k10.canvas->canvasText("Offline Neural ASR Engine", 14, 294, COLOR_TEXT_MUTED, k10.canvas->eCNAndENFont16, 50, false);

    k10.canvas->updateCanvas();
}

// Partial refresh for wake state pill
void updateWakeState(bool awake) {
    k10.canvas->canvasRectangle(114, 68, 110, 28, COLOR_CARD, COLOR_CARD, true);

    if (awake) {
        k10.canvas->canvasRectangle(120, 70, 96, 22, COLOR_GREEN, COLOR_GREEN, true);
        k10.canvas->canvasText("LISTENING...", 126, 73, COLOR_WHITE, k10.canvas->eCNAndENFont16, 50, false);
    } else {
        k10.canvas->canvasRectangle(130, 70, 84, 22, COLOR_BORDER, COLOR_CONSOLE, true);
        k10.canvas->canvasText("STANDBY", 140, 73, COLOR_TEXT_MUTED, k10.canvas->eCNAndENFont16, 50, false);
    }

    k10.canvas->updateCanvas();
}

// Partial refresh for equalizer spectrum bars (always lively and animated)
void updateEqualizer(bool active) {
    k10.canvas->canvasRectangle(18, 142, 204, 44, COLOR_CARD, COLOR_CARD, true);

    eqPhase++;
    for (int i = 0; i < 9; i++) {
        int barH;
        if (active) {
            // High dynamic speech visualizer when awake
            barH = 10 + (int)(sin((eqPhase * 0.4) + (i * 0.7)) * 12.0) + random(0, 10);
            if (barH < 6) barH = 6;
            if (barH > 36) barH = 36;
        } else {
            // Ambient resting audio wave to show microphone is actively listening
            barH = 6 + (int)(sin((eqPhase * 0.2) + (i * 0.6)) * 4.0);
            if (barH < 3) barH = 3;
            if (barH > 14) barH = 14;
        }

        int barX = 24 + (i * 22);
        int barY = 182 - barH;
        uint32_t barColor = active ? COLOR_VIOLET : 0xA5B4FC; // Soft indigo in standby, vibrant violet when active
        k10.canvas->canvasRectangle(barX, barY, 12, barH, barColor, barColor, true);
    }
    k10.canvas->updateCanvas();
}

// Partial refresh for light command state
void updateLightState(bool on) {
    k10.canvas->canvasRectangle(18, 246, 204, 28, COLOR_CARD, COLOR_CARD, true);

    if (on) {
        k10.canvas->canvasRectangle(20, 248, 180, 22, COLOR_GREEN, COLOR_GREEN, true);
        k10.canvas->canvasText("LIGHT: ON (WHITE)", 38, 251, COLOR_WHITE, k10.canvas->eCNAndENFont16, 50, false);
    } else {
        k10.canvas->canvasRectangle(20, 248, 180, 22, COLOR_BORDER, COLOR_CONSOLE, true);
        k10.canvas->canvasText("LIGHT: OFF", 66, 251, COLOR_TEXT_MUTED, k10.canvas->eCNAndENFont16, 50, false);
    }

    k10.canvas->updateCanvas();
}

void setup() {
    Serial.begin(115200);

    // 1. Initialize hardware
    k10.begin();

    // 2. Initialize Neural Speech Recognition BEFORE screen canvas to ensure clean DMA buffer allocation
    asr.asrInit(CONTINUOUS, EN_MODE, 6000);
    while (asr._asrState == 0) {
        delay(100);
    }

    // 3. Initialize screen and canvas
    k10.initScreen(screen_dir);
    k10.creatCanvas();

    // 4. Configure trained English commands
    asr.addASRCommand(0 + 1, "Switch on");
    asr.addASRCommand(1 + 1, "Switch off");

    k10.rgb->brightness(5);
    k10.rgb->write(-1, 0x000000);

    drawStaticChrome();
    updateWakeState(false);
    updateEqualizer(false);
    updateLightState(false);
}

void loop() {
    bool awake = asr.isWakeUp();
    if (awake != lastWakeState) {
        lastWakeState = awake;
        updateWakeState(awake);

        if (awake) {
            k10.rgb->write(-1, 0x00FFFF); // Cyan glow when awake and listening
        } else {
            if (!lightIsOn) {
                k10.rgb->write(-1, 0x000000);
            }
        }
    }

    // Animated spectrum visualizer tick every 80ms
    unsigned long now = millis();
    if (now - lastEqTick >= 80) {
        lastEqTick = now;
        updateEqualizer(awake);
    }

    // Command recognition evaluation
    if (asr.isDetectCmdID(0 + 1)) {
        lightIsOn = true;
        k10.rgb->brightness(9);
        k10.rgb->write(-1, 0xFFFFFF); // Bright white light
        updateLightState(true);
    } else if (asr.isDetectCmdID(1 + 1)) {
        lightIsOn = false;
        k10.rgb->brightness(0);
        k10.rgb->write(-1, 0x000000); // Light off
        updateLightState(false);
    }

    delay(20);
}
