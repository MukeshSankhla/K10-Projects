## What this tutorial is??

This tutorial demonstrates **2D vector drawing and computer graphics rasterization** on the UNIHIKER K10 screen. 

You will learn how to render basic geometric primitives—lines, diagonal crossbars, filled rectangles with borders, hollow rings, and concentric solid circles—forming the visual building blocks of dashboards, charts, games, and embedded user interfaces.

### Expected Behavior
The screen presents a retro-futuristic graphical composition against a deep midnight background (`0x050515`):
1. **Background Gridlines**: Two diagonal lines forming a large subtle 'X' intersecting across the entire screen.
2. **Top Card Panel**: A rounded rectangular card with a cyan border (`0x00FFFF`) and deep blue interior fill (`0x003366`).
3. **Center Target Reticle**: A glowing neon pink circular ring (`0xFF007F`) containing a solid yellow central core (`0xFFFF00`).
4. **Bottom Bounding Box**: A hollow green status box (`0x00FF88`).

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

void setup() {
    k10.begin();
    k10.initScreen(2);               // 2 = Portrait orientation (240x320)
    k10.creatCanvas();               // Allocate 2D graphics canvas buffer in RAM
    k10.setScreenBackground(0x050515); // Deep space background

    // =========================================================================
    // 1. canvasLine API:
    //    k10.canvas->canvasLine(x1, y1, x2, y2, color);
    //    - x1, y1 : Starting coordinate pair
    //    - x2, y2 : Ending coordinate pair
    //    - color  : 24-bit Hex RGB color (0xRRGGBB)
    // =========================================================================
    k10.canvas->canvasLine(0, 0, 240, 320, 0x333366);
    k10.canvas->canvasLine(240, 0, 0, 320, 0x333366);

    // =========================================================================
    // 2. canvasRectangle API:
    //    k10.canvas->canvasRectangle(x, y, w, h, borderColor, fillColor, fill);
    //    - x, y        : Top-left coordinate
    //    - w, h        : Width and Height in pixels
    //    - borderColor : Outer edge outline color
    //    - fillColor   : Interior fill color
    //    - fill        : true = filled interior, false = transparent outline
    // =========================================================================
    // Filled rectangle with cyan border and deep blue interior
    k10.canvas->canvasRectangle(20, 30, 200, 70, 0x00FFFF, 0x003366, true);

    // =========================================================================
    // 3. canvasCircle API:
    //    k10.canvas->canvasCircle(x, y, radius, borderColor, fillColor, fill);
    //    - x, y        : Center point coordinate
    //    - radius      : Radius in pixels
    //    - borderColor : Circumference outline color
    //    - fillColor   : Interior disk color
    //    - fill        : true = solid filled disk, false = hollow ring
    // =========================================================================
    // Hollow circle: Outer pink neon ring
    k10.canvas->canvasCircle(120, 160, 45, 0xFF007F, 0x000000, false);

    // Solid circle: Inner yellow core
    k10.canvas->canvasCircle(120, 160, 20, 0xFFFF00, 0xFFFF00, true);

    // Hollow rectangle: Green bounding box
    k10.canvas->canvasRectangle(30, 230, 180, 60, 0x00FF88, 0x000000, false);

    // Push frame buffer to physical ST7789 display
    k10.canvas->updateCanvas();
}

void loop() {
    // Static vector shapes; loop remains idle
}
```
