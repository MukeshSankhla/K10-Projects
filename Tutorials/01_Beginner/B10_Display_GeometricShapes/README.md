# Beginner 10: 2D Geometric Vector Shapes & Drawing API Guide

**Category**: Beginner - Fundamentals  
**Target Board**: UNIHIKER K10  
**Difficulty**: Beginner (Level 1)

---

## What This Example Is Of

Introduces the UNIHIKER K10 2D vector graphics drawing primitives: diagonal lines, filled and hollow bordered rectangles, concentric solid and hollow circles, and points. Explains every parameter, color format, coordinate space, and stroke width control.

### Expected Behavior
Renders a geometric vector composition on a dark space background (`0x050515`):
- Intersecting diagonal background lines (`0x333366`).
- A filled rounded rectangle with a vibrant cyan border (`0x00FFFF`) and dark blue interior (`0x003366`).
- Concentric circle pair: An outer hollow neon-pink ring (`0xFF007F`) enclosing a solid bright yellow core (`0xFFFF00`).
- A bottom hollow green bounding box (`0x00FF88`).

---

## Hardware Requirements

- **Development Board**: UNIHIKER K10 (ESP32-S3)
- **Peripherals Needed**: UNIHIKER K10 onboard 2.8" ST7789 IPS LCD (240x320 resolution).
- **Connection**: USB Type-C cable for power and programming.

---

## Detailed 2D Drawing API & Parameter Reference

### 1. `canvasRectangle` — Drawing Rectangles & Bounding Boxes

```cpp
k10.canvas->canvasRectangle(x, y, w, h, color, bg_color, fill);
```

| # | Parameter | Type | Example | Description |
|---|---|---|---|---|
| **1** | **`x`** | `int` | `20` | **Left X-Coordinate**: Pixel offset of the rectangle's top-left corner from the screen's left edge (`0` to `240`). |
| **2** | **`y`** | `int` | `30` | **Top Y-Coordinate**: Pixel offset of the rectangle's top-left corner from the screen's top edge (`0` to `320`). |
| **3** | **`w`** | `int` | `200` | **Width**: Horizontal span of the rectangle in pixels. |
| **4** | **`h`** | `int` | `70` | **Height**: Vertical span of the rectangle in pixels. |
| **5** | **`color`** | `uint32_t` | `0x00FFFF` | **Border / Outline Color**: 24-bit Hex RGB (`0xRRGGBB`) defining the color of the rectangle's outer perimeter. |
| **6** | **`bg_color`** | `uint32_t` | `0x003366` | **Fill / Interior Color**: 24-bit Hex RGB defining the color inside the rectangle (used when `fill = true`). |
| **7** | **`fill`** | `bool` | `true` / `false` | **Fill Switch**: <br>• `true`: Fills the interior with `bg_color` and outlines with `color`. <br>• `false`: Leaves interior transparent (`LV_OPA_TRANSP`), drawing **only** the border outline. |

---

### 2. `canvasCircle` — Drawing Rings & Solid Circles

```cpp
k10.canvas->canvasCircle(x, y, r, color, bg_color, fill);
```

| # | Parameter | Type | Example | Description |
|---|---|---|---|---|
| **1** | **`x`** | `int` | `120` | **Center X-Coordinate**: Horizontal position of the circle's center point. |
| **2** | **`y`** | `int` | `160` | **Center Y-Coordinate**: Vertical position of the circle's center point. |
| **3** | **`r`** | `int` | `45` | **Radius (pixels)**: Distance from center to the outer perimeter. |
| **4** | **`color`** | `uint32_t` | `0xFF007F` | **Border / Outline Color**: 24-bit Hex RGB for the circumference edge. |
| **5** | **`bg_color`** | `uint32_t` | `0xFFFF00` | **Fill / Interior Color**: 24-bit Hex RGB used to paint the circle's interior disk. |
| **6** | **`fill`** | `bool` | `true` / `false` | **Fill Switch**: <br>• `true`: Paints a solid disk of `bg_color` with an outer ring of `color`. <br>• `false`: Leaves center transparent, rendering a hollow circular ring / hoop. |

