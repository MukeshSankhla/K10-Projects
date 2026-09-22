#include "unihiker_k10.h"

UNIHIKER_K10 k10;
uint8_t screen_dir = 2; // Portrait orientation (240x320)
Music music;

const char* playlist[] = {
    "track1.wav",
    "sound.wav",
    "track2.wav"
};
const char* artists[] = {
    "Unihiker K10",
    "Voice Memo",
    "Acoustic Beats"
};
const int totalTracks = sizeof(playlist) / sizeof(playlist[0]);
int currentTrack = 0;
bool isPlaying = false;
int trackProgressPercent = 25;
unsigned long lastScrubUpdate = 0;

// Clean Minimalist Light Theme Palette
const uint32_t COLOR_BG        = 0xF8FAFC; // Soft Slate Off-White
const uint32_t COLOR_CARD      = 0xFFFFFF; // Pure White Card Fill
const uint32_t COLOR_TRACK     = 0xF1F5F9; // Soft Track Fill
const uint32_t COLOR_BORDER    = 0xE2E8F0; // Delicate 1px Border
const uint32_t COLOR_BORDER_TRK= 0xCBD5E1; // Track 1px Border
const uint32_t COLOR_TEXT_PRI  = 0x0F172A; // Deep Slate Charcoal
const uint32_t COLOR_TEXT_SEC  = 0x334155; // Slate Secondary
const uint32_t COLOR_TEXT_MUTED= 0x64748B; // Slate Muted Label
const uint32_t COLOR_ROSE      = 0xE11D48; // Clean Rose Brand Accent
const uint32_t COLOR_GREEN     = 0x16A34A; // Playing Green

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
    k10.canvas->canvasRectangle(0, 0, 240, 320, COLOR_BG, COLOR_BG, true);

    // App Header Bar (y: 0 to 40)
    k10.canvas->canvasRectangle(0, 0, 240, 40, COLOR_CARD, COLOR_CARD, true);
    k10.canvas->canvasLine(0, 40, 240, 40, COLOR_BORDER);
    k10.canvas->canvasText("Music Player", 14, 12, COLOR_TEXT_PRI, k10.canvas->eCNAndENFont16, 50, false);
    // Rose brand dot
    k10.canvas->canvasCircle(224, 20, 4, COLOR_ROSE, COLOR_ROSE, true);

    // Album Art Frame (y: 48 to 164)
    k10.canvas->canvasRectangle(10, 48, 220, 116, COLOR_BORDER, COLOR_CARD, true);

    // Decorative vinyl disc rings
    k10.canvas->canvasCircle(120, 106, 42, COLOR_BORDER, COLOR_TRACK, true);
    k10.canvas->canvasCircle(120, 106, 28, COLOR_BORDER_TRK, COLOR_CARD, true);
    k10.canvas->canvasCircle(120, 106, 10, COLOR_ROSE, COLOR_ROSE, true);

    // Track Info Card (y: 172 to 276)
    k10.canvas->canvasRectangle(10, 172, 220, 104, COLOR_BORDER, COLOR_CARD, true);

    // Timeline Track Frame (y: 242 to 252)
    k10.canvas->canvasRectangle(20, 242, 200, 10, COLOR_BORDER_TRK, COLOR_TRACK, true);

    // Footer Bar (y: 284 to 320)
    k10.canvas->canvasRectangle(0, 284, 240, 36, COLOR_CARD, COLOR_CARD, true);
    k10.canvas->canvasLine(0, 284, 240, 284, COLOR_BORDER);
    k10.canvas->canvasText("[A] Play/Pause    [B] Next Track", 14, 294, COLOR_ROSE, k10.canvas->eCNAndENFont16, 50, false);
}

void updateTrackInfo(int trackIdx, bool playing) {
    // Clear track name and artist lines (y: 180..232)
    k10.canvas->canvasRectangle(12, 176, 216, 62, COLOR_CARD, COLOR_CARD, true);

    // Play/Pause state pill
    if (playing) {
        k10.canvas->canvasRectangle(20, 180, 68, 18, COLOR_GREEN, COLOR_GREEN, true);
        k10.canvas->canvasText("PLAYING", 26, 181, COLOR_CARD, k10.canvas->eCNAndENFont16, 50, false);
    } else {
        k10.canvas->canvasRectangle(20, 180, 68, 18, COLOR_BORDER, COLOR_TRACK, true);
        k10.canvas->canvasText("PAUSED", 28, 181, COLOR_TEXT_MUTED, k10.canvas->eCNAndENFont16, 50, false);
    }

    // Track Title
    k10.canvas->canvasText(playlist[trackIdx], 20, 204, COLOR_TEXT_PRI, k10.canvas->eCNAndENFont16, 24, false);

    // Artist
    k10.canvas->canvasText(artists[trackIdx], 20, 224, COLOR_TEXT_MUTED, k10.canvas->eCNAndENFont16, 24, false);

    k10.canvas->updateCanvas();
}

void updateScrubBar(int percent) {
    // Clear and redraw progress fill
    k10.canvas->canvasRectangle(21, 243, 198, 8, COLOR_TRACK, COLOR_TRACK, true);

    int fillW = map(constrain(percent, 0, 100), 0, 100, 0, 198);
    if (fillW > 0) {
        k10.canvas->canvasRectangle(21, 243, fillW, 8, COLOR_ROSE, COLOR_ROSE, true);
    }
    k10.canvas->updateCanvas();
}

void setup() {
    k10.begin();
    k10.initScreen(screen_dir);
    k10.creatCanvas();
    k10.setScreenBackground(COLOR_BG);

    k10.rgb->brightness(5);
    k10.rgb->write(-1, 0xE11D48);

    drawStaticChrome();
    updateTrackInfo(currentTrack, isPlaying);
    updateScrubBar(trackProgressPercent);
}

void loop() {
    if (checkButtonAPressed()) {
        isPlaying = !isPlaying;
        updateTrackInfo(currentTrack, isPlaying);

        if (isPlaying) {
            music.playMusic(ODE, Once);
            k10.rgb->write(-1, 0x16A34A);
        } else {
            k10.rgb->write(-1, 0xE11D48);
        }
    }

    if (checkButtonBPressed()) {
        currentTrack = (currentTrack + 1) % totalTracks;
        trackProgressPercent = 0;
        updateTrackInfo(currentTrack, isPlaying);
        updateScrubBar(trackProgressPercent);
        music.playTone(880, 500);
    }

    if (isPlaying) {
        unsigned long now = millis();
        if (now - lastScrubUpdate >= 300) {
            lastScrubUpdate = now;
            trackProgressPercent = (trackProgressPercent + 1) % 100;
            updateScrubBar(trackProgressPercent);
        }
    }

    delay(20);
}
