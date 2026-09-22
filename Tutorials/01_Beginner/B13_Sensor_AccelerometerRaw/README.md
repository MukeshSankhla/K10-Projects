# Beginner 13: 6-Axis Accelerometer (IMU) Display

**Category**: Beginner - Fundamentals  
**Target Board**: UNIHIKER K10  
**Difficulty**: Beginner (Level 1)

---

## What This Example Is Of

Reads raw 3-axis acceleration vectors (X, Y, Z) from the onboard SC7A20 IMU sensor and displays the live values directly on the screen as simple, clean text.

### Expected Behavior
- Displays simple text lines on the screen:
  - **Row 1**: Title (`6-AXIS ACCEL`).
  - **Row 3**: Lateral tilt force (`Accel X: <val>`).
  - **Row 5**: Pitch / Forward-Backward force (`Accel Y: <val>`).
  - **Row 7**: Vertical gravitational force (`Accel Z: <val>`).

---

## Hardware Requirements

- **Development Board**: UNIHIKER K10 (ESP32-S3)
- **Peripherals Needed**: UNIHIKER K10 onboard 6-axis IMU, 2.8" LCD screen.
- **Connection**: USB Type-C cable for power and programming.

---

## Detailed Code Explanation

### 1. Simple Row-Based Text API
- `canvasText(text, row, color)`:
  - `text`: String or integer to show.
  - `row`: Screen text row number (`1` to `13`). The library automatically clears and updates each row cleanly.
  - `color`: 24-bit Hex RGB color code.
- `k10.getAccelerometerX()` / `Y()` / `Z()`: Reads raw acceleration in milli-gravities (`mG`).

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
    k10.canvas->canvasText("6-AXIS ACCEL", 1, 0xFEE715);
    k10.canvas->updateCanvas();
}

void loop() {
    // Read raw 3-axis acceleration in milli-gravities (mG)
    int ax = k10.getAccelerometerX();
    int ay = k10.getAccelerometerY();
    int az = k10.getAccelerometerZ();

    // Row 3: X-axis acceleration
    k10.canvas->canvasText("Accel X: " + String(ax), 3, 0xFF5555);

    // Row 5: Y-axis acceleration
    k10.canvas->canvasText("Accel Y: " + String(ay), 5, 0x00FF00);

    // Row 7: Z-axis acceleration
    k10.canvas->canvasText("Accel Z: " + String(az), 7, 0x00E5FF);

    // Refresh display
    k10.canvas->updateCanvas();
    delay(100);
}
```
