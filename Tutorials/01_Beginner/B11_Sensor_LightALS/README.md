## What this tutorial is??

This tutorial introduces **optical environmental sensing** on the UNIHIKER K10. 

You will learn how to read ambient illuminance levels using the onboard digital Ambient Light Sensor (ALS), quantify light intensity in **Lux**, categorize lighting environments into qualitative comfort tiers (Dark, Dim, Normal, Very Bright), and display live telemetry on the screen.

### Expected Behavior
- Row 1 displays the golden title `"LIGHT SENSOR"`.
- Row 3 continuously displays the real-time calibrated illuminance reading (e.g., `Light: 245 Lux`) in cyan (`0x00E5FF`).
- Row 5 displays a color-coded state classification that updates instantly when light levels change:
  - `< 50 Lux`: `State: Dark` (Grey `0x888888`)
  - `50 - 300 Lux`: `State: Dim Light` (Orange `0xFFA500`)
  - `300 - 800 Lux`: `State: Normal` (Green `0x00FF00`)
  - `> 800 Lux`: `State: Very Bright` (White `0xFFFFFF`)

---

## How it works

1. **The Photoelectric Effect in Silicon**:
   - The onboard ALS sensor contains a precision silicon photodiode. When incident photons of ambient light strike the semiconductor's depletion region, their energy excites valence electrons into the conduction band, generating a tiny, proportional photocurrent.
2. **Human Eye Photopic Response ($V(\lambda)$)**:
   - Cheap photoresistors (LDRs) are overly sensitive to invisible infrared and ultraviolet light, giving inaccurate readings under sunlight or incandescent bulbs.
   - The K10's digital ALS sensor incorporates optical filters and dual photodiode arrays (Visible + Infrared) to precisely match the CIE photopic spectral curve $V(\lambda)$, measuring light exactly as the human eye perceives brightness.
3. **Internal Analog-to-Digital Conversion (ADC)**:
   - An internal 16-bit Sigma-Delta ADC integrates the photocurrent over a selectable window (typically 100 ms), converting raw optical energy into a calibrated digital Lux value.
4. **I2C Bus Communication**:
   - The ESP32-S3 communicates with the ALS sensor (address `0x29`) over the shared I2C bus (`SDA = Pin 47`, `SCL = Pin 48`).
   - Calling `k10.readALS()` reads the high and low data registers, computes the lux equation, and returns an integer illuminance value.
5. **Threshold Categorization**:
   - An `if-else if` ladder compares the Lux value against standardized architectural lighting benchmarks, rendering the status on the display every 200 ms.

---

## Sensors/actuator detaile

### What is Lux ($\text{lx}$)?
**Lux** is the SI unit of illuminance and luminous emittance, measuring luminous flux per unit area:
$$1\text{ Lux} = 1\text{ Lumen per square meter } (1\text{ lx} = 1\text{ lm/m}^2)$$

Common real-world illuminance benchmarks:
- **0.001 - 0.02 Lux**: Clear night sky with Moonless starlight
- **0.25 - 1.0 Lux**: Full moon on a clear night
- **20 - 50 Lux**: Dimly lit hallway / living room at night
- **100 - 300 Lux**: Comfortable living room / classroom lighting
- **500 - 1,000 Lux**: Bright office / laboratory / electronics workbench
- **10,000 - 25,000 Lux**: Full daylight (indirect ambient sunlight)
- **32,000 - 100,000 Lux**: Direct summer sunlight

### Onboard LTR-303ALS Digital Ambient Light Sensor
- The UNIHIKER K10 integrates a high-performance **LTR-303ALS** light sensor:
  - **Dynamic Range**: 0.01 Lux to 64,000 Lux
  - **Interface**: $I^2C$ digital bus operating at 400 kHz Fast Mode
  - **Dual Channels**:
    - Channel 0: Visible light + Infrared
    - Channel 1: Pure Infrared
    - By subtracting Channel 1 from Channel 0, the sensor eliminates infrared interference from direct sunlight and heaters.

---

## Step by step function wise code break down

