## What this tutorial is??

This tutorial demonstrates how to read temperature and relative humidity from the onboard **AHT20 Environmental Sensor** and display the data as clear, beginner-friendly text rows on the UNIHIKER K10 screen.

You will learn how to initialize the AHT20 sensor driver, read temperature in both Celsius and Fahrenheit, read relative humidity percentage, and display formatted text values without complex UI cards.

### Expected Behavior
- Displays a clean Cyan title on row 1: `"Temp & Humidity"`.
- Row 3-4: Current Temperature in Celsius (`°C`).
- Row 6-7: Current Temperature in Fahrenheit (`°F`).
- Row 9-10: Current Relative Humidity (`%`).

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
AHT20 aht20;

void setup() {
    k10.begin();
    k10.initScreen(2); // Portrait orientation (240x320)
    k10.creatCanvas();
    k10.setScreenBackground(0x000000); // Black background

    // Title
    k10.canvas->canvasText("Temp & Humidity", 1, 0x00F0FF);
}

void loop() {
    float tempC = aht20.getData(AHT20::eAHT20TempC);
    float tempF = aht20.getData(AHT20::eAHT20TempF);
    float humidity = aht20.getData(AHT20::eAHT20HumiRH);

    k10.canvas->canvasText("Temperature (C):", 3, 0xFFFFFF);
    k10.canvas->canvasText(String(tempC, 1) + " C", 4, 0xFF6B6B);

    k10.canvas->canvasText("Temperature (F):", 6, 0xFFFFFF);
    k10.canvas->canvasText(String(tempF, 1) + " F", 7, 0xF59E0B);

    k10.canvas->canvasText("Humidity:", 9, 0xFFFFFF);
    k10.canvas->canvasText(String(humidity, 1) + " %", 10, 0x00B4D8);

    k10.canvas->updateCanvas();
    delay(500);
}
```
