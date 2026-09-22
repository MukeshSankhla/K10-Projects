# Beginner 03: LED Rainbow Wave

**Category**: Beginner - Fundamentals  
**Target Board**: UNIHIKER K10  
**Difficulty**: Beginner (Level 1)

---

## What This Example Is Of

Uses mathematical HSV (Hue, Saturation, Value) color space conversion to generate smooth, organic rainbow color transitions across all 3 onboard LEDs.

### Expected Behavior
Creates a continuous flowing rainbow wave where each of the 3 LEDs displays an offset chromatic hue that smoothly cycles through 360 degrees.

---

## Hardware Requirements

- **Development Board**: UNIHIKER K10 (ESP32-S3)
- **Peripherals Needed**: UNIHIKER K10 onboard 3x WS2812 RGB LEDs.
- **Connection**: USB Type-C cable for power, programming, and Serial Monitor

---

## Detailed Code Explanation

### 1. HSV to RGB Math
- RGB color space is unwieldy for smooth fading because changing hues requires altering three separate channels non-linearly.
- The `hsvToRgb()` helper takes Hue (`0° - 360°`), Saturation (`1.0`), and Value/Brightness (`1.0`) and returns a packed `0xRRGGBB` value.

### 2. Spatial Phase Shifting
- In `loop()`, each of the 3 LEDs (`i = 0, 1, 2`) receives a phase offset of `i * 30.0` degrees. This produces a spatial gradient across the 3 LEDs rather than all changing uniformly.
- Incrementing `hue += 2.0` every 20ms yields a smooth 50 FPS visual animation.

---

## Complete Sketch Source Code

```cpp
#include "unihiker_k10.h"

UNIHIKER_K10 k10;

// Helper function to convert HSV (0-360 deg) to 24-bit RGB
uint32_t hsvToRgb(float h, float s, float v) {
    int i = int(h / 60.0) % 6;
    float f = (h / 60.0) - i;
    float p = v * (1.0 - s);
    float q = v * (1.0 - f * s);
    float t = v * (1.0 - (1.0 - f) * s);
    float r = 0, g = 0, b = 0;
    switch (i) {
        case 0: r = v; g = t; b = p; break;
        case 1: r = q; g = v; b = p; break;
        case 2: r = p; g = v; b = t; break;
        case 3: r = p; g = q; b = v; break;
        case 4: r = t; g = p; b = v; break;
        case 5: r = v; g = p; b = q; break;
    }
    return ((uint32_t(r * 255) << 16) | (uint32_t(g * 255) << 8) | uint32_t(b * 255));
}

void setup() {
    k10.begin();
    k10.rgb->brightness(5);
}

void loop() {
    static float hue = 0;
    // Update each LED with a slight phase shift (30 degrees apart)
    for (int i = 0; i < 3; i++) {
        float ledHue = fmod(hue + (i * 30.0), 360.0);
        uint32_t color = hsvToRgb(ledHue, 1.0, 1.0);
        k10.rgb->write(i, color);
    }
    hue += 2.0;
    if (hue >= 360.0) hue = 0;
    delay(20);
}
```
