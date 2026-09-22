## What this tutorial is??

This tutorial demonstrates **graphical typography and text layout** on the UNIHIKER K10 color display. 

You will learn how to render static headers, subtitles, multi-line paragraphs, and dynamic real-time numeric counters with precise $(X, Y)$ pixel coordinates, custom font sizes, text wrapping thresholds, and background auto-cleaning.

### Expected Behavior
- The screen initializes with a dark background (`0x101820`).
- A bright gold header (`UNIHIKER K10`) and clean white subtitle (`Hello, World!`) appear at the top in 24px bold font.
- Section tags and multi-line descriptive text appear in 16px font in cyan and silver.
- At the bottom, a live green counter (`Uptime: X s`) updates dynamically every second without flickering or leaving visual text ghosting.

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

void setup() {
  k10.begin();
  k10.initScreen(2); // Screen direction: 2 = Portrait mode (240x320)
  k10.creatCanvas(); // Allocate internal frame buffer in RAM
  k10.setScreenBackground(0x101820); // Set sleek dark background color

  // =========================================================================
  // canvasText API Parameter Guide:
  // k10.canvas->canvasText(text, x, y, color, font, count, autoClean);
  //
  //  1. text      : String / const char* / float to display
  //  2. x         : Horizontal pixel position (0 to 240 in portrait mode)
  //  3. y         : Vertical pixel position (0 to 320 in portrait mode)
  //  4. color     : 24-bit Hex RGB color code (0xRRGGBB)
  //  5. font      : eCNAndENFont24 (24px bold) or eCNAndENFont16 (16px compact)
  //  6. count     : Line wrap threshold (characters per line before wrapping)
  //  7. autoClean : If true, auto-erases background before drawing (no ghosting)
  // =========================================================================

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

  // Guide for the live counter below
  k10.canvas->canvasText("Live autoClean Demo:", 25, 230, 0x94A3B8,
                         k10.canvas->eCNAndENFont16, 20, true);

  // Push the initial canvas buffer to the physical screen
  k10.canvas->updateCanvas();
}

void loop() {
  // Dynamic text demonstration:
  // With autoClean = true, the counter updates cleanly without ghosting or flickering!
  static int seconds = 0;
  seconds++;

  String timeStr = "Uptime: " + String(seconds) + " s";
  k10.canvas->canvasText(timeStr, 25, 260, 0x10B981, k10.canvas->eCNAndENFont24,
                         20, true);
  k10.canvas->updateCanvas();

  delay(1000); // Update once every second
}
```
