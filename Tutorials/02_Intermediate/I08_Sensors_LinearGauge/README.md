## What this tutorial is??

This tutorial demonstrates how to connect real-time onboard environmental telemetry (Ambient Light Sensor ALS and AHT20 Temperature/Humidity) to **Linear Bar Gauges** on the UNIHIKER K10.

Features:
- **Ambient Light Gauge**: Real-time 0 to 4000 lux illumination monitor.
- **Thermal Monitor Gauge**: Real-time 15°C to 45°C sensor with dynamic safety color shifts (Green $\to$ Amber $\to$ Red).
- **Humidity Level Gauge**: Real-time relative humidity (20% to 90%).

---

## How it works

1. **Hardware Sensor Ingestion**:
   - `k10.readALS()` returns the 12-bit ambient light intensity (0 to 4095).
   - `aht20.getData(AHT20::eAHT20TempC)` and `aht20.getData(AHT20::eAHT20HumiRH)` read calibrated I2C temperature and relative humidity values.
2. **Gauge Proportion Mapping & Alerts**:
   - Values are converted into pixel widths:
     $$\text{fillWidth} = \left(\frac{V - V_{\min}}{V_{\max} - V_{\min}}\right) \cdot (\text{width} - 4)$$
   - Color thresholds trigger dynamic thermal alerts above 32°C.
3. **Dynamic Partial Component Refresh**:
   - Screen header banner, footer, gauge labels, and container track frames are drawn once in `setup()` (`drawScreenChrome()`).
   - The loop only clears and redraws the numeric readout text box and inner fill bar (`updateLinearGaugeValue()`), completely avoiding full-screen clearing and flicker.

---

## API and Functions detailes

| API / Method | Arguments | Return Type | Description |
| :--- | :--- | :--- | :--- |
| `k10.readALS()` | None | `int` | Reads onboard ambient light level. |
| `aht20.getData(type)` | `eAHT20TempC` / `eAHT20HumiRH` | `float` | Reads temperature or relative humidity. |
| `k10.canvas->canvasRectangle(...)` | Bounds & colors | `void` | Draws gauge tracks, clears text box, and draws fill levels. |
| `k10.canvas->canvasLine(...)` | Coordinates & color | `void` | Draws tick marks and divider lines. |
| `k10.canvas->updateCanvas()` | None | `void` | Flushes canvas to screen. |

---

## Full Code

