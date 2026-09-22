## What this tutorial is??

This tutorial demonstrates **additive color synthesis** and data-driven programming using the onboard WS2812 addressable RGB LEDs. 

Instead of hardcoding individual color values, this project stores an array of distinct primary, secondary, and tertiary colors and loops through them sequentially with consistent timing intervals.

### Expected Behavior
All 3 onboard RGB LEDs cycle continuously through 7 vibrant colors in repeating sequence:
1. **Red** (`0xFF0000`)
2. **Green** (`0x00FF00`)
3. **Blue** (`0x0000FF`)
4. **Yellow** (`0xFFFF00`)
5. **Cyan** (`0x00FFFF`)
6. **Magenta** (`0xFF00FF`)
7. **White** (`0xFFFFFF`)  
Each color is displayed for 800 milliseconds before transitioning to the next.

---

## How it works

1. **Additive Color Mixing (RGB)**:
   - Inside each WS2812 LED package are three microscopic LED dies emitting the three primary colors of light: **Red** (~620 nm), **Green** (~525 nm), and **Blue** (~465 nm).
   - By mixing these three light channels at full intensity in different combinations, secondary colors are produced:
     - **Red + Green = Yellow**
     - **Green + Blue = Cyan**
     - **Red + Blue = Magenta**
     - **Red + Green + Blue = White**
2. **24-Bit Hexadecimal Color Representation**:
   - Each color is encoded as a 32-bit unsigned integer (`uint32_t`) formatted in hexadecimal: `0xRRGGBB`.
   - Bits 23-16 represent the **Red** channel (0-255).
   - Bits 15-8 represent the **Green** channel (0-255).
   - Bits 7-0 represent the **Blue** channel (0-255).
3. **Array Traversal**:
   - An array of color values `colors[]` is defined in memory.
   - The constant `numColors` dynamically calculates the array size using the formula `sizeof(colors) / sizeof(colors[0])`.
   - A `for` loop in `loop()` iterates from index `0` to `numColors - 1`, writing each color value to the LED driver using `k10.rgb->write(-1, colors[i])` followed by an 800 ms delay.

---

## Sensors/actuator detaile

### What is an RGB LED?
An **RGB LED** combines three individual light-emitting diodes (Red, Green, and Blue) into a single compact housing.

### Additive vs. Subtractive Color Theory
- **Subtractive Colors (Pigments / Ink / Paints)**: Uses Cyan, Magenta, and Yellow. Mixing all colors absorbs (subtracts) light, producing muddy dark brown/black.
- **Additive Colors (Emitted Light / Screens / LEDs)**: Starts from darkness (black). Adding primary wavelengths together increases light energy until all three together produce pure white light.

| Color Name | Red Channel | Green Channel | Blue Channel | Hex Code (`0xRRGGBB`) |
| :--- | :--- | :--- | :--- | :--- |
| **Black (Off)** | `00` (0) | `00` (0) | `00` (0) | `0x000000` |
| **Red** | `FF` (255) | `00` (0) | `00` (0) | `0xFF0000` |
| **Green** | `00` (0) | `FF` (255) | `00` (0) | `0x00FF00` |
| **Blue** | `00` (0) | `00` (0) | `FF` (255) | `0x0000FF` |
| **Yellow** | `FF` (255) | `FF` (255) | `00` (0) | `0xFFFF00` |
| **Cyan** | `00` (0) | `FF` (255) | `FF` (255) | `0x00FFFF` |
| **Magenta** | `FF` (255) | `00` (0) | `FF` (255) | `0xFF00FF` |
| **White** | `FF` (255) | `FF` (255) | `FF` (255) | `0xFFFFFF` |

### Discrete 4-Pin RGB LEDs vs. Smart Digital LEDs
1. **Traditional 4-Pin Discrete RGB LEDs**:
   - Have 4 physical leads: one per color channel (R, G, B) plus one common terminal.
   - **Common Anode**: One shared long positive lead connected to VCC; individual colors are grounded to light up.
   - **Common Cathode**: One shared long negative lead connected to GND; individual colors receive positive voltage to light up.
   - Requires 3 separate GPIO pins and 3 distinct current-limiting resistors.
2. **UNIHIKER K10 Integrated WS2812 Smart LEDs**:
   - On the K10 board, all 3 RGB LEDs are connected in a **single-wire digital chain** on GPIO 46.
   - Each chip has an internal 24-bit shift register and constant-current PWM driver, providing 256 brightness levels per color ($256 \times 256 \times 256 = 16.7\text{ million colors}$) without consuming multiple microcontroller pins.

---

## Step by step function wise code break down

### 1. Library Inclusion & Color Lookup Table
```cpp
#include "unihiker_k10.h"

UNIHIKER_K10 k10;

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
```
- `colors[]`: An array of 32-bit unsigned integers storing the hex color codes.
- `sizeof(colors)`: Total memory footprint of the array in bytes ($7 \times 4 = 28$ bytes).
- `sizeof(colors[0])`: Size of a single element (4 bytes).
- Dividing them gives `7`, ensuring the loop automatically adapts if new colors are added.

### 2. Hardware Initialization in `setup()`
```cpp
void setup() {
    k10.begin();
    k10.rgb->brightness(6);
}
```
- `k10.begin()`: Initializes system hardware and clocks.
- `k10.rgb->brightness(6)`: Configures medium-high brightness (`6` on 0-9 scale), ensuring pure white and mixed colors render vividly without overheating the LEDs.

### 3. Iterative Sequence in `loop()`
```cpp
void loop() {
    for (int i = 0; i < numColors; i++) {
        k10.rgb->write(-1, colors[i]);
        delay(800);
    }
}
```
- The `for` loop cycles through indices `0` to `6`.
- `k10.rgb->write(-1, colors[i])` updates all 3 LEDs to color `colors[i]`.
- `delay(800)` pauses for 800 ms per step before repeating.

---

## API and Functions detailes

| API / Method | Parameters | Return Type | Description |
| :--- | :--- | :--- | :--- |
| `k10.begin()` | None | `void` | Initializes power supply rails and peripheral bus controllers. |
| `k10.rgb->brightness(level)` | `uint8_t level` (`0` to `9`) | `void` | Configures the master brightness multiplier for onboard LEDs. |
| `k10.rgb->write(index, color)` | `int8_t index`, `uint32_t color` | `void` | Writes a 24-bit Hex RGB color code to the specified LED (`-1` = all LEDs). |
| `sizeof(array)` | Array identifier | `size_t` | Compile-time operator returning total byte size of an array. |
| `delay(ms)` | `unsigned long ms` | `void` | Pauses execution for the given duration in milliseconds. |

---

## Full Code

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
