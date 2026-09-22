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
2. **Dynamic Partial Refresh Architecture**:
   - The outer bezel ring, dial background face, all 13 angular tick marks, unit label, and header/footer chrome are drawn once during `setup()` (`drawScreenChrome()`).
   - The 30ms animation loop performs a targeted partial redraw: it only clears the inner dial face (`radius - 14`) and digital readout box (`updateCircularGaugeNeedle()`), leaving the surrounding ticks, bezel, and chrome intact without screen flicker.

---

## API and Functions detailes

| API / Method | Arguments | Return Type | Description |
| :--- | :--- | :--- | :--- |
| `k10.canvas->canvasCircle(...)` | Center, radius, colors, fill | `void` | Renders dial bezel, clears inner face, and renders pivot cap. |
| `k10.canvas->canvasLine(...)` | Coordinates & color | `void` | Renders radial ticks, dividers, and rotating needle. |
| `k10.canvas->canvasText(...)` | Text, coordinates, color, font | `void` | Renders numerical speed and unit labels. |

---

## Full Code

```cpp
#include "unihiker_k10.h"

UNIHIKER_K10 k10;
uint8_t screen_dir = 2; // Portrait orientation (240x320)

const float START_ANGLE = 150.0 * (PI / 180.0);
const float SWEEP_ANGLE = 240.0 * (PI / 180.0);

// 1. Draw static dial chrome (bezel rings, face background, tick scale, unit) once
void drawCircularGaugeStaticChrome(int centerX, int centerY, int radius, const char* unit) {
    // Outer dial bezel ring (Supercar aluminum trim)
    k10.canvas->canvasCircle(centerX, centerY, radius + 8, 0x475569, 0x000000, false);
    k10.canvas->canvasCircle(centerX, centerY, radius + 7, 0x1E293B, 0x000000, false);

    // Dial face background
    k10.canvas->canvasCircle(centerX, centerY, radius, 0x1E293B, 0x111317, true);

    // Draw tick marks around 240-degree arc (from 150 deg to 390 deg)
    for (int t = 0; t <= 12; t++) {
        float theta = START_ANGLE + (t * SWEEP_ANGLE / 12.0);
        int x1 = centerX + (int)(cos(theta) * (radius - 4));
        int y1 = centerY + (int)(sin(theta) * (radius - 4));
        int x2 = centerX + (int)(cos(theta) * (radius - 12));
        int y2 = centerY + (int)(sin(theta) * (radius - 12));

        uint32_t tickColor = (t >= 10) ? 0xEF4444 : 0x94A3B8;
        k10.canvas->canvasLine(x1, y1, x2, y2, tickColor);
    }

    // Static Unit Label below dial center
    int unitX = centerX - (int)(strlen(unit) * 4);
    k10.canvas->canvasText(unit, unitX, centerY + 50, 0xFACC15,
                           k10.canvas->eCNAndENFont16, 10, false);
}

// 2. Dynamic Partial Refresh: update ONLY inner dial face, needle, pivot cap, and digital readout
void updateCircularGaugeNeedle(int centerX, int centerY, int radius, int value, int minVal, int maxVal, uint32_t activeColor) {
    // Clear inner dial face (radius - 14) without touching ticks (at radius - 12) or outer bezel
    k10.canvas->canvasCircle(centerX, centerY, radius - 14, 0x111317, 0x111317, true);

    // Clear digital readout text region
    k10.canvas->canvasRectangle(centerX - 30, centerY + 22, 60, 24, 0x111317, 0x111317, true);

    // Calculate indicator needle vector
    int constrainedVal = constrain(value, minVal, maxVal);
    float normVal = (float)(constrainedVal - minVal) / (float)(maxVal - minVal);
    float needleAngle = START_ANGLE + (normVal * SWEEP_ANGLE);

    int needleX = centerX + (int)(cos(needleAngle) * (radius - 16));
    int needleY = centerY + (int)(sin(needleAngle) * (radius - 16));

    // Needle vector lines
    k10.canvas->canvasLine(centerX, centerY, needleX, needleY, activeColor);
    k10.canvas->canvasLine(centerX + 1, centerY, needleX, needleY, activeColor);
    k10.canvas->canvasLine(centerX, centerY + 1, needleX, needleY, activeColor);

    // Center pivot cap
    k10.canvas->canvasCircle(centerX, centerY, 8, 0xFACC15, 0xFACC15, true);
    k10.canvas->canvasCircle(centerX, centerY, 4, 0x000000, 0x000000, true);

    // Digital Readout below pivot
    String valStr = String(value);
    int textX = centerX - (int)(valStr.length() * 7);
    k10.canvas->canvasText(valStr, textX, centerY + 24, 0xF8FAFC,
                           k10.canvas->eCNAndENFont24, 8, false);
}

// Render static screen layout (Header banner and footer telemetry) once
void drawScreenChrome() {
    // 1. Cockpit Header Banner
    k10.canvas->canvasRectangle(0, 0, 240, 42, 0x181A20, 0x181A20, true);
    k10.canvas->canvasLine(0, 42, 240, 42, 0xEF4444); // Racing red line
    k10.canvas->canvasText("SPEEDOMETER", 46, 10, 0xF8FAFC,
                           k10.canvas->eCNAndENFont24, 15, false);

    // 2. Dial Gauge Static Scale and Trim
    drawCircularGaugeStaticChrome(120, 155, 72, "KM / H");

    // 3. Centered Cockpit Footer (Zero-overflow)
    k10.canvas->canvasLine(15, 276, 225, 276, 0x27272A);
    k10.canvas->canvasText("Cockpit Velocity Dial", 36, 290, 0x94A3B8,
                           k10.canvas->eCNAndENFont16, 24, false);
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

    // Initial paint: static chrome + initial needle state
    drawScreenChrome();
    updateCircularGaugeNeedle(120, 155, 72, gaugeVal, 0, 100, 0x00E5FF);
    k10.canvas->updateCanvas();
}

void loop() {
    // Dynamic Partial Refresh: update ONLY the needle, pivot, and digital readout
    uint32_t needleColor = (gaugeVal > 80) ? 0xEF4444 : ((gaugeVal > 50) ? 0xFACC15 : 0x00E5FF);
    updateCircularGaugeNeedle(120, 155, 72, gaugeVal, 0, 100, needleColor);

    // Push updated canvas without any screen flicker
    k10.canvas->updateCanvas();

    gaugeVal += gaugeSpeed;
    if (gaugeVal >= 100 || gaugeVal <= 0) {
        gaugeSpeed = -gaugeSpeed;
    }

    delay(30);
}
```
