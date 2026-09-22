## What this tutorial is??

This tutorial demonstrates using the UNIHIKER K10 onboard **Dual-Microphone Offline Neural Speech Recognition Engine** (`asr.h`) to detect the `"Hi, Telly"` wake word and control onboard hardware with spoken English commands.

Features:
- **Offline Neural Wake-Word Detection**: Listens continuously for the English wake word `"Hi, Telly"` (or `"Jarvis"`) without cloud connectivity.
- **Voice Command Automation**: Recognizes command phrases (`"Switch on"` / `"Light on"` / `"Lights on"` and `"Switch off"` / `"Light off"` / `"Lights off"`) to toggle the onboard RGB LED.
- **Pre-Merged Flash Binary with Acoustic Model**: The pre-built `A13_AI_Speech_Recognition.bin` includes the 4.37MB English neural acoustic model (`srmodels4.bin`) merged at flash offset `0x510000`, preventing missing-model crashes.
- **Visual Boot Guard**: Initializes the screen first, displaying `"LOADING ASR MODEL..."`, with a 7-second watchdog guard that presents an on-screen diagnostic alert if the flash partition is missing.
- **Acoustic Waveform Royal Purple & Gold Theme**: Studio aesthetic with deep purple background (`0x0D031A`), neon violet (`0xC084FC`), acoustic gold (`0xFACC15`), and dynamic 9-band frequency visualizer.

---

## Why ASR Can Crash & How It Is Prevented

1. **Acoustic Model Partition at `0x510000`**:
   - The ESP32-S3 offline speech recognition engine (`esp-sr`) relies on a pre-trained neural network acoustic model stored in a dedicated raw SPIFFS flash partition located at address `0x510000`.
   - In standard sketch compilations, only the bootloader (`0x0`), partition table (`0x8000`), and user application code (`0x10000`) are included. If `0x510000` is empty, `esp_srmodel_init()` returns `NULL` and causes an immediate **Kernel Panic / LoadProhibited crash loop**.
   - **Fix Provided**: Our pre-compiled `A13_AI_Speech_Recognition.bin` (9.68 MB) includes `srmodels4.bin` pre-merged directly at `0x510000`.
2. **Language Mode Alignment (`EN_MODE`)**:
   - The English wake-word `"Hi, Telly"` and English commands require `asr.asrInit(CONTINUOUS, EN_MODE, 6000)`. Passing `CN_MODE` with English strings causes MultiNet Pinyin parser corruption.
3. **If Compiling via Arduino IDE**:
   - Open Arduino IDE.
   - Go to menu: **Tools -> Model -> EN** (this instructs Arduino IDE to upload `srmodels4.bin` to `0x510000`).

---

## How it works

1. **ASR Initialization & Safety Guard**:
   - The screen is initialized before calling `asrInit()`.
   - `asr.asrInit(CONTINUOUS, EN_MODE, 6000)` loads the English WakeNet and MultiNet models.
   - A 7-second timeout loop verifies that `asr._asrState` transitions to 1. If it fails (due to a missing model partition), a helpful error card is shown on the LCD rather than locking the CPU.
2. **Multi-Phrase Command Aliases**:
   - Registers both `"Switch on"` and `"Light on"` / `"Lights on"` under Command ID 1.
   - Registers both `"Switch off"` and `"Light off"` / `"Lights off"` under Command ID 2.
3. **Dynamic Component Partial Refresh**:
   - Static chrome is drawn once during `setup()`.
   - In `loop()`, only the wake status pill (`STANDBY` vs `AWAKE`), the 9 equalizer spectrum bars, and the LED status pill are erased and redrawn inside their bounding boxes.

---

## API and Functions details

| API / Method | Arguments | Return Type | Description |
| :--- | :--- | :--- | :--- |
| `asr.asrInit(mode, lang, time)` | Mode, Language, Timeout | `void` | Initializes offline ASR engine (`EN_MODE` = English). |
| `asr.addASRCommand(id, cmd)` | Command ID, string | `void` | Registers voice phrase to vocabulary. |
| `asr.isWakeUp()` | None | `bool` | True when wake word has been heard and engine is listening. |
| `asr.isDetectCmdID(id)` | Command ID | `bool` | True when a registered command phrase is detected. |
| `k10.rgb->brightness(val)` | 0 - 9 | `void` | Sets onboard RGB LED brightness. |
| `k10.canvas->updateCanvas()` | None | `void` | Flushes graphics buffer to display. |

