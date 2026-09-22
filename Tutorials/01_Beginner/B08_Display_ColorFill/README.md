# Beginner 08: Display Color Fill

**Category**: Beginner - Fundamentals  
**Target Board**: UNIHIKER K10  
**Difficulty**: Beginner (Level 1)

---

## What This Example Is Of

Initializes the 2.8-inch ST7789 IPS LCD controller and demonstrates whole-frame background color filling.

### Expected Behavior
Cycles the full screen background through Teal, Amber, Purple, and Navy every 1.5 seconds.

---

## Hardware Requirements

- **Development Board**: UNIHIKER K10 (ESP32-S3)
- **Peripherals Needed**: UNIHIKER K10 onboard 2.8" ST7789 color display (240x320 resolution).
- **Connection**: USB Type-C cable for power, programming, and Serial Monitor

---

## Detailed Code Explanation

### 1. Screen Pipeline
- `k10.initScreen(dir)`: Powers up the SPI display bus and sets screen orientation (`2` = standard upright portrait).
- `k10.creatCanvas()`: Initializes the frame buffer.
- `k10.setScreenBackground(color)`: Fills all 76,800 pixels with the chosen 24-bit color.

---

## Complete Sketch Source Code

```cpp
#include "unihiker_k10.h"

UNIHIKER_K10 k10;

void setup() {
    k10.begin();
    k10.initScreen(2); // Portrait orientation
    k10.creatCanvas();
}

void loop() {
    k10.setScreenBackground(0x008080); // Teal
    delay(1500);

    k10.setScreenBackground(0xFFBF00); // Amber
    delay(1500);

    k10.setScreenBackground(0x800080); // Purple
    delay(1500);

    k10.setScreenBackground(0x000080); // Navy
    delay(1500);
}
```
