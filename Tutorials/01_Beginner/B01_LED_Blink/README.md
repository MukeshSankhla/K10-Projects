# Beginner 01: LED Blink

**Category**: Beginner - Fundamentals  
**Target Board**: UNIHIKER K10  
**Difficulty**: Beginner (Level 1)

---

## What This Example Is Of

The embedded 'Hello World'. Demonstrates how to initialize the UNIHIKER K10 onboard WS2812 addressable RGB LEDs and blink them synchronously at regular intervals.

### Expected Behavior
All 3 onboard RGB LEDs will illuminate simultaneously in bright red for 1 second, turn completely off for 1 second, and repeat indefinitely.

---

## Hardware Requirements

- **Development Board**: UNIHIKER K10 (ESP32-S3)
- **Peripherals Needed**: UNIHIKER K10 onboard 3x WS2812 RGB LEDs. No external wiring required.
- **Connection**: USB Type-C cable for power, programming, and Serial Monitor

---

## Detailed Code Explanation

### 1. Library Inclusion & Initialization
- `#include "unihiker_k10.h"`: Includes the primary UNIHIKER K10 board control library.
- `UNIHIKER_K10 k10;`: Instantiates the global K10 driver object.
- `k10.begin()`: Initializes power rails, system clocks, and I2C buses.
- `k10.rgb->brightness(5)`: Configures the global brightness level for all WS2812 LEDs on a scale from `0` (off) to `9` (maximum intensity). Setting it to `5` provides vivid illumination without excessive glare.

### 2. Blinking Logic in `loop()`
- `k10.rgb->write(index, color)`:
  - An `index` of `-1` applies the color to all three onboard LEDs at once. Passing `0`, `1`, or `2` targets a single specific LED.
  - `0xFF0000`: 24-bit hexadecimal RGB color for saturated Red.
  - `0x000000`: Turns off all color channels (black).
- `delay(1000)`: Freezes execution for 1000 milliseconds (1 second) to create the blinking interval.

---

## Complete Sketch Source Code

```cpp
#include "unihiker_k10.h"

UNIHIKER_K10 k10;

void setup() {
    // Initialize K10 hardware and power management
    k10.begin();
    // Set brightness to a comfortable level (scale 0-9)
    k10.rgb->brightness(5);
}

void loop() {
    // Turn all 3 LEDs (-1 index) Red (0xFF0000)
    k10.rgb->write(-1, 0xFF0000);
    delay(1000);

    // Turn all 3 LEDs off (0x000000)
    k10.rgb->write(-1, 0x000000);
    delay(1000);
}
```
