# Beginner 04: LED Breathing Glow

**Category**: Beginner - Fundamentals  
**Target Board**: UNIHIKER K10  
**Difficulty**: Beginner (Level 1)

---

## What This Example Is Of

Demonstrates controlling perceived illumination using pulse-like brightness modulation to create a gentle 'breathing' or standby indicator.

### Expected Behavior
The onboard LEDs smoothly fade from pitch black up to maximum brightness in soft cyan, then fade back down to zero, mimicking natural respiration.

---

## Hardware Requirements

- **Development Board**: UNIHIKER K10 (ESP32-S3)
- **Peripherals Needed**: UNIHIKER K10 onboard 3x WS2812 RGB LEDs.
- **Connection**: USB Type-C cable for power, programming, and Serial Monitor

---

## Detailed Code Explanation

### 1. Dynamic Brightness Scaling
- `k10.rgb->brightness(b)` accepts discrete levels from `0` to `9`.
- The first `for` loop ramps brightness upwards from 0 to 9 in steps of 100ms.
- The second `for` loop ramps brightness downwards from 9 to 0.
- A 400ms pause at the bottom mimics the natural pause between breaths.

---

## Complete Sketch Source Code

```cpp
#include "unihiker_k10.h"

UNIHIKER_K10 k10;

void setup() {
    k10.begin();
}

void loop() {
    uint32_t cyanColor = 0x00FFFF;

    // Fade IN (Brightness 0 -> 9)
    for (int b = 0; b <= 9; b++) {
        k10.rgb->brightness(b);
        k10.rgb->write(-1, cyanColor);
        delay(100);
    }

    // Fade OUT (Brightness 9 -> 0)
    for (int b = 9; b >= 0; b--) {
        k10.rgb->brightness(b);
        k10.rgb->write(-1, cyanColor);
        delay(100);
    }

    // Brief pause at rest
    delay(400);
}
```
