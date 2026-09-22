#include "unihiker_k10.h"

UNIHIKER_K10 k10;
uint8_t screen_dir = 2; // Portrait orientation (240x320)
Music music;

// Playlist stored on the MicroSD card
const char* playlist[] = {
    "track1.wav",
    "sound.wav",
    "track2.wav"
};
const char* artists[] = {
    "Unihiker K10",
    "Voice Memo",
    "Retro Beats"
};
const int totalTracks = sizeof(playlist) / sizeof(playlist[0]);
int currentTrack = 0;
bool isPlaying = false;
int trackProgressPercent = 25; // Simulated playback position (0-100%)
unsigned long lastScrubUpdate = 0;

// Non-blocking button state tracker
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

// 1. Render static iPod Classic chrome once (status bar shell, album art frame/vinyl, controls)
void drawScreenChrome() {
    // Top status bar background & divider
    k10.canvas->canvasRectangle(0, 0, 240, 26, 0x1E293B, 0x1E293B, true);
    k10.canvas->canvasLine(0, 26, 240, 26, 0x334155);

    // Iconic "iPod" emblem centered
    k10.canvas->canvasText("iPod", 104, 5, 0xF8FAFC, k10.canvas->eCNAndENFont16, 10, false);

    // Battery gauge on the right
    k10.canvas->canvasRectangle(206, 8, 20, 10, 0x94A3B8, 0x0F172A, true);
    k10.canvas->canvasRectangle(226, 11, 2, 4, 0x94A3B8, 0x94A3B8, true); // Terminal pip
    k10.canvas->canvasRectangle(208, 10, 14, 6, 0x10B981, 0x10B981, true); // Full green charge

    // Outer artwork plinth frame with drop-shadow effect
    k10.canvas->canvasRectangle(64, 38, 112, 112, 0x94A3B8, 0x0F172A, true);
    k10.canvas->canvasRectangle(66, 40, 108, 108, 0x1E293B, 0x111827, true);

    // Stylized vinyl record disc
    k10.canvas->canvasCircle(120, 94, 46, 0x334155, 0x0A0A0A, true);
    k10.canvas->canvasCircle(120, 94, 34, 0x1E293B, 0x0A0A0A, false);
    k10.canvas->canvasCircle(120, 94, 24, 0x334155, 0x0A0A0A, false);

    // Static Album Name
    k10.canvas->canvasText("K10 Classics Album", 48, 216, 0x64748B, k10.canvas->eCNAndENFont16, 24, false);

    // Bottom controls guide bar
    k10.canvas->canvasLine(12, 272, 228, 272, 0xCBD5E1);
    k10.canvas->canvasRectangle(12, 278, 216, 34, 0x94A3B8, 0xF1F5F9, true);
    k10.canvas->canvasText("[A] Play/Pause", 20, 287, 0x0284C7, k10.canvas->eCNAndENFont16, 15, false);
    k10.canvas->canvasText("[B] Next >>", 136, 287, 0x0F172A, k10.canvas->eCNAndENFont16, 12, false);
}

// 2. Dynamic Partial Refresh: update ONLY play/pause glyph and vinyl core label
void updatePlayPauseStatus(bool playing) {
    // Clear status bar glyph box
    k10.canvas->canvasRectangle(12, 6, 16, 14, 0x1E293B, 0x1E293B, true);

    if (playing) {
        // Play triangle glyph
        k10.canvas->canvasLine(14, 8, 14, 18, 0x10B981);
        k10.canvas->canvasLine(14, 8, 22, 13, 0x10B981);
        k10.canvas->canvasLine(14, 18, 22, 13, 0x10B981);
        k10.canvas->canvasLine(15, 9, 21, 13, 0x10B981);
    } else {
        // Pause double-bar glyph
        k10.canvas->canvasRectangle(14, 8, 3, 10, 0xF59E0B, 0xF59E0B, true);
        k10.canvas->canvasRectangle(19, 8, 3, 10, 0xF59E0B, 0xF59E0B, true);
    }

    // Center vinyl label (vibrant album core)
    uint32_t labelColor = playing ? 0x0284C7 : 0x64748B;
    k10.canvas->canvasCircle(120, 94, 15, labelColor, labelColor, true);
    k10.canvas->canvasCircle(120, 94, 4, 0xE2E8F0, 0xE2E8F0, true); // Spindle hole
}