### 1. Library Inclusion & Screen Initialization
```cpp
#include "unihiker_k10.h"

UNIHIKER_K10 k10;

void setup() {
    k10.begin();
    k10.initScreen(2);               // 2 = Portrait orientation
    k10.creatCanvas();               // Create canvas
    k10.setScreenBackground(0x000000); // Black background

    k10.canvas->canvasText("LIGHT SENSOR", 1, 0xFEE715);
    k10.canvas->updateCanvas();
}
```
- `k10.begin()`: Initializes I2C buses and wakes up the LTR-303ALS sensor from sleep mode.
- Sets portrait mode, creates canvas buffer, paints black background, and renders title on row 1 in gold.

### 2. Live Sensor Reading in `loop()`
```cpp
void loop() {
    uint16_t lightLevel = k10.readALS();
    k10.canvas->canvasText("Light: " + String(lightLevel) + " Lux", 3, 0x00E5FF);
```
- `k10.readALS()`: Queries the I2C registers and returns current ambient light in Lux.
- `k10.canvas->canvasText(..., 3, ...)`: Formats and displays the reading on row 3 in cyan.

### 3. Threshold Decision Matrix & Screen Update
```cpp
    if (lightLevel < 50) {
        k10.canvas->canvasText("State: Dark", 5, 0x888888);
    } else if (lightLevel < 300) {
        k10.canvas->canvasText("State: Dim Light", 5, 0xFFA500);
    } else if (lightLevel < 800) {
        k10.canvas->canvasText("State: Normal", 5, 0x00FF00);
    } else {
        k10.canvas->canvasText("State: Very Bright", 5, 0xFFFFFF);
    }

    k10.canvas->updateCanvas();
    delay(200);
}
```
- Classifies lighting into Dark, Dim Light, Normal, or Very Bright.
- `updateCanvas()` flushes the canvas buffer to the physical LCD.
- `delay(200)` provides a 5 Hz update rate, smooth for live viewing without unnecessary I2C bus traffic.

---

## API and Functions detailes

| API / Method | Parameters | Return Type | Description |
| :--- | :--- | :--- | :--- |
| `k10.begin()` | None | `void` | Initializes hardware peripherals, clocks, and I2C sensor bus. |
| `k10.readALS()` | None | `uint16_t` | Reads ambient illuminance from the onboard LTR-303ALS sensor in Lux (0 - 64000+). |
| `k10.canvas->canvasText(text, row, color)` | `String text`, `uint8_t row`, `uint32_t color` | `void` | Renders text on row `1-13` with automatic background clearing. |
| `k10.canvas->updateCanvas()` | None | `void` | Pushes modified RAM frame buffer to the physical ST7789 display panel. |

---

## Full Code

```cpp
#include "unihiker_k10.h"

UNIHIKER_K10 k10;

void setup() {
    k10.begin();
    k10.initScreen(2);               // 2 = Portrait orientation
    k10.creatCanvas();               // Create canvas
    k10.setScreenBackground(0x000000); // Black background

    // Row 1: Title
    k10.canvas->canvasText("LIGHT SENSOR", 1, 0xFEE715);
    k10.canvas->updateCanvas();
}

void loop() {
    // Read onboard Ambient Light Sensor (0 - 1000+ lux)
    uint16_t lightLevel = k10.readALS();

    // Row 3: Display raw sensor reading
    k10.canvas->canvasText("Light: " + String(lightLevel) + " Lux", 3, 0x00E5FF);

    // Row 5: Display condition description
    if (lightLevel < 50) {
        k10.canvas->canvasText("State: Dark", 5, 0x888888);
    } else if (lightLevel < 300) {
        k10.canvas->canvasText("State: Dim Light", 5, 0xFFA500);
    } else if (lightLevel < 800) {
        k10.canvas->canvasText("State: Normal", 5, 0x00FF00);
    } else {
        k10.canvas->canvasText("State: Very Bright", 5, 0xFFFFFF);
    }

    // Refresh display
    k10.canvas->updateCanvas();
    delay(200);
}
```
