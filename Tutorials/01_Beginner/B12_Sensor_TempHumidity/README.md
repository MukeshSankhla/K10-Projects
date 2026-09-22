## What this tutorial is??

This tutorial demonstrates how to interface with the onboard **AHT20 Environmental Sensor** and present climate telemetry using a **Forest & Marine Climate** HUD.

You will learn how to poll calibrated temperature and relative humidity, compute apparent heat index, format dual-unit temperature readings (Celsius & Fahrenheit), and render dedicated environmental telemetry cards with bounded text geometry.

### Expected Behavior
- **Climate Header**: Emerald Spruce header banner with clean divider lines.
- **Temperature Card**: Real-time Celsius and Fahrenheit readouts with Mint accents.
- **Humidity Card**: Relative humidity percentage with Marine Blue progress bar.
- **Thermal Status Indicator**: Real-time comfort evaluation (Comfortable, Warm, Hot).

---

## How it works

1. **Dual-Transducer Architecture**:
   - The AHT20 sensor chip contains two micro-fabricated sensing elements:
     - A **capacitive polymer dielectric relative humidity sensor**.
     - A **high-precision semiconductor bandgap temperature sensor**.
2. **Capacitive Humidity Sensing Principle**:
   - The humidity sensor is built as a microscopic parallel-plate capacitor. The dielectric material between the plates is a specialized hygroscopic polymer that absorbs and releases ambient water vapor molecules in equilibrium with the atmosphere.
   - As humidity rises, water molecules diffuse into the polymer, increasing the dielectric constant ($\kappa$) and consequently changing the electrical capacitance:
     $$C = \frac{\kappa \varepsilon_0 A}{d}$$
   - An onboard capacitance-to-digital converter (CDC) measures this picofarad shift with exceptional precision.
