## What this tutorial is??

This tutorial demonstrates a multi-particle physics simulation creating an animated **Meteor Shower** across the UNIHIKER K10 screen.

Features:
- **Particle Dynamics**: 8 concurrent meteors fall at randomized speeds, lengths, and starting coordinates.
- **Visual Glow**: Glowing meteor heads rendered using filled circles and color-tail streaks (cyan, pink, and gold).
- **Background Starfield**: Static background stars anchored in the night sky.

---

## How it works

1. **Trajectory Mathematics**:
   - Meteors travel diagonally at 45 degrees:
     $$x_{t+1} = x_t + \text{speed} \cdot 0.7$$
     $$y_{t+1} = y_t + \text{speed} \cdot 0.7$$
   - The tail extends backward:
     $$\text{endX} = \text{startX} - \text{length} \cdot 0.7$$
     $$\text{endY} = \text{startY} - \text{length} \cdot 0.7$$
2. **Line and Circle Primitives**:
   - `k10.canvas->canvasLine(endX, endY, startX, startY, tailColor)` renders the ion trail.
   - `k10.canvas->canvasCircle(startX, startY, 2, headColor, headColor, true)` renders the superheated meteor core.
3. **Screen Bounds & Recycling**:
   - When a meteor exceeds `y > 330` or `x > 260`, `resetMeteor(i)` resets its position above the viewport with randomized attributes.

---

## API and Functions detailes

| API / Method | Arguments | Return Type | Description |
| :--- | :--- | :--- | :--- |
| `k10.canvas->canvasLine(...)` | Line coordinates & color | `void` | Renders streak trails. |
| `k10.canvas->canvasCircle(...)` | Center, radius, colors, fill | `void` | Renders glowing particle heads. |
| `k10.canvas->canvasPoint(...)` | Coordinates & color | `void` | Renders background sky stars. |
| `k10.canvas->updateCanvas()` | None | `void` | Flushes double buffer to display. |

---

## Full Code

```cpp
#include "unihiker_k10.h"

UNIHIKER_K10 k10;
uint8_t screen_dir = 2; // Portrait orientation (240x320)

uint32_t rgbToColor(uint8_t r, uint8_t g, uint8_t b) {
    return ((uint32_t)r << 16) | ((uint32_t)g << 8) | (uint32_t)b;
}

struct Meteor {
    float x;
    float y;
    float length;
    float speed;
    uint32_t headColor;
    uint32_t tailColor;
};

const int NUM_METEORS = 8;
Meteor meteors[NUM_METEORS];

void resetMeteor(int i) {
    meteors[i].x = random(-20, 220);
    meteors[i].y = random(-100, 0);
    meteors[i].length = random(25, 60);
    meteors[i].speed = random(8, 16);

    uint8_t colorPick = random(0, 3);
    if (colorPick == 0) {
        meteors[i].headColor = 0xFFFFFF; // White-hot
        meteors[i].tailColor = 0x00E5FF; // Neon cyan tail
    } else if (colorPick == 1) {
        meteors[i].headColor = 0xFFFFAA; // Warm core
        meteors[i].tailColor = 0xFF007F; // Pink tail
    } else {
        meteors[i].headColor = 0xFFFFFF;
        meteors[i].tailColor = 0xFEE715; // Golden tail
    }
}

void setup() {
    k10.begin();
    k10.initScreen(screen_dir);
    k10.creatCanvas();
    k10.setScreenBackground(0x02020A); // Midnight sky

    for (int i = 0; i < NUM_METEORS; i++) {
        resetMeteor(i);
        meteors[i].y = random(0, 300); // Stagger initial positions
    }

    k10.rgb->brightness(5);
    k10.rgb->write(-1, 0x002244);
}

void loop() {
    k10.canvas->canvasClear();

    // 1. Static Night Sky Elements & Header
    k10.canvas->canvasText("METEOR SHOWER", 35, 14, 0xFEE715,
                           k10.canvas->eCNAndENFont24, 20, false);
    k10.canvas->canvasLine(15, 44, 225, 44, 0x334155);

    // 2. Background twinkling stars
    for (int s = 0; s < 15; s++) {
        int sx = (s * 37 + 13) % 230 + 5;
        int sy = (s * 49 + 29) % 240 + 50;
        k10.canvas->canvasPoint(sx, sy, 0x64748B);
    }

    // 3. Update and Render Meteors
    for (int i = 0; i < NUM_METEORS; i++) {
        // Move diagonally (45 degree fall)
        meteors[i].x += meteors[i].speed * 0.7;
        meteors[i].y += meteors[i].speed * 0.7;

        float startX = meteors[i].x;
        float startY = meteors[i].y;
        float endX = startX - (meteors[i].length * 0.7);
        float endY = startY - (meteors[i].length * 0.7);

        // Draw meteor streak
        k10.canvas->canvasLine(endX, endY, startX, startY, meteors[i].tailColor);

        // Draw glowing meteor head
        k10.canvas->canvasCircle(startX, startY, 2, meteors[i].headColor, meteors[i].headColor, true);

        // Reset if meteor leaves the screen bounds
        if (meteors[i].y > 330 || meteors[i].x > 260) {
            resetMeteor(i);
        }
    }

    // 4. Footer
    k10.canvas->canvasLine(15, 290, 225, 290, 0x334155);
    k10.canvas->canvasText("Dynamic Vector Particle System", 15, 298, 0x00E5FF,
                           k10.canvas->eCNAndENFont16, 30, false);

    k10.canvas->updateCanvas();
    delay(25);
}
```
