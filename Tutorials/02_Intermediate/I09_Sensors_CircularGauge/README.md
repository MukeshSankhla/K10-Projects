## What this tutorial is??

This tutorial demonstrates how to render live telemetry data from physical onboard sensors (Temperature and Ambient Light) onto an analog **Circular Dial Gauge** with needle animation on the UNIHIKER K10.

Features:
- **Button [A] (Toggle Sensor)**: Switches the dial display mode between Temperature (°C) and Ambient Light (lux).
- **Smooth Arc Physics**: The indicator needle dynamically swings and responds to real-time physical environmental fluctuations.

---

## How it works

1. **Sensor Switching**:
   - `activeSensor` toggles between Mode 0 (Temperature) and Mode 1 (Light ALS).
   - Value scales, labels, and needle color thresholds dynamically reconfigure according to the active channel.
2. **Dynamic Needle Rotation**:
   - Angle is mapped across 240 degrees:
     $$\theta = 150^\circ + \left(\frac{V - V_{\min}}{V_{\max} - V_{\min}}\right) \cdot 240^\circ$$
   - The needle tip line is drawn from the center pivot to calculated radial offsets.

---

## API and Functions detailes

| API / Method | Arguments | Return Type | Description |
| :--- | :--- | :--- | :--- |
| `k10.buttonA->isPressed()` | None | `bool` | Toggles between Temperature and Light dials. |
| `aht20.getData(AHT20::eAHT20TempC)` | Sensor enum | `float` | Reads temperature. |
| `k10.readALS()` | None | `int` | Reads ambient light. |
| `k10.canvas->canvasCircle(...)` | Geometry & colors | `void` | Draws dial face and pivot cap. |

---

## Full Code

```cpp
#include "unihiker_k10.h"

UNIHIKER_K10 k10;
uint8_t screen_dir = 2; // Portrait orientation (240x320)
AHT20 aht20;

int activeSensor = 0; // 0 = Temperature (10-45 C), 1 = Light Sensor (0-4000 lux)

// Non-blocking button edge-detection tracker
bool checkButtonAPressed() {
    static bool lastState = false;
    static unsigned long lastDebounceTime = 0;
    bool reading = k10.buttonA->isPressed();
    bool pressedEvent = false;

    if (reading != lastState) {
        lastDebounceTime = millis();
    }
    if ((millis() - lastDebounceTime) > 35) {
        static bool stableState = false;
        if (reading != stableState) {
            stableState = reading;
            if (stableState) {
                pressedEvent = true;
            }
        }
    }
    lastState = reading;
    return pressedEvent;
}

void drawCircularGauge(int centerX, int centerY, int radius, float value, float minVal, float maxVal, const char* label, const char* unit, uint32_t activeColor) {
    // 1. Outer dial bezel (Vintage brass/gold trim)
    k10.canvas->canvasCircle(centerX, centerY, radius + 8, 0xD4AF37, 0x000000, false);
    k10.canvas->canvasCircle(centerX, centerY, radius, 0x2A2D35, 0x181A1F, true);

    // 2. Dial tick marks
    float startAngle = 150.0 * (PI / 180.0);
    float sweepAngle = 240.0 * (PI / 180.0);

    for (int t = 0; t <= 10; t++) {
        float theta = startAngle + (t * sweepAngle / 10.0);
        int x1 = centerX + (int)(cos(theta) * (radius - 4));
        int y1 = centerY + (int)(sin(theta) * (radius - 4));
        int x2 = centerX + (int)(cos(theta) * (radius - 12));
        int y2 = centerY + (int)(sin(theta) * (radius - 12));

        k10.canvas->canvasLine(x1, y1, x2, y2, 0x8E94A0);
    }

    // 3. Indicator Needle
    float constrainedVal = constrain(value, minVal, maxVal);
    float normVal = (constrainedVal - minVal) / (maxVal - minVal);
    float needleAngle = startAngle + (normVal * sweepAngle);

    int needleX = centerX + (int)(cos(needleAngle) * (radius - 16));
    int needleY = centerY + (int)(sin(needleAngle) * (radius - 16));

    k10.canvas->canvasLine(centerX, centerY, needleX, needleY, activeColor);
    k10.canvas->canvasLine(centerX + 1, centerY, needleX, needleY, activeColor);
    k10.canvas->canvasCircle(centerX, centerY, 7, 0xD4AF37, 0xD4AF37, true);
    k10.canvas->canvasCircle(centerX, centerY, 3, 0x000000, 0x000000, true);

    // 4. Digital Readouts (Centered)
    int labelX = centerX - (int)(strlen(label) * 4);
    k10.canvas->canvasText(label, labelX, centerY - 42, 0xD4AF37,
                           k10.canvas->eCNAndENFont16, 16, false);

    String valStr = String(value, 1);
    int valX = centerX - (int)(valStr.length() * 7);
    k10.canvas->canvasText(valStr, valX, centerY + 22, 0xF4EBD9,
                           k10.canvas->eCNAndENFont24, 8, false);

    int unitX = centerX - (int)(strlen(unit) * 4);
    k10.canvas->canvasText(unit, unitX, centerY + 48, 0x64748B,
                           k10.canvas->eCNAndENFont16, 10, false);
}

void setup() {
    k10.begin();
    k10.initScreen(screen_dir);
    k10.creatCanvas();
    // Aviator Charcoal background
    k10.setScreenBackground(0x121417);

    k10.rgb->brightness(5);
    k10.rgb->write(-1, 0xD4AF37); // Chrono Gold glow
}

void loop() {
    float tempC = aht20.getData(AHT20::eAHT20TempC);
    int lightALS = k10.readALS();

    k10.canvas->canvasClear();

    // 1. Aviator Header Banner
    k10.canvas->canvasRectangle(0, 0, 240, 42, 0x1B1E24, 0x1B1E24, true);
    k10.canvas->canvasLine(0, 42, 240, 42, 0xD4AF37);
    k10.canvas->canvasText("CHRONO DIAL", 52, 10, 0xD4AF37,
                           k10.canvas->eCNAndENFont24, 15, false);

    // 2. Render Circular Gauge based on active sensor
    if (activeSensor == 0) {
        uint32_t needleColor = (tempC > 30.0) ? 0xEF4444 : 0x22C55E;
        drawCircularGauge(120, 155, 72, tempC, 10.0, 45.0, "Temperature", "deg C", needleColor);
    } else {
        drawCircularGauge(120, 155, 72, lightALS, 0, 4000, "Ambient Light", "lux", 0x38BDF8);
    }

    // 3. Navigation Controls Bar (Zero-overflow)
    k10.canvas->canvasLine(15, 276, 225, 276, 0x2A2D35);
    k10.canvas->canvasRectangle(20, 282, 200, 32, 0x2A2D35, 0x1B1E24, true);
    k10.canvas->canvasText("[A] Toggle Sensor", 44, 290, 0xD4AF37,
                           k10.canvas->eCNAndENFont16, 20, false);

    k10.canvas->updateCanvas();

    // Non-blocking Button A: Toggle Active Sensor
    if (checkButtonAPressed()) {
        activeSensor = (activeSensor + 1) % 2;
    }

    delay(80); // Responsive loop tick
}
```