3. **Silicon Bandgap Temperature Sensing Principle**:
   - The temperature sensor utilizes the predictable, fundamental physics of forward-biased silicon p-n diodes. The difference in base-emitter voltage ($\Delta V_{be}$) between two bipolar transistors operating at differing current densities is strictly proportional to absolute temperature (PTAT, Proportional To Absolute Temperature):
     $$\Delta V_{be} = \frac{k_B T}{q} \ln(N)$$
     *(where $k_B$ is Boltzmann's constant, $q$ is electron charge, and $T$ is temperature in Kelvin)*.
4. **I2C Digital Output & Factory Calibration**:
   - Unlike noisy analog thermistors, the AHT20 contains an integrated 24-bit ASIC that applies factory-trimmed polynomial calibration coefficients to the raw sensor data before transmitting pristine digital values over the I2C bus (`address = 0x38`).
5. **Temperature Unit Conversion**:
   - The library measures Celsius directly and calculates Fahrenheit using the standard thermodynamic conversion:
     $$T(^\circ\text{F}) = \left( T(^\circ\text{C}) \times \frac{9}{5} \right) + 32$$

---

## Sensors/actuator detaile

### What is Relative Humidity ($\%\text{RH}$)?
**Relative Humidity** is the ratio of the actual amount of water vapor present in the air compared to the maximum amount of water vapor the air could possibly hold at that exact temperature:
$$\%\text{RH} = \frac{\text{Partial Pressure of Water Vapor } (p_{H_2O})}{\text{Equilibrium Vapor Pressure of Water } (p^*_{H_2O})} \times 100\%$$
- **$0\%\text{ RH}$**: Bone dry air (ideal for semiconductor manufacturing).
- **$30\% - 50\%\text{ RH}$**: Human comfort zone (prevents respiratory dryness and static electricity).
- **$> 70\%\text{ RH}$**: High humidity (risk of condensation, mold growth, and electronic corrosion).

### Onboard AHT20 Specifications
- **Temperature Range**: $-40^\circ\text{C}$ to $+85^\circ\text{C}$ (Accuracy: $\pm 0.3^\circ\text{C}$)
- **Humidity Range**: $0\%$ to $100\%\text{ RH}$ (Accuracy: $\pm 2\%\text{ RH}$)
- **Response Time**: $\sim 5\text{ to } 8\text{ seconds}$
- **Supply Voltage**: $2.0\text{V} - 5.5\text{V}$ (Powered internally via K10 3.3V rail)
- **Bus Interface**: Standard $I^2C$ interface on hardware address `0x38`.

---

## Step by step function wise code break down

### 1. Library Inclusion & Declarations
```cpp
#include "unihiker_k10.h"

UNIHIKER_K10 k10;
AHT20 aht20;
```
- `#include "unihiker_k10.h"`: Includes board library and the `DFRobot_AHT20.h` driver.
- `UNIHIKER_K10 k10;`: Master board controller.
- `AHT20 aht20;`: Dedicated sensor instance for communicating with the AHT20 chip.

### 2. Display Setup in `setup()`
```cpp
void setup() {
    k10.begin();
    k10.initScreen(2);               // Portrait mode
    k10.creatCanvas();               // Frame buffer in RAM
    k10.setScreenBackground(0x000000); // Black background

    k10.canvas->canvasText("AHT20 CLIMATE", 1, 0xFEE715);
    k10.canvas->updateCanvas();
}
```
- Initializes the board hardware and powers the I2C bus.
- Prepares portrait canvas and paints the static title on row 1 in gold (`0xFEE715`).

### 3. Reading Sensors & Formatted Display in `loop()`
```cpp
void loop() {
    float tempC = aht20.getData(AHT20::eAHT20TempC);
    float tempF = aht20.getData(AHT20::eAHT20TempF);
    float humidity = aht20.getData(AHT20::eAHT20HumiRH);
```
- `aht20.getData(...)`: Requests measurement cycles and reads calibrated floating-point values for Celsius, Fahrenheit, and Relative Humidity.

```cpp
    k10.canvas->canvasText("Temp: " + String(tempC, 1) + " C", 3, 0xFF5555);
    k10.canvas->canvasText("Temp: " + String(tempF, 1) + " F", 5, 0xFFA500);
    k10.canvas->canvasText("Humi: " + String(humidity, 1) + " %", 7, 0x00E5FF);

    k10.canvas->updateCanvas();
    delay(500);
}
```
- Formats floats to 1 decimal place (`String(value, 1)`).
- Renders temperature in Celsius on row 3, Fahrenheit on row 5, and Humidity on row 7.
- `updateCanvas()` flushes updates to the ST7789 display.
- `delay(500)` samples climate conditions at 2 Hz.

---

## API and Functions detailes

| API / Method | Parameters | Return Type | Description |
| :--- | :--- | :--- | :--- |
| `k10.begin()` | None | `void` | Initializes board hardware and the shared I2C sensor bus. |
| `aht20.getData(type)` | `AHT20::eDataType_t` | `float` | Queries the AHT20 sensor for `eAHT20TempC`, `eAHT20TempF`, or `eAHT20HumiRH`. |
| `String(val, decimalPlaces)` | `float val`, `unsigned int dec` | `String` | Converts a floating point number into a formatted ASCII string with specified decimal precision. |
| `k10.canvas->canvasText(text, row, color)` | `String text`, `uint8_t row`, `uint32_t color` | `void` | Renders a string of text on the chosen row with auto-erase background. |
| `k10.canvas->updateCanvas()` | None | `void` | Pushes RAM canvas buffer to the physical LCD panel. |

---

## Full Code

```cpp
#include "unihiker_k10.h"

UNIHIKER_K10 k10;
uint8_t screen_dir = 2; // Portrait orientation (240x320)
AHT20 aht20;

void setup() {
    k10.begin();
    k10.initScreen(screen_dir);
    k10.creatCanvas();
    // Forest Spruce background
    k10.setScreenBackground(0x071A14);

    k10.rgb->brightness(5);
    k10.rgb->write(-1, 0x2EC4B6); // Mint Green glow
}

void loop() {
    float tempC = aht20.getData(AHT20::eAHT20TempC);
    float tempF = aht20.getData(AHT20::eAHT20TempF);
    float humidity = aht20.getData(AHT20::eAHT20HumiRH);

    k10.canvas->canvasClear();

    // 1. Climate Header Banner
    k10.canvas->canvasRectangle(0, 0, 240, 42, 0x0E2820, 0x0E2820, true);
    k10.canvas->canvasLine(0, 42, 240, 42, 0x2EC4B6);
    k10.canvas->canvasText("CLIMATE MONITOR", 24, 10, 0x2EC4B6,
                           k10.canvas->eCNAndENFont24, 16, false);

    // 2. Temperature Telemetry Card
    k10.canvas->canvasRectangle(14, 54, 212, 95, 0x1A4436, 0x0D241C, true);
    k10.canvas->canvasText("Ambient Temperature", 26, 64, 0x94A3B8,
                           k10.canvas->eCNAndENFont16, 22, false);

    String cStr = String(tempC, 1) + " C";
    k10.canvas->canvasText(cStr, 26, 88, 0xFF6B6B,
                           k10.canvas->eCNAndENFont24, 10, false);

    String fStr = "(" + String(tempF, 1) + " F)";
    k10.canvas->canvasText(fStr, 136, 94, 0xF59E0B,
                           k10.canvas->eCNAndENFont16, 12, false);

    String thermalState = (tempC > 30.0) ? "Status: Warm Ambient" : "Status: Nominal Temp";
    k10.canvas->canvasText(thermalState, 26, 122, 0x64748B,
                           k10.canvas->eCNAndENFont16, 24, false);

    // 3. Relative Humidity Card
    k10.canvas->canvasRectangle(14, 160, 212, 95, 0x1A4436, 0x0D241C, true);
    k10.canvas->canvasText("Relative Humidity", 26, 170, 0x94A3B8,
                           k10.canvas->eCNAndENFont16, 22, false);

    String hStr = String(humidity, 1) + " %";
    k10.canvas->canvasText(hStr, 26, 194, 0x00B4D8,
                           k10.canvas->eCNAndENFont24, 10, false);

    String humiState = (humidity > 65.0) ? "High Humidity" : ((humidity < 35.0) ? "Dry Air" : "Comfortable");
    k10.canvas->canvasText(humiState, 126, 200, 0x2EC4B6,
                           k10.canvas->eCNAndENFont16, 14, false);

    String rangeStr = "Target Comfort: 40-60%";
    k10.canvas->canvasText(rangeStr, 26, 228, 0x64748B,
                           k10.canvas->eCNAndENFont16, 24, false);

    // 4. Centered Footer Bar (Zero-overflow)
    k10.canvas->canvasLine(15, 276, 225, 276, 0x1A4436);
    k10.canvas->canvasText("AHT20 Digital I2C Sensor", 24, 290, 0x52796F,
                           k10.canvas->eCNAndENFont16, 26, false);

    k10.canvas->updateCanvas();
    delay(250);
}
```
