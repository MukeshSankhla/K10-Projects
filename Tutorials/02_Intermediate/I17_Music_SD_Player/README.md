## What this tutorial is??

This tutorial demonstrates how to build an interactive **MicroSD Music Jukebox Player** on the UNIHIKER K10 complete with digital media player UI, album art vinyl visualization, and hardware playback control.

Features:
- **Button [A] (Play / Pause)**: Toggles audio playback of the currently selected track through the onboard speaker, illuminating the RGB LED in green during playback.
- **Button [B] (Next Track)**: Skips to the next WAV track in the playlist array (`Track X of Y`) and immediately switches playback.
- **Vinyl Record Visualization**: Renders concentric dynamic vinyl grooves and an animated color-shifting center cap.

---

## How it works

1. **Playlist Storage Matrix**:
   - Audio tracks are organized as an array of SD card path strings:
     ```cpp
     const char* playlist[] = { "S:/track1.wav", "S:/sound.wav", "S:/track2.wav" };
     ```
2. **I2S Audio Pipeline**:
   - `music.playTFCardAudio(playlist[currentTrack])` decodes the WAV stream from the MicroSD card and feeds PCM samples to the onboard amplifier.
   - `music.stopPlayTone()` halts active playback.
3. **Player State Tracking**:
   - `isPlaying` toggles the visual badge between `[PLAYING]` (Green) and `[PAUSED]` (Red).

---

## API and Functions detailes

| API / Method | Arguments | Return Type | Description |
| :--- | :--- | :--- | :--- |
| `music.playTFCardAudio(path)` | `const char* path` | `void` | Streams and plays WAV audio file from SD card. |
| `music.stopPlayTone()` | None | `void` | Halts active audio output. |
| `k10.buttonA->isPressed()` | None | `bool` | Play / Pause button. |
| `k10.buttonB->isPressed()` | None | `bool` | Next track button. |

---

## Full Code

```cpp
#include "unihiker_k10.h"

UNIHIKER_K10 k10;
uint8_t screen_dir = 2; // Portrait orientation (240x320)
Music music;

// Playlist stored on the MicroSD card
const char* playlist[] = {
    "S:/track1.wav",
    "S:/sound.wav",
    "S:/track2.wav"
};
const int totalTracks = sizeof(playlist) / sizeof(playlist[0]);
int currentTrack = 0;
bool isPlaying = false;

void renderPlayerUI() {
    k10.canvas->canvasClear();
    k10.setScreenBackground(0x0F172A);

    // 1. Header
    k10.canvas->canvasText("SD MUSIC PLAYER", 30, 14, 0xFEE715,
                           k10.canvas->eCNAndENFont24, 20, false);
    k10.canvas->canvasLine(15, 44, 225, 44, 0x334155);

    // 2. Vinyl / Album Visualizer Card
    k10.canvas->canvasRectangle(20, 56, 200, 130, 0x334155, 0x1E293B, true);
    // Stylized record grooving
    k10.canvas->canvasCircle(120, 121, 46, 0x334155, 0x111827, true);
    k10.canvas->canvasCircle(120, 121, 30, 0x1E293B, 0x0F172A, true);
    uint32_t centerCapColor = isPlaying ? 0x00FF88 : 0x00E5FF;
    k10.canvas->canvasCircle(120, 121, 14, centerCapColor, centerCapColor, true);

    // 3. Track Information
    String trackNum = "Track " + String(currentTrack + 1) + " of " + String(totalTracks);
    k10.canvas->canvasText(trackNum, 75, 198, 0x94A3B8,
                           k10.canvas->eCNAndENFont16, 20, false);

    k10.canvas->canvasText(playlist[currentTrack], 30, 220, 0xFEE715,
                           k10.canvas->eCNAndENFont16, 22, false);

    // Status Badge
    String stateStr = isPlaying ? "[PLAYING]" : "[PAUSED]";
    uint32_t stateCol = isPlaying ? 0x00FF88 : 0xFF5555;
    k10.canvas->canvasText(stateStr, 80, 245, stateCol,
                           k10.canvas->eCNAndENFont16, 15, false);

    // 4. Controls Guide Bar
    k10.canvas->canvasLine(15, 272, 225, 272, 0x334155);
    k10.canvas->canvasText("[A] Play/Stop", 20, 282, 0x00E5FF,
                           k10.canvas->eCNAndENFont16, 20, false);
    k10.canvas->canvasText("[B] Next Track", 130, 282, 0x38BDF8,
                           k10.canvas->eCNAndENFont16, 20, false);

    k10.canvas->updateCanvas();
}

void setup() {
    k10.begin();
    k10.initScreen(screen_dir);
    k10.creatCanvas();
    k10.initSDFile(); // Mount SD card

    k10.rgb->brightness(5);
    k10.rgb->write(-1, 0x000000);

    renderPlayerUI();
}

void loop() {
    // Button A: Toggle Play / Stop
    if (k10.buttonA->isPressed()) {
        isPlaying = !isPlaying;

        if (isPlaying) {
            k10.rgb->write(-1, 0x00FF88);
            renderPlayerUI();
            music.playTFCardAudio(playlist[currentTrack]);
        } else {
            k10.rgb->write(-1, 0x000000);
            music.stopPlayTone();
            renderPlayerUI();
        }

        while (k10.buttonA->isPressed()) {
            delay(50);
        }
    }
    // Button B: Next Track
    else if (k10.buttonB->isPressed()) {
        currentTrack = (currentTrack + 1) % totalTracks;
        if (isPlaying) {
            music.stopPlayTone();
            music.playTFCardAudio(playlist[currentTrack]);
        }
        renderPlayerUI();

        while (k10.buttonB->isPressed()) {
            delay(50);
        }
    }

    delay(25);
}
```
