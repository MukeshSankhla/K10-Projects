## What this tutorial is??

This tutorial demonstrates using the UNIHIKER K10 onboard **Neural Barcode & QR Code Scanning Engine** (`AIRecognition::Code`) with real-time camera viewfinder alignment and decoded string extraction.

Features:
- **Optical Viewfinder Reticle**: Graphical alignment brackets indicating the optimal scanning zone.
- **Instant QR Decoding**: Decodes ISO/IEC 18004 QR codes and displays extracted URLs or text messages.
- **Cyber Matrix Neon Lime Theme**: Matrix black-green background (`0x021005`), optical panel (`0x08220D`), neon lime (`0x84CC16`), and matrix green (`0x22C55E`).
- **Success Feedback**: Flashes onboard RGB LED to bright green upon successful code reading.

---

## How it works

1. **Barcode / QR Decoding Pipeline**:
   - `ai.switchAiMode(ai.Code)` activates the barcode / QR code decoding model on the camera image buffer.
   - `ai.isDetectContent(AIRecognition::Code)` signals when a valid QR pattern is recognized.
   - `ai.getQrCodeContent()` returns the decoded text payload as a `String`.
2. **Dynamic Partial Refresh**:
   - The top header, corner alignment reticle brackets, and payload card container are rendered once during `setup()`.
   - In `loop()`, only the decoded payload text box is refreshed without resetting the camera feed.

---

## API and Functions detailes

| API / Method | Arguments | Return Type | Description |
| :--- | :--- | :--- | :--- |
| `ai.switchAiMode(ai.Code)` | Mode enum | `void` | Activates QR Code Scanning mode. |
| `ai.isDetectContent(mode)` | Mode enum | `bool` | True when a valid QR code is recognized. |
| `ai.getQrCodeContent()` | None | `String` | Returns decoded text payload. |
| `k10.canvas->updateCanvas()` | None | `void` | Flushes canvas buffer to display. |

---

## Full Code

```cpp
#include "unihiker_k10.h"
#include "AIRecognition.h"

UNIHIKER_K10 k10;
uint8_t screen_dir = 2; // Portrait (240x320)
AIRecognition ai;

// Cyber Matrix Neon Lime & Emerald Theme
const uint32_t COLOR_BG        = 0x021005; // Matrix Black-Green
const uint32_t COLOR_PANEL     = 0x08220D; // Optical Panel Fill
const uint32_t COLOR_BORDER    = 0x16461D; // Matrix Green Border
const uint32_t COLOR_LIME      = 0x84CC16; // Neon Lime
const uint32_t COLOR_GREEN     = 0x22C55E; // Matrix Bright Green
const uint32_t COLOR_YELLOW    = 0xFACC15;
const uint32_t COLOR_WHITE     = 0xFFFFFF;
const uint32_t COLOR_MUTED     = 0x4ADE80;

String lastPayload = "";

void drawStaticChrome() {
    // Header Bar (y: 0 to 36)
    k10.canvas->canvasRectangle(0, 0, 240, 36, COLOR_PANEL, COLOR_PANEL, true);
    k10.canvas->canvasLine(0, 36, 240, 36, COLOR_LIME);
    k10.canvas->canvasText("OPTICAL QR CODE SCANNER", 10, 10, COLOR_LIME, k10.canvas->eCNAndENFont16, 50, false);

    // Center Viewfinder Framing Brackets (x: 40..200, y: 70..210)
    k10.canvas->canvasLine(40, 70, 65, 70, COLOR_GREEN);
    k10.canvas->canvasLine(40, 70, 40, 95, COLOR_GREEN);

    k10.canvas->canvasLine(200, 70, 175, 70, COLOR_GREEN);
    k10.canvas->canvasLine(200, 70, 200, 95, COLOR_GREEN);

    k10.canvas->canvasLine(40, 210, 65, 210, COLOR_GREEN);
    k10.canvas->canvasLine(40, 210, 40, 185, COLOR_GREEN);

    k10.canvas->canvasLine(200, 210, 175, 210, COLOR_GREEN);
    k10.canvas->canvasLine(200, 210, 200, 185, COLOR_GREEN);

    // Decoded Payload Card (y: 230 to 286)
    k10.canvas->canvasRectangle(8, 230, 224, 56, COLOR_BORDER, COLOR_PANEL, true);
    k10.canvas->canvasText("DECODED QR PAYLOAD", 16, 236, COLOR_MUTED, k10.canvas->eCNAndENFont16, 50, false);

    // Footer Status Bar (y: 292 to 320)
    k10.canvas->canvasRectangle(0, 292, 240, 28, COLOR_PANEL, COLOR_PANEL, true);
    k10.canvas->canvasLine(0, 292, 240, 292, COLOR_BORDER);
    k10.canvas->canvasText("ALIGN QR CODE IN RETICLE", 16, 298, COLOR_LIME, k10.canvas->eCNAndENFont16, 50, false);

    k10.canvas->updateCanvas();
}

void updatePayloadDisplay(String payload) {
    k10.canvas->canvasRectangle(14, 252, 212, 30, COLOR_PANEL, COLOR_PANEL, true);

    if (payload.length() > 0) {
        String displayStr = payload.substring(0, min((int)payload.length(), 22));
        k10.canvas->canvasText(displayStr, 16, 256, COLOR_WHITE, k10.canvas->eCNAndENFont16, 50, false);
        k10.rgb->write(-1, 0x00FF44);
    } else {
        k10.canvas->canvasText("WAITING FOR TARGET...", 16, 256, COLOR_MUTED, k10.canvas->eCNAndENFont16, 50, false);
        k10.rgb->write(-1, 0x002200);
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
    ai.switchAiMode(ai.Code);

    k10.rgb->brightness(6);
    drawStaticChrome();
    updatePayloadDisplay("");
}

void loop() {
    if (ai.isDetectContent(AIRecognition::Code)) {
        String content = ai.getQrCodeContent();
        if (content.length() > 0 && content != lastPayload) {
            lastPayload = content;
            updatePayloadDisplay(content);
        }
    }

    delay(150);
}
```
