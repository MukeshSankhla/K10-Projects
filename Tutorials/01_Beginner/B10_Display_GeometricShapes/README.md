## What this tutorial is??

This tutorial demonstrates 2D vector primitive rendering on the UNIHIKER K10 screen using a **Neon Synthwave Cyberpunk** visual theme.

You will learn how to draw bounded vector rectangles, decorative diagonal alignment lines, concentric target circles, and raster node cards with strict 240x320 zero-overflow coordinates.

### Expected Behavior
- **Cyberpunk Header**: Neon Pink (`0xFF007F`) divider and Sky Cyan (`0x00F0FF`) vector typography.
- **Geometric Arena**: Dark obsidian stage card featuring diagonal alignment crosses.
- **Concentric Circular Targets**: Centered vector circles demonstrating border styling, solid fills, and multi-layer nesting.
- **Magenta Ambient LED**: Onboard RGB LED synchronized with the neon aesthetic.

---

## How it works

1. **Rasterization in Embedded Graphics**:
   - In computer graphics, "vector" primitives (mathematical definitions of shapes like circles and lines) cannot be sent directly to display hardware.
   - They must be converted into discrete pixel grids through **rasterization algorithms**:
     - **Bresenham's Line Algorithm**: Calculates the closest integer pixel coordinates to approximate a continuous diagonal slope with zero floating-point arithmetic.
     - **Midpoint Circle Algorithm**: Uses 8-way symmetry to compute the perimeter of a circle using only integer additions and subtractions.
     - **Scanline Polygon Filling**: Iterates row-by-row between boundary coordinates to flood interior areas with fill colors.
2. **Compound Styling (Border vs. Fill)**:
   - High-level methods like `canvasRectangle` and `canvasCircle` accept both a `borderColor` and a `fillColor`, plus a boolean `fill` switch:
     - `fill = false`: Draws only the 1-pixel outer boundary outline (hollow wireframe shape).
     - `fill = true`: Rasterizes both the outer stroke border and floods the inner area with the fill color.
3. **One-Shot Rendering**:
   - Because this graphic layout is static, all rendering occurs once inside `setup()`.
   - `k10.canvas->updateCanvas()` pushes the completed frame buffer to the ST7789 display controller, leaving `loop()` idle to conserve power.

---

## Sensors/actuator detaile

### The 2D Screen Cartesian Coordinate System
The display operates in a 2D Cartesian plane where $(0, 0)$ begins at the top-left corner:
- $X$ increases moving from **left to right** ($0 \rightarrow 240\text{ px}$).
- $Y$ increases moving from **top to bottom** ($0 \rightarrow 320\text{ px}$).
- When drawing a rectangle with parameters $(x, y, w, h)$:
  - $(x, y)$ defines the **top-left corner**.
  - $w$ is the pixel width extending rightward.
  - $h$ is the pixel height extending downward.
- When drawing a circle with parameters $(x, y, r)$:
  - $(x, y)$ defines the **geometric center point**.
  - $r$ is the radial distance in pixels extending uniformly in all directions.

```
(0,0) ───────────────────────────> +X (240px)
  │   [Rectangle: x, y, w, h]
  │   ┌──────────────┐
  │   │              │ h
  │   └──────────────┘
  │          w
  │
  │            (x,y)
  │              •─── r ───) [Circle]
  V
 +Y (320px)
```

---

## Step by step function wise code break down

### 1. Library Inclusion & Frame Buffer Initialization
```cpp
#include "unihiker_k10.h"

UNIHIKER_K10 k10;

void setup() {
    k10.begin();
    k10.initScreen(2);               // 2 = Portrait orientation (240x320)
    k10.creatCanvas();               // Allocate 2D graphics canvas buffer in RAM
    k10.setScreenBackground(0x050515); // Deep space background
```
- Initializes board hardware, configures 240x320 portrait mode, allocates RAM canvas, and sets dark background `0x050515`.

### 2. Diagonal Lines
```cpp
    k10.canvas->canvasLine(0, 0, 240, 320, 0x333366);
    k10.canvas->canvasLine(240, 0, 0, 320, 0x333366);
```
- Draws line 1 from top-left `(0, 0)` to bottom-right `(240, 320)`.
- Draws line 2 from top-right `(240, 0)` to bottom-left `(0, 320)` in subtle slate-blue `0x333366`.

