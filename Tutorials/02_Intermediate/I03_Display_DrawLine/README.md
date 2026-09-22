## What this tutorial is??

This tutorial demonstrates advanced vector line rendering on the UNIHIKER K10 ST7789 display using `canvasLine(x1, y1, x2, y2, color)` to create geometric art and rotating radial wireframes.

Features:
- **Interactive Switching**: Pressing **Button [A]** toggles between a 3D perspective hyperboloid funnel web and a dynamic rotating geometric radial star.
- **Pure Math Rendering**: Uses trigonometric rotation matrices (`cos`, `sin`) to render vector lines without external graphics libraries.

---

## How it works

1. **Hyperboloid Funnel Web**:
   - Loops through horizontal offsets, drawing cross-connecting diagonals between top and bottom borders:
     `k10.canvas->canvasLine(i, 75, 240 - i, 265, 0x00E5FF);`
   - Creates an optical illusion of a continuous curved surface formed entirely from straight 1-pixel line segments.
2. **Rotating Radial Star**:
   - Evaluates vertex angles along a circle: $\theta_i = \text{angle} + i \cdot \frac{2\pi}{N}$.
   - Connects center origins to circumference points and cross-weaves inner harmonic points.
3. **Double Buffering via Canvas**:
   - `canvasClear()` erases the previous frame, lines are drawn in RAM, and `updateCanvas()` flips the frame to the display, eliminating flicker.

---

## API and Functions detailes

| API / Method | Arguments | Return Type | Description |
| :--- | :--- | :--- | :--- |
| `k10.canvas->canvasLine(...)` | `x1, y1, x2, y2, color` | `void` | Renders a 1-pixel vector line between two coordinates. |
| `k10.canvas->canvasText(...)` | Text, coordinates, color, font | `void` | Renders typography labels. |
| `k10.canvas->updateCanvas()` | None | `void` | Pushes the rendered canvas buffer to the physical screen. |
| `k10.buttonA->isPressed()` | None | `bool` | Detects physical press on Button A to toggle patterns. |

---

## Full Code

```cpp
#include "unihiker_k10.h"

UNIHIKER_K10 k10;
uint8_t screen_dir = 2; // Portrait orientation (240x320)

int currentMode = 0; // 0 = Funnel Hyperboloid, 1 = Rotating Ray Web
float angle = 0;

void setup() {
    k10.begin();
    k10.initScreen(screen_dir);
    k10.creatCanvas();
    k10.setScreenBackground(0x050510);

    k10.rgb->brightness(5);
    k10.rgb->write(-1, 0x00FF88); // Emerald green aura
}

void loop() {
    k10.canvas->canvasClear();

    // 1. Header
    k10.canvas->canvasText("VECTOR LINE ART", 25, 14, 0xFEE715,
                           k10.canvas->eCNAndENFont24, 20, false);
    k10.canvas->canvasLine(15, 44, 225, 44, 0x334155);

    // 2. Draw Vector Graphic depending on mode
    if (currentMode == 0) {
        k10.canvas->canvasText("Pattern: Perspective Funnel", 20, 52, 0x94A3B8,
                               k10.canvas->eCNAndENFont16, 26, false);

        // Funnel hyperboloid line web
        for (int i = 0; i <= 240; i += 12) {
            k10.canvas->canvasLine(i, 75, 240 - i, 265, 0x00E5FF);
            k10.canvas->canvasLine(0, 75 + (i * 190 / 240), 240, 265 - (i * 190 / 240), 0xFF007F);
        }
    } else {
        k10.canvas->canvasText("Pattern: Dynamic Radial Star", 20, 52, 0x94A3B8,
                               k10.canvas->eCNAndENFont16, 26, false);

        int centerX = 120;
        int centerY = 170;
        int radius = 80;

        for (int i = 0; i < 24; i++) {
            float theta = angle + (i * (2 * PI / 24));
            int x1 = centerX + (int)(cos(theta) * radius);
            int y1 = centerY + (int)(sin(theta) * radius);

            int x2 = centerX + (int)(cos(theta + PI / 2) * (radius / 2));
            int y2 = centerY + (int)(sin(theta + PI / 2) * (radius / 2));

            k10.canvas->canvasLine(centerX, centerY, x1, y1, 0x38BDF8);
            k10.canvas->canvasLine(x1, y1, x2, y2, 0xFEE715);
        }
        angle += 0.04;
    }

    // 3. Footer Control Bar
    k10.canvas->canvasLine(15, 275, 225, 275, 0x334155);
    k10.canvas->canvasText("Press [A] to Toggle Pattern", 20, 288, 0x00FF88,
                           k10.canvas->eCNAndENFont16, 26, false);

    k10.canvas->updateCanvas();

    // Toggle pattern mode
    if (k10.buttonA->isPressed()) {
        currentMode = (currentMode + 1) % 2;
        while (k10.buttonA->isPressed()) {
            delay(50);
        }
    }

    delay(30);
}
```
