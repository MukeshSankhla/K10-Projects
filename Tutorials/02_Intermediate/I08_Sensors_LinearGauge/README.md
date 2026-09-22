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
2. **Gauge Proportion Mapping**:
   - Values are converted into pixel widths:
     $$\text{fillWidth} = \left(\frac{V - V_{\min}}{V_{\max} - V_{\min}}\right) \cdot (\text{width} - 4)$$
   - Color thresholds trigger dynamic thermal alerts above 32°C.

---

## API and Functions detailes

| API / Method | Arguments | Return Type | Description |
| :--- | :--- | :--- | :--- |
| `k10.readALS()` | None | `int` | Reads onboard ambient light level. |
| `aht20.getData(type)` | `eAHT20TempC` / `eAHT20HumiRH` | `float` | Reads temperature or relative humidity. |
| `k10.canvas->canvasRectangle(...)` | Bounds & colors | `void` | Draws gauge tracks and fill levels. |

---

## Full Code

```cpp
#include "unihiker_k10.h"

UNIHIKER_K10 k10;
uint8_t screen_dir = 2; // Portrait orientation (240x320)
AHT20 aht20;

void drawLinearGauge(int x, int y, int width, int height, float value, float minVal, float maxVal, const char* label, const char* unit, uint32_t barColor) {
    // 1. Label and numeric readout
    k10.canvas->canvasText(label, x, y - 22, 0xFEE715,
                           k10.canvas->eCNAndENFont16, 20, false);

    String valStr = String(value, 1) + " " + unit;
    k10.canvas->canvasText(valStr, x + width - 60, y - 22, 0x00E5FF,
                           k10.canvas->eCNAndENFont16, 15, false);

    // 2. Track container
    k10.canvas->canvasRectangle(x, y, width, height, 0x334155, 0x111827, true);

    // 3. Dynamic bar
    float constrainedVal = constrain(value, minVal, maxVal);
    int fillWidth = (int)(((constrainedVal - minVal) / (maxVal - minVal)) * (width - 4));

    if (fillWidth > 0) {
        k10.canvas->canvasRectangle(x + 2, y + 2, fillWidth, height - 4, barColor, barColor, true);
    }
}

void setup() {
    k10.begin();
    k10.initScreen(screen_dir);
    k10.creatCanvas();
    k10.setScreenBackground(0x0F172A);

    k10.rgb->brightness(5);
    k10.rgb->write(-1, 0x00E5FF);
}

void loop() {
    // 1. Read onboard hardware sensors
    int lightALS = k10.readALS();                 // Light Sensor (0 - 4095)
    float tempC = aht20.getData(AHT20::eAHT20TempC); // Temp in Celsius (0 - 50 C)
    float humi = aht20.getData(AHT20::eAHT20HumiRH); // Humidity in % (0 - 100 %)

    k10.canvas->canvasClear();

    // 2. Header
    k10.canvas->canvasText("SENSOR GAUGES", 38, 14, 0xFEE715,
                           k10.canvas->eCNAndENFont24, 20, false);
    k10.canvas->canvasLine(15, 44, 225, 44, 0x334155);

    // 3. Render Gauges
    // Gauge 1: Ambient Light (0 - 4095 lux scaled)
    drawLinearGauge(20, 88, 200, 22, lightALS, 0, 4000, "Ambient Light", "lux", 0x38BDF8);

    // Gauge 2: Temperature (15 - 45 C)
    uint32_t tempColor = (tempC > 32.0) ? 0xFF4444 : ((tempC > 24.0) ? 0xFEE715 : 0x00FF88);
    drawLinearGauge(20, 155, 200, 22, tempC, 15.0, 45.0, "Temperature", "C", tempColor);

    // Gauge 3: Relative Humidity (20 - 90 %)
    drawLinearGauge(20, 222, 200, 22, humi, 20.0, 90.0, "Rel. Humidity", "%", 0x00E5FF);

    // 4. Footer
    k10.canvas->canvasLine(15, 275, 225, 275, 0x334155);
    k10.canvas->canvasText("Live Environmental Telemetry", 18, 288, 0x64748B,
                           k10.canvas->eCNAndENFont16, 30, false);

    k10.canvas->updateCanvas();
    delay(100);
}
```
