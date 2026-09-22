## What this tutorial is??

This is the quintessential "Hello, World!" of physical computing and embedded electronics. 

The tutorial introduces how to control the onboard addressable RGB LEDs on the UNIHIKER K10 board. You will learn how to configure the LED driver, set brightness, write saturated color values, and create a periodic blinking rhythm using software timing delays.

### Expected Behavior
All 3 onboard RGB LEDs illuminate simultaneously in bright red (`0xFF0000`) for exactly 1 second (1000 ms), extinguish completely (`0x000000`) for 1 second, and repeat indefinitely.

---

## How it works

1. **Power & Bus Initialization**:
   - The sketch calls `k10.begin()`, which powers up the board's internal power delivery rails and configures the hardware timers and GPIO pin mappings.
2. **Driver Configuration**:
   - The onboard RGB LEDs are driven via the ESP32-S3's high-speed RMT (Remote Control) peripheral connected to GPIO 46. Calling `k10.rgb->brightness(5)` establishes a safe, vivid duty cycle scale (0 to 9) across all three LED pixels.
3. **Color Transmission**:
   - In `loop()`, calling `k10.rgb->write(-1, 0xFF0000)` sends 24 bits of color data per LED across the single-wire digital data line. Passing an index of `-1` instructs the driver to broadcast the color to all three LEDs simultaneously.
4. **Timing Loop**:
   - `delay(1000)` pauses program execution for 1000 milliseconds while the LEDs remain lit. Then, `k10.rgb->write(-1, 0x000000)` sends zero intensity (black/off), followed by another 1000 millisecond delay. The loop repeats forever.

---

## Sensors/actuator detaile

### What is an LED?
An **LED (Light Emitting Diode)** is a specialized semiconductor p-n junction diode that emits incoherent light through a quantum phenomenon known as **electroluminescence**.

### How an LED Works (Physics & Electronics)
- When a forward voltage is applied across the p-n junction, electrons from the n-type semiconductor recombine with electron holes in the p-type region.
- As electrons drop from higher energy conduction bands to lower energy valence bands, they release energy in the form of **photons** (light particles).
- The bandgap energy of the semiconductor material determines the wavelength (color) of emitted light (e.g., Gallium Arsenide for Red/Infrared, Indium Gallium Nitride for Blue/Green).

### Physical Terminals & Polarity of Traditional Discrete LEDs
Standard through-hole LEDs are polarized components (current only flows in one direction, from positive to negative):
1. **Anode (+)**:
   - Positive terminal.
   - **Identification**: Noticeably **longer lead (leg)** on a new LED.
   - Must be connected to the positive voltage source or microcontroller GPIO pin.
2. **Cathode (-)**:
   - Negative terminal.
   - **Identification**: **Shorter lead (leg)**; also indicated by a **flat edge** or notch on the circular plastic epoxy lens rim.
   - Must be connected to Ground (GND).

```
         ┌─────────┐
         │ ( Lens )│
         └───┬─┬───┘
             │ │
             │ │  <-- Flat notch on rim indicates Cathode (-)
    Anode    │ │  Cathode
    (+)      │ │  (-)
    [Long]   │ └── [Short]
             │
```

### Forward Voltage & Current-Limiting Resistors
- LEDs have a characteristic **Forward Voltage ($V_f$)** (typically 1.8V - 2.2V for Red, 3.0V - 3.4V for Blue/White).
- An LED has near-zero internal resistance once turned on. Connecting an LED directly to a 3.3V or 5V power supply without a current limiter will cause an overcurrent surge that burns out the LED or the microcontroller pin.
- A **current-limiting resistor ($R$)** must always be placed in series using Ohm's Law:
  $$R = \frac{V_{supply} - V_f}{I_{forward}}$$
  *(For a 3.3V pin, a 2.0V red LED, and a safe 10mA current: $R = (3.3 - 2.0) / 0.010 = 130\,\Omega$)*.

### Onboard WS2812 Addressable RGB LEDs on UNIHIKER K10
- Rather than standard discrete LEDs, the UNIHIKER K10 features **3 onboard WS2812 addressable smart RGB LEDs** connected to **GPIO 46**.
- Each WS2812 package contains:
  1. A tiny silicon control integrated circuit (IC).
  2. Three microscopic LED dies (Red, Green, and Blue).
  3. Built-in constant-current drivers (no external resistors needed).
- The ESP32-S3 communicates with the LEDs using high-precision Non-Return-to-Zero (NZR) digital timing pulses at 800 kHz, allowing individual control of millions of colors over a single GPIO pin.

---

## Step by step function wise code break down

### 1. Library Inclusion & Global Object Instantiation
```cpp
#include "unihiker_k10.h"

UNIHIKER_K10 k10;
```
- `#include "unihiker_k10.h"`: Includes the master hardware header file containing peripheral classes (RGB, Display, Canvas, Audio, Sensors).
- `UNIHIKER_K10 k10;`: Instantiates the global singleton board controller object `k10`.

### 2. `setup()` Configuration
```cpp
void setup() {
    k10.begin();
    k10.rgb->brightness(5);
}
```
- `k10.begin()`: Initializes system clocks, GPIO pins, and power distribution circuits.
- `k10.rgb->brightness(5)`: Sets the global brightness multiplier to `5` on a scale from `0` (completely off) to `9` (maximum full intensity), providing vivid illumination without harsh glare.

### 3. `loop()` Execution
```cpp
void loop() {
    k10.rgb->write(-1, 0xFF0000);
    delay(1000);

    k10.rgb->write(-1, 0x000000);
    delay(1000);
}
```
- `k10.rgb->write(-1, 0xFF0000)`: Writes 24-bit Hex red (`0xFF0000`) to all LEDs (`-1` targets every onboard pixel).
- `delay(1000)`: Freezes execution for 1 second while maintaining the red illumination.
- `k10.rgb->write(-1, 0x000000)`: Writes RGB value 0 (all channels off).
- `delay(1000)`: Freezes execution for 1 second with LEDs dark before restarting the loop.

---

## API and Functions detailes

| API / Method | Parameters | Return Type | Description |
| :--- | :--- | :--- | :--- |
| `k10.begin()` | None | `void` | Initializes board hardware, system timers, and peripheral buses. |
| `k10.rgb->brightness(level)` | `uint8_t level` (`0` to `9`) | `void` | Sets the global brightness scaling factor for all onboard RGB LEDs. |
| `k10.rgb->write(index, color)` | `int8_t index`, `uint32_t color` | `void` | Sets pixel color. `index = -1` targets all LEDs; `0`, `1`, `2` target specific pixels. `color` is 24-bit Hex `0xRRGGBB`. |
| `delay(ms)` | `unsigned long ms` | `void` | Halts CPU program execution for the specified duration in milliseconds. |

---

## Full Code

```cpp
#include "unihiker_k10.h"

UNIHIKER_K10 k10;

void setup() {
    // Initialize K10 hardware and power management
    k10.begin();
    // Set brightness to a comfortable level (scale 0-9)
    k10.rgb->brightness(5);
}

void loop() {
    // Turn all 3 LEDs (-1 index) Red (0xFF0000)
    k10.rgb->write(-1, 0xFF0000);
    delay(1000);

    // Turn all 3 LEDs off (0x000000)
    k10.rgb->write(-1, 0x000000);
    delay(1000);
}
```
