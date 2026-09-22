## What this tutorial is??

This tutorial demonstrates how to design and render an analog **Circular Speedometer / Dial Gauge** on the UNIHIKER K10 2.8" display using trigonometric geometry.

Features:
- **Radial Bezel & Dial Face**: Deep dual-ring bezel with subtle slate dial styling.
- **240-Degree Sweep**: Industrial arc scale spanning 150° to 390° with radial tick divisions.
- **Dynamic Indicator Needle**: Real-time rotating needle with center cap pivot and digital value readout.

---

## How it works

1. **Trigonometric Coordinate Geometry**:
   - The indicator needle sweeps across a 240-degree angle from $\theta_{\text{start}} = 150^\circ$ to $\theta_{\text{end}} = 390^\circ$.
   - Any value $V$ between $V_{\min}$ and $V_{\max}$ is normalized to angle:
     $$\theta = \theta_{\text{start}} + \left(\frac{V - V_{\min}}{V_{\max} - V_{\min}}\right) \cdot \text{sweepAngle}$$
   - Needle tip coordinates are determined by:
     $$x = \text{centerX} + r \cdot \cos(\theta)$$
     $$y = \text{centerY} + r \cdot \sin(\theta)$$
2. **Double-Buffered Rendering**:
   - The entire gauge, ticks, needle, and typography are rendered to the RAM canvas before updating the screen.

---

## API and Functions detailes

| API / Method | Arguments | Return Type | Description |
| :--- | :--- | :--- | :--- |
| `k10.canvas->canvasCircle(...)` | Center, radius, colors, fill | `void` | Renders dial bezel and pivot cap. |
| `k10.canvas->canvasLine(...)` | Coordinates & color | `void` | Renders radial ticks and rotating needle. |
| `k10.canvas->canvasText(...)` | Text, coordinates, color, font | `void` | Renders numerical speed and unit labels. |

---

## Full Code

```cpp
#include "unihiker_k10.h"

UNIHIKER_K10 k10;
uint8_t screen_dir = 2; // Portrait orientation (240x320)

// Helper to draw a circular dial speedometer gauge
void drawCircularGauge(int centerX, int centerY, int radius, int value, int minVal, int maxVal, const char* unit, uint32_t activeColor) {
    // 1. Outer dial bezel ring (Supercar aluminum trim)
    k10.canvas->canvasCircle(centerX, centerY, radius + 8, 0x475569, 0x000000, false);
    k10.canvas->canvasCircle(centerX, centerY, radius + 7, 0x1E293B, 0x000000, false);

    // 2. Dial face background
    k10.canvas->canvasCircle(centerX, centerY, radius, 0x1E293B, 0x111317, true);

    // 3. Draw tick marks around 240-degree arc (from 150 deg to 390 deg)
    float startAngle = 150.0 * (PI / 180.0);
    float sweepAngle = 240.0 * (PI / 180.0);

    for (int t = 0; t <= 12; t++) {
        float theta = startAngle + (t * sweepAngle / 12.0);
        int x1 = centerX + (int)(cos(theta) * (radius - 4));
        int y1 = centerY + (int)(sin(theta) * (radius - 4));
        int x2 = centerX + (int)(cos(theta) * (radius - 12));
        int y2 = centerY + (int)(sin(theta) * (radius - 12));

        uint32_t tickColor = (t >= 10) ? 0xEF4444 : 0x94A3B8;
        k10.canvas->canvasLine(x1, y1, x2, y2, tickColor);
    }

    // 4. Draw Indicator Needle
    int constrainedVal = constrain(value, minVal, maxVal);
    float normVal = (float)(constrainedVal - minVal) / (float)(maxVal - minVal);
    float needleAngle = startAngle + (normVal * sweepAngle);

    int needleX = centerX + (int)(cos(needleAngle) * (radius - 16));
    int needleY = centerY + (int)(sin(needleAngle) * (radius - 16));

    // Needle vector lines
    k10.canvas->canvasLine(centerX, centerY, needleX, needleY, activeColor);
    k10.canvas->canvasLine(centerX + 1, centerY, needleX, needleY, activeColor);
    k10.canvas->canvasLine(centerX, centerY + 1, needleX, needleY, activeColor);

    // Center pivot cap
    k10.canvas->canvasCircle(centerX, centerY, 8, 0xFACC15, 0xFACC15, true);
    k10.canvas->canvasCircle(centerX, centerY, 4, 0x000000, 0x000000, true);

    // 5. Digital Readout below pivot (Properly centered)
    String valStr = String(value);
    int textX = centerX - (int)(valStr.length() * 7);
    k10.canvas->canvasText(valStr, textX, centerY + 24, 0xF8FAFC,
                           k10.canvas->eCNAndENFont24, 8, false);

    int unitX = centerX - (int)(strlen(unit) * 4);
    k10.canvas->canvasText(unit, unitX, centerY + 50, 0xFACC15,
                           k10.canvas->eCNAndENFont16, 10, false);
}

int gaugeVal = 20;
int gaugeSpeed = 2;

void setup() {
    k10.begin();
    k10.initScreen(screen_dir);
    k10.creatCanvas();
    // Supercar Cockpit Dark Carbon background
    k10.setScreenBackground(0x0C0D10);

    k10.rgb->brightness(5);
    k10.rgb->write(-1, 0xEF4444);
}

void loop() {
    k10.canvas->canvasClear();

    // 1. Cockpit Header Banner
    k10.canvas->canvasRectangle(0, 0, 240, 42, 0x181A20, 0x181A20, true);
    k10.canvas->canvasLine(0, 42, 240, 42, 0xEF4444); // Racing red line
    k10.canvas->canvasText("SPEEDOMETER", 46, 10, 0xF8FAFC,
                           k10.canvas->eCNAndENFont24, 15, false);

    // 2. Render Main Dial Gauge
    uint32_t needleColor = (gaugeVal > 80) ? 0xEF4444 : ((gaugeVal > 50) ? 0xFACC15 : 0x00E5FF);
    drawCircularGauge(120, 155, 72, gaugeVal, 0, 100, "KM / H", needleColor);

    // 3. Centered Cockpit Footer (Zero-overflow)
    k10.canvas->canvasLine(15, 276, 225, 276, 0x27272A);
    k10.canvas->canvasText("Cockpit Velocity Dial", 36, 290, 0x94A3B8,
                           k10.canvas->eCNAndENFont16, 24, false);

    k10.canvas->updateCanvas();

    gaugeVal += gaugeSpeed;
    if (gaugeVal >= 100 || gaugeVal <= 0) {
        gaugeSpeed = -gaugeSpeed;
    }

    delay(30);
}
```
