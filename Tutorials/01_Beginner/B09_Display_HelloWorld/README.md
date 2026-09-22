## What this tutorial is??

This tutorial demonstrates typography and structured embedded GUI card layout on the UNIHIKER K10 using the **Modern Indigo Slate** theme.

You will learn how to budget text sizes to prevent screen overflow, center headers mathematically, render container cards with high contrast, and implement flicker-free dynamic live counter updates using selective canvas redraws.

### Expected Behavior
- **Indigo Slate Banner**: Centered header ("UNIHIKER K10") and divider bar.
- **Welcome & Overview Cards**: Crisp 24px and 16px metadata detailing resolution (240x320) and color depth.
- **Live Heartbeat Card**: An active uptime counter updating every second without screen-wipe flicker.

---

## How it works

1. **Pixel Coordinate System**:
   - In Portrait mode ($240 \times 320$), the coordinate origin $(0, 0)$ is situated at the **top-left corner**:
     - **X-axis**: Extends horizontally from `0` (left edge) to `240` (right edge).
     - **Y-axis**: Extends vertically from `0` (top edge) to `320` (bottom edge).
2. **Font Glyph Rasterization**:
   - Microcontrollers do not use heavy TrueType/OpenType vector font engines due to memory constraints.
   - Instead, the K10 firmware incorporates pre-compiled bitmap font tables:
     - `eCNAndENFont24`: 24-pixel height font ideal for prominent titles and counters.
     - `eCNAndENFont16`: 16-pixel height font optimized for dense body text and labels.
3. **The `autoClean` Feature (Preventing Ghosting)**:
   - In traditional frame buffers, printing "10" over "09" leaves residual pixels if the background is not erased first, resulting in illegible overlapping artifacts ("ghosting").
   - By enabling `autoClean = true` in `canvasText()`, the canvas automatically recalculates the text bounding box and clears previous background pixels before drawing new characters.
4. **Buffer Synchronization (`updateCanvas`)**:
   - Multiple text drawing commands are executed in RAM without immediate screen flicker. Calling `k10.canvas->updateCanvas()` pushes the entire modified buffer to the ST7789 display controller in a single high-speed SPI transfer.

---

## Sensors/actuator detaile

### Embedded Font Engines: How Letters Appear on Glass
- A computer font glyph is fundamentally a 2D matrix of binary bits (1s and 0s).
- For a 16px font, a character like capital 'A' is stored in memory as an array of 16 bytes:
  - Each `1` bit instructs the rasterizer to paint a pixel in the foreground text color (`0xFFFFFF`).
  - Each `0` bit represents transparent background.
- When `canvasText()` is invoked, the library loops through the ASCII string, matches each character code to its bitmap table, shifts the cursor rightward by the character width, and handles automatic line wrapping when the character count exceeds the line limit (`count`).

### Why `autoClean` is Critical in Embedded UIs
- Without partial redraws or autoClean, developers are forced to wipe the entire screen (`canvasClear()`) every time a number changes.
- Full-screen wipes over SPI cause severe screen flicker, eye fatigue, and slow down execution loops.
- `autoClean` updates only the exact rectangular bounding box enclosing the text, yielding professional, 60-fps-smooth user interfaces.

---

## Step by step function wise code break down

### 1. Library Inclusion & Global Setup
```cpp
#include "unihiker_k10.h"

UNIHIKER_K10 k10;
```
- Includes K10 graphics libraries and instantiates master controller `k10`.

### 2. Static Layout Setup in `setup()`
```cpp
void setup() {
  k10.begin();
  k10.initScreen(2); // Portrait mode (240x320)
  k10.creatCanvas(); // Allocate frame buffer
  k10.setScreenBackground(0x101820); // Dark theme background
```
- Powers on the display, configures 240x320 portrait mode, allocates RAM canvas, and fills the screen with dark slate `0x101820`.

```cpp
  // Header in bright gold (24px font)
  k10.canvas->canvasText("UNIHIKER K10", 40, 35, 0xFEE715,
                         k10.canvas->eCNAndENFont24, 20, true);

  // Subtitle in crisp white (24px font)
  k10.canvas->canvasText("Hello, World!", 40, 65, 0xFFFFFF,
                         k10.canvas->eCNAndENFont24, 20, true);

  // Section label in vibrant cyan (16px font)
  k10.canvas->canvasText("Level 1: Beginner", 25, 120, 0x00E5FF,
                         k10.canvas->eCNAndENFont16, 20, true);

  // Multi-line description text in soft silver
  k10.canvas->canvasText("Ready to create amazing", 25, 150, 0xCCCCCC,
                         k10.canvas->eCNAndENFont16, 25, true);
  k10.canvas->canvasText("embedded projects!", 25, 175, 0xCCCCCC,
                         k10.canvas->eCNAndENFont16, 20, true);

  k10.canvas->canvasText("Live autoClean Demo:", 25, 230, 0x94A3B8,
                         k10.canvas->eCNAndENFont16, 20, true);

  k10.canvas->updateCanvas();
}
```
- Places static labels using exact $(X, Y)$ pixel coordinates.
- Pushes initial static graphics to the screen via `k10.canvas->updateCanvas()`.