---

### 3. `canvasLine` — Drawing Vectors & Grids

```cpp
k10.canvas->canvasLine(x1, y1, x2, y2, color);
```

| # | Parameter | Type | Example | Description |
|---|---|---|---|---|
| **1** | **`x1`** | `int` | `0` | **Start X**: Horizontal position where the line begins. |
| **2** | **`y1`** | `int` | `0` | **Start Y**: Vertical position where the line begins. |
| **3** | **`x2`** | `int` | `240` | **End X**: Horizontal position where the line terminates. |
| **4** | **`y2`** | `int` | `320` | **End Y**: Vertical position where the line terminates. |
| **5** | **`color`** | `uint32_t` | `0x333366` | **Line Color**: 24-bit Hex RGB (`0xRRGGBB`). |

---

### 4. `canvasPoint` — Plotting Individual Pixels

```cpp
k10.canvas->canvasPoint(x, y, color);
```

| # | Parameter | Type | Example | Description |
|---|---|---|---|---|
| **1** | **`x`** | `int16_t` | `100` | **Point X**: Horizontal pixel coordinate. |
| **2** | **`y`** | `int16_t` | `150` | **Point Y**: Vertical pixel coordinate. |
| **3** | **`color`** | `uint32_t` | `0xFFFFFF` | **Point Color**: 24-bit Hex RGB color. |

---

### 5. `canvasSetLineWidth` — Controlling Line & Border Stroke Thickness

```cpp
k10.canvas->canvasSetLineWidth(width);
```

- **`width`** (`uint8_t`): Thickness of lines and shape borders in pixels (default is `5`px).
- Modifying this changes the border thickness of subsequent `canvasLine` and `canvasRectangle` drawing calls.

---

### 6. The Screen Coordinate System

In standard portrait orientation (`initScreen(2)`):
- **Origin `(0, 0)`**: Top-Left corner of the screen.
- **X Axis**: Extends horizontally to the right from `0` to `240` pixels.
- **Y Axis**: Extends vertically downwards from `0` to `320` pixels.
- **Bottom-Right Corner**: Pixel coordinate `(240, 320)`.

```text
(0,0) ───────────────────> X (240px)
  │
  │     (x, y) ──────── w ──────┐
  │        │                    │
  │        h    [RECTANGLE]     │
  │        │                    │
  │        └────────────────────┘
  │
  v
  Y (320px)
```

---

## Complete Sketch Source Code

```cpp
#include "unihiker_k10.h"

UNIHIKER_K10 k10;

void setup() {
    k10.begin();
    k10.initScreen(2);               // 2 = Portrait orientation (240x320)
    k10.creatCanvas();               // Create graphics buffer in RAM
    k10.setScreenBackground(0x050515); // Deep midnight space background

    // 1. Draw diagonal cross lines spanning full corners
    // Parameters: canvasLine(x1, y1, x2, y2, color)
    k10.canvas->canvasLine(0, 0, 240, 320, 0x333366);
    k10.canvas->canvasLine(240, 0, 0, 320, 0x333366);

    // 2. Draw a filled rectangle with a cyan border and deep blue interior
    // Parameters: canvasRectangle(x, y, width, height, borderColor, fillColor, fill)
    k10.canvas->canvasRectangle(20, 30, 200, 70, 0x00FFFF, 0x003366, true);

    // 3. Draw a hollow circle (outer pink ring)
    // Parameters: canvasCircle(x, y, radius, borderColor, fillColor, fill)
    k10.canvas->canvasCircle(120, 160, 45, 0xFF007F, 0x000000, false);

    // 4. Draw a solid circle (inner yellow core)
    k10.canvas->canvasCircle(120, 160, 20, 0xFFFF00, 0xFFFF00, true);

    // 5. Draw a hollow bounding box rectangle at the bottom
    k10.canvas->canvasRectangle(30, 230, 180, 60, 0x00FF88, 0x000000, false);

    // Push the composite vector buffer to the physical ST7789 display
    k10.canvas->updateCanvas();
}

void loop() {
    // Static vector drawing; loop remains idle
}
```
