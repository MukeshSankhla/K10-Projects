## What this tutorial is??

This tutorial demonstrates how to build an integrated multi-widget **Sensors Dashboard** on the UNIHIKER K10 that simultaneously tracks environmental metrics and 3-axis motion kinematics.

Features:
- **4-Card Telemetry Grid**:
  - **Temperature Card**: Displays Celsius and Fahrenheit dual readouts.
  - **Humidity Card**: Displays relative humidity (%) with climate comfort indexing.
  - **Light Card**: Displays ambient illumination (lux) with indoor/outdoor tagging.
  - **IMU Strength Card**: Displays net gravitational acceleration magnitude ($mg$).
- **Kinematic Vector Panel**: Displays raw 3-axis accelerometer components ($X, Y, Z$ in $mg$).

---

## How it works

1. **Simultaneous Hardware Ingestion**:
   - Reads the AHT20 sensor over I2C for temperature and humidity.
   - Reads the ALS photodiode for ambient light.
   - Reads the 6-axis IMU for linear acceleration vectors $A_x, A_y, A_z$ and total scalar magnitude:
     $$G = \sqrt{A_x^2 + A_y^2 + A_z^2}$$
2. **Modular Card Component Layout**:
   - `drawSensorCard(x, y, w, h, title, val1, val2, color)` modularizes dashboard construction, drawing consistent card borders, accent stripes, and typography.

---

## API and Functions detailes

| API / Method | Arguments | Return Type | Description |
| :--- | :--- | :--- | :--- |
| `aht20.getData(...)` | Metric enum | `float` | Reads Temperature / Humidity. |
| `k10.readALS()` | None | `int` | Reads ambient light level. |
| `k10.getAccelerometerX()` | None | `int` | Reads X-axis acceleration ($mg$). |
| `k10.getAccelerometerY()` | None | `int` | Reads Y-axis acceleration ($mg$). |
| `k10.getAccelerometerZ()` | None | `int` | Reads Z-axis acceleration ($mg$). |
| `k10.getStrength()` | None | `int` | Reads net acceleration vector magnitude ($mg$). |

---

## Full Code

```cpp
#include "unihiker_k10.h"

UNIHIKER_K10 k10;
uint8_t screen_dir = 2; // Portrait orientation (240x320)
AHT20 aht20;

void drawSensorCard(int x, int y, int w, int h, const char* title, String value1, String value2, uint32_t accentColor) {
    // Card background & accent left border line
    k10.canvas->canvasRectangle(x, y, w, h, 0x334155, 0x1E293B, true);
    k10.canvas->canvasLine(x, y, x, y + h, accentColor);
    k10.canvas->canvasLine(x + 1, y, x + 1, y + h, accentColor);

    // Title
    k10.canvas->canvasText(title, x + 10, y + 6, 0x94A3B8,
                           k10.canvas->eCNAndENFont16, 16, false);

    // Primary Value
    k10.canvas->canvasText(value1, x + 10, y + 26, accentColor,
                           k10.canvas->eCNAndENFont16, 18, false);

    // Secondary Value (if present)
    if (value2.length() > 0) {
        k10.canvas->canvasText(value2, x + 10, y + 44, 0x64748B,
                               k10.canvas->eCNAndENFont16, 18, false);
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
    // Read hardware sensors
    float tempC = aht20.getData(AHT20::eAHT20TempC);
    float tempF = aht20.getData(AHT20::eAHT20TempF);
    float humi = aht20.getData(AHT20::eAHT20HumiRH);
    int lightALS = k10.readALS();
    int accX = k10.getAccelerometerX();
    int accY = k10.getAccelerometerY();
    int accZ = k10.getAccelerometerZ();
    int strength = k10.getStrength();

    k10.canvas->canvasClear();

    // 1. Dashboard Header
    k10.canvas->canvasText("SENSOR DASHBOARD", 18, 12, 0xFEE715,
                           k10.canvas->eCNAndENFont24, 20, false);
    k10.canvas->canvasLine(15, 40, 225, 40, 0x334155);

    // 2. Render 4 Sensor Telemetry Cards
    // Card 1: Temperature
    String tempStr1 = String(tempC, 1) + " C";
    String tempStr2 = String(tempF, 1) + " F";
    drawSensorCard(12, 50, 102, 68, "Temperature", tempStr1, tempStr2, 0xFF5555);

    // Card 2: Humidity
    String humiStr1 = String(humi, 1) + " %";
    String humiStr2 = (humi > 60.0) ? "Humid" : "Optimal";
    drawSensorCard(126, 50, 102, 68, "Humidity", humiStr1, humiStr2, 0x38BDF8);

    // Card 3: Ambient Light
    String lightStr1 = String(lightALS) + " lux";
    String lightStr2 = (lightALS > 1000) ? "Bright" : "Indoor";
    drawSensorCard(12, 126, 102, 68, "Light ALS", lightStr1, lightStr2, 0xFEE715);

    // Card 4: Accelerometer Vector Magnitude
    String strStr1 = String(strength) + " mg";
    String strStr2 = "G-Force Mag";
    drawSensorCard(126, 126, 102, 68, "IMU Strength", strStr1, strStr2, 0x00FF88);

    // 3. Wide IMU 3-Axis Breakdown Card
    k10.canvas->canvasRectangle(12, 202, 216, 68, 0x334155, 0x1E293B, true);
    k10.canvas->canvasText("3-Axis Motion Vectors", 22, 208, 0x94A3B8,
                           k10.canvas->eCNAndENFont16, 25, false);

    String axStr = "X: " + String(accX) + " mg";
    String ayStr = "Y: " + String(accY) + " mg";
    String azStr = "Z: " + String(accZ) + " mg";
    k10.canvas->canvasText(axStr, 22, 230, 0x00E5FF, k10.canvas->eCNAndENFont16, 15, false);
    k10.canvas->canvasText(ayStr, 22, 248, 0x38BDF8, k10.canvas->eCNAndENFont16, 15, false);
    k10.canvas->canvasText(azStr, 128, 230, 0xFEE715, k10.canvas->eCNAndENFont16, 15, false);

    // 4. Footer
    k10.canvas->canvasLine(15, 280, 225, 280, 0x334155);
    k10.canvas->canvasText("Real-Time Telemetry Node", 24, 292, 0x64748B,
                           k10.canvas->eCNAndENFont16, 26, false);

    k10.canvas->updateCanvas();
    delay(100);
}
```
