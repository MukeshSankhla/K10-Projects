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

// Curated Aurora cosmic color palette
const uint32_t auroraColors[] = {
    0x00F0FF, // Electric Cyan
    0x39FF14, // Neon Lime
    0xFF007F, // Neon Pink
    0xFFB800, // Star Gold
    0xBD00FF, // Cosmic Violet
    0x38BDF8  // Ice Blue
};
const int numColors = sizeof(auroraColors) / sizeof(auroraColors[0]);

void setup() {
    k10.begin();
    k10.initScreen(screen_dir);
    k10.creatCanvas();
    // Deep Cosmos background
    k10.setScreenBackground(0x040514);

    k10.rgb->brightness(5);
    k10.rgb->write(-1, 0xBD00FF); // Cosmic violet glow
}

void loop() {
    k10.canvas->canvasClear();

    // 1. Cosmic Starlight Header Banner
    k10.canvas->canvasRectangle(0, 0, 240, 42, 0x0D0B24, 0x0D0B24, true);
    k10.canvas->canvasLine(0, 42, 240, 42, 0x7209B7);
    k10.canvas->canvasText("METEOR SHOWER", 44, 10, 0x38BDF8,
                           k10.canvas->eCNAndENFont24, 15, false);

    // 2. Draw subtle background starfield points
    for (int s = 0; s < 25; s++) {
        int sx = (s * 47) % 230 + 5;
        int sy = (s * 61) % 240 + 50;
        k10.canvas->canvasPoint(sx, sy, 0x64748B);
    }

    // 3. Draw bounded dynamic meteor streaks
    k10.canvas->canvasSetLineWidth(2);
    for (int i = 0; i < 12; i++) {
        int startX = random(10, 200);
        int startY = random(48, 220);
        int length = random(15, 45);
        int endX = min(230, startX + length);
        int endY = min(300, startY + length);

        uint32_t color = auroraColors[random(0, numColors)];
        k10.canvas->canvasLine(startX, startY, endX, endY, color);
        // Bright meteor head spark
        k10.canvas->canvasPoint(endX, endY, 0xFFFFFF);
    }

    // 4. Centered Footer Status (Zero-overflow)
    k10.canvas->canvasLine(15, 276, 225, 276, 0x1E1538);
    k10.canvas->canvasText("Cosmic Star Stream", 48, 290, 0x818CF8,
                           k10.canvas->eCNAndENFont16, 22, false);

    k10.canvas->updateCanvas();
    delay(120);
}
```
