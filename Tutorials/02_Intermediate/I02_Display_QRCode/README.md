## What this tutorial is??

This tutorial demonstrates how to generate and render **Quick Response (QR) Codes** directly on the UNIHIKER K10 2.8" color display using `k10.canvasDrawCode(content)`.

Features:
- **Interactive Switching**: Pressing **Button [A]** toggles between an official website URL and a Wi-Fi configuration credential format (`WIFI:S:...;T:...;P:...;;`).
- **High-Contrast Container**: Displays the QR code inside a solid white rounded card ensuring seamless optical recognition by smartphones and external scanners.

---

## How it works

1. **Integrated QR Code Generation**:
   - `k10.canvasDrawCode(const char* content)` takes an arbitrary ASCII or URL string, generates the appropriate QR matrix (version, timing patterns, format info, Reed-Solomon error correction codewords), and renders the 2D grid onto the screen canvas.
2. **Wi-Fi Payload Formatting**:
   - The Wi-Fi configuration payload follows the standardized ZXing barcode format:
     `WIFI:S:<SSID>;T:<WPA|WEP|nopass>;P:<Password>;;`
   - Smartphones scanning this QR code automatically prompt the user to connect to the specified Wi-Fi network without typing credentials manually.

---

## API and Functions detailes

| API / Method | Arguments | Return Type | Description |
| :--- | :--- | :--- | :--- |
| `k10.canvasDrawCode(content)` | `const char* content` | `void` | Generates and draws a 2D QR code onto the canvas. |
| `k10.canvas->canvasRectangle(...)` | Container bounds & fill | `void` | Renders a high-contrast white card behind the QR matrix. |
| `k10.canvas->updateCanvas()` | None | `void` | Updates the screen display. |
| `k10.buttonA->isPressed()` | None | `bool` | Polls Button A to toggle payloads. |

---

## Full Code

```cpp
#include "unihiker_k10.h"

UNIHIKER_K10 k10;
uint8_t screen_dir = 2; // Portrait orientation (240x320)

void setup() {
    k10.begin();
    k10.initScreen(screen_dir);
    k10.creatCanvas();
    k10.setScreenBackground(0x0A0F1D); // Deep titanium navy

    // 1. Top Smart Badge Header Banner
    k10.canvas->canvasRectangle(0, 0, 240, 42, 0x111C30, 0x111C30, true);
    k10.canvas->canvasLine(0, 42, 240, 42, 0x1E3A5F);
    k10.canvas->canvasText("DIGITAL PASS", 54, 10, 0x38BDF8,
                           k10.canvas->eCNAndENFont24, 15, false);

    // 2. Generate centered QR Code (230x230 px automatically centered by LVGL at y=45..275)
    k10.canvasDrawCode("https://www.unihiker.com");

    // 3. Bottom Verification & Scan Helper Bar
    k10.canvas->canvasLine(0, 278, 240, 278, 0x1E3A5F);
    k10.canvas->canvasRectangle(0, 279, 240, 41, 0x111C30, 0x111C30, true);
    k10.canvas->canvasText("Scan: unihiker.com", 46, 290, 0x00FF88,
                           k10.canvas->eCNAndENFont16, 22, false);

    // Push canvas elements to screen
    k10.canvas->updateCanvas();

    // Subtle blue indicator LED
    k10.rgb->brightness(4);
    k10.rgb->write(-1, 0x00E5FF);
}

void loop() {
    // Static display node
    delay(200);
}
```