### 3. Filled & Outlined Rectangles
```cpp
    // Filled rectangle with cyan border and deep blue interior
    k10.canvas->canvasRectangle(20, 30, 200, 70, 0x00FFFF, 0x003366, true);

    // Hollow rectangle: Green bounding box
    k10.canvas->canvasRectangle(30, 230, 180, 60, 0x00FF88, 0x000000, false);
```
- The top card is $200\text{ px}$ wide by $70\text{ px}$ tall with a solid fill (`true`).
- The bottom bounding box is hollow (`false`), leaving the background visible inside.

### 4. Concentric Circles & Screen Refresh
```cpp
    // Hollow circle: Outer pink neon ring
    k10.canvas->canvasCircle(120, 160, 45, 0xFF007F, 0x000000, false);

    // Solid circle: Inner yellow core
    k10.canvas->canvasCircle(120, 160, 20, 0xFFFF00, 0xFFFF00, true);

    k10.canvas->updateCanvas();
}
```
- Draws an outer ring of radius 45 px centered at `(120, 160)`.
- Draws an inner solid core of radius 20 px at the identical center coordinate.
- `updateCanvas()` flushes all vector primitives to the physical display panel.

---

## API and Functions detailes

| API / Method | Parameters | Return Type | Description |
| :--- | :--- | :--- | :--- |
| `k10.canvas->canvasLine(...)` | `x1, y1, x2, y2, color` | `void` | Draws a 1-pixel Bresenham vector line connecting point $(x1, y1)$ to $(x2, y2)$. |
| `k10.canvas->canvasRectangle(...)` | `x, y, w, h, border, fill, isFill` | `void` | Draws a rectangle with specified border and interior colors. `isFill = true` solid fills interior. |
| `k10.canvas->canvasCircle(...)` | `x, y, radius, border, fill, isFill` | `void` | Draws a circle centered at $(x, y)$ of radius $r$. `isFill = false` draws a hollow ring. |
| `k10.canvas->updateCanvas()` | None | `void` | Flushes dirty canvas buffer regions to the ST7789 display controller. |

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
    // Neon Synthwave Obsidian background
    k10.setScreenBackground(0x08071A);

    // 1. Cyberpunk Header Banner
    k10.canvas->canvasRectangle(0, 0, 240, 42, 0x150E2E, 0x150E2E, true);
    k10.canvas->canvasLine(0, 42, 240, 42, 0xFF007F);
    k10.canvas->canvasText("VECTOR SHAPES", 42, 10, 0x00F0FF,
                           k10.canvas->eCNAndENFont24, 15, false);

    // 2. Geometric Stage Arena (x=14, y=52, w=212, h=218)
    k10.canvas->canvasRectangle(14, 52, 212, 218, 0x2A194C, 0x100A24, true);

    // Decorative diagonals
    k10.canvas->canvasLine(14, 52, 226, 270, 0x1E1238);
    k10.canvas->canvasLine(226, 52, 14, 270, 0x1E1238);

    // Outer Neon Rounded Card
    k10.canvas->canvasRectangle(26, 64, 188, 56, 0x00F0FF, 0x0C223A, true);
    k10.canvas->canvasText("Linear Bounding Box", 38, 82, 0x00F0FF,
                           k10.canvas->eCNAndENFont16, 22, false);

    // Concentric Neon Circular Targets (Center at x=120, y=175)
    k10.canvas->canvasCircle(120, 175, 42, 0xFF007F, 0x000000, false);
    k10.canvas->canvasCircle(120, 175, 28, 0xBD00FF, 0x1D0630, true);
    k10.canvas->canvasCircle(120, 175, 14, 0xFFE600, 0xFFE600, true);
    k10.canvas->canvasCircle(120, 175, 5, 0x000000, 0x000000, true);

    // Lower Neon Lime Floating Card
    k10.canvas->canvasRectangle(26, 230, 188, 30, 0x39FF14, 0x0A2610, true);
    k10.canvas->canvasText("Raster Graphic Nodes", 44, 237, 0x39FF14,
                           k10.canvas->eCNAndENFont16, 22, false);

    // 3. Centered Footer Label (Zero-overflow)
    k10.canvas->canvasLine(15, 278, 225, 278, 0x2A194C);
    k10.canvas->canvasText("2D Vector Graphics Demo", 32, 290, 0x94A3B8,
                           k10.canvas->eCNAndENFont16, 24, false);

    k10.canvas->updateCanvas();

    // Magenta ambient LED
    k10.rgb->brightness(5);
    k10.rgb->write(-1, 0xFF007F);
}

void loop() {
    // Static vector shapes demonstration
    delay(200);
}
```
