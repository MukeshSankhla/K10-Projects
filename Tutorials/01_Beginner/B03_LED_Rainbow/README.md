## What this tutorial is??

This tutorial demonstrates how to generate a **fluid, continuous rainbow gradient animation** across multiple RGB LEDs using mathematical color space transformation.

Rather than stepping abruptly between a handful of predefined colors, this sketch calculates colors dynamically along the continuous 360° color wheel using the **HSV (Hue, Saturation, Value)** color model, creating an organic flowing light wave across all three onboard LEDs.

### Expected Behavior
A smooth, flowing rainbow wave traverses across the 3 onboard LEDs. Each LED is slightly offset in phase (by 30 degrees of hue) from its neighbor, creating the visual appearance of a propagating spectral wave that completes a full hue cycle smoothly every 3.6 seconds.

---

## How it works

1. **The Limitation of Direct RGB Animation**:
   - Creating smooth color transitions directly in the RGB color space is difficult because increasing one primary channel while decreasing another often results in non-linear perceptual brightness dips.
2. **The HSV Color Space**:
   - The **HSV model** decouples chromaticity (color tint) from brightness:
     - **Hue ($H$)**: An angle on a circular wheel ($0^\circ$ to $360^\circ$). $0^\circ$ = Red, $60^\circ$ = Yellow, $120^\circ$ = Green, $180^\circ$ = Cyan, $240^\circ$ = Blue, $300^\circ$ = Magenta, $360^\circ$ = Red again.
     - **Saturation ($S$)**: Color purity ($0.0$ = washed out white/grey, $1.0$ = fully saturated, pure color).
     - **Value ($V$)**: Intensity/brightness ($0.0$ = dark black, $1.0$ = maximum intensity).
   - By fixing $S = 1.0$ and $V = 1.0$, varying Hue continuously from $0^\circ$ to $360^\circ$ sweeps effortlessly through every visible rainbow hue at uniform brightness!
3. **HSV to RGB Mathematical Conversion**:
   - The `hsvToRgb()` function divides the 360° color circle into 6 equal $60^\circ$ sextants ($i = 0$ to $5$) and linearly interpolates the intermediate ramp value ($f$).
   - The resulting floating-point $R, G, B$ values ($0.0$ to $1.0$) are scaled to integers ($0$ to $255$) and bit-packed into a standard 24-bit integer:
     $$\text{Color} = (R \ll 16) \mid (G \ll 8) \mid B$$
4. **Spatial Phase Shifting**:
   - In `loop()`, each LED $i$ (indices 0, 1, 2) is assigned an angular offset:
     $$\text{ledHue} = (\text{hue} + i \times 30^\circ) \pmod{360^\circ}$$
   - This creates a travelling spatial gradient across the physical board rather than all LEDs changing simultaneously.

---

## Sensors/actuator detaile

### The Physics of Color Spaces: RGB vs. HSV
- **Hardware Layer (RGB)**:
  Physical display hardware and LED silicon dies operate strictly in RGB mode because they produce light by exciting three distinct phosphor/semiconductor compounds with PWM duty cycles.
- **Human Perception Layer (HSV)**:
  Human vision perceives color in terms of attributes: *Which color is it?* (Hue), *How pure/vivid is it?* (Saturation), and *How bright is it?* (Value).
- By performing mathematical animations in HSV space and converting to RGB at the very last moment before writing to hardware, we achieve perfectly balanced, flicker-free optical transitions.

### Addressable LED Indexing on UNIHIKER K10
- On the UNIHIKER K10, the 3 onboard WS2812 LEDs are positioned horizontally below the screen.
- Unlike `k10.rgb->write(-1, color)` which writes to all pixels globally, passing an explicit index `0`, `1`, or `2` targets a single specific LED in the daisy chain:
  - `0`: Leftmost LED
  - `1`: Middle LED
  - `2`: Rightmost LED
- This enables individual spatial animation and gradient propagation.

---

## Step by step function wise code break down

### 1. Library Inclusion & Global Setup
```cpp
#include "unihiker_k10.h"

UNIHIKER_K10 k10;
```
- Includes K10 board driver and instantiates master controller `k10`.

### 2. The `hsvToRgb()` Conversion Function
```cpp
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
```
- Determines the active $60^\circ$ wedge ($i = 0$ to $5$).
- Calculates intermediary ramp values $p, q, t$.
- Uses `switch(i)` to assign Red, Green, and Blue intensities.
- Bit-shifts the 8-bit channels into a single 24-bit hexadecimal integer `0xRRGGBB`.

### 3. Hardware Initialization in `setup()`
```cpp
void setup() {
    k10.begin();
    k10.rgb->brightness(5);
}
```
- Wakes up power buses and sets moderate brightness level `5`.

### 4. Continuous Animation Loop in `loop()`
```cpp
void loop() {
    static float hue = 0;
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
- `static float hue`: Preserves current global hue position across iterations.
- Loop over `i` (0, 1, 2) applies the $+30^\circ$ spatial phase shift using floating-point modulo `fmod()`.
- `k10.rgb->write(i, color)` writes the calculated color to the respective individual LED.
- `hue += 2.0` advances the angle by $2^\circ$ every 20 milliseconds (50 frames per second).
- Once `hue` reaches $360^\circ$, it wraps back to $0^\circ$ seamlessly.

---

## API and Functions detailes

| API / Method | Parameters | Return Type | Description |
| :--- | :--- | :--- | :--- |
| `k10.begin()` | None | `void` | Initializes board hardware and system peripherals. |
| `k10.rgb->brightness(level)` | `uint8_t level` (`0` to `9`) | `void` | Sets the global brightness multiplier for onboard LEDs. |
| `k10.rgb->write(index, color)` | `int8_t index`, `uint32_t color` | `void` | Writes color to specific pixel index `0`, `1`, or `2`. |
| `hsvToRgb(h, s, v)` | `float h` (0-360), `float s` (0-1), `float v` (0-1) | `uint32_t` | Converts cylindrical HSV coordinates into standard 24-bit RGB format. |
| `fmod(x, y)` | `double x`, `double y` | `double` | C math function returning the floating-point remainder of $x / y$. |
| `delay(ms)` | `unsigned long ms` | `void` | Regulates animation frame timing (20 ms = 50 Hz refresh rate). |

---

## Full Code

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
