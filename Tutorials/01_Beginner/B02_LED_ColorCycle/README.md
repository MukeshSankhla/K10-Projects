# Beginner 02: LED Color Cycle

**Category**: Beginner - Fundamentals  
**Target Board**: UNIHIKER K10  
**Difficulty**: Beginner (Level 1)

---

## What This Example Is Of

Introduces 24-bit hex color representation (`0xRRGGBB`) by sequencing the onboard RGB LEDs through primary and secondary colors.

### Expected Behavior
Cycles through Red, Green, Blue, Yellow, Cyan, Magenta, and White every 800ms.

---

## Hardware Requirements

- **Development Board**: UNIHIKER K10 (ESP32-S3)
- **Peripherals Needed**: UNIHIKER K10 onboard 3x WS2812 RGB LEDs.
- **Connection**: USB Type-C cable for power, programming, and Serial Monitor

---

## Detailed Code Explanation

### 1. Hexadecimal Color Representation
- In 24-bit true color:
  - Top 8 bits (bits 16-23) represent **Red** (`0xFF0000`)
  - Middle 8 bits (bits 8-15) represent **Green** (`0x00FF00`)
  - Lowest 8 bits (bits 0-7) represent **Blue** (`0x0000FF`)
- Combining channels produces secondary colors:
  - Red + Green = `0xFFFF00` (Yellow)
  - Green + Blue = `0x00FFFF` (Cyan)
  - Red + Blue = `0xFF00FF` (Magenta)
  - All channels full = `0xFFFFFF` (White)

### 2. Iteration Loop
- Uses an array loop `for (int i = 0; i < numColors; i++)` to iterate through the color array cleanly with `k10.rgb->write(-1, colors[i])`.

---

## Complete Sketch Source Code

```cpp
#include "unihiker_k10.h"

UNIHIKER_K10 k10;

// Color table: Red, Green, Blue, Yellow, Cyan, Magenta, White
const uint32_t colors[] = {
    0xFF0000, // Red
    0x00FF00, // Green
    0x0000FF, // Blue
    0xFFFF00, // Yellow (Red + Green)
    0x00FFFF, // Cyan (Green + Blue)
    0xFF00FF, // Magenta (Red + Blue)
    0xFFFFFF  // White (Red + Green + Blue)
};
const int numColors = sizeof(colors) / sizeof(colors[0]);

void setup() {
    k10.begin();
    k10.rgb->brightness(6);
}

void loop() {
    for (int i = 0; i < numColors; i++) {
        k10.rgb->write(-1, colors[i]);
        delay(800);
    }
}
```