```cpp
#include "unihiker_k10.h"

UNIHIKER_K10 k10;
uint8_t screen_dir = 2; // Portrait orientation (240x320)
AHT20 aht20;

// 1. Draw static gauge chrome (label, outer track container, tick marks) once
void drawGaugeStaticChrome(int x, int y, int width, int height, const char* label) {
    // Label on the left
    k10.canvas->canvasText(label, x, y - 20, 0xF1F5F9,
                           k10.canvas->eCNAndENFont16, 16, false);

    // Track container
    k10.canvas->canvasRectangle(x, y, width, height, 0x1E3A5F, 0x0C1F33, true);

    // Tick marks along bottom
    for (int t = 0; t <= 4; t++) {
        int tickX = x + (t * (width - 1) / 4);
        k10.canvas->canvasLine(tickX, y + height, tickX, y + height + 4, 0x1E3A5F);
    }
}

// 2. Dynamic Partial Refresh: update ONLY numeric sensor readout and inner fill bar
void updateLinearGaugeValue(int x, int y, int width, int height, float value, float minVal, float maxVal, const char* unit, uint32_t barColor) {
    // Clear previous right-aligned readout area (width 75px, height 18px)
    k10.canvas->canvasRectangle(x + width - 75, y - 20, 75, 18, 0x071524, 0x071524, true);

    // Draw updated sensor numeric readout
    String valStr = String(value, 1) + " " + unit;
    int readoutX = x + width - (int)(valStr.length() * 8);
    k10.canvas->canvasText(valStr, readoutX, y - 20, barColor,
                           k10.canvas->eCNAndENFont16, 12, false);

    // Clear previous inner bar area with track container background
    k10.canvas->canvasRectangle(x + 2, y + 2, width - 4, height - 4, 0x0C1F33, 0x0C1F33, true);

    // Draw updated dynamic fill bar
    float constrainedVal = constrain(value, minVal, maxVal);
    int fillWidth = (int)(((constrainedVal - minVal) / (maxVal - minVal)) * (width - 4));

    if (fillWidth > 0) {
        k10.canvas->canvasRectangle(x + 2, y + 2, fillWidth, height - 4, barColor, barColor, true);
    }
}

// Render static screen layout (Header banner, footer, and gauge frames) once
void drawScreenChrome() {
    // 1. Diagnostic Header Banner
    k10.canvas->canvasRectangle(0, 0, 240, 42, 0x0E2438, 0x0E2438, true);
    k10.canvas->canvasLine(0, 42, 240, 42, 0x00F0FF);
    k10.canvas->canvasText("ENV TELEMETRY", 44, 10, 0x00F0FF,
                           k10.canvas->eCNAndENFont24, 15, false);

    // 2. Static Gauge Containers and Labels
    drawGaugeStaticChrome(20, 84, 200, 22, "Ambient Light");
    drawGaugeStaticChrome(20, 154, 200, 22, "Temperature");
    drawGaugeStaticChrome(20, 224, 200, 22, "Humidity");

    // 3. Centered Footer (Zero-overflow)
    k10.canvas->canvasLine(15, 276, 225, 276, 0x1E3A5F);
    k10.canvas->canvasText("Live Lab Telemetry", 48, 290, 0x64748B,
                           k10.canvas->eCNAndENFont16, 22, false);
}

void setup() {
    k10.begin();
    k10.initScreen(screen_dir);
    k10.creatCanvas();
    // Deep Marine Diagnostic theme background
    k10.setScreenBackground(0x071524);

    k10.rgb->brightness(5);
    k10.rgb->write(-1, 0x00F0FF);

    // Initial paint: static chrome + initial sensor readings
    drawScreenChrome();

    int lightALS = k10.readALS();
    float tempC = aht20.getData(AHT20::eAHT20TempC);
    float humi = aht20.getData(AHT20::eAHT20HumiRH);

    updateLinearGaugeValue(20, 84, 200, 22, lightALS, 0, 4000, "lx", 0x38BDF8);
    uint32_t tempColor = (tempC > 32.0) ? 0xEF4444 : ((tempC > 24.0) ? 0xF59E0B : 0x10B981);
    updateLinearGaugeValue(20, 154, 200, 22, tempC, 15.0, 45.0, "C", tempColor);
    updateLinearGaugeValue(20, 224, 200, 22, humi, 20.0, 90.0, "%", 0x00F0FF);

    k10.canvas->updateCanvas();
}

void loop() {
    // 1. Read onboard hardware sensors
    int lightALS = k10.readALS();
    float tempC = aht20.getData(AHT20::eAHT20TempC);
    float humi = aht20.getData(AHT20::eAHT20HumiRH);

    // 2. Dynamic Partial Refresh: update ONLY gauge readouts and fill bars
    updateLinearGaugeValue(20, 84, 200, 22, lightALS, 0, 4000, "lx", 0x38BDF8);

    uint32_t tempColor = (tempC > 32.0) ? 0xEF4444 : ((tempC > 24.0) ? 0xF59E0B : 0x10B981);
    updateLinearGaugeValue(20, 154, 200, 22, tempC, 15.0, 45.0, "C", tempColor);

    updateLinearGaugeValue(20, 224, 200, 22, humi, 20.0, 90.0, "%", 0x00F0FF);

    // 3. Flush canvas to screen without full-screen flicker
    k10.canvas->updateCanvas();
    delay(100);
}
```
