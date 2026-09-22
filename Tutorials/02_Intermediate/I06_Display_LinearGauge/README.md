## What this tutorial is??

This tutorial demonstrates how to design and render a reusable **Linear Progress & Level Gauge Widget** on the UNIHIKER K10 2.8" color screen.

Features:
- **Modular Bar Widget**: Configurable `(x, y, width, height, min, max, label, color)`.
- **Dynamic Color Shifts**: Demonstrates real-time color transitions (Green $\to$ Yellow $\to$ Red) as monitored values approach critical warning thresholds.
- **Tick Scale & Units**: Features scale division ticks along the bottom rail and clear percentage numeric readouts.

---

## How it works

1. **Mapping Scale Values to Pixels**:
   - The fill width of the gauge is calculated using Arduino's `map()` and `constrain()`:
     $$\text{fillWidth} = \text{map}(\text{constrainedVal}, \text{minVal}, \text{maxVal}, 0, \text{width} - 4)$$
2. **Double-Buffered Vector Shapes**:
   - The outer track is rendered with `canvasRectangle(x, y, w, h, border, fill, true)`.
   - The inner fill rectangle is rendered inside the padded boundaries.
   - Scale ticks are rendered with `canvasLine()`.

---

## API and Functions detailes

| API / Method | Arguments | Return Type | Description |
| :--- | :--- | :--- | :--- |
| `k10.canvas->canvasRectangle(...)` | Geometry & colors | `void` | Draws track background and inner fill bar. |
| `k10.canvas->canvasLine(...)` | Coordinates & color | `void` | Draws tick marks along scale rail. |
| `k10.canvas->updateCanvas()` | None | `void` | Flushes canvas to screen. |

---

## Full Code

```cpp
#include "unihiker_k10.h"

UNIHIKER_K10 k10;
uint8_t screen_dir = 2; // Portrait orientation (240x320)

// Helper to draw a modern linear level gauge
void drawLinearGauge(int x, int y, int width, int height, int value, int minVal, int maxVal, const char* label, uint32_t barColor) {
    // 1. Label and numeric readout
    k10.canvas->canvasText(label, x, y - 24, 0xFEE715,
                           k10.canvas->eCNAndENFont16, 20, false);

    String valStr = String(value) + "%";
    k10.canvas->canvasText(valStr, x + width - 40, y - 24, 0x00E5FF,
                           k10.canvas->eCNAndENFont16, 10, false);

    // 2. Gauge Track Background & Border
    k10.canvas->canvasRectangle(x, y, width, height, 0x334155, 0x111827, true);

    // 3. Filled Bar Indicator
    int constrainedVal = constrain(value, minVal, maxVal);
    int fillWidth = map(constrainedVal, minVal, maxVal, 0, width - 4);

    if (fillWidth > 0) {
        k10.canvas->canvasRectangle(x + 2, y + 2, fillWidth, height - 4, barColor, barColor, true);
    }

    // 4. Tick marks along bottom
    for (int t = 0; t <= 4; t++) {
        int tickX = x + (t * (width - 1) / 4);
        k10.canvas->canvasLine(tickX, y + height, tickX, y + height + 5, 0x64748B);
    }
}

int demoPercent = 0;
int step = 2;

void setup() {
    k10.begin();
    k10.initScreen(screen_dir);
    k10.creatCanvas();
    k10.setScreenBackground(0x0F172A);

    k10.rgb->brightness(5);
    k10.rgb->write(-1, 0x00E5FF);
}

void loop() {
    k10.canvas->canvasClear();

    // Header
    k10.canvas->canvasText("LINEAR GAUGE", 42, 14, 0xFEE715,
                           k10.canvas->eCNAndENFont24, 20, false);
    k10.canvas->canvasLine(15, 44, 225, 44, 0x334155);

    // Gauge 1: Cyan Progress Level
    drawLinearGauge(20, 90, 200, 24, demoPercent, 0, 100, "Progress Level", 0x00E5FF);

    // Gauge 2: Thermal Level (Color shifting with percentage)
    uint32_t tempColor = (demoPercent > 75) ? 0xFF4444 : ((demoPercent > 40) ? 0xFEE715 : 0x00FF88);
    drawLinearGauge(20, 170, 200, 24, demoPercent, 0, 100, "Thermal Monitor", tempColor);

    // Gauge 3: Inverted Battery Level
    int batteryLevel = 100 - demoPercent;
    drawLinearGauge(20, 245, 200, 20, batteryLevel, 0, 100, "Battery Storage", 0x38BDF8);

    k10.canvas->updateCanvas();

    // Animate demo percentage value
    demoPercent += step;
    if (demoPercent >= 100 || demoPercent <= 0) {
        step = -step;
    }

    delay(30);
}
```
