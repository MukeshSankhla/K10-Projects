## What this tutorial is??

This tutorial demonstrates using the UNIHIKER K10 onboard **Neural Pet & Wildlife Tracking Engine** (`AIRecognition::Cat`) to detect feline faces and stream real-time geometry metrics.

Features:
- **Neural Cat Detection Model**: Recognizes feline facial characteristics in live camera video.
- **Safari Sand & Mint Wildlife Tracker Theme**: Warm savanna earth background (`0x140B04`), safari amber (`0xFB923C`), feline tracker mint (`0x2DD4BF`), and earth borders (`0x4A2C18`).
- **Telemetry Bounds**: Displays target center coordinates ($X, Y$), face length, and face width in pixels.
- **Mint Indicator**: Lights onboard RGB LED to mint cyan when a cat is tracked in the camera view.

---

## How it works

1. **Wildlife Tracking Engine**:
   - `ai.switchAiMode(ai.Cat)` activates the feline face classification and bounding model on the ESP32-S3 AI accelerator.
   - `ai.isDetectContent(AIRecognition::Cat)` evaluates true when a cat face is detected.
   - `ai.getCatData(...)` extracts dimensions and centroid coordinates.
2. **Dynamic Partial Refresh**:
   - The top banner and bottom telemetry container are drawn once in `setup()`.
   - In `loop()`, only the coordinate strings inside the bottom telemetry card are erased and redrawn without wiping the camera background.

---

## API and Functions detailes

| API / Method | Arguments | Return Type | Description |
| :--- | :--- | :--- | :--- |
| `ai.switchAiMode(ai.Cat)` | Mode enum | `void` | Activates Cat Detection neural network. |
| `ai.isDetectContent(AIRecognition::Cat)` | Mode enum | `bool` | True when a feline face is in view. |
| `ai.getCatData(type)` | Metric enum | `int` | Reads CenterX, CenterY, Length, Width. |
| `k10.canvas->updateCanvas()` | None | `void` | Flushes canvas buffer to screen. |

---

## Full Code

```cpp
#include "unihiker_k10.h"
#include "AIRecognition.h"

UNIHIKER_K10 k10;
uint8_t screen_dir = 2; // Portrait (240x320)
AIRecognition ai;

// Safari Sand & Mint Wildlife Tracker Theme
const uint32_t COLOR_BG        = 0x140B04; // Deep Savanna Earth
const uint32_t COLOR_PANEL     = 0x241408; // Wildlife Panel
const uint32_t COLOR_BORDER    = 0x4A2C18; // Earth Border
const uint32_t COLOR_AMBER     = 0xFB923C; // Safari Amber
const uint32_t COLOR_MINT      = 0x2DD4BF; // Feline Tracker Mint
const uint32_t COLOR_YELLOW    = 0xFACC15;
const uint32_t COLOR_WHITE     = 0xFFFFFF;
const uint32_t COLOR_MUTED     = 0xA8A29E;

bool hadCat = false;

void drawStaticChrome() {
    // Header Bar (y: 0 to 36)
    k10.canvas->canvasRectangle(0, 0, 240, 36, COLOR_PANEL, COLOR_PANEL, true);
    k10.canvas->canvasLine(0, 36, 240, 36, COLOR_AMBER);
    k10.canvas->canvasText("WILDLIFE TRACKER // FELINE", 10, 10, COLOR_AMBER, k10.canvas->eCNAndENFont16, 50, false);

    // Bottom Telemetry Card (y: 240 to 320)
    k10.canvas->canvasRectangle(0, 240, 240, 80, COLOR_PANEL, COLOR_PANEL, true);
    k10.canvas->canvasLine(0, 240, 240, 240, COLOR_MINT);

    k10.canvas->canvasText("FELINE TARGET METRICS", 12, 246, COLOR_AMBER, k10.canvas->eCNAndENFont16, 50, false);
    k10.canvas->canvasText("CENTER: --, --", 12, 268, COLOR_WHITE, k10.canvas->eCNAndENFont16, 50, false);
    k10.canvas->canvasText("BOUNDS: -- x --", 12, 290, COLOR_MINT, k10.canvas->eCNAndENFont16, 50, false);

    k10.canvas->updateCanvas();
}

void updateCatDisplay(int cx, int cy, int len, int wid) {
    k10.canvas->canvasRectangle(12, 264, 220, 52, COLOR_PANEL, COLOR_PANEL, true);

    String centerStr = "CENTER: X=" + String(cx) + " Y=" + String(cy);
    String boundStr = "BOUNDS: " + String(len) + "x" + String(wid) + " px";

    k10.canvas->canvasText(centerStr, 12, 268, COLOR_WHITE, k10.canvas->eCNAndENFont16, 50, false);
    k10.canvas->canvasText(boundStr, 12, 290, COLOR_MINT, k10.canvas->eCNAndENFont16, 50, false);

    k10.rgb->write(-1, 0x00FFBB);
    k10.canvas->updateCanvas();
}

void clearCatDisplay() {
    k10.canvas->canvasRectangle(12, 264, 220, 52, COLOR_PANEL, COLOR_PANEL, true);
    k10.canvas->canvasText("STATUS: SEARCHING FOV...", 12, 268, COLOR_AMBER, k10.canvas->eCNAndENFont16, 50, false);
    k10.canvas->canvasText("NO CAT DETECTED", 12, 290, COLOR_MUTED, k10.canvas->eCNAndENFont16, 50, false);

    k10.rgb->write(-1, 0x332200);
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
    ai.switchAiMode(ai.Cat);

    k10.rgb->brightness(6);
    drawStaticChrome();
    clearCatDisplay();
}

void loop() {
    if (ai.isDetectContent(AIRecognition::Cat)) {
        int cx = ai.getCatData(AIRecognition::CenterX);
        int cy = ai.getCatData(AIRecognition::CenterY);
        int len = ai.getCatData(AIRecognition::Length);
        int wid = ai.getCatData(AIRecognition::Width);

        updateCatDisplay(cx, cy, len, wid);
        hadCat = true;
    } else if (hadCat) {
        hadCat = false;
        clearCatDisplay();
    }

    delay(200);
}
```
