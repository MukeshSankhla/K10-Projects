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
2. **Dynamic Needle Rotation & Partial Refresh**:
   - The outer brass bezel, face background, 10 angular scale tick marks, header banner, and bottom navigation bar are rendered once in `setup()` (`drawScreenChrome()` and `drawCircularGaugeStaticChrome()`).
   - In `loop()`, only the inner dial face circle (`radius - 14`) and digital readout box are refreshed (`updateGaugeNeedleAndValue()`).
   - The label and unit text (`updateGaugeLabels()`) are only redrawn when Button A toggles the active sensor channel.

---

## API and Functions detailes

| API / Method | Arguments | Return Type | Description |
| :--- | :--- | :--- | :--- |
| `k10.buttonA->isPressed()` | None | `bool` | Toggles between Temperature and Light dials. |
| `aht20.getData(AHT20::eAHT20TempC)` | Sensor enum | `float` | Reads temperature. |
| `k10.readALS()` | None | `int` | Reads ambient light. |
| `k10.canvas->canvasCircle(...)` | Geometry & colors | `void` | Draws dial face, clears inner face, and renders pivot cap. |
| `k10.canvas->updateCanvas()` | None | `void` | Flushes canvas buffer to screen. |

---

## Full Code

```cpp
#include "unihiker_k10.h"

UNIHIKER_K10 k10;
uint8_t screen_dir = 2; // Portrait orientation (240x320)
AHT20 aht20;

int activeSensor = 0; // 0 = Temperature (10-45 C), 1 = Light Sensor (0-4000 lux)

const float START_ANGLE = 150.0 * (PI / 180.0);
const float SWEEP_ANGLE = 240.0 * (PI / 180.0);

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

// 1. Draw static dial bezel and tick marks once
void drawCircularGaugeStaticChrome(int centerX, int centerY, int radius) {
    // Outer dial bezel (Vintage brass/gold trim)
    k10.canvas->canvasCircle(centerX, centerY, radius + 8, 0xD4AF37, 0x000000, false);
    k10.canvas->canvasCircle(centerX, centerY, radius, 0x2A2D35, 0x181A1F, true);

    // Dial tick marks
    for (int t = 0; t <= 10; t++) {
        float theta = START_ANGLE + (t * SWEEP_ANGLE / 10.0);
        int x1 = centerX + (int)(cos(theta) * (radius - 4));
        int y1 = centerY + (int)(sin(theta) * (radius - 4));
        int x2 = centerX + (int)(cos(theta) * (radius - 12));
        int y2 = centerY + (int)(sin(theta) * (radius - 12));

        k10.canvas->canvasLine(x1, y1, x2, y2, 0x8E94A0);
    }
}

// 2. Update sensor title label and unit string (called only on sensor switch)
void updateGaugeLabels(int centerX, int centerY, const char* label, const char* unit) {
    // Clear title label and unit areas with dial background color
    k10.canvas->canvasRectangle(centerX - 60, centerY - 46, 120, 18, 0x181A1F, 0x181A1F, true);
    k10.canvas->canvasRectangle(centerX - 40, centerY + 46, 80, 18, 0x181A1F, 0x181A1F, true);

    int labelX = centerX - (int)(strlen(label) * 4);
    k10.canvas->canvasText(label, labelX, centerY - 42, 0xD4AF37,
                           k10.canvas->eCNAndENFont16, 16, false);

    int unitX = centerX - (int)(strlen(unit) * 4);
    k10.canvas->canvasText(unit, unitX, centerY + 48, 0x64748B,
                           k10.canvas->eCNAndENFont16, 10, false);
}

// 3. Dynamic Partial Refresh: update ONLY inner dial, needle, pivot, and numeric value
void updateGaugeNeedleAndValue(int centerX, int centerY, int radius, float value, float minVal, float maxVal, uint32_t activeColor) {
    // Clear inner dial face (radius - 14) without touching outer tick marks
    k10.canvas->canvasCircle(centerX, centerY, radius - 14, 0x181A1F, 0x181A1F, true);

    // Clear digital value readout text box
    k10.canvas->canvasRectangle(centerX - 35, centerY + 20, 70, 24, 0x181A1F, 0x181A1F, true);

    // Calculate indicator needle vector
    float constrainedVal = constrain(value, minVal, maxVal);
    float normVal = (constrainedVal - minVal) / (maxVal - minVal);
    float needleAngle = START_ANGLE + (normVal * SWEEP_ANGLE);

    int needleX = centerX + (int)(cos(needleAngle) * (radius - 16));
    int needleY = centerY + (int)(sin(needleAngle) * (radius - 16));

    // Draw needle
    k10.canvas->canvasLine(centerX, centerY, needleX, needleY, activeColor);
    k10.canvas->canvasLine(centerX + 1, centerY, needleX, needleY, activeColor);

    // Center pivot cap
    k10.canvas->canvasCircle(centerX, centerY, 7, 0xD4AF37, 0xD4AF37, true);
    k10.canvas->canvasCircle(centerX, centerY, 3, 0x000000, 0x000000, true);

    // Draw digital readout
    String valStr = (activeSensor == 0) ? String(value, 1) : String((int)value);
    int valX = centerX - (int)(valStr.length() * 7);
    k10.canvas->canvasText(valStr, valX, centerY + 22, 0xF4EBD9,
                           k10.canvas->eCNAndENFont24, 8, false);
}

// Render static screen layout (Header banner and bottom prompt bar) once
void drawScreenChrome() {
    // 1. Aviator Header Banner
    k10.canvas->canvasRectangle(0, 0, 240, 42, 0x1B1E24, 0x1B1E24, true);
    k10.canvas->canvasLine(0, 42, 240, 42, 0xD4AF37);
    k10.canvas->canvasText("CHRONO DIAL", 52, 10, 0xD4AF37,
                           k10.canvas->eCNAndENFont24, 15, false);

    // 2. Navigation Controls Bar (Zero-overflow)
    k10.canvas->canvasLine(15, 276, 225, 276, 0x2A2D35);
    k10.canvas->canvasRectangle(20, 282, 200, 32, 0x2A2D35, 0x1B1E24, true);
    k10.canvas->canvasText("[A] Toggle Sensor", 44, 290, 0xD4AF37,
                           k10.canvas->eCNAndENFont16, 20, false);
}

void setup() {
    k10.begin();
    k10.initScreen(screen_dir);
    k10.creatCanvas();
    // Aviator Charcoal background
    k10.setScreenBackground(0x121417);

    k10.rgb->brightness(5);
    k10.rgb->write(-1, 0xD4AF37); // Chrono Gold glow

    // Initial paint: static chrome + dial frame + initial readings
    drawScreenChrome();
    drawCircularGaugeStaticChrome(120, 155, 72);
    updateGaugeLabels(120, 155, "Temperature", "deg C");

    float tempC = aht20.getData(AHT20::eAHT20TempC);
    uint32_t needleColor = (tempC > 30.0) ? 0xEF4444 : 0x22C55E;
    updateGaugeNeedleAndValue(120, 155, 72, tempC, 10.0, 45.0, needleColor);

    k10.canvas->updateCanvas();
}

void loop() {
    // Non-blocking Button A: Toggle Active Sensor
    if (checkButtonAPressed()) {
        activeSensor = (activeSensor + 1) % 2;
        if (activeSensor == 0) {
            updateGaugeLabels(120, 155, "Temperature", "deg C");
        } else {
            updateGaugeLabels(120, 155, "Ambient Light", "lux");
        }
    }

    // Dynamic Partial Refresh: update ONLY needle and numeric value
    if (activeSensor == 0) {
        float tempC = aht20.getData(AHT20::eAHT20TempC);
        uint32_t needleColor = (tempC > 30.0) ? 0xEF4444 : 0x22C55E;
        updateGaugeNeedleAndValue(120, 155, 72, tempC, 10.0, 45.0, needleColor);
    } else {
        int lightALS = k10.readALS();
        updateGaugeNeedleAndValue(120, 155, 72, lightALS, 0, 4000, 0x38BDF8);
    }

    // Flush canvas without full-screen flicker
    k10.canvas->updateCanvas();
    delay(80); // Responsive loop tick
}
```
