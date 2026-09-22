#include "unihiker_k10.h"

UNIHIKER_K10 k10;
uint8_t screen_dir = 2; // Portrait orientation (240x320)
Music music;

enum AudioState {
    STATE_STANDBY,
    STATE_RECORDING,
    STATE_PLAYING
};

AudioState currentState = STATE_STANDBY;
unsigned long stateStartTime = 0;
const unsigned long RECORD_DURATION_MS = 3000;
const unsigned long PLAY_DURATION_MS = 3000;

// Non-blocking button edge-detection trackers
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

// Render Audio Studio UI
void drawUI(const char* statusText, uint32_t statusColor) {
    k10.canvas->canvasClear();
    // Studio Obsidian-Purple background
    k10.setScreenBackground(0x140F19);

    // 1. Header Title Banner
    k10.canvas->canvasRectangle(0, 0, 240, 42, 0x221829, 0x221829, true);
    k10.canvas->canvasLine(0, 42, 240, 42, 0xE0A96D);
    k10.canvas->canvasText("VOICE STUDIO", 44, 10, 0xE0A96D,
                           k10.canvas->eCNAndENFont24, 15, false);

    // 2. Storage File Metadata Pod
    k10.canvas->canvasRectangle(14, 54, 212, 68, 0x382342, 0x1B1322, true);
    k10.canvas->canvasText("Target: S:/sound.wav", 26, 66, 0xD4A373,
                           k10.canvas->eCNAndENFont16, 24, false);
    k10.canvas->canvasText("Format: 16-bit PCM WAV", 26, 92, 0x94A3B8,
                           k10.canvas->eCNAndENFont16, 24, false);

    // 3. Dynamic Console Status Pod
    k10.canvas->canvasRectangle(14, 134, 212, 64, statusColor, 0x1B1322, true);
    int statusX = 120 - (int)(strlen(statusText) * 4);
    k10.canvas->canvasText(statusText, statusX, 156, statusColor,
                           k10.canvas->eCNAndENFont16, 22, false);

    // 4. Progress / Level Bar
    k10.canvas->canvasRectangle(14, 210, 212, 12, 0x382342, 0x0E0A12, true);
    if (currentState != STATE_STANDBY) {
        unsigned long elapsed = millis() - stateStartTime;
        int barW = map(constrain(elapsed, 0, 3000), 0, 3000, 0, 208);
        if (barW > 0) {
            k10.canvas->canvasRectangle(16, 212, barW, 8, statusColor, statusColor, true);
        }
    }

    // 5. Centered Two-Column Control Footer (Zero-overflow)
    k10.canvas->canvasLine(15, 268, 225, 268, 0x382342);
    k10.canvas->canvasRectangle(12, 274, 216, 36, 0x382342, 0x221829, true);
    k10.canvas->canvasText("[A] Rec (3s)", 22, 284, 0xFF2A55,
                           k10.canvas->eCNAndENFont16, 12, false);
    k10.canvas->canvasText("[B] Playback", 132, 284, 0x22C55E,
                           k10.canvas->eCNAndENFont16, 12, false);

    k10.canvas->updateCanvas();
}

void setup() {
    k10.begin();
    k10.initScreen(screen_dir);
    k10.creatCanvas();
    k10.initSDFile(); // Mount SD card

    k10.rgb->brightness(5);
    k10.rgb->write(-1, 0x000000);

    drawUI("Status: Console Ready", 0x22C55E);
}

void loop() {
    // Non-blocking Button A: Trigger 3-Second Recording
    if (checkButtonAPressed() && currentState == STATE_STANDBY) {
        currentState = STATE_RECORDING;
        stateStartTime = millis();
        k10.rgb->write(-1, 0xFF2A55); // Crimson recording light
        drawUI("RECORDING (3s)...", 0xFF2A55);

        music.recordSaveToTFCard("S:/sound.wav", 3);
    }

    // Non-blocking Button B: Trigger Audio Playback
    if (checkButtonBPressed() && currentState == STATE_STANDBY) {
        currentState = STATE_PLAYING;
        stateStartTime = millis();
        k10.rgb->write(-1, 0x22C55E); // Green playback light
        drawUI("PLAYING AUDIO...", 0xE0A96D);

        music.playTFCardAudio("S:/sound.wav");
    }

    // Non-blocking state transition timeout
    if (currentState == STATE_RECORDING && (millis() - stateStartTime >= RECORD_DURATION_MS)) {
        currentState = STATE_STANDBY;
        k10.rgb->write(-1, 0x000000);
        drawUI("Audio Saved: sound.wav", 0x22C55E);
    } else if (currentState == STATE_PLAYING && (millis() - stateStartTime >= PLAY_DURATION_MS)) {
        currentState = STATE_STANDBY;
        k10.rgb->write(-1, 0x000000);
        music.stopPlayAudio();
        drawUI("Playback Completed", 0x22C55E);
    }

    delay(30); // Responsive loop tick
}