### 3. Dynamic Real-Time Uptime Counter in `loop()`
```cpp
void loop() {
  static int seconds = 0;
  seconds++;

  String timeStr = "Uptime: " + String(seconds) + " s";
  k10.canvas->canvasText(timeStr, 25, 260, 0x10B981, k10.canvas->eCNAndENFont24,
                         20, true);
  k10.canvas->updateCanvas();

  delay(1000);
}
```
- Increments `seconds` every iteration.
- Constructs the dynamic label `timeStr`.
- Passes `true` for the `autoClean` parameter so previous numerals are automatically overwritten cleanly without ghosting.
- Calls `updateCanvas()` and pauses for 1000 ms.

---

## API and Functions detailes

| API / Method | Parameters | Return Type | Description |
| :--- | :--- | :--- | :--- |
| `k10.initScreen(dir)` | `int dir` (`2` = Portrait) | `void` | Initializes ST7789 display controller orientation. |
| `k10.creatCanvas()` | None | `void` | Creates graphics drawing buffer in microcontroller SRAM. |
| `k10.setScreenBackground(color)` | `uint32_t color` | `void` | Sets the ambient screen background color. |
| `k10.canvas->canvasText(...)` | `text, x, y, color, font, count, autoClean` | `void` | Advanced text rendering with coordinate precision, font selection, line wrapping, and background auto-cleaning. |
| `k10.canvas->updateCanvas()` | None | `void` | Flushes dirty canvas buffer regions to the physical display panel. |

---

## Full Code

```cpp
#include "unihiker_k10.h"

UNIHIKER_K10 k10;
uint8_t screen_dir = 2; // Portrait orientation (240x320)

void setup() {
    k10.begin();
    k10.initScreen(screen_dir);
    k10.creatCanvas();
    // Modern Indigo Dark theme background
    k10.setScreenBackground(0x0F172A);

    // 1. Header Banner
    k10.canvas->canvasRectangle(0, 0, 240, 44, 0x1E1B4B, 0x1E1B4B, true);
    k10.canvas->canvasLine(0, 44, 240, 44, 0x6366F1);
    k10.canvas->canvasText("UNIHIKER K10", 42, 10, 0xFEE715,
                           k10.canvas->eCNAndENFont24, 15, false);

    // 2. Welcome Card
    k10.canvas->canvasRectangle(14, 56, 212, 60, 0x6366F1, 0x1E293B, true);
    k10.canvas->canvasText("Hello, World!", 26, 68, 0xF8FAFC,
                           k10.canvas->eCNAndENFont24, 15, false);
    k10.canvas->canvasText("Embedded GUI Architecture", 26, 96, 0x818CF8,
                           k10.canvas->eCNAndENFont16, 26, false);

    // 3. Feature Overview Card
    k10.canvas->canvasRectangle(14, 126, 212, 80, 0x334155, 0x1E293B, true);
    k10.canvas->canvasText("Resolution: 240 x 320 px", 24, 136, 0x00E5FF,
                           k10.canvas->eCNAndENFont16, 24, false);
    k10.canvas->canvasText("Color Depth: 24-bit RGB", 24, 158, 0x94A3B8,
                           k10.canvas->eCNAndENFont16, 24, false);
    k10.canvas->canvasText("Buffer: RAM Frame Canvas", 24, 180, 0x94A3B8,
                           k10.canvas->eCNAndENFont16, 24, false);

    // 4. Live Telemetry Card Base
    k10.canvas->canvasRectangle(14, 216, 212, 54, 0x10B981, 0x064E3B, true);
    k10.canvas->canvasText("Live Heartbeat Telemetry", 24, 224, 0xA7F3D0,
                           k10.canvas->eCNAndENFont16, 24, false);

    // 5. Centered Footer Bar
    k10.canvas->canvasLine(15, 282, 225, 282, 0x334155);
    k10.canvas->canvasText("Ready for Development", 36, 294, 0x64748B,
                           k10.canvas->eCNAndENFont16, 24, false);

    k10.canvas->updateCanvas();
}

void loop() {
    static int seconds = 0;
    seconds++;

    // Selective redraw of the dynamic counter card (eliminates autoClean full-screen wipe bug)
    k10.canvas->canvasRectangle(24, 244, 192, 20, 0x064E3B, 0x064E3B, true);

    String timeStr = "Uptime: " + String(seconds) + " s";
    k10.canvas->canvasText(timeStr, 24, 246, 0x34D399,
                           k10.canvas->eCNAndENFont16, 20, false);

    k10.canvas->updateCanvas();
    delay(1000); // 1-second cadence
}
```
