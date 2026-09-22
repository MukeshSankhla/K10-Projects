## What this tutorial is??

This tutorial demonstrates running the UNIHIKER K10 **Optical Motion & Movement Detection Engine** (`AIRecognition::Move`) with configurable sensitivity thresholds and security alert indicators.

Features:
- **Optical Frame Differencing**: Real-time pixel motion tracking using the ESP32-S3 camera pipeline.
- **Configurable Sensitivity Threshold**: Tuned to threshold `50` (range: 10 to 200).
- **High-Alert Laser Red Security Theme**: Obsidian red background (`0x140305`), alert panel (`0x24080D`), laser red border (`0x4E121A`), and alarm red (`0xEF4444`).
- **Dynamic Intrusion Alert Banner**: Flashes `ALERT DETECT` and records incident counts upon detecting movement; returns to `ARMED SECURE` when quiet.

---

## How it works

1. **Movement Detection Algorithm**:
   - `ai.switchAiMode(ai.Move)` initializes optical differential comparison between sequential camera frames.
   - `ai.setMotinoThreshold(50)` sets the trigger boundary for motion sensitivity.
   - `ai.isDetectContent(AIRecognition::Move)` returns true when motion surpasses the threshold.
2. **Dynamic Partial Refresh**:
   - The top header bar and sensitivity info card are rendered once during `setup()`.
   - In `loop()`, only the alarm state pill (`ARMED SECURE` vs `ALERT DETECT`) and incident counter are erased and updated without display blanking.

---

## API and Functions detailes

| API / Method | Arguments | Return Type | Description |
| :--- | :--- | :--- | :--- |
| `ai.switchAiMode(ai.Move)` | Mode enum | `void` | Activates Movement Detection mode. |
| `ai.setMotinoThreshold(val)` | Integer `10..200` | `void` | Configures optical motion sensitivity. |
| `ai.isDetectContent(mode)` | Mode enum | `bool` | True when optical motion is detected. |
| `k10.canvas->updateCanvas()` | None | `void` | Flushes canvas buffer to screen. |

---

## Full Code

```cpp
#include "unihiker_k10.h"
#include "AIRecognition.h"

UNIHIKER_K10 k10;
uint8_t screen_dir = 2; // Portrait (240x320)
AIRecognition ai;

// Laser Red & Alert Orange Security Theme
const uint32_t COLOR_BG        = 0x140305; // Dark Alert Obsidian
const uint32_t COLOR_PANEL     = 0x24080D; // Alert Card Fill
const uint32_t COLOR_BORDER    = 0x4E121A; // Crimson Border
const uint32_t COLOR_RED       = 0xEF4444; // Bright Laser Red
const uint32_t COLOR_ORANGE    = 0xF97316; // Warning Orange
const uint32_t COLOR_BLUE      = 0x38BDF8; // Armed Cyan-Blue
const uint32_t COLOR_WHITE     = 0xFFFFFF;
const uint32_t COLOR_MUTED     = 0x9F7177;

uint32_t motionTriggers = 0;
bool lastMotion = false;

void drawStaticChrome() {
    // Header Bar (y: 0 to 36)
    k10.canvas->canvasRectangle(0, 0, 240, 36, COLOR_PANEL, COLOR_PANEL, true);
    k10.canvas->canvasLine(0, 36, 240, 36, COLOR_RED);
    k10.canvas->canvasText("OPTICAL MOTION SENSOR", 10, 10, COLOR_RED, k10.canvas->eCNAndENFont16, 50, false);

    // Sensitivity & Profile Card (y: 44 to 90)
    k10.canvas->canvasRectangle(8, 44, 224, 46, COLOR_BORDER, COLOR_PANEL, true);
    k10.canvas->canvasText("SENSITIVITY THRESHOLD", 16, 52, COLOR_MUTED, k10.canvas->eCNAndENFont16, 50, false);
    k10.canvas->canvasText("LEVEL: 50 / 200 (NOMINAL)", 16, 70, COLOR_ORANGE, k10.canvas->eCNAndENFont16, 50, false);

    // Alarm Status Banner Card (y: 236 to 284)
    k10.canvas->canvasRectangle(8, 236, 224, 48, COLOR_BORDER, COLOR_PANEL, true);

    // Footer Status Bar (y: 290 to 320)
    k10.canvas->canvasRectangle(0, 290, 240, 30, COLOR_PANEL, COLOR_PANEL, true);
    k10.canvas->canvasLine(0, 290, 240, 290, COLOR_BORDER);
    k10.canvas->canvasText("DIFF: REAL-TIME VISION", 12, 296, COLOR_MUTED, k10.canvas->eCNAndENFont16, 50, false);

    k10.canvas->updateCanvas();
}

void updateMotionState(bool detected) {
    k10.canvas->canvasRectangle(12, 240, 216, 40, COLOR_PANEL, COLOR_PANEL, true);

    if (detected) {
        k10.canvas->canvasRectangle(14, 244, 114, 22, COLOR_RED, COLOR_RED, true);
        k10.canvas->canvasText("ALERT DETECT", 20, 247, COLOR_WHITE, k10.canvas->eCNAndENFont16, 50, false);
        k10.canvas->canvasText("#" + String(motionTriggers), 140, 247, COLOR_RED, k10.canvas->eCNAndENFont16, 50, false);
        k10.rgb->write(-1, 0xFF0033);
    } else {
        k10.canvas->canvasRectangle(14, 244, 96, 22, COLOR_BORDER, COLOR_BORDER, true);
        k10.canvas->canvasText("ARMED SECURE", 20, 247, COLOR_BLUE, k10.canvas->eCNAndENFont16, 50, false);
        k10.canvas->canvasText("No Motion", 136, 247, COLOR_MUTED, k10.canvas->eCNAndENFont16, 50, false);
        k10.rgb->write(-1, 0x0044FF);
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

    k10.setBgCamerImage(true);
    ai.switchAiMode(ai.Move);
    ai.setMotinoThreshold(50);

    k10.rgb->brightness(6);
    drawStaticChrome();
    updateMotionState(false);
}

void loop() {
    bool detected = ai.isDetectContent(AIRecognition::Move);

    if (detected) {
        if (!lastMotion) {
            motionTriggers++;
            updateMotionState(true);
            lastMotion = true;
        }
    } else if (lastMotion) {
        lastMotion = false;
        updateMotionState(false);
    }

    delay(80);
}
```
