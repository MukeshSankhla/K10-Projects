## What this tutorial is??

This tutorial introduces **digital input sensing and non-blocking user interaction** on the UNIHIKER K10.

You will learn how to monitor physical tactile push buttons using non-blocking software debouncing, maintain interactive state counters, mirror states onto the onboard RGB LEDs, and render an **Interactive Dual-Button HUD** on the 2.8" color display.

### Expected Behavior
- **Deep Navy Dashboard**: The 240x320 screen displays dedicated telemetry cards for **Button A** and **Button B**.
- **Button A Interaction**: Pressing Button A increments the Button A counter, toggles its state indicator to active, and lights up the onboard LEDs in **Emerald Green** (`0x10B981`).
- **Button B Interaction**: Pressing Button B increments the Button B counter, toggles its state indicator to active, and lights up the onboard LEDs in **Cobalt Blue** (`0x3B82F6`).
- **Non-Blocking Architecture**: Debouncing occurs with zero busy-wait loops, preserving high frame rates for live telemetry.

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
uint8_t screen_dir = 2; // Portrait orientation (240x320)

int countA = 0;
int countB = 0;

// Non-blocking button edge-detection trackers
bool checkButtonAPressed() {
    static bool lastState = false;
    static unsigned long lastDebounceTime = 0;
    bool reading = k10.buttonA->isPressed();
    bool pressedEvent = false;

    if (reading != lastState) {
        lastDebounceTime = millis();
    }
    if ((millis() - lastDebounceTime) > 35) {
        static bool stableState = false;
        if (reading != stableState) {
            stableState = reading;
            if (stableState) {
                pressedEvent = true;
            }
        }
    }
    lastState = reading;
    return pressedEvent;
}

bool checkButtonBPressed() {
    static bool lastState = false;
    static unsigned long lastDebounceTime = 0;
    bool reading = k10.buttonB->isPressed();
    bool pressedEvent = false;

    if (reading != lastState) {
        lastDebounceTime = millis();
    }
    if ((millis() - lastDebounceTime) > 35) {
        static bool stableState = false;
        if (reading != stableState) {
            stableState = reading;
            if (stableState) {
                pressedEvent = true;
            }
        }
    }
    lastState = reading;
    return pressedEvent;
}

void renderButtonUI(bool heldA, bool heldB) {
    k10.canvas->canvasClear();
    // Deep Navy theme background
    k10.setScreenBackground(0x0B0F19);

    // 1. Header Banner
    k10.canvas->canvasRectangle(0, 0, 240, 42, 0x111928, 0x111928, true);
    k10.canvas->canvasLine(0, 42, 240, 42, 0x38BDF8);
    k10.canvas->canvasText("BUTTON MONITOR", 28, 10, 0x38BDF8,
                           k10.canvas->eCNAndENFont24, 16, false);

    // 2. Button A Visualizer Pod (Left: x=14, w=100)
    uint32_t borderA = heldA ? 0x10B981 : 0x1E293B;
    uint32_t bgA = heldA ? 0x064E3B : 0x0F172A;
    k10.canvas->canvasRectangle(14, 56, 100, 130, borderA, bgA, true);

    k10.canvas->canvasText("BUTTON [A]", 22, 68, 0x94A3B8,
                           k10.canvas->eCNAndENFont16, 12, false);

    k10.canvas->canvasCircle(64, 115, 22, borderA, heldA ? 0x10B981 : 0x1E293B, true);
    k10.canvas->canvasText(heldA ? "HELD" : "IDLE", heldA ? 48 : 50, 107, heldA ? 0xFFFFFF : 0x64748B,
                           k10.canvas->eCNAndENFont16, 6, false);

    String cntStrA = "Hits: " + String(countA);
    int cntXA = 64 - (int)(cntStrA.length() * 4);
    k10.canvas->canvasText(cntStrA, cntXA, 154, 0x10B981,
                           k10.canvas->eCNAndENFont16, 12, false);

    // 3. Button B Visualizer Pod (Right: x=126, w=100)
    uint32_t borderB = heldB ? 0x3B82F6 : 0x1E293B;
    uint32_t bgB = heldB ? 0x1E3A8A : 0x0F172A;
    k10.canvas->canvasRectangle(126, 56, 100, 130, borderB, bgB, true);

    k10.canvas->canvasText("BUTTON [B]", 134, 68, 0x94A3B8,
                           k10.canvas->eCNAndENFont16, 12, false);

    k10.canvas->canvasCircle(176, 115, 22, borderB, heldB ? 0x3B82F6 : 0x1E293B, true);
    k10.canvas->canvasText(heldB ? "HELD" : "IDLE", heldB ? 160 : 162, 107, heldB ? 0xFFFFFF : 0x64748B,
                           k10.canvas->eCNAndENFont16, 6, false);

    String cntStrB = "Hits: " + String(countB);
    int cntXB = 176 - (int)(cntStrB.length() * 4);
    k10.canvas->canvasText(cntStrB, cntXB, 154, 0x38BDF8,
                           k10.canvas->eCNAndENFont16, 12, false);

    // 4. Combined Status Card
    k10.canvas->canvasRectangle(14, 200, 212, 58, 0x1E293B, 0x0F172A, true);
    if (heldA && heldB) {
        k10.canvas->canvasText("DUAL COMBO TRIGGER!", 36, 218, 0xF59E0B,
                               k10.canvas->eCNAndENFont16, 22, false);
    } else if (heldA) {
        k10.canvas->canvasText("Button A Active (Green)", 32, 218, 0x10B981,
                               k10.canvas->eCNAndENFont16, 24, false);
    } else if (heldB) {
        k10.canvas->canvasText("Button B Active (Blue)", 36, 218, 0x38BDF8,
                               k10.canvas->eCNAndENFont16, 24, false);
    } else {
        k10.canvas->canvasText("Non-Blocking State: IDLE", 28, 218, 0x64748B,
                               k10.canvas->eCNAndENFont16, 24, false);
    }

    // 5. Centered Footer Bar (Zero-overflow)
    k10.canvas->canvasLine(15, 276, 225, 276, 0x1E293B);
    k10.canvas->canvasText("Press / Hold Buttons to Test", 18, 290, 0x64748B,
                           k10.canvas->eCNAndENFont16, 26, false);

    k10.canvas->updateCanvas();
}

void setup() {
    k10.begin();
    k10.initScreen(screen_dir);
    k10.creatCanvas();

    k10.rgb->brightness(5);
    k10.rgb->write(-1, 0x000000); // Start with LEDs OFF

    renderButtonUI(false, false);
}

void loop() {
    // Edge detection for press counts
    if (checkButtonAPressed()) {
        countA++;
    }
    if (checkButtonBPressed()) {
        countB++;
    }

    // Real-time held state polling
    bool heldA = k10.buttonA->isPressed();
    bool heldB = k10.buttonB->isPressed();

    // Mirror on onboard RGB LEDs
    if (heldA && heldB) {
        k10.rgb->write(-1, 0x00FFFF); // Cyan for both
    } else if (heldA) {
        k10.rgb->write(-1, 0x00FF00); // Green for A
    } else if (heldB) {
        k10.rgb->write(-1, 0x0000FF); // Blue for B
    } else {
        k10.rgb->write(-1, 0x000000); // OFF
    }

    renderButtonUI(heldA, heldB);
    delay(30); // Smooth 33 FPS non-blocking UI refresh
}
```
