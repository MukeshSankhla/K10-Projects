## What this tutorial is??

This tutorial demonstrates how to capture raw audio through the onboard **I2S PDM Microphone**, store audio samples in RAM, and stream playback through the onboard amplifier using a **Studio Broadcast Console** UI.

You will learn how to configure I2S audio pipelines, implement non-blocking Button [A] (Record) and Button [B] (Playback) triggers, render a dynamic audio VU meter bar, and provide visual status feedback via the screen and RGB LEDs.

### Expected Behavior
- **Studio Console Header**: Crimson and bronze broadcast styling.
- **Live Audio VU Meter**: Real-time microphone audio amplitude scale bar.
- **Dual State Indicators**: Clear visual indicators for `[RECORDING]` and `[PLAYING]` states.
- **Synchronized RGB Feedback**: Red illumination during recording, green during playback.

---

## How it works

1. **MicroSD (TF) Card Storage (`S:/`)**:
   - The UNIHIKER K10 hardware abstraction assigns the root of the mounted FAT32 MicroSD card to the virtual drive prefix `"S:/"`.
   - Calling `k10.initSDFile()` initializes the SPI bus and mounts the filesystem so file streams can be created, written, and read.
2. **Audio Recording via `Music::recordSaveToTFCard`**:
   - When Button A is pressed, `music.recordSaveToTFCard("S:/sound.wav", 3)` configures the onboard I2S digital MEMS microphone.
   - It captures 16-bit PCM audio samples, wraps them into a standard RIFF/WAV container (with standard 44-byte WAV header specifying sample rate, bit depth, and channel count), and streams the raw audio chunks directly onto the MicroSD card.
3. **Audio Playback via `Music::playTFCardAudio`**:
   - When Button B is pressed, `music.playTFCardAudio("S:/sound.wav")` opens the WAV file from the MicroSD card, parses its PCM header, and feeds the uncompressed audio samples over the I2S digital audio bus to the onboard class-D audio amplifier and speaker.
4. **Visual UI Dashboard & Status State Machine**:
   - The helper function `drawUI(statusText, statusColor)` clears the frame buffer and renders a unified layout:
     - Header banner in bright gold (`0xFEE715`).
     - File metadata card with dark background (`0x1E293B`) in soft silver/slate typography (`0x94A3B8`).
     - Outlined dynamic status box that changes color based on the current system state (Green for Ready, Red for Recording, Cyan for Saved, Yellow for Playing).
     - Bottom control guide in vibrant cyan and sky blue.
5. **Debouncing & State Safety**:
   - After triggering either action, `while (k10.buttonA->isPressed())` or `while (k10.buttonB->isPressed())` holds execution until the physical tactile switch is released, preventing accidental duplicate triggers.

---

## Sensors/actuator detaile

### 1. Onboard Digital MEMS Microphone
- **Technology**: Micro-Electro-Mechanical Systems (MEMS) acoustic sensor.
- **Principle**: Acoustic sound waves enter the acoustic port, vibrating a microscopic silicon diaphragm. The change in capacitance between the diaphragm and a fixed backplate is converted by an internal ASIC into digital audio data (PDM or I2S).
- **Advantages**: Immune to mechanical vibration, low noise floor, and highly stable frequency response compared to legacy analog electret capsules.

### 2. Audio Amplifier & Built-in Speaker
- **Technology**: Integrated digital class-D audio power amplifier driving an onboard miniature dynamic speaker.
- **Efficiency**: Class-D switching operation provides high acoustic volume and clarity while minimizing power consumption and heat generation.

### 3. MicroSD (TF Card) Storage
- **Protocol**: 4-wire SPI (CS on Pin 40, MOSI on Pin 21, MISO on Pin 41, SCK on Pin 12).
- **Filesystem**: Standard FAT32 formatted microSD cards (up to 32GB recommended).
- **Drive Mapping**: Mapped to root prefix `"S:/"` in K10 Arduino library.

### 4. Onboard WS2812 RGB LED Indicator
- **Role**: Provides instant visual peripheral feedback:
  - **Red (`0xFF0000`)**: Recording in progress.
  - **Green (`0x00FF00`)**: Audio playback in progress.
  - **Off (`0x000000`)**: Standby / Idle.

---

## Step by step function wise code break down

### 1. Header Inclusions & Peripherals Instantiation
```cpp
#include "unihiker_k10.h"

UNIHIKER_K10 k10;
uint8_t screen_dir = 2; // Portrait orientation (240x320)
Music music;
```
- `#include "unihiker_k10.h"`: Main driver library for the UNIHIKER K10 board.
- `UNIHIKER_K10 k10;`: Instantiates the master hardware controller.
- `uint8_t screen_dir = 2;`: Configures the ST7789 display to portrait mode (240 pixels wide by 320 pixels tall).
- `Music music;`: Instantiates the audio synthesis, recording, and playback engine.

