## What this tutorial is??

This tutorial demonstrates running the UNIHIKER K10 onboard **Neural Face Detection Model** (`AIRecognition::Face`) with real-time camera streaming and tactical HUD telemetry.

Features:
- **Real-Time Camera Stream**: Streams live color video directly from the onboard camera sensor behind the graphics layer.
- **Edge AI Face Detection**: Neural model calculates face presence, center coordinates $(X, Y)$, and bounding box dimensions (width and length).
- **Tactical Laser Crimson HUD Theme**: High-tech viewfinder aesthetic (`0x1A0508` dark crimson background, `0xFF3366` laser crimson, and `0x00FFFF` targeting cyan).
- **Target Lock Indicator**: Flashes onboard RGB LED to cyan upon face lock; returns to standby red when searching.

---

## How it works

1. **AI Pipeline & Camera Layering**:
   - `k10.initBgCamerImage()` and `k10.setBgCamerImage(true)` streams camera frames directly onto the hardware display layer.
   - `ai.switchAiMode(ai.Face)` activates the neural face detection network on the ESP32-S3 AI accelerator.
2. **Dynamic Partial Refresh**:
   - The top targeting header and bottom telemetry container are drawn once during `setup()`.
   - In `loop()`, only the coordinate text lines (`CENTER: X=... Y=...` and `BOUNDS: ...x... px`) are refreshed inside the bottom HUD pod without wiping or flickering the camera stream.

---

## API and Functions detailes

| API / Method | Arguments | Return Type | Description |
| :--- | :--- | :--- | :--- |
| `ai.initAi()` | None | `void` | Initializes neural AI engine. |
| `ai.switchAiMode(mode)` | `eAiType_t` | `void` | Activates Face Detection mode. |
| `ai.isDetectContent(mode)` | `eAiType_t` | `bool` | Returns true when a human face is in FOV. |
| `ai.getFaceData(type)` | `eFaceOrCatData_t` | `int` | Reads CenterX, CenterY, Length, Width. |
| `k10.setBgCamerImage(bool)` | State | `void` | Enables hardware background camera feed. |

---

## Full Code

```cpp
#include "unihiker_k10.h"
#include "AIRecognition.h"

UNIHIKER_K10 k10;
uint8_t screen_dir = 2; // Portrait (240x320)
AIRecognition ai;

// Tactical Laser Crimson HUD Theme
const uint32_t COLOR_HUD_BG     = 0x1A0508; // Dark Tactical Crimson
const uint32_t COLOR_HUD_PANEL  = 0x2E0B12; // HUD Metadata Pod
const uint32_t COLOR_HUD_BORDER = 0x5C1523; // HUD Border
const uint32_t COLOR_CRIMSON    = 0xFF3366; // High-Tech Crimson
const uint32_t COLOR_CYAN       = 0x00FFFF; // Targeting Cyan
const uint32_t COLOR_YELLOW     = 0xFACC15;
const uint32_t COLOR_WHITE      = 0xFFFFFF;

bool hadFace = false;

void drawStaticChrome() {
    // Top Targeting Header Bar (y: 0 to 36)
    k10.canvas->canvasRectangle(0, 0, 240, 36, COLOR_HUD_PANEL, COLOR_HUD_PANEL, true);
    k10.canvas->canvasLine(0, 36, 240, 36, COLOR_CRIMSON);
    k10.canvas->canvasText("TARGET LOCK // FACE HUD", 10, 10, COLOR_CRIMSON, k10.canvas->eCNAndENFont16, 50, false);

    // Bottom Telemetry Overlay Pod (y: 244 to 320)
    k10.canvas->canvasRectangle(0, 244, 240, 76, COLOR_HUD_PANEL, COLOR_HUD_PANEL, true);
    k10.canvas->canvasLine(0, 244, 240, 244, COLOR_CRIMSON);

    k10.canvas->canvasText("TARGET COORDINATES", 12, 250, COLOR_YELLOW, k10.canvas->eCNAndENFont16, 50, false);
    k10.canvas->canvasText("CENTER: --, --", 12, 272, COLOR_WHITE, k10.canvas->eCNAndENFont16, 50, false);
    k10.canvas->canvasText("BOUNDS: -- x --", 12, 294, COLOR_CYAN, k10.canvas->eCNAndENFont16, 50, false);

    k10.canvas->updateCanvas();
}

void updateFaceTracking(int cx, int cy, int len, int wid) {
    k10.canvas->canvasRectangle(12, 268, 220, 48, COLOR_HUD_PANEL, COLOR_HUD_PANEL, true);

    String centerStr = "CENTER: X=" + String(cx) + " Y=" + String(cy);
    String boundStr = "BOUNDS: " + String(len) + "x" + String(wid) + " px";

    k10.canvas->canvasText(centerStr, 12, 272, COLOR_WHITE, k10.canvas->eCNAndENFont16, 50, false);
    k10.canvas->canvasText(boundStr, 12, 294, COLOR_CYAN, k10.canvas->eCNAndENFont16, 50, false);

    k10.rgb->write(-1, 0x00FFFF);
    k10.canvas->updateCanvas();
}

void clearFaceTracking() {
    k10.canvas->canvasRectangle(12, 268, 220, 48, COLOR_HUD_PANEL, COLOR_HUD_PANEL, true);
    k10.canvas->canvasText("STATUS: SCANNING FOR FACE", 12, 272, COLOR_CRIMSON, k10.canvas->eCNAndENFont16, 50, false);
    k10.canvas->canvasText("BOUNDS: NO TARGET IN FOV", 12, 294, 0x94A3B8, k10.canvas->eCNAndENFont16, 50, false);

    k10.rgb->write(-1, 0xFF0033);
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
    ai.switchAiMode(ai.Face);

    k10.rgb->brightness(6);
    drawStaticChrome();
    clearFaceTracking();
}

void loop() {
    if (ai.isDetectContent(AIRecognition::Face)) {
        int cx = ai.getFaceData(AIRecognition::CenterX);
        int cy = ai.getFaceData(AIRecognition::CenterY);
        int len = ai.getFaceData(AIRecognition::Length);
        int wid = ai.getFaceData(AIRecognition::Width);

        updateFaceTracking(cx, cy, len, wid);
        hadFace = true;
    } else if (hadFace) {
        hadFace = false;
        clearFaceTracking();
    }

    delay(200);
}
```
