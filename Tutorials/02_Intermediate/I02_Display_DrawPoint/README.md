## What this tutorial is??

This tutorial demonstrates how to use the single-pixel graphics primitive `canvasPoint(x, y, color)` on the UNIHIKER K10 to simulate a dynamic **3D Starfield & Particle Point System**.

The sketch projects 60 3D spatial coordinate points onto the 2D display plane using perspective projection math, dynamically adjusting color intensity and point cluster sizing as particles approach the observer.

---

## How it works

1. **3D Perspective Projection Mathematics**:
   - Each star maintains 3D coordinates `(x, y, z)`.
   - As `z` decreases, the star approaches the screen plane:
     $$\text{screenX} = \text{centerX} + \frac{x \cdot \text{scale}}{z}$$
     $$\text{screenY} = \text{centerY} + \frac{y \cdot \text{scale}}{z}$$
2. **Point Rendering with `canvasPoint`**:
   - `k10.canvas->canvasPoint(x, y, color)` manipulates individual pixels in the RAM frame buffer without the overhead of drawing polygons.
   - For stars close to the viewport (`z < 60`), cross-cluster points are rendered to simulate optical blooming/glow.
3. **RGB Color Synthesis**:
   - `rgbToColor(r, g, b)` packs 8-bit red, green, and blue values into a standard 24-bit RGB hex word (`0xRRGGBB`).
   - Distance attenuation brightens stars as `z` approaches zero.

---

## Sensors/actuator detaile

### 1. ST7789 IPS Display & RAM Framebuffer
- **Resolution**: 240x320 pixels.
- **Pixel Manipulation**: `canvasPoint` modifies 16-bit/24-bit memory addresses in the allocated RAM canvas.

---

## Step by step function wise code break down

### 1. Data Structure & Star Reset
```cpp
struct Star {
    float x;
    float y;
    float z;
};
const int NUM_STARS = 60;
Star stars[NUM_STARS];
```

### 2. Main Animation Loop
```cpp
for (int i = 0; i < NUM_STARS; i++) {
    stars[i].z -= 4.0;
    if (stars[i].z <= 2.0) {
        stars[i].x = random(-120, 120);
        stars[i].y = random(-160, 160);
        stars[i].z = 240;
    }
    int screenX = 120 + (int)((stars[i].x * 120.0) / stars[i].z);
    int screenY = 170 + (int)((stars[i].y * 120.0) / stars[i].z);

    if (screenX >= 5 && screenX < 235 && screenY >= 70 && screenY < 280) {
        uint8_t brightness = map((int)stars[i].z, 2, 240, 255, 60);
        uint32_t starColor = rgbToColor(brightness, brightness, 255);
        k10.canvas->canvasPoint(screenX, screenY, starColor);
    }
}
```

---

## API and Functions detailes

| API / Method | Arguments | Return Type | Description |
| :--- | :--- | :--- | :--- |
| `k10.canvas->canvasPoint(x, y, color)` | `int x, int y, uint32_t color` | `void` | Renders a single pixel at `(x, y)`. |
| `k10.canvas->canvasLine(...)` | Line coordinates & color | `void` | Draws 1-pixel separator line. |
| `k10.canvas->canvasText(...)` | Text, coordinates, color, font | `void` | Renders text. |
| `k10.canvas->updateCanvas()` | None | `void` | Flushes canvas buffer to display. |

---

## Full Code

```cpp
#include "unihiker_k10.h"

UNIHIKER_K10 k10;
uint8_t screen_dir = 2; // Portrait orientation (240x320)

// Helper to convert RGB components into 24-bit Hex RGB
uint32_t rgbToColor(uint8_t r, uint8_t g, uint8_t b) {
    return ((uint32_t)r << 16) | ((uint32_t)g << 8) | (uint32_t)b;
}

// Structure to model a star in 3D projection
struct Star {
    float x;
    float y;
    float z;
};

const int NUM_STARS = 60;
Star stars[NUM_STARS];

void initStars() {
    for (int i = 0; i < NUM_STARS; i++) {
        stars[i].x = random(-120, 120);
        stars[i].y = random(-160, 160);
        stars[i].z = random(20, 240);
    }
}

void setup() {
    k10.begin();
    k10.initScreen(screen_dir);
    k10.creatCanvas();
    k10.setScreenBackground(0x050515);

    initStars();

    k10.rgb->brightness(5);
    k10.rgb->write(-1, 0x1E1B4B); // Deep purple aura
}

void loop() {
    k10.canvas->canvasClear();

    // 1. Header Banner
    k10.canvas->canvasText("DRAW POINT DEMO", 28, 14, 0xFEE715,
                           k10.canvas->eCNAndENFont24, 20, false);
    k10.canvas->canvasText("3D Starfield & Particle Points", 18, 42, 0x94A3B8,
                           k10.canvas->eCNAndENFont16, 30, false);
    k10.canvas->canvasLine(15, 62, 225, 62, 0x334155);

    // 2. Render 3D Starfield Points
    for (int i = 0; i < NUM_STARS; i++) {
        stars[i].z -= 4.0; // Move star closer to observer

        // Recycle star when it passes the screen plane
        if (stars[i].z <= 2.0) {
            stars[i].x = random(-120, 120);
            stars[i].y = random(-160, 160);
            stars[i].z = 240;
        }

        // Perspective 3D to 2D projection
        int screenX = 120 + (int)((stars[i].x * 120.0) / stars[i].z);
        int screenY = 170 + (int)((stars[i].y * 120.0) / stars[i].z);

        if (screenX >= 5 && screenX < 235 && screenY >= 70 && screenY < 280) {
            // Brightness attenuates with distance
            uint8_t brightness = map((int)stars[i].z, 2, 240, 255, 60);
            uint32_t starColor = rgbToColor(brightness, brightness, 255);

            // Draw primary point
            k10.canvas->canvasPoint(screenX, screenY, starColor);

            // If close, draw cross cluster for enhanced glow
            if (stars[i].z < 60) {
                k10.canvas->canvasPoint(screenX + 1, screenY, starColor);
                k10.canvas->canvasPoint(screenX - 1, screenY, starColor);
                k10.canvas->canvasPoint(screenX, screenY + 1, starColor);
                k10.canvas->canvasPoint(screenX, screenY - 1, starColor);
            }
        }
    }

    // 3. Footer Stats
    k10.canvas->canvasLine(15, 285, 225, 285, 0x334155);
    k10.canvas->canvasText("Active Points: 60  |  API: canvasPoint", 15, 295, 0x00E5FF,
                           k10.canvas->eCNAndENFont16, 30, false);

    k10.canvas->updateCanvas();
    delay(20);
}
```