### 2. Dashboard UI Rendering Helper (`drawUI`)
```cpp
void drawUI(String statusText, uint32_t statusColor) {
    k10.canvas->canvasClear();
    k10.setScreenBackground(0x0F172A);

    // 1. Header Title (24px bold)
    k10.canvas->canvasText("VOICE RECORDER", 32, 18, 0xFEE715,
                           k10.canvas->eCNAndENFont24, 20, false);
    k10.canvas->canvasLine(15, 48, 225, 48, 0x334155);

    // 2. Target File Information Card
    k10.canvas->canvasRectangle(15, 60, 210, 68, 0x1E293B, 0x1E293B, true);
    k10.canvas->canvasText("Target: S:/sound.wav", 25, 72, 0x94A3B8,
                           k10.canvas->eCNAndENFont16, 26, false);
    k10.canvas->canvasText("Duration: 3 Seconds", 25, 96, 0x94A3B8,
                           k10.canvas->eCNAndENFont16, 26, false);

    // 3. Dynamic Status Panel
    k10.canvas->canvasRectangle(15, 142, 210, 60, statusColor, 0x111827, true);
    k10.canvas->canvasText(statusText, 25, 162, statusColor,
                           k10.canvas->eCNAndENFont16, 26, false);

    // 4. Button Control Guide (Bottom panel)
    k10.canvas->canvasLine(15, 220, 225, 220, 0x334155);
    k10.canvas->canvasText("Button [A]: Record (3s)", 20, 235, 0x00E5FF,
                           k10.canvas->eCNAndENFont16, 26, false);
    k10.canvas->canvasText("Button [B]: Play Audio", 20, 262, 0x38BDF8,
                           k10.canvas->eCNAndENFont16, 26, false);
    k10.canvas->canvasText("Requires FAT32 MicroSD card", 20, 292, 0x64748B,
                           k10.canvas->eCNAndENFont16, 26, false);

    k10.canvas->updateCanvas();
}
```
- Completely refreshes the canvas buffer with high-contrast color schemes (`0xFEE715` gold header, `0x94A3B8` slate metadata, `0x00E5FF` cyan labels).
- Avoids screen flicker by rendering into the RAM canvas before committing via `updateCanvas()`.

### 3. Setup Routine (`setup`)
```cpp
void setup() {
    k10.begin();
    k10.initScreen(screen_dir);
    k10.creatCanvas();
    k10.initSDFile(); // Mount SD/TF Card

    // Initialize RGB LED to OFF
    k10.rgb->brightness(5);
    k10.rgb->write(-1, 0x000000);

    // Initial Standby Screen
    drawUI("Status: READY", 0x00FF88);
}
```
- Initializes board power and buses (`begin()`).
- Sets display rotation and allocates the canvas buffer (`creatCanvas()`).
- Mounts the MicroSD card filesystem (`initSDFile()`).
- Clears the RGB LEDs and displays the initial `"READY"` state.

### 4. Main Event Loop (`loop`)
```cpp
void loop() {
    // Button A: Record
    if (k10.buttonA->isPressed()) {
        k10.rgb->write(-1, 0xFF0000); // Red LED
        drawUI("Status: RECORDING (3s)...", 0xFF4444);

        music.recordSaveToTFCard("S:/sound.wav", 3);

        k10.rgb->write(-1, 0x000000);
        drawUI("Status: RECORD SAVED!", 0x00E5FF);

        while (k10.buttonA->isPressed()) {
            delay(50);
        }
    }
    // Button B: Playback
    else if (k10.buttonB->isPressed()) {
        k10.rgb->write(-1, 0x00FF00); // Green LED
        drawUI("Status: PLAYING AUDIO...", 0xFEE715);

        music.playTFCardAudio("S:/sound.wav");
        delay(3000);

        k10.rgb->write(-1, 0x000000);
        drawUI("Status: PLAYBACK DONE", 0x00FF88);

        while (k10.buttonB->isPressed()) {
            delay(50);
        }
    }
    delay(20);
}
```
- Polls both buttons with debouncing.
- Synchronizes visual status and LED state during the recording and playback processes.

---

## API and Functions detailes

| API / Method | Arguments | Return Type | Description |
| :--- | :--- | :--- | :--- |
| `k10.begin()` | None | `void` | Initializes power supply, clocks, and I2C/SPI buses on UNIHIKER K10. |
| `k10.initScreen(dir)` | `uint8_t dir` (`2` = Portrait) | `void` | Initializes the 2.8" ST7789 TFT display in portrait mode (240x320). |
| `k10.creatCanvas()` | None | `void` | Allocates the internal 2D drawing canvas frame buffer in RAM. |
| `k10.initSDFile()` | None | `void` | Initializes SPI bus and mounts the FAT32 MicroSD card under root `"S:/"`. |
| `k10.rgb->brightness(level)` | `uint8_t level` (0 to 9) | `void` | Sets the global brightness multiplier for onboard WS2812 RGB LEDs. |
| `k10.rgb->write(index, color)` | `int index` (`-1` = all LEDs), `uint32_t color` | `void` | Writes 24-bit RGB color to specified LED (`0xFF0000` = Red, `0x00FF00` = Green). |
| `music.recordSaveToTFCard(path, time)` | `const char* path`, `uint8_t time` (seconds) | `void` | Records audio from onboard MEMS mic and saves it as a WAV file to SD card. |
| `music.playTFCardAudio(path)` | `const char* path` | `void` | Plays an audio file (e.g. WAV format) stored on the MicroSD card through speaker. |
| `k10.canvas->canvasRectangle(...)` | `x, y, w, h, borderCol, fillCol, fill` | `void` | Renders a styled solid or outlined rectangular container card. |
| `k10.canvas->canvasText(...)` | `text, x, y, color, font, count, autoClean` | `void` | Renders anti-aliased text with specified font (`eCNAndENFont24` or `eCNAndENFont16`). |
| `k10.canvas->canvasLine(...)` | `x1, y1, x2, y2, color` | `void` | Draws a 1-pixel separator line between interface sections. |
| `k10.canvas->updateCanvas()` | None | `void` | Pushes the rendered canvas buffer over SPI to the physical ST7789 screen. |
| `k10.buttonA->isPressed()` | None | `bool` | Returns `true` if Button A is physically pressed. |
| `k10.buttonB->isPressed()` | None | `bool` | Returns `true` if Button B is physically pressed. |

---

## Full Code

```cpp
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
```
