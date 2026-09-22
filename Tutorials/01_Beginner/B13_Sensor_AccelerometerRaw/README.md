## What this tutorial is??

This tutorial demonstrates how to read raw acceleration values from the onboard **SC7A20H 3-Axis Accelerometer (IMU)** and display them as simple text rows on the UNIHIKER K10 screen.

You will learn how to read acceleration across three spatial dimensions ($X$, $Y$, and $Z$) in milli-gravities ($\text{mG}$), read total acceleration vector magnitude, and display these dynamic values on the screen.

### Expected Behavior
- Displays a clean Cyan title on row 1: `"Accelerometer (IMU)"`.
- Rows 3-4: Live X-Axis acceleration in $\text{mG}$.
- Rows 5-6: Live Y-Axis acceleration in $\text{mG}$.
- Rows 7-8: Live Z-Axis acceleration in $\text{mG}$.
- Rows 10-11: Total acceleration strength in $\text{mG}$.
- Row 13: Helper text prompt: `"(Tilt board to test)"`.

---

## How it works

1. **MEMS Capacitive Accelerometer Physics**:
   - Inside the microscopic sensor die is a tiny **proof mass** (micro-machined silicon seismic mass) suspended by flexible polysilicon springs.
   - Extending from the proof mass are sets of interleaved capacitive finger combs that fit between stationary fixed fingers attached to the silicon substrate.
