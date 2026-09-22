## What this tutorial is??

This tutorial demonstrates how to use the UNIHIKER K10 onboard 3-axis accelerometer and motion gestures to detect the physical orientation/tilt of the board and render dynamic directional vector compass arrows.

Features:
- **4-Way Tilt Detection**: Recognizes **Tilt FORWARD**, **Tilt BACKWARD**, **Tilt LEFT**, and **Tilt RIGHT** via `k10.isGesture()` and raw acceleration thresholds.
- **Dynamic Compass Art**: Draws stylized neon vector arrows aligned to the detected direction, or an emerald center level indicator when placed flat on a table.
- **Raw IMU Telemetry**: Continuously displays raw $X$ and $Y$ axis gravitational components in milli-g ($mg$).

---

## How it works

1. **Gravitational Vector Tilt**:
   - When the board rests flat on a table facing up, gravitational acceleration acts almost entirely along the $Z$-axis ($Z \approx +1000\,mg$, $X \approx 0, Y \approx 0$).
   - Tilting the board shifts gravitational acceleration into the $X$ and $Y$ sensor axes:
     - Pitch forward: $Y < -300\,mg$
     - Pitch backward: $Y > +300\,mg$
     - Roll left: $X < -300\,mg$
     - Roll right: $X > +300\,mg$
2. **Vector Arrow Generation & Dynamic Partial Refresh**:
   - `drawDirectionArrow(centerX, centerY, direction, color)` calculates the tip and shaft coordinates using thick line segments (`canvasLine`).
   - The tactical header banner and outer radar frame are drawn once in `setup()` (`drawScreenChrome()`).
   - In `loop()`, only the inner radar reticle (`updateRadarReticle()`), direction status banner (`updateDirectionBanner()`), and raw accelerometer readout (`updateRawAccel()`) are partially redrawn, eliminating screen flicker.

---

## API and Functions detailes

| API / Method | Arguments | Return Type | Description |
| :--- | :--- | :--- | :--- |
| `k10.isGesture(gesture)` | `TiltForward` / `TiltBack` / `TiltLeft` / `TiltRight` | `bool` | Evaluates whether board matches specific tilt gesture. |
| `k10.getAccelerometerX()` | None | `int` | Reads X-axis acceleration in $mg$. |
| `k10.getAccelerometerY()` | None | `int` | Reads Y-axis acceleration in $mg$. |
| `k10.canvas->canvasLine(...)` | Coordinates & color | `void` | Draws vector arrow heads and shafts. |
| `k10.canvas->updateCanvas()` | None | `void` | Flushes canvas buffer to display. |

---

## Full Code

