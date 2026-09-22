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
2. **Dynamic Partial Component Refresh**:
   - The mission control header banner, footer line, 4 sensor pod containers, and IMU pod frame are drawn once in `setup()` (`drawScreenChrome()`).
   - In `loop()`, only the inner value region of each pod (`updateSensorCardValues()`) and the IMU vector coordinates (`updateIMUVectors()`) are erased and redrawn, preserving all background frames, titles, and layout borders without screen flicker.

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
| `k10.canvas->updateCanvas()` | None | `void` | Flushes canvas buffer to display. |

---

## Full Code

```cpp
#include "unihiker_k10.h"

UNIHIKER_K10 k10;
uint8_t screen_dir = 2; // Portrait orientation (240x320)
AHT20 aht20;

// 1. Draw static sensor card container (border, background, title, accent strip) once
void drawStaticSensorCard(int x, int y, int w, int h, const char* title, uint32_t accentColor) {
    // Card background & left accent border
    k10.canvas->canvasRectangle(x, y, w, h, 0x1E293B, 0x101726, true);
    k10.canvas->canvasLine(x, y, x, y + h, accentColor);
    k10.canvas->canvasLine(x + 1, y, x + 1, y + h, accentColor);

    // Card Title
    k10.canvas->canvasText(title, x + 8, y + 6, 0x94A3B8,
                           k10.canvas->eCNAndENFont16, 12, false);
}

// 2. Dynamic Partial Refresh: update ONLY primary and secondary sensor values inside card
void updateSensorCardValues(int x, int y, int w, const String& value1, const String& value2, uint32_t accentColor) {
    // Clear value region inside card (width w-4, height 38) with card background color 0x101726
    k10.canvas->canvasRectangle(x + 3, y + 24, w - 5, 38, 0x101726, 0x101726, true);

    // Primary Value
    k10.canvas->canvasText(value1, x + 8, y + 26, accentColor,
                           k10.canvas->eCNAndENFont16, 14, false);

    // Secondary Value / Condition Badge
    if (value2.length() > 0) {
        k10.canvas->canvasText(value2, x + 8, y + 44, 0x64748B,
                               k10.canvas->eCNAndENFont16, 12, false);
    }
}

// 3. Draw static IMU pod container once
void drawIMUPodStaticChrome() {
    k10.canvas->canvasRectangle(14, 198, 212, 66, 0x1E293B, 0x101726, true);
    k10.canvas->canvasText("3-Axis Vectors (mG)", 24, 204, 0x94A3B8,
                           k10.canvas->eCNAndENFont16, 22, false);
}

// 4. Dynamic Partial Refresh: update ONLY IMU 3-axis vector strings
void updateIMUVectors(int accX, int accY, int accZ) {
    // Clear IMU vector numbers area
    k10.canvas->canvasRectangle(16, 222, 208, 40, 0x101726, 0x101726, true);

    String axStr = "X: " + String(accX);
    String ayStr = "Y: " + String(accY);
    String azStr = "Z: " + String(accZ);
    k10.canvas->canvasText(axStr, 24, 226, 0x00F0FF, k10.canvas->eCNAndENFont16, 12, false);
    k10.canvas->canvasText(ayStr, 24, 244, 0x38BDF8, k10.canvas->eCNAndENFont16, 12, false);
    k10.canvas->canvasText(azStr, 126, 226, 0xFACC15, k10.canvas->eCNAndENFont16, 12, false);
}

// Render static screen layout (Header banner, footer line, and card containers) once
void drawScreenChrome() {
    // 1. Mission Control Header Banner
    k10.canvas->canvasRectangle(0, 0, 240, 42, 0x0F1524, 0x0F1524, true);
    k10.canvas->canvasLine(0, 42, 240, 42, 0x00F0FF);
    k10.canvas->canvasText("TELEMETRY HUD", 46, 10, 0x00F0FF,
                           k10.canvas->eCNAndENFont24, 15, false);

    // 2. 4 Symmetrical Sensor Pods
    drawStaticSensorCard(14, 50, 100, 66, "Temp", 0xFF5555);
    drawStaticSensorCard(126, 50, 100, 66, "Humidity", 0x38BDF8);
    drawStaticSensorCard(14, 124, 100, 66, "Light", 0xFACC15);
    drawStaticSensorCard(126, 124, 100, 66, "G-Force", 0x00FF9D);

    // 3. IMU Pod Container
    drawIMUPodStaticChrome();

    // 4. Centered Footer Bar (Zero-overflow)
    k10.canvas->canvasLine(15, 276, 225, 276, 0x1E293B);
    k10.canvas->canvasText("Mission Control Node", 42, 290, 0x64748B,
                           k10.canvas->eCNAndENFont16, 24, false);
}

void setup() {
    k10.begin();
    k10.initScreen(screen_dir);
    k10.creatCanvas();
    // Spacecraft Dark Void background
    k10.setScreenBackground(0x080B14);

    k10.rgb->brightness(5);
    k10.rgb->write(-1, 0x00F0FF); // Cyan HUD glow

    // Initial paint: static chrome + initial telemetry readings
    drawScreenChrome();

    float tempC = aht20.getData(AHT20::eAHT20TempC);
    float humi = aht20.getData(AHT20::eAHT20HumiRH);
    int lightALS = k10.readALS();
    int strength = k10.getStrength();

    updateSensorCardValues(14, 50, 100, String(tempC, 1) + " C", (tempC > 30.0) ? "Warm" : "Nominal", 0xFF5555);
    updateSensorCardValues(126, 50, 100, String(humi, 1) + " %", (humi > 60.0) ? "Humid" : "Optimal", 0x38BDF8);
    updateSensorCardValues(14, 124, 100, String(lightALS) + " lx", (lightALS > 1000) ? "Bright" : "Indoor", 0xFACC15);
    updateSensorCardValues(126, 124, 100, String(strength) + " mg", "1.0G Ref", 0x00FF9D);

    updateIMUVectors(k10.getAccelerometerX(), k10.getAccelerometerY(), k10.getAccelerometerZ());

    k10.canvas->updateCanvas();
}

void loop() {
    // Read hardware sensors
    float tempC = aht20.getData(AHT20::eAHT20TempC);
    float humi = aht20.getData(AHT20::eAHT20HumiRH);
    int lightALS = k10.readALS();
    int accX = k10.getAccelerometerX();
    int accY = k10.getAccelerometerY();
    int accZ = k10.getAccelerometerZ();
    int strength = k10.getStrength();

    // Dynamic Partial Refresh: update ONLY sensor card values and IMU vectors
    String tempStr1 = String(tempC, 1) + " C";
    String tempStr2 = (tempC > 30.0) ? "Warm" : "Nominal";
    updateSensorCardValues(14, 50, 100, tempStr1, tempStr2, 0xFF5555);

    String humiStr1 = String(humi, 1) + " %";
    String humiStr2 = (humi > 60.0) ? "Humid" : "Optimal";
    updateSensorCardValues(126, 50, 100, humiStr1, humiStr2, 0x38BDF8);

    String lightStr1 = String(lightALS) + " lx";
    String lightStr2 = (lightALS > 1000) ? "Bright" : "Indoor";
    updateSensorCardValues(14, 124, 100, lightStr1, lightStr2, 0xFACC15);

    String strStr1 = String(strength) + " mg";
    updateSensorCardValues(126, 124, 100, strStr1, "1.0G Ref", 0x00FF9D);

    updateIMUVectors(accX, accY, accZ);

    // Flush canvas without full-screen flicker
    k10.canvas->updateCanvas();
    delay(100);
}
```
