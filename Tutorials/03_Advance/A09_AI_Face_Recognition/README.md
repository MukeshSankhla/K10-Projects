## What this tutorial is??

This tutorial demonstrates using the UNIHIKER K10 onboard **Neural Face Recognition & Biometric Enrollment Engine** to learn face features and authenticate verified users.

Features:
- **Biometric Face Enrollment**: Press Button A to register a new user face into internal flash memory (confirmed by green RGB illumination).
- **Identity Verification**: Press Button B to compare live camera video against enrolled templates, displaying matched user IDs (e.g. `VERIFIED ID: #0`).
- **Biometric Security Cobalt Theme**: Deep oceanic navy background (`0x030B18`), security panel (`0x091C38`), scanner cyan (`0x06B6D4`), and electric blue (`0x3B82F6`).
- **Live State Pill**: Dynamic status pill reflecting enrollment, scanning, and match results.

---

## How it works

1. **Enrollment & Recognition Commands**:
   - `ai.sendFaceCmd(ENROLL)`: Captures face landmarks and generates a feature embedding stored in local non-volatile memory.
   - `ai.sendFaceCmd(RECOGNIZE)`: Matches the current camera frame against enrolled embeddings.
   - `ai.isRecognized()` & `ai.getRecognitionID()`: Extracts the matched template ID.
2. **Dynamic Partial Refresh**:
   - Static chrome (header bar, card outlines, and button guide) is rendered once during `setup()`.
   - In `loop()`, only the mode status pill and the verified ID readout box are erased and redrawn.

---

## API and Functions detailes

| API / Method | Arguments | Return Type | Description |
| :--- | :--- | :--- | :--- |
| `ai.sendFaceCmd(ENROLL)` | Command enum | `void` | Triggers face enrollment sequence. |
| `ai.sendFaceCmd(RECOGNIZE)` | Command enum | `void` | Triggers face matching sequence. |
| `ai.isRecognized()` | None | `bool` | True when recognition evaluation is complete. |
| `ai.getRecognitionID()` | None | `int` | Returns enrolled integer user ID (or -1 if no match). |
| `k10.canvas->updateCanvas()` | None | `void` | Flushes canvas buffer to display. |

---

## Full Code

```cpp
#include "unihiker_k10.h"
#include "AIRecognition.h"

UNIHIKER_K10 k10;
uint8_t screen_dir = 2; // Portrait (240x320)
AIRecognition ai;

// Biometric Security Cobalt Theme
const uint32_t COLOR_BG        = 0x030B18; // Deep Biometric Navy
const uint32_t COLOR_PANEL     = 0x091C38; // Security Card Fill
const uint32_t COLOR_BORDER    = 0x163A70; // Cobalt Border
const uint32_t COLOR_CYAN      = 0x06B6D4; // Scanner Cyan
const uint32_t COLOR_BLUE      = 0x3B82F6; // Electric Blue
const uint32_t COLOR_GREEN     = 0x10B981; // Enrolled Green
const uint32_t COLOR_YELLOW    = 0xFACC15;
const uint32_t COLOR_WHITE     = 0xFFFFFF;
const uint32_t COLOR_MUTED     = 0x64748B;

int lastID = -999;
String currentMode = "STANDBY";

void updateModePill(String mode, uint32_t color) {
    k10.canvas->canvasRectangle(12, 48, 216, 34, COLOR_PANEL, COLOR_PANEL, true);
    k10.canvas->canvasText(mode, 16, 56, color, k10.canvas->eCNAndENFont16, 50, false);
    k10.canvas->updateCanvas();
}

void onButtonAPressed() {
    k10.rgb->write(-1, 0x00FF88);
    ai.sendFaceCmd(ENROLL);
    currentMode = "ENROLLING FACE...";
    updateModePill(currentMode, COLOR_GREEN);
}

void onButtonBPressed() {
    k10.rgb->write(-1, 0x0088FF);
    ai.sendFaceCmd(RECOGNIZE);
    currentMode = "SCANNING TO VERIFY...";
    updateModePill(currentMode, COLOR_BLUE);
}

void drawStaticChrome() {
    // Header Bar (y: 0 to 36)
    k10.canvas->canvasRectangle(0, 0, 240, 36, COLOR_PANEL, COLOR_PANEL, true);
    k10.canvas->canvasLine(0, 36, 240, 36, COLOR_CYAN);
    k10.canvas->canvasText("BIOMETRIC FACE ID // AUTH", 10, 10, COLOR_CYAN, k10.canvas->eCNAndENFont16, 50, false);

    // Mode Banner Card (y: 44 to 86)
    k10.canvas->canvasRectangle(8, 44, 224, 42, COLOR_BORDER, COLOR_PANEL, true);

    // Bottom Telemetry & ID Card (y: 236 to 284)
    k10.canvas->canvasRectangle(8, 236, 224, 48, COLOR_BORDER, COLOR_PANEL, true);
    k10.canvas->canvasText("MATCHED USER IDENTITY", 16, 242, COLOR_MUTED, k10.canvas->eCNAndENFont16, 50, false);

    // Footer Control Guide (y: 290 to 320)
    k10.canvas->canvasRectangle(0, 290, 240, 30, COLOR_PANEL, COLOR_PANEL, true);
    k10.canvas->canvasLine(0, 290, 240, 290, COLOR_BORDER);
    k10.canvas->canvasText("A: Enroll Face | B: Recognize", 12, 296, COLOR_WHITE, k10.canvas->eCNAndENFont16, 50, false);

    k10.canvas->updateCanvas();
}

void updateIDDisplay(int id) {
    k10.canvas->canvasRectangle(14, 258, 212, 24, COLOR_PANEL, COLOR_PANEL, true);
    if (id >= 0) {
        String idStr = "VERIFIED ID: #" + String(id);
        k10.canvas->canvasText(idStr, 16, 260, COLOR_GREEN, k10.canvas->eCNAndENFont16, 50, false);
        k10.rgb->write(-1, 0x00FF88);
    } else {
        k10.canvas->canvasText("UNKNOWN USER / NO MATCH", 16, 260, 0xEF4444, k10.canvas->eCNAndENFont16, 50, false);
        k10.rgb->write(-1, 0xFF2200);
    }
    k10.canvas->updateCanvas();
}

void setup() {
    Serial.begin(115200);
    k10.begin();
    k10.initScreen(screen_dir);
    ai.initAi();

    k10.initBgCamerImage();
    k10.setBgCamerImage(false);
    k10.creatCanvas();
    ai.switchAiMode(ai.NoMode);

    k10.buttonA->setPressedCallback(onButtonAPressed);
    k10.buttonB->setPressedCallback(onButtonBPressed);

    k10.setBgCamerImage(true);
    ai.switchAiMode(ai.Face);

    k10.rgb->brightness(6);
    drawStaticChrome();
    updateModePill("STANDBY: PRESS A OR B", COLOR_CYAN);
    updateIDDisplay(-1);
}

void loop() {
    if (ai.isRecognized()) {
        int id = ai.getRecognitionID();
        if (id != lastID) {
            lastID = id;
            updateIDDisplay(id);
            updateModePill("MATCH EVALUATED", COLOR_YELLOW);
        }
    }
    delay(200);
}
```