```cpp
#include "unihiker_k10.h"

UNIHIKER_K10 k10;
uint8_t screen_dir = 2; // Portrait orientation (240x320)

// Helper to draw a bold directional arrow pointing in one of 4 cardinal directions
void drawDirectionArrow(int centerX, int centerY, int direction, uint32_t color) {
    // direction: 0 = Flat/Center, 1 = Forward (Up), 2 = Back (Down), 3 = Left, 4 = Right
    if (direction == 0) {
        // Level Target Reticle
        k10.canvas->canvasCircle(centerX, centerY, 22, color, 0x0E1F18, true);
        k10.canvas->canvasCircle(centerX, centerY, 7, color, color, true);
    } else if (direction == 1) { // Forward (Up)
        k10.canvas->canvasLine(centerX, centerY - 40, centerX - 26, centerY - 14, color);
        k10.canvas->canvasLine(centerX, centerY - 40, centerX + 26, centerY - 14, color);
        k10.canvas->canvasLine(centerX, centerY - 40, centerX, centerY + 40, color);
        k10.canvas->canvasLine(centerX - 1, centerY - 40, centerX - 1, centerY + 40, color);
        k10.canvas->canvasLine(centerX + 1, centerY - 40, centerX + 1, centerY + 40, color);
    } else if (direction == 2) { // Back (Down)
        k10.canvas->canvasLine(centerX, centerY + 40, centerX - 26, centerY + 14, color);
        k10.canvas->canvasLine(centerX, centerY + 40, centerX + 26, centerY + 14, color);
        k10.canvas->canvasLine(centerX, centerY - 40, centerX, centerY + 40, color);
        k10.canvas->canvasLine(centerX - 1, centerY - 40, centerX - 1, centerY + 40, color);
        k10.canvas->canvasLine(centerX + 1, centerY - 40, centerX + 1, centerY + 40, color);
    } else if (direction == 3) { // Left
        k10.canvas->canvasLine(centerX - 40, centerY, centerX - 14, centerY - 26, color);
        k10.canvas->canvasLine(centerX - 40, centerY, centerX - 14, centerY + 26, color);
        k10.canvas->canvasLine(centerX - 40, centerY, centerX + 40, centerY, color);
        k10.canvas->canvasLine(centerX - 40, centerY - 1, centerX + 40, centerY - 1, color);
        k10.canvas->canvasLine(centerX - 40, centerY + 1, centerX + 40, centerY + 1, color);
    } else if (direction == 4) { // Right
        k10.canvas->canvasLine(centerX + 40, centerY, centerX + 14, centerY - 26, color);
        k10.canvas->canvasLine(centerX + 40, centerY, centerX + 14, centerY + 26, color);
        k10.canvas->canvasLine(centerX - 40, centerY, centerX + 40, centerY, color);
        k10.canvas->canvasLine(centerX - 40, centerY - 1, centerX + 40, centerY - 1, color);
        k10.canvas->canvasLine(centerX - 40, centerY + 1, centerX + 40, centerY + 1, color);
    }
}

// 1. Render static tactical header banner and outer radar frame once
void drawScreenChrome() {
    // Tactical Header Banner
    k10.canvas->canvasRectangle(0, 0, 240, 42, 0x0C1914, 0x0C1914, true);
    k10.canvas->canvasLine(0, 42, 240, 42, 0x39FF14);
    k10.canvas->canvasText("TACTICAL RADAR", 30, 10, 0x39FF14,
                           k10.canvas->eCNAndENFont24, 16, false);

    // Outer Radar Scope Ring
    k10.canvas->canvasCircle(120, 155, 74, 0x163326, 0x0A1712, true);
}

// 2. Dynamic Partial Refresh: update ONLY radar reticle and arrow
void updateRadarReticle(int centerX, int centerY, int direction, uint32_t arrowColor) {
    // Clear inner scope area (radius 73)
    k10.canvas->canvasCircle(centerX, centerY, 73, 0x163326, 0x0A1712, true);

    // Redraw concentric range rings
    k10.canvas->canvasCircle(centerX, centerY, 52, 0x163326, 0x0A1712, false);
    k10.canvas->canvasCircle(centerX, centerY, 30, 0x163326, 0x0A1712, false);

    // Redraw crosshair lines
    k10.canvas->canvasLine(centerX, centerY - 70, centerX, centerY + 70, 0x132B20);
    k10.canvas->canvasLine(centerX - 70, centerY, centerX + 70, centerY, 0x132B20);

    // Draw active directional arrow
    drawDirectionArrow(centerX, centerY, direction, arrowColor);
}

// 3. Dynamic Partial Refresh: update ONLY direction status banner
void updateDirectionBanner(const String& dirLabel, uint32_t arrowColor) {
    k10.canvas->canvasRectangle(24, 242, 192, 32, arrowColor, 0x0C1914, true);
    int labelX = 120 - (int)(dirLabel.length() * 4);
    k10.canvas->canvasText(dirLabel, labelX, 250, arrowColor,
                           k10.canvas->eCNAndENFont16, 20, false);
}

// 4. Dynamic Partial Refresh: update ONLY raw accelerometer readout
void updateRawAccel(int accX, int accY) {
    // Clear raw accel text area
    k10.canvas->canvasRectangle(20, 288, 200, 22, 0x060D0A, 0x060D0A, true);

    String rawStr = "X: " + String(accX) + "  Y: " + String(accY);
    int rawX = 120 - (int)(rawStr.length() * 4);
    k10.canvas->canvasText(rawStr, rawX, 290, 0x64748B,
                           k10.canvas->eCNAndENFont16, 22, false);
}

void setup() {
    k10.begin();
    k10.initScreen(screen_dir);
    k10.creatCanvas();
    // Tactical Radar theme background
    k10.setScreenBackground(0x060D0A);

    k10.rgb->brightness(5);
    k10.rgb->write(-1, 0x39FF14); // Radar phosphor green

    // Initial paint: static chrome + initial reticle and text
    drawScreenChrome();
    updateRadarReticle(120, 155, 0, 0x39FF14);
    updateDirectionBanner("Level (Centered)", 0x39FF14);
    updateRawAccel(0, 0);
    k10.canvas->updateCanvas();
}

void loop() {
    int accX = k10.getAccelerometerX();
    int accY = k10.getAccelerometerY();

    int direction = 0; // 0 = Center
    String dirLabel = "Level (Centered)";
    uint32_t arrowColor = 0x39FF14;

    // Evaluate tilt direction using accelerometer gravity vectors
    if (k10.isGesture(TiltForward) || accY < -300) {
        direction = 1;
        dirLabel = "Pitch FORWARD";
        arrowColor = 0x00F0FF;
    } else if (k10.isGesture(TiltBack) || accY > 300) {
        direction = 2;
        dirLabel = "Pitch BACKWARD";
        arrowColor = 0xFFB800;
    } else if (k10.isGesture(TiltLeft) || accX < -300) {
        direction = 3;
        dirLabel = "Roll LEFT";
        arrowColor = 0x38BDF8;
    } else if (k10.isGesture(TiltRight) || accX > 300) {
        direction = 4;
        dirLabel = "Roll RIGHT";
        arrowColor = 0xFF007F;
    }

    // Dynamic Partial Refresh: update ONLY reticle, banner, and raw text
    updateRadarReticle(120, 155, direction, arrowColor);
    updateDirectionBanner(dirLabel, arrowColor);
    updateRawAccel(accX, accY);

    // Flush canvas without full-screen flicker
    k10.canvas->updateCanvas();
    delay(50);
}
```
