## What this tutorial is??

This tutorial demonstrates how to create a calm, organic **"breathing" or "pulsing" light effect** using brightness modulation on the onboard addressable RGB LEDs.

Breathing animations are widely used in modern consumer electronics (laptops, smart speakers, charging docks) to indicate standby mode, system health, or background processing without appearing harsh or alarming.

### Expected Behavior
All 3 onboard RGB LEDs illuminate in cyan (`0x00FFFF`) and smoothly ramp up in intensity from complete darkness to peak brightness over ~1 second, ramp back down to darkness over ~1 second, pause briefly for 400 milliseconds at rest, and repeat continuously like a gentle breath.

---

## How it works

1. **Pulse-Width Modulation (PWM)**:
   - Digital microcontroller pins cannot directly output an analog voltage like 1.5V or 2.3V—they can only switch between 0V (LOW) and 3.3V (HIGH).
   - To make an LED appear dimmer or brighter, microcontrollers use **Pulse-Width Modulation (PWM)**, rapidly switching the power on and off thousands of times per second.
   - The ratio of "ON" time to total cycle time is called the **Duty Cycle**:
     - **0% Duty Cycle**: Always OFF $\rightarrow$ 0% brightness
     - **50% Duty Cycle**: ON half the time, OFF half the time $\rightarrow$ perceived medium brightness
     - **100% Duty Cycle**: Always ON $\rightarrow$ maximum brightness
   - Because the switching occurs far faster than human visual persistence of vision (~60 Hz), our eyes integrate the pulses into a steady, dimmed light level.
2. **K10 Brightness Scaling (0 to 9)**:
   - The UNIHIKER K10 driver abstracts the 8-bit WS2812 PWM duty cycle into 10 discrete user-friendly brightness levels from `0` (off) to `9` (maximum safe intensity).
3. **Ascending and Descending Loops**:
   - The sketch first runs a `for` loop incrementing brightness $b$ from `0` up to `9` in 100 ms steps (fade in).
   - It then runs a second `for` loop decrementing $b$ from `9` down to `0` in 100 ms steps (fade out).
   - An additional 400 ms resting delay at the bottom mimics the natural exhalation pause of human breathing.

---

## Sensors/actuator detaile

### Human Visual Perception & Brightness Linearity
- The human eye perceives light intensity **logarithmically**, not linearly (governed by the **Weber-Fechner Law** and Stevens' Power Law).
- In low-light conditions, small changes in electrical power produce large noticeable changes in perceived brightness. In bright conditions, substantial power increases are barely perceptible.
- Stepping through calibrated brightness curves produces a pleasing, natural glow that feels comfortable to the human eye.

### Onboard WS2812 PWM Engine
- Inside each WS2812 LED package, an onboard digital state machine drives three internal constant-current PWM circuits operating at a frequency of approximately **1.2 kHz**.
- When `k10.rgb->brightness(b)` is called, the library scales the 24-bit color channel values ($R, G, B$) proportionally before shifting the bits over GPIO 46, ensuring consistent color temperature regardless of brightness level.

---

## Step by step function wise code break down

### 1. Library Inclusion & Setup
```cpp
#include "unihiker_k10.h"

UNIHIKER_K10 k10;

void setup() {
    k10.begin();
}
```
- `#include "unihiker_k10.h"`: Loads board definitions.
- `k10.begin()`: Initializes system hardware. Note that initial brightness is not set in `setup()` because it will be dynamically modulated inside `loop()`.

### 2. Fade IN Phase (Inhalation)
```cpp
void loop() {
    uint32_t cyanColor = 0x00FFFF;

    for (int b = 0; b <= 9; b++) {
        k10.rgb->brightness(b);
        k10.rgb->write(-1, cyanColor);
        delay(100);
    }
```
- Defines the target color as cyan (`0x00FFFF`).
- The `for` loop counts upward from `b = 0` to `b = 9`.
- `k10.rgb->brightness(b)` sets the new duty cycle scaling.
- `k10.rgb->write(-1, cyanColor)` reapplies the scaled color to all 3 LEDs.
- `delay(100)` holds each brightness step for 100 ms ($10 \text{ steps} \times 100\text{ ms} = 1000\text{ ms}$ total fade-in duration).

### 3. Fade OUT Phase (Exhalation) & Rest
```cpp
    for (int b = 9; b >= 0; b--) {
        k10.rgb->brightness(b);
        k10.rgb->write(-1, cyanColor);
        delay(100);
    }

    delay(400);
}
```
- The second `for` loop decrements `b` from `9` down to `0`, gently fading the LEDs to black.
- `delay(400)` adds a 400 ms resting pause before starting the next breath.

---

## API and Functions detailes

| API / Method | Parameters | Return Type | Description |
| :--- | :--- | :--- | :--- |
| `k10.begin()` | None | `void` | Initializes hardware timers, power channels, and buses. |
| `k10.rgb->brightness(level)` | `uint8_t level` (`0` to `9`) | `void` | Sets the global brightness scaling multiplier for the onboard LEDs. |
| `k10.rgb->write(index, color)` | `int8_t index`, `uint32_t color` | `void` | Writes 24-bit Hex RGB color to LED index (`-1` = all 3 LEDs). |
| `delay(ms)` | `unsigned long ms` | `void` | Pauses execution for the specified milliseconds to establish the fade cadence. |

---

## Full Code

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