---

## Full Code

```cpp
#include "unihiker_k10.h"
#include "asr.h"

UNIHIKER_K10 k10;
uint8_t screen_dir = 2; // Portrait (240x320)
ASR asr;

// Acoustic Waveform Royal Purple & Bright Gold Theme
const uint32_t COLOR_BG        = 0x0D031A; // Deep Studio Purple
const uint32_t COLOR_PANEL     = 0x1E0A36; // Acoustic Card Fill
const uint32_t COLOR_BORDER    = 0x411873; // Studio Purple Border
const uint32_t COLOR_PURPLE    = 0xC084FC; // Neon Violet-Purple
const uint32_t COLOR_GOLD      = 0xFACC15; // Bright Acoustic Gold
const uint32_t COLOR_CYAN      = 0x38BDF8; // Frequency Cyan
const uint32_t COLOR_WHITE     = 0xFFFFFF;
const uint32_t COLOR_MUTED     = 0x9333EA;
const uint32_t COLOR_GREEN     = 0x10B981;
const uint32_t COLOR_RED       = 0xEF4444;

bool lastWakeState = false;
bool lightIsOn = false;
unsigned long lastEqTick = 0;

void drawStaticChrome() {
    k10.canvas->canvasClear();
    k10.canvas->canvasRectangle(0, 0, 240, 320, COLOR_BG, COLOR_BG, true);

    // Studio Header Bar (y: 0 to 36)
    k10.canvas->canvasRectangle(0, 0, 240, 36, COLOR_PANEL, COLOR_PANEL, true);
    k10.canvas->canvasLine(0, 36, 240, 36, COLOR_PURPLE);
    k10.canvas->canvasText("NEURAL SPEECH RECOGNITION", 8, 10, COLOR_PURPLE, k10.canvas->eCNAndENFont16, 50, false);

    // Wake Word & Status Card (y: 44 to 102)
    k10.canvas->canvasRectangle(8, 44, 224, 58, COLOR_BORDER, COLOR_PANEL, true);
    k10.canvas->canvasText("WAKE WORD: \"Hi, Telly\"", 16, 52, COLOR_GOLD, k10.canvas->eCNAndENFont16, 50, false);

    // Equalizer Frequency Shell (y: 110 to 186)
    k10.canvas->canvasRectangle(8, 110, 224, 76, COLOR_BORDER, COLOR_PANEL, true);
    k10.canvas->canvasText("ACOUSTIC SPECTRUM", 16, 116, COLOR_MUTED, k10.canvas->eCNAndENFont16, 50, false);

    // Detected Command Card (y: 194 to 282)
    k10.canvas->canvasRectangle(8, 194, 224, 88, COLOR_BORDER, COLOR_PANEL, true);
    k10.canvas->canvasText("VOICE COMMAND TRIGGER", 16, 202, COLOR_MUTED, k10.canvas->eCNAndENFont16, 50, false);
    k10.canvas->canvasText("Cmds: \"Light on\" / \"Light off\"", 16, 222, COLOR_WHITE, k10.canvas->eCNAndENFont16, 50, false);

    // Footer Info Bar (y: 290 to 320)
    k10.canvas->canvasRectangle(0, 290, 240, 30, COLOR_PANEL, COLOR_PANEL, true);
    k10.canvas->canvasLine(0, 290, 240, 290, COLOR_BORDER);
    k10.canvas->canvasText("OFFLINE DUAL-MIC ASR (EN)", 14, 296, COLOR_PURPLE, k10.canvas->eCNAndENFont16, 50, false);

    k10.canvas->updateCanvas();
}

// Display bootloader / loading progress
void drawBootStatus(const char* msg) {
    k10.canvas->canvasRectangle(14, 72, 212, 26, COLOR_PANEL, COLOR_PANEL, true);
    k10.canvas->canvasText(msg, 18, 77, COLOR_GOLD, k10.canvas->eCNAndENFont16, 50, false);
    k10.canvas->updateCanvas();
}

// Display model missing warning if flash partition 0x510000 has not been flashed
void drawModelMissingAlert() {
    k10.canvas->canvasRectangle(8, 44, 224, 240, 0x7F1D1D, 0x1F0A0A, true);
    k10.canvas->canvasRectangle(16, 52, 208, 26, COLOR_RED, COLOR_RED, true);
    k10.canvas->canvasText("MODEL INIT TIMEOUT", 28, 57, COLOR_WHITE, k10.canvas->eCNAndENFont16, 50, false);

    k10.canvas->canvasText("Speech Model partition", 16, 90, COLOR_GOLD, k10.canvas->eCNAndENFont16, 50, false);
    k10.canvas->canvasText("at 0x510000 missing!", 16, 110, COLOR_GOLD, k10.canvas->eCNAndENFont16, 50, false);

    k10.canvas->canvasText("To fix:", 16, 140, COLOR_WHITE, k10.canvas->eCNAndENFont16, 50, false);
    k10.canvas->canvasText("1. Flash A13 merged bin", 16, 162, COLOR_CYAN, k10.canvas->eCNAndENFont16, 50, false);
    k10.canvas->canvasText("   (contains EN model)", 16, 182, COLOR_CYAN, k10.canvas->eCNAndENFont16, 50, false);
    k10.canvas->canvasText("2. Or in Arduino IDE:", 16, 206, COLOR_PURPLE, k10.canvas->eCNAndENFont16, 50, false);
    k10.canvas->canvasText("   Tools -> Model -> EN", 16, 226, COLOR_PURPLE, k10.canvas->eCNAndENFont16, 50, false);

    k10.canvas->updateCanvas();
    Serial.println(F("[ERROR] ASR Model not detected at 0x510000!"));
    Serial.println(F("[TIP] Flash A13_AI_Speech_Recognition.bin or select Tools -> Model -> EN"));
}

// Partial refresh for wake state pill
void updateWakeState(bool awake) {
    k10.canvas->canvasRectangle(14, 72, 212, 26, COLOR_PANEL, COLOR_PANEL, true);

    if (awake) {
        k10.canvas->canvasRectangle(16, 74, 90, 22, COLOR_GREEN, COLOR_GREEN, true);
        k10.canvas->canvasText("AWAKE", 36, 77, COLOR_WHITE, k10.canvas->eCNAndENFont16, 50, false);
        k10.canvas->canvasText("Listening...", 116, 77, COLOR_GOLD, k10.canvas->eCNAndENFont16, 50, false);
    } else {
        k10.canvas->canvasRectangle(16, 74, 90, 22, COLOR_BORDER, COLOR_BORDER, true);
        k10.canvas->canvasText("STANDBY", 26, 77, COLOR_PURPLE, k10.canvas->eCNAndENFont16, 50, false);
        k10.canvas->canvasText("Say: Hi Telly", 116, 77, COLOR_WHITE, k10.canvas->eCNAndENFont16, 50, false);
    }

    k10.canvas->updateCanvas();
}

// Partial refresh for equalizer spectrum bars
void updateEqualizer(bool active) {
    k10.canvas->canvasRectangle(16, 136, 208, 42, COLOR_PANEL, COLOR_PANEL, true);

    for (int i = 0; i < 9; i++) {
        int barH = active ? random(6, 36) : 4;
        int barX = 22 + (i * 22);
        int barY = 174 - barH;
        uint32_t barColor = (i % 2 == 0) ? COLOR_GOLD : COLOR_PURPLE;
        k10.canvas->canvasRectangle(barX, barY, 14, barH, barColor, barColor, true);
    }
    k10.canvas->updateCanvas();
}

// Partial refresh for light command state
void updateLightState(bool on) {
    k10.canvas->canvasRectangle(14, 246, 212, 30, COLOR_PANEL, COLOR_PANEL, true);

    if (on) {
        k10.canvas->canvasRectangle(16, 248, 196, 24, COLOR_RED, COLOR_RED, true);
        k10.canvas->canvasText("LED STATE: ON (RED)", 36, 252, COLOR_WHITE, k10.canvas->eCNAndENFont16, 50, false);
    } else {
        k10.canvas->canvasRectangle(16, 248, 196, 24, COLOR_BORDER, COLOR_BORDER, true);
        k10.canvas->canvasText("LED STATE: OFF", 54, 252, COLOR_WHITE, k10.canvas->eCNAndENFont16, 50, false);
    }

    k10.canvas->updateCanvas();
}

void setup() {
    Serial.begin(115200);
    k10.begin();

    // Initialize screen first so boot progress is displayed immediately
    k10.initScreen(screen_dir);
    k10.creatCanvas();
    drawStaticChrome();
    drawBootStatus("LOADING ASR MODEL...");

    // Initialize English WakeNet & MultiNet ASR
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

    // Register voice command aliases
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
```