// 3. Dynamic Partial Refresh: update ONLY track title, artist, and track index
void updateTrackMetadata(int trackIndex) {
    // Clear track metadata text region (x=10, y=156, w=220, h=58)
    k10.canvas->canvasRectangle(10, 156, 220, 58, 0xE2E8F0, 0xE2E8F0, true);

    // Track index badge (e.g. "Track 1 of 3")
    String indexStr = "Track " + String(trackIndex + 1) + " of " + String(totalTracks);
    k10.canvas->canvasText(indexStr, 82, 158, 0x64748B, k10.canvas->eCNAndENFont16, 15, false);

    // Track filename
    String titleStr = String(playlist[trackIndex]);
    int titleX = max(10, (240 - (int)(titleStr.length() * 8)) / 2);
    k10.canvas->canvasText(titleStr, titleX, 178, 0x0F172A, k10.canvas->eCNAndENFont16, 24, false);

    // Artist name
    String artistStr = String(artists[trackIndex]);
    int artistX = max(10, (240 - (int)(artistStr.length() * 8)) / 2);
    k10.canvas->canvasText(artistStr, artistX, 198, 0x0284C7, k10.canvas->eCNAndENFont16, 24, false);
}

// 4. Dynamic Partial Refresh: update ONLY timeline scrubber bar and timestamps
void updateScrubTimeline(int progressPercent) {
    // Clear scrubber rail and timestamps region (x=10, y=235, w=220, h=34)
    k10.canvas->canvasRectangle(10, 235, 220, 34, 0xE2E8F0, 0xE2E8F0, true);

    // Scrub timeline rail (x=24, y=238, w=192, h=6)
    k10.canvas->canvasRectangle(24, 238, 192, 6, 0x94A3B8, 0xCBD5E1, true);

    // Elapsed scrub progress fill
    int fillW = map(progressPercent, 0, 100, 0, 192);
    if (fillW > 0) {
        k10.canvas->canvasRectangle(24, 238, fillW, 6, 0x0284C7, 0x0284C7, true);
    }
    // Scrub diamond/pip thumb
    int thumbX = 24 + fillW;
    k10.canvas->canvasCircle(thumbX, 241, 5, 0x0F172A, 0xFFFFFF, true);

    // Timestamps: Elapsed (Left) & Remaining (Right)
    int elapsedSec = (progressPercent * 180) / 100;
    int remainSec = 180 - elapsedSec;
    char timeBuffer[16];

    sprintf(timeBuffer, "%d:%02d", elapsedSec / 60, elapsedSec % 60);
    k10.canvas->canvasText(timeBuffer, 24, 248, 0x475569, k10.canvas->eCNAndENFont16, 8, false);

    sprintf(timeBuffer, "-%d:%02d", remainSec / 60, remainSec % 60);
    k10.canvas->canvasText(timeBuffer, 178, 248, 0x475569, k10.canvas->eCNAndENFont16, 8, false);
}

void setup() {
    k10.begin();
    k10.initScreen(screen_dir);
    k10.creatCanvas();
    k10.initSDFile(); // Mount SD card
    // iPod Classic Ice Silver LCD background
    k10.setScreenBackground(0xE2E8F0);

    k10.rgb->brightness(5);
    k10.rgb->write(-1, 0x000000);

    // Initial paint: static chrome + initial track details + scrubber
    drawScreenChrome();
    updatePlayPauseStatus(isPlaying);
    updateTrackMetadata(currentTrack);
    updateScrubTimeline(trackProgressPercent);
    k10.canvas->updateCanvas();
}

void loop() {
    // Non-blocking Button A: Toggle Play / Pause
    if (checkButtonAPressed()) {
        isPlaying = !isPlaying;

        if (isPlaying) {
            k10.rgb->write(-1, 0x0284C7); // Classic iPod Blue glow
            String path = "S:/" + String(playlist[currentTrack]);
            music.playTFCardAudio(path.c_str());
        } else {
            k10.rgb->write(-1, 0x000000);
            music.stopPlayAudio();
        }
        // Dynamic Partial Refresh: update ONLY play/pause indicator and vinyl label
        updatePlayPauseStatus(isPlaying);
        k10.canvas->updateCanvas();
    }

    // Non-blocking Button B: Next Track
    if (checkButtonBPressed()) {
        currentTrack = (currentTrack + 1) % totalTracks;
        trackProgressPercent = 0; // Reset scrub bar for new track

        if (isPlaying) {
            music.stopPlayAudio();
            String path = "S:/" + String(playlist[currentTrack]);
            music.playTFCardAudio(path.c_str());
        }
        // Dynamic Partial Refresh: update track metadata, scrub bar, and status
        updateTrackMetadata(currentTrack);
        updateScrubTimeline(trackProgressPercent);
        updatePlayPauseStatus(isPlaying);
        k10.canvas->updateCanvas();
    }

    // Advance simulated timeline scrubber when playing (every 1s)
    if (isPlaying && (millis() - lastScrubUpdate > 1000)) {
        lastScrubUpdate = millis();
        trackProgressPercent = (trackProgressPercent + 1) % 100;
        // Dynamic Partial Refresh: update ONLY scrub bar and timestamps
        updateScrubTimeline(trackProgressPercent);
        k10.canvas->updateCanvas();
    }

    delay(20); // Responsive loop tick
}
