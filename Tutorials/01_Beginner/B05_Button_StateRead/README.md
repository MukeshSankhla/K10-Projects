# Beginner 05: Momentary Button LED Control

**Category**: Beginner - Fundamentals  
**Target Board**: UNIHIKER K10  
**Difficulty**: Beginner (Level 1)

---

## What This Example Is Of

Demonstrates momentary tactile button input polling to control the onboard WS2812 RGB LEDs without any display overhead.

### Expected Behavior
- **Press & Hold Button A**: All onboard RGB LEDs glow **Green** (`0x00FF00`).
- **Press & Hold Button B**: All onboard RGB LEDs glow **Blue** (`0x0000FF`).
- **Release Button**: The RGB LEDs turn **OFF** (`0x000000`) immediately when released.

---

## Hardware Requirements

- **Development Board**: UNIHIKER K10 (ESP32-S3)
- **Peripherals Needed**: UNIHIKER K10 onboard Buttons A and B, onboard 3x WS2812 RGB LEDs.
- **Connection**: USB Type-C cable for power and programming.

---

## Detailed Code Explanation

### 1. RGB Brightness Initialization
- `k10.rgb->brightness(5)` sets a balanced LED illumination level.
- `k10.rgb->write(-1, 0x000000)` initializes all 3 LEDs to the OFF state.

### 2. Real-Time Momentary Polling
- `k10.buttonA->isPressed()` returns `true` continuously as long as Button A is held down.
- `k10.buttonB->isPressed()` returns `true` continuously as long as Button B is held down.
- The `else` block activates the moment fingers release the switches, resetting the LEDs to `0x000000`.
- A 20ms delay in `loop()` provides switch debounce and prevents unnecessary CPU load.

---

## Complete Sketch Source Code

```cpp
#include "unihiker_k10.h"

UNIHIKER_K10 k10;

void setup() {
    k10.begin();
    k10.rgb->brightness(5);
    k10.rgb->write(-1, 0x000000); // Start with LEDs OFF
}

void loop() {
    // When Button A is held, glow LEDs Green
    if (k10.buttonA->isPressed()) {
        k10.rgb->write(-1, 0x00FF00); // Green
    }
    // When Button B is held, glow LEDs Blue
    else if (k10.buttonB->isPressed()) {
        k10.rgb->write(-1, 0x0000FF); // Blue
    }
    // On release (neither button pressed), turn off LEDs
    else {
        k10.rgb->write(-1, 0x000000); // OFF
    }

    delay(20); // Small debounce delay
}
```
