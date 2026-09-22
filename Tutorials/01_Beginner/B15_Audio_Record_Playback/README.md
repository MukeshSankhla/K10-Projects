## What this tutorial is??

This tutorial demonstrates how to build an interactive **Voice Recorder & Audio Player** on the UNIHIKER K10 using the onboard digital MEMS microphone, audio amplifier/speaker, and FAT32 MicroSD (TF) card storage.

The sketch features a sleek, high-contrast dark UI dashboard with real-time hardware status indicators:
- **Button [A]**: Records 3 seconds of high-fidelity voice audio directly from the onboard microphone and saves it to `S:/sound.wav` on the MicroSD card. An onboard RGB LED illuminates **Red** during the active recording window.
- **Button [B]**: Plays back the recorded `S:/sound.wav` audio file through the onboard speaker. The onboard RGB LED illuminates **Green** during active playback.
- **Display UI**: A modern dashboard provides a bold 24px header, target file specifications, a color-coded status badge (`READY`, `RECORDING`, `RECORD SAVED!`, `PLAYING AUDIO`, `PLAYBACK DONE`), and persistent button control instructions.

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

// Helper function to render a clean, high-contrast UI dashboard
void drawUI(String statusText, uint32_t statusColor) {
    // Clear canvas
    k10.canvas->canvasClear();
    k10.setScreenBackground(0x0F172A); // Deep slate dark background

    // 1. Header Title (24px bold)
    k10.canvas->canvasText("VOICE RECORDER", 32, 18, 0xFEE715,
                           k10.canvas->eCNAndENFont24, 20, false);

    // Decorative divider line
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

    // Push canvas buffer to physical screen
    k10.canvas->updateCanvas();
}

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

void loop() {
    // ==========================================
    // Button A: Record Audio for 3 Seconds
    // ==========================================
    if (k10.buttonA->isPressed()) {
        // Red indicator LED & Recording status
        k10.rgb->write(-1, 0xFF0000);
        drawUI("Status: RECORDING (3s)...", 0xFF4444);

        // Record audio from microphone to TF card
        music.recordSaveToTFCard("S:/sound.wav", 3);

        // Turn off LED & show completion status
        k10.rgb->write(-1, 0x000000);
        drawUI("Status: RECORD SAVED!", 0x00E5FF);

        // Debounce wait for button release
        while (k10.buttonA->isPressed()) {
            delay(50);
        }
    }
    // ==========================================
    // Button B: Playback Recorded Audio
    // ==========================================
    else if (k10.buttonB->isPressed()) {
        // Green indicator LED & Playing status
        k10.rgb->write(-1, 0x00FF00);
        drawUI("Status: PLAYING AUDIO...", 0xFEE715);

        // Play WAV file through onboard speaker
        music.playTFCardAudio("S:/sound.wav");
        delay(3000); // Wait for playback duration

        // Turn off LED & show completion status
        k10.rgb->write(-1, 0x000000);
        drawUI("Status: PLAYBACK DONE", 0x00FF88);

        // Debounce wait for button release
        while (k10.buttonB->isPressed()) {
            delay(50);
        }
    }

    delay(20);
}
```
