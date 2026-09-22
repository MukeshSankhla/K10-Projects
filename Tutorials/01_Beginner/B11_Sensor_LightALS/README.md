# Beginner 11: Light Sensor (ALS) Display

**Category**: Beginner - Fundamentals  
**Target Board**: UNIHIKER K10  
**Difficulty**: Beginner (Level 1)

---

## What This Example Is Of

Demonstrates reading the UNIHIKER K10 onboard Ambient Light Sensor (ALS) and displaying the real-time Lux value and light condition directly onto the screen as simple, clean text.

### Expected Behavior
- Displays simple text lines on the screen:
  - **Row 1**: Title (`LIGHT SENSOR`).
  - **Row 3**: Real-time reading in Lux (`Light: <value> Lux`).
  - **Row 5**: Light condition (`Dark`, `Dim Light`, `Normal`, or `Very Bright`).

---

## Hardware Requirements

- **Development Board**: UNIHIKER K10 (ESP32-S3)
- **Peripherals Needed**: UNIHIKER K10 onboard Ambient Light Sensor, 2.8" LCD screen.
- **Connection**: USB Type-C cable for power and programming.

---

## Detailed Code Explanation

### 1. Simple Row-Based Text API
- `canvasText(text, row, color)`:
  - `text`: String or number to show.
  - `row`: Screen text row number (`1` to `13`). The library automatically clears and updates each row cleanly without any overlapping ghost text.
  - `color`: 24-bit Hex RGB color code (e.g., `0xFEE715` for gold, `0x00E5FF` for cyan).
- `k10.readALS()`: Returns raw ambient light intensity in Lux.

---

## Complete Sketch Source Code

```cpp
#include "unihiker_k10.h"

UNIHIKER_K10 k10;

void setup() {
    k10.begin();
    k10.initScreen(2);               // 2 = Portrait orientation
    k10.creatCanvas();               // Create canvas
    k10.setScreenBackground(0x000000); // Black background

    // Row 1: Title
    k10.canvas->canvasText("LIGHT SENSOR", 1, 0xFEE715);
    k10.canvas->updateCanvas();
}

void loop() {
    // Read onboard Ambient Light Sensor (0 - 1000+ lux)
    uint16_t lightLevel = k10.readALS();

    // Row 3: Display raw sensor reading
    k10.canvas->canvasText("Light: " + String(lightLevel) + " Lux", 3, 0x00E5FF);

    // Row 5: Display condition description
    if (lightLevel < 50) {
        k10.canvas->canvasText("State: Dark", 5, 0x888888);
    } else if (lightLevel < 300) {
        k10.canvas->canvasText("State: Dim Light", 5, 0xFFA500);
    } else if (lightLevel < 800) {
        k10.canvas->canvasText("State: Normal", 5, 0x00FF00);
    } else {
        k10.canvas->canvasText("State: Very Bright", 5, 0xFFFFFF);
    }

    // Refresh display
    k10.canvas->updateCanvas();
    delay(200);
}
```