2. **Newton's Second Law of Motion ($F = ma$)**:
   - When the board accelerates (or when tilted relative to Earth's gravitational pull), the inertia of the proof mass causes it to deflect relative to the fixed frame.
   - The deflection shifts the microscopic distances ($d_1, d_2$) between the capacitor fingers:
     $$C_1 = \frac{\varepsilon A}{d_1}, \quad C_2 = \frac{\varepsilon A}{d_2}$$
   - An onboard Charge-to-Voltage converter translates this differential capacitance ($\Delta C = C_1 - C_2$) into an analog voltage directly proportional to acceleration.
3. **Milli-Gravities ($\text{mG}$) & Gravitational Vector**:
   - Earth's standard gravitational acceleration at sea level is:
     $$1\text{g} \approx 9.80665\text{ m/s}^2 = 1000\text{ mG (milli-gravities)}$$
   - When the UNIHIKER K10 lies flat on a desk facing upward:
     - The $X$-axis (horizontal lateral) experiences $0\text{ mG}$.
     - The $Y$-axis (vertical longitudinal) experiences $0\text{ mG}$.
     - The $Z$-axis (pointing perpendicular out of the glass) experiences $\approx +1000\text{ mG}$ ($+1\text{g}$) as it opposes Earth's gravity.
   - Tilting the board redistributes Earth's $1000\text{ mG}$ vector between the $X$, $Y$, and $Z$ axes according to trigonometry ($\sin\theta, \cos\theta$).
4. **I2C Bus Sampling**:
   - The ESP32-S3 queries the onboard accelerometer (SC7A20H / MSA311) over I2C at address `0x19`.
   - `k10.getAccelerometerX()`, `k10.getAccelerometerY()`, and `k10.getAccelerometerZ()` fetch 16-bit signed integer values expressed in calibrated $\text{mG}$.

---

## Sensors/actuator detaile

### 3-Axis Cartesian Coordinate Frame on UNIHIKER K10
When holding the UNIHIKER K10 upright in Portrait orientation (screen facing you, buttons at the bottom):
- **$X$-Axis**: Runs horizontally across the screen.
  - Tilting board to the **Right**: $X$ becomes positive ($+1000\text{ mG}$ when vertical).
  - Tilting board to the **Left**: $X$ becomes negative ($-1000\text{ mG}$ when vertical).
- **$Y$-Axis**: Runs vertically along the screen.
  - Tilting board **Downwards (Head down)**: $Y$ becomes negative.
  - Tilting board **Upwards (Normal standing)**: $Y$ becomes positive ($\approx +1000\text{ mG}$).
- **$Z$-Axis**: Points perpendicular directly out through the front glass.
  - Board lying **Flat on table screen up**: $Z \approx +1000\text{ mG}$.
  - Board lying **Face down screen on table**: $Z \approx -1000\text{ mG}$.

```
                 +Y (Top of Screen)
                  ▲
                  │
                  │
   -X ◄───────────┼───────────► +X (Right Edge)
  (Left)          │
                  │
                  ▼
                 -Y (Buttons A/B)

       Z-Axis points OUT of the screen toward you (+Z)
```

### Static vs. Dynamic Acceleration
- **Static Acceleration**: Constant gravitational force ($\approx 1\text{g}$). Used for tilt angles, roll/pitch calculation, and orientation detection (portrait vs landscape).
- **Dynamic Acceleration**: Forces caused by motion, tapping, shaking, impacts, or vehicle movement. Used for pedometers (step counters), fall detection, and gesture control.

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

    k10.canvas->canvasText("6-AXIS ACCEL", 1, 0xFEE715);
    k10.canvas->updateCanvas();
}
```
- Initializes board hardware, boots the accelerometer ASIC via I2C, sets up 240x320 portrait canvas, and writes header title on row 1.

### 2. Reading Accelerometer & Formatted Display in `loop()`
```cpp
void loop() {
    int ax = k10.getAccelerometerX();
    int ay = k10.getAccelerometerY();
    int az = k10.getAccelerometerZ();
```
- Fetches raw acceleration values in milli-gravities ($\text{mG}$) for all three spatial dimensions.

```cpp
    k10.canvas->canvasText("Accel X: " + String(ax), 3, 0xFF5555);
    k10.canvas->canvasText("Accel Y: " + String(ay), 5, 0x00FF00);
    k10.canvas->canvasText("Accel Z: " + String(az), 7, 0x00E5FF);

    k10.canvas->updateCanvas();
    delay(100);
}
```
- Renders $X$-axis in coral red (`0xFF5555`) on row 3.
- Renders $Y$-axis in vivid green (`0x00FF00`) on row 5.
- Renders $Z$-axis in cyan (`0x00E5FF`) on row 7.
- `updateCanvas()` pushes updates to the ST7789 display.
- `delay(100)` creates a smooth 10 Hz refresh rate.

---

## API and Functions detailes

| API / Method | Parameters | Return Type | Description |
| :--- | :--- | :--- | :--- |
| `k10.begin()` | None | `void` | Initializes board hardware, I2C buses, and accelerometer ASIC. |
| `k10.getAccelerometerX()` | None | `int` | Returns linear acceleration along the $X$-axis in milli-gravities ($\text{mG}$). |
| `k10.getAccelerometerY()` | None | `int` | Returns linear acceleration along the $Y$-axis in milli-gravities ($\text{mG}$). |
| `k10.getAccelerometerZ()` | None | `int` | Returns linear acceleration along the $Z$-axis in milli-gravities ($\text{mG}$). |
| `k10.canvas->canvasText(text, row, color)` | `String text`, `uint8_t row`, `uint32_t color` | `void` | Renders a text string on the designated row with auto background clearing. |
| `k10.canvas->updateCanvas()` | None | `void` | Transfers RAM canvas buffer to physical LCD controller. |

---

## Full Code

```cpp
#include "unihiker_k10.h"

UNIHIKER_K10 k10;

void setup() {
    k10.begin();
    k10.initScreen(2); // Portrait orientation (240x320)
    k10.creatCanvas();
    k10.setScreenBackground(0x000000); // Black background

    // Title
    k10.canvas->canvasText("Accelerometer (IMU)", 1, 0x00F0FF);
}

void loop() {
    int ax = k10.getAccelerometerX();
    int ay = k10.getAccelerometerY();
    int az = k10.getAccelerometerZ();
    int strength = k10.getStrength();

    k10.canvas->canvasText("Axis X:", 3, 0xFFFFFF);
    k10.canvas->canvasText(String(ax) + " mG", 4, 0xFF6B35);

    k10.canvas->canvasText("Axis Y:", 5, 0xFFFFFF);
    k10.canvas->canvasText(String(ay) + " mG", 6, 0x20BF55);

    k10.canvas->canvasText("Axis Z:", 7, 0xFFFFFF);
    k10.canvas->canvasText(String(az) + " mG", 8, 0x00F5D4);

    k10.canvas->canvasText("Total Strength:", 10, 0xFFFFFF);
    k10.canvas->canvasText(String(strength) + " mG", 11, 0xFEE715);

    k10.canvas->canvasText("Tilt board to test", 13, 0x888888);

    k10.canvas->updateCanvas();
    delay(100);
}
```
