# Beginner 09: Display Hello World & `canvasText` API Guide

**Category**: Beginner - Fundamentals  
**Target Board**: UNIHIKER K10  
**Difficulty**: Beginner (Level 1)

---

## What This Example Is Of

Demonstrates the UNIHIKER K10 canvas typography engine, explaining coordinate positioning, font selection, color formatting, line wrapping, and dynamic flicker-free updating using the `canvasText` API.

### Expected Behavior
- Displays a multi-line title and description banner with contrasting colors (`0xFEE715` gold, `0xFFFFFF` white, `0x00E5FF` cyan, and `0xCCCCCC` silver).
- Runs a dynamic **live uptime counter** at the bottom of the screen updating every second, proving how `autoClean = true` keeps numbers crisp without ghosting or flickering.

---

## Hardware Requirements

- **Development Board**: UNIHIKER K10 (ESP32-S3)
- **Peripherals Needed**: UNIHIKER K10 onboard 2.8" ST7789 IPS LCD (240x320 resolution).
- **Connection**: USB Type-C cable for power and programming.

---

## Detailed `canvasText` API Parameter Guide

The primary method for rendering typography on the UNIHIKER K10 canvas is:

```cpp
k10.canvas->canvasText(text, x, y, color, font, count, autoClean);
```

### Parameter Breakdown

| # | Parameter | Type | Example | Description |
|---|---|---|---|---|
| **1** | **`text`** | `const char*` / `String` / `float` | `"embedded projects!"` | **String or Number**: The text content or floating-point number to display. |
| **2** | **`x`** | `int16_t` | `40` | **Horizontal Position (Pixels)**: X-coordinate offset from the left edge. In portrait mode (`dir = 2`), valid range is `0` to `240`. |
| **3** | **`y`** | `int16_t` | `200` | **Vertical Position (Pixels)**: Y-coordinate offset from the top edge. In portrait mode, valid range is `0` to `320`. |
| **4** | **`color`** | `uint32_t` | `0xCCCCCC` | **24-bit Hex RGB Color (`0xRRGGBB`)**: E.g., `0xFFFFFF` (White), `0xFEE715` (Gold), `0x10B981` (Emerald Green). |
| **5** | **`font`** | `eFontSize_t` | `k10.canvas->eCNAndENFont16` | **Font Size / Typeface**: Built-in bitmap font enum (see Font Options below). |
| **6** | **`count`** | `int` | `20` | **Line Wrap Character Count**: Maximum characters per line before the text automatically wraps to the next line. |
| **7** | **`autoClean`** | `bool` | `true` | **Background Auto-Erase**: When `true`, automatically clears the text bounding box with the background color before drawing new glyphs. Prevents overlapping ghosting during dynamic updates. |

---

### Available Built-in Font Enums

Accessed via `k10.canvas->...`:

| Font Enum | Pixel Height | Best Used For |
|---|---|---|
| **`eCNAndENFont24`** | **24px** | Big Titles, headlines, bold sensor numbers |
| **`eCNAndENFont16`** | **16px** | Standard body text, subtitles, descriptions, status logs |
| **`eJapanFont24x24`** | **24px** | Japanese Kanji / Hiragana / Katakana |
| **`eKoreanFont24x24`** | **24px** | Korean Hangul characters |
| **`eGreeceFont12x24`** | **24px (12w)** | Greek mathematical & scientific symbols |

---

### Why `autoClean = true` is Essential for Dynamic Text

When updating text inside `loop()` (such as a clock, pedometer steps, or sensor telemetry):
- **`autoClean = false`**: The new characters are drawn directly over previous characters without erasing the pixels underneath, creating a messy, unreadable "ghosting" artifact.
- **`autoClean = true`**: The canvas driver automatically draws a background-colored rectangle over the text bounding box right before rendering the new text, giving you **clean, flicker-free updates** without needing to clear the whole screen!

---

## Complete Sketch Source Code

```cpp
#include "unihiker_k10.h"

UNIHIKER_K10 k10;

void setup() {
    k10.begin();
    k10.initScreen(2);               // Screen direction: 2 = Portrait mode (240x320)
    k10.creatCanvas();               // Allocate internal frame buffer in RAM
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
    k10.canvas->canvasText("UNIHIKER K10", 40, 35, 0xFEE715, k10.canvas->eCNAndENFont24, 20, true);

    // Subtitle in crisp white (24px font)
    k10.canvas->canvasText("Hello, World!", 40, 65, 0xFFFFFF, k10.canvas->eCNAndENFont24, 20, true);

    // Section label in vibrant cyan (16px font)
    k10.canvas->canvasText("Level 1: Beginner", 25, 120, 0x00E5FF, k10.canvas->eCNAndENFont16, 20, true);

    // Multi-line description text in soft silver
    k10.canvas->canvasText("Ready to create amazing", 25, 150, 0xCCCCCC, k10.canvas->eCNAndENFont16, 25, true);
    k10.canvas->canvasText("embedded projects!", 25, 175, 0xCCCCCC, k10.canvas->eCNAndENFont16, 20, true);

    // Guide for the live counter below
    k10.canvas->canvasText("Live autoClean Demo:", 25, 230, 0x94A3B8, k10.canvas->eCNAndENFont16, 20, true);

    // Push the initial canvas buffer to the physical screen
    k10.canvas->updateCanvas();
}

void loop() {
    // Dynamic text demonstration:
    // With autoClean = true, the counter updates cleanly without ghosting or flickering!
    static int seconds = 0;
    seconds++;

    String timeStr = "Uptime: " + String(seconds) + " s";
    k10.canvas->canvasText(timeStr, 25, 260, 0x10B981, k10.canvas->eCNAndENFont24, 20, true);
    k10.canvas->updateCanvas();

    delay(1000); // Update once every second
}
```
