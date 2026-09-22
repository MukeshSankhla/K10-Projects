## What this tutorial is??

This tutorial demonstrates how to implement a wearable **Smart Pedometer (Step Counter)** on the UNIHIKER K10 using the onboard 3-axis accelerometer and dynamic threshold detection algorithms.

Features:
- **Big-Digit Step Counter**: Large 24px primary step count card.
- **Fitness Estimates**: Real-time distance calculation (km) and calorie expenditure estimation (kcal).
- **G-Force Activity Gauge**: Dynamic visual motion bar showing instantaneous gait acceleration impulses.
- **Button [A] (Reset)**: Resets the step counter to zero.

---

## How it works

1. **Step Detection Physics**:
   - As a person walks or jogs, foot impact creates periodic dynamic acceleration peaks in the net gravitational vector:
     $$\text{Strength} = \sqrt{A_x^2 + A_y^2 + A_z^2}$$
   - When resting, $\text{Strength} \approx 1000\,mg$ (1G).
   - During heel-strike, $\text{Strength}$ exceeds the walking threshold ($\ge 1150\,mg$).
2. **State Machine & Timing Debounce**:
   - To avoid counting high-frequency vibrations as multiple steps, the algorithm requires a minimum interval of 300 ms between registered steps (`MIN_STEP_INTERVAL = 300`).
   - A hysteresis guard resets the trigger only after acceleration drops below $1100\,mg$.

---

## API and Functions detailes

| API / Method | Arguments | Return Type | Description |
| :--- | :--- | :--- | :--- |
| `k10.getStrength()` | None | `int` | Net vector acceleration magnitude in $mg$. |
| `k10.buttonA->isPressed()` | None | `bool` | Resets the step counter. |
| `k10.canvas->canvasRectangle(...)` | Bounds & styling | `void` | Renders fitness dashboard cards. |

---

## Full Code

```cpp
#include "unihiker_k10.h"

UNIHIKER_K10 k10;
uint8_t screen_dir = 2; // Portrait orientation (240x320)

int stepCount = 0;
bool isHigh = false;
unsigned long lastStepTime = 0;
const int STEP_THRESHOLD = 1150; // Threshold above standard 1000mg 1G gravity
const unsigned long MIN_STEP_INTERVAL = 300; // Minimum 300ms between human steps

void renderPedometerUI() {
    k10.canvas->canvasClear();
    k10.setScreenBackground(0x0F172A);

    // 1. Header
    k10.canvas->canvasText("SMART PEDOMETER", 18, 14, 0xFEE715,
                           k10.canvas->eCNAndENFont24, 20, false);
    k10.canvas->canvasLine(15, 44, 225, 44, 0x334155);

    // 2. Large Central Step Count Card
    k10.canvas->canvasRectangle(15, 55, 210, 110, 0x38BDF8, 0x1E293B, true);
    k10.canvas->canvasText("TOTAL STEPS", 65, 68, 0x94A3B8,
                           k10.canvas->eCNAndENFont16, 20, false);

    String stepStr = String(stepCount);
    // Center alignment approximation
    int textX = 120 - (stepStr.length() * 7);
    k10.canvas->canvasText(stepStr, textX, 105, 0x00FF88,
                           k10.canvas->eCNAndENFont24, 15, false);

    // 3. Secondary Metrics Grid
    // Distance (0.75m per step average)
    float distanceKm = (stepCount * 0.75) / 1000.0;
    String distStr = String(distanceKm, 2) + " km";
    k10.canvas->canvasRectangle(15, 175, 100, 65, 0x334155, 0x1E293B, true);
    k10.canvas->canvasText("Distance", 30, 185, 0x94A3B8,
                           k10.canvas->eCNAndENFont16, 10, false);
    k10.canvas->canvasText(distStr, 25, 210, 0x00E5FF,
                           k10.canvas->eCNAndENFont16, 12, false);

    // Estimated Calories (0.04 kcal per step)
    float calories = stepCount * 0.04;
    String calStr = String((int)calories) + " kcal";
    k10.canvas->canvasRectangle(125, 175, 100, 65, 0x334155, 0x1E293B, true);
    k10.canvas->canvasText("Calories", 140, 185, 0x94A3B8,
                           k10.canvas->eCNAndENFont16, 10, false);
    k10.canvas->canvasText(calStr, 135, 210, 0xFEE715,
                           k10.canvas->eCNAndENFont16, 12, false);

    // 4. Live G-Force Motion Bar
    int strength = k10.getStrength();
    int barWidth = map(constrain(strength, 800, 1600), 800, 1600, 0, 206);
    k10.canvas->canvasRectangle(15, 250, 210, 14, 0x334155, 0x111827, true);
    uint32_t barColor = (strength > STEP_THRESHOLD) ? 0x00FF88 : 0x38BDF8;
    if (barWidth > 0) {
        k10.canvas->canvasRectangle(17, 252, barWidth, 10, barColor, barColor, true);
    }

    // 5. Footer Reset Prompt
    k10.canvas->canvasLine(15, 275, 225, 275, 0x334155);
    k10.canvas->canvasText("Press [A] to Reset Counter", 20, 288, 0x64748B,
                           k10.canvas->eCNAndENFont16, 26, false);

    k10.canvas->updateCanvas();
}

void setup() {
    k10.begin();
    k10.initScreen(screen_dir);
    k10.creatCanvas();
    k10.setScreenBackground(0x0F172A);

    k10.rgb->brightness(5);
    k10.rgb->write(-1, 0x00FF88);

    renderPedometerUI();
}

void loop() {
    int strength = k10.getStrength();
    unsigned long now = millis();

    // Step detection state machine with time debounce
    if (strength > STEP_THRESHOLD && !isHigh) {
        if (now - lastStepTime > MIN_STEP_INTERVAL) {
            stepCount++;
            lastStepTime = now;
            isHigh = true;
            renderPedometerUI();
        }
    } else if (strength < STEP_THRESHOLD - 50 && isHigh) {
        isHigh = false;
    }

    // Button A resets counter
    if (k10.buttonA->isPressed()) {
        stepCount = 0;
        renderPedometerUI();
        while (k10.buttonA->isPressed()) {
            delay(50);
        }
    }

    delay(20);
}
```
