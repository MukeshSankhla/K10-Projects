#include "unihiker_k10.h"

UNIHIKER_K10 k10;
uint8_t screen_dir = 2; // Portrait orientation (240x320)
Music music;

#define COLOR_BG        0xF8FAFC
#define COLOR_CARD      0xFFFFFF
#define COLOR_BORDER    0xE2E8F0
#define COLOR_TEXT_PRI  0x0F172A
#define COLOR_TEXT_MUTED 0x64748B
#define COLOR_ROSE      0xE11D48
#define COLOR_EMERALD   0x059669
#define COLOR_VIOLET    0x7C3AED

enum AudioState {
    STATE_STANDBY,
    STATE_RECORDING,
    STATE_PLAYING
};

AudioState currentState = STATE_STANDBY;
unsigned long stateStartTime = 0;
const unsigned long RECORD_DURATION_MS = 3000;
const unsigned long PLAY_DURATION_MS = 3000;

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

void drawStaticChrome() {
    k10.canvas->canvasClear();
    k10.setScreenBackground(COLOR_BG);

    k10.canvas->canvasSetLineWidth(1);

    // Header Card
    k10.canvas->canvasRectangle(16, 16, 208, 44, COLOR_BORDER, COLOR_CARD, true);
    k10.canvas->canvasCircle(36, 38, 5, COLOR_VIOLET, COLOR_VIOLET, true);
    k10.canvas->canvasText("Voice Memo", 52, 30, COLOR_TEXT_PRI,
                           k10.canvas->eCNAndENFont16, 20, false);

    // File Metadata Card
    k10.canvas->canvasRectangle(16, 68, 208, 62, COLOR_BORDER, COLOR_CARD, true);
    k10.canvas->canvasText("FILE DETAILS", 28, 78, COLOR_TEXT_MUTED,
                           k10.canvas->eCNAndENFont16, 20, false);
    k10.canvas->canvasText("S:/sound.wav (16-bit)", 28, 100, COLOR_TEXT_PRI,
                           k10.canvas->eCNAndENFont16, 20, false);

    // Status Card Shell
    k10.canvas->canvasRectangle(16, 138, 208, 116, COLOR_BORDER, COLOR_CARD, true);
    k10.canvas->canvasText("CURRENT STATUS", 28, 148, COLOR_TEXT_MUTED,
                           k10.canvas->eCNAndENFont16, 20, false);

    // Baseline Track for Progress Bar
    k10.canvas->canvasRectangle(28, 224, 184, 8, COLOR_BORDER, 0xF1F5F9, true);

    // Footer Control Bar
    k10.canvas->canvasRectangle(16, 264, 208, 42, COLOR_BORDER, COLOR_CARD, true);
    k10.canvas->canvasText("[A] Rec (3s)", 26, 276, COLOR_ROSE,
                           k10.canvas->eCNAndENFont16, 12, false);
    k10.canvas->canvasLine(120, 270, 120, 300, COLOR_BORDER);
    k10.canvas->canvasText("[B] Play", 136, 276, COLOR_EMERALD,
                           k10.canvas->eCNAndENFont16, 12, false);

    k10.canvas->updateCanvas();
}

void updateStatusPod(const char* statusText, uint32_t statusColor, int progressPercent) {
    // Dynamic partial refresh of the status line (y: 172..214)
    k10.canvas->canvasRectangle(26, 170, 188, 48, COLOR_CARD, COLOR_CARD, true);

    k10.canvas->canvasCircle(38, 186, 4, statusColor, statusColor, true);
    k10.canvas->canvasText(statusText, 50, 178, statusColor,
                           k10.canvas->eCNAndENFont16, 20, false);

    // Progress bar fill (y: 224, h: 8)
    k10.canvas->canvasRectangle(28, 224, 184, 8, COLOR_BORDER, 0xF1F5F9, true);
    if (progressPercent > 0) {
        int barW = map(constrain(progressPercent, 0, 100), 0, 100, 0, 184);
        if (barW > 0) {
            k10.canvas->canvasRectangle(28, 224, barW, 8, statusColor, statusColor, true);
        }
    }

    k10.canvas->updateCanvas();
}

void setup() {
    k10.begin();
    k10.initScreen(screen_dir);
    k10.creatCanvas();
    k10.initSDFile(); // Mount SD card

    k10.rgb->brightness(5);
    k10.rgb->write(-1, 0x000000);

    drawStaticChrome();
    updateStatusPod("Ready to record", COLOR_TEXT_PRI, 0);
}

void loop() {
    if (checkButtonAPressed() && currentState == STATE_STANDBY) {
        currentState = STATE_RECORDING;
        stateStartTime = millis();
        k10.rgb->write(-1, 0xE11D48); // Rose red
        updateStatusPod("Recording (3s)...", COLOR_ROSE, 10);
        music.recordSaveToTFCard("S:/sound.wav", 3);
    }

    if (checkButtonBPressed() && currentState == STATE_STANDBY) {
        currentState = STATE_PLAYING;
        stateStartTime = millis();
        k10.rgb->write(-1, 0x059669); // Emerald green
        updateStatusPod("Playing audio...", COLOR_EMERALD, 10);
        music.playTFCardAudio("S:/sound.wav");
    }

    if (currentState != STATE_STANDBY) {
        unsigned long elapsed = millis() - stateStartTime;
        int pct = map(constrain(elapsed, 0, 3000), 0, 3000, 0, 100);

        if (currentState == STATE_RECORDING) {
            updateStatusPod("Recording (3s)...", COLOR_ROSE, pct);
            if (elapsed >= RECORD_DURATION_MS) {
                currentState = STATE_STANDBY;
                k10.rgb->write(-1, 0x000000);
                updateStatusPod("Saved to sound.wav", COLOR_EMERALD, 0);
            }
        } else if (currentState == STATE_PLAYING) {
            updateStatusPod("Playing audio...", COLOR_EMERALD, pct);
            if (elapsed >= PLAY_DURATION_MS) {
                currentState = STATE_STANDBY;
                k10.rgb->write(-1, 0x000000);
                music.stopPlayAudio();
                updateStatusPod("Playback finished", COLOR_TEXT_PRI, 0);
            }
        }
    }

    delay(30);
}
