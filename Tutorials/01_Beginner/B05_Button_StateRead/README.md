## What this tutorial is??

This tutorial introduces digital input sensing and button reading on the UNIHIKER K10.

You will learn how to read physical tactile push buttons (Button A and Button B) and provide immediate interactive visual feedback using the onboard RGB LEDs.

### Expected Behavior
- **Idle (Neither Button Pressed)**: All onboard RGB LEDs are OFF.
- **Holding Button A**: Onboard RGB LEDs illuminate **Green** (`0x00FF00`).
- **Holding Button B**: Onboard RGB LEDs illuminate **Blue** (`0x0000FF`).
- **Releasing Buttons**: LEDs turn off immediately.

---

## How it works

1. **Digital Inputs & Logic Levels**:
   - A digital input pin senses only two distinct electrical states:
     - **HIGH (Logic 1)**: Near supply voltage ($\sim 3.3\text{V}$)
     - **LOW (Logic 0)**: Near ground reference ($0\text{V}$)
2. **Pull-Up Resistor Architecture**:
   - If a microcontroller pin is not connected to a fixed voltage, it acts as an antenna, randomly fluctuating between HIGH and LOW due to electromagnetic background noise (a state known as **floating**).
   - To prevent floating, an internal or external **pull-up resistor** connects the pin to 3.3V, keeping it steadily HIGH when idle.
   - When a tactile button is pressed, it bridges the pin directly to Ground (GND), pulling the logic level to LOW (0V). This is called **Active-LOW logic**.
3. **K10 Button Abstraction**:
   - The UNIHIKER K10 library abstracts this low-level voltage inversion. Calling `k10.buttonA->isPressed()` returns `true` whenever the user presses Button A, handling pin reading and active-low inversion automatically.
4. **Conditional Logic & Polling**:
   - In `loop()`, an `if-else if-else` structure polls Button A first, then Button B. If both buttons are released, the `else` clause switches the LEDs off.
   - A short `delay(20)` provides a simple software debounce filter to eliminate mechanical contact chatter.

---

## Sensors/actuator detaile

### What is a Push Button (Tactile Switch)?
A **push button** (or momentary tactile switch) is a mechanical device that temporarily connects two electrical contact points when physical pressure is applied to its actuator cap.

### How a Tactile Switch Works Internally
- Inside the switch housing is a slightly curved, spring-tempered metal dome resting above stationary metal contact pads.
- When you press the button, the central part of the flexible metal dome collapses downward, bridging the outer contact ring with the central pad to complete the electrical circuit.
- When you release your finger, the mechanical spring force of the dome snaps it back to its curved shape, breaking the circuit instantly.

```
   ACTUATOR (Button Plunger)
          ┌───┐
          │   │
  ────────┴───┴────────  Switch Housing
         \     /         Flexible Metal Dome (Open Circuit)
  ───[Pad A] [Pad B]───  PCB Contact Traces

  When pressed:
          │   │
  ────────┴───┴────────
        ───────          Dome collapses, bridging Pad A to Pad B (Closed Circuit)
  ───[Pad A]=[Pad B]───
```

### Switch Contact Bounce (Chatter) & Debouncing
- When the metal dome snaps down, the microscopic metal contacts do not make a clean, single connection. Instead, they physically bounce against each other for a few milliseconds, opening and closing the circuit dozens of times within 5 to 20 ms.
- If a microcontroller counts button presses without debouncing, a single physical press can be registered as 5 or 10 rapid presses!
- **Debouncing Solutions**:
  - **Hardware Debouncing**: An RC low-pass filter (resistor + capacitor) or a Schmitt trigger IC smooths out rapid voltage spikes.
  - **Software Debouncing**: Sampling the pin, waiting 10-30 ms for contacts to settle, and verifying the state before taking action (as implemented via `delay(20)` in this sketch).

### Onboard Buttons on UNIHIKER K10
- The UNIHIKER K10 features two prominent tactile switches on its front panel:
  - **Button A**: Located on the left side
  - **Button B**: Located on the right side
- Both buttons have built-in pull-up circuitry and hardware filtering for responsive, bounce-free input.

---

## Step by step function wise code break down

### 1. Library Inclusion & Initialization
```cpp
#include "unihiker_k10.h"

UNIHIKER_K10 k10;

void setup() {
    k10.begin();
    k10.rgb->brightness(5);
    k10.rgb->write(-1, 0x000000); // Start with LEDs OFF
}
```
- `k10.begin()`: Configures the GPIO inputs for Button A and Button B and enables internal pull-ups.
- `k10.rgb->brightness(5)`: Configures moderate LED brightness.
- `k10.rgb->write(-1, 0x000000)`: Ensures LEDs begin in the OFF state.

### 2. Button Polling & State Handling in `loop()`
```cpp
void loop() {
    if (k10.buttonA->isPressed()) {
        k10.rgb->write(-1, 0x00FF00); // Green
    }
    else if (k10.buttonB->isPressed()) {
        k10.rgb->write(-1, 0x0000FF); // Blue
    }
    else {
        k10.rgb->write(-1, 0x000000); // OFF
    }

    delay(20);
}
```
- `k10.buttonA->isPressed()`: Queries Button A. If held down, turns all LEDs Green (`0x00FF00`).
- `else if (k10.buttonB->isPressed())`: If Button A is not held but Button B is, turns all LEDs Blue (`0x0000FF`).
- `else`: When neither button is pressed, turns all LEDs off.
- `delay(20)`: Pauses execution for 20 ms, which dampens switch contact bounce and prevents CPU bus congestion.

---

## API and Functions detailes

| API / Method | Parameters | Return Type | Description |
| :--- | :--- | :--- | :--- |
| `k10.begin()` | None | `void` | Initializes board hardware, GPIO pins, and pull-up resistors. |
| `k10.buttonA->isPressed()` | None | `bool` | Returns `true` if Button A is physically pressed/held down, `false` if released. |
| `k10.buttonB->isPressed()` | None | `bool` | Returns `true` if Button B is physically pressed/held down, `false` if released. |
| `k10.rgb->write(index, color)` | `int8_t index`, `uint32_t color` | `void` | Writes 24-bit Hex RGB color to LED index (`-1` = all 3 LEDs). |
| `delay(ms)` | `unsigned long ms` | `void` | Debounce delay preventing switch bounce artifacts. |

---

## Full Code

```cpp
#include "unihiker_k10.h"

UNIHIKER_K10 k10;

void setup() {
    k10.begin();
    k10.rgb->brightness(5);
    k10.rgb->write(-1, 0x000000); // Start with LEDs OFF
}

void loop() {
    // When Button A is held, glow LEDs Green
    if (k10.buttonA->isPressed()) {
        k10.rgb->write(-1, 0x00FF00); // Green
    }
    // When Button B is held, glow LEDs Blue
    else if (k10.buttonB->isPressed()) {
        k10.rgb->write(-1, 0x0000FF); // Blue
    }
    // On release (neither button pressed), turn off LEDs
    else {
        k10.rgb->write(-1, 0x000000); // OFF
    }

    delay(20); // Small debounce delay
}
```
