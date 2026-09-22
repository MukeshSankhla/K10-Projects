## What this tutorial is??

This tutorial demonstrates an interactive 2D physics simulation of a **Rolling Ball with Elastic Boundary Collisions** governed by real-time gravitational vectors from the UNIHIKER K10 onboard accelerometer.

Features:
- **Gravity-Driven Motion**: Tilting the physical board accelerates the virtual ball across the screen.
- **Realistic Kinematics**: Implements viscous friction, drag, and restitution bounce coefficients.
- **Multimodal Feedback**: Emits an acoustic impact beep on the onboard buzzer and flashes the RGB LED red whenever the ball strikes an arena wall at high velocity.

---

## How it works

1. **Newtonian Mechanics Integration**:
   - Accelerometer readings $A_x, A_y$ act as continuous force vectors:
     $$v_x = (v_x + \frac{A_x}{120}) \cdot \mu$$
     $$v_y = (v_y + \frac{A_y}{120}) \cdot \mu$$
     where $\mu = 0.96$ is the damping friction coefficient.
   - Position updates by adding velocity:
     $$p_x = p_x + v_x, \quad p_y = p_y + v_y$$
2. **Restitution & Collision Inversion**:
   - When the ball touches an arena wall ($p_x - r < X_{\min}$), position is clamped and velocity reverses with an inelastic coefficient:
     $$v_x = v_x \cdot (-0.75)$$

---

## API and Functions detailes

| API / Method | Arguments | Return Type | Description |
| :--- | :--- | :--- | :--- |
| `k10.getAccelerometerX()` | None | `int` | Reads X-axis tilt acceleration. |
| `k10.getAccelerometerY()` | None | `int` | Reads Y-axis tilt acceleration. |
| `music.playTone(freq, dur)` | `uint16_t, uint32_t` | `void` | Generates impact sound upon wall collision. |
| `k10.canvas->canvasCircle(...)` | Coordinates & styling | `void` | Renders the rolling physics ball. |

---

## Full Code

```cpp
#include "unihiker_k10.h"

UNIHIKER_K10 k10;
uint8_t screen_dir = 2; // Portrait orientation (240x320)
Music music;

// Ball physical state
float posX = 120.0;
float posY = 160.0;
float velX = 0.0;
float velY = 0.0;
const float BALL_RADIUS = 12.0;
const float FRICTION = 0.96;
const float BOUNCE = -0.75; // Restitution coefficient

// Bounding box for the playing arena
const int ARENA_MIN_X = 15;
const int ARENA_MAX_X = 225;
const int ARENA_MIN_Y = 55;
const int ARENA_MAX_Y = 275;

void setup() {
    k10.begin();
    k10.initScreen(screen_dir);
    k10.creatCanvas();
    k10.setScreenBackground(0x0F172A);

    k10.rgb->brightness(5);
    k10.rgb->write(-1, 0x00FF88);
}

void loop() {
    // 1. Ingest tilt acceleration
    int accX = k10.getAccelerometerX();
    int accY = k10.getAccelerometerY();

    // Map tilt acceleration to applied physics force
    velX += (accX / 120.0);
    velY += (accY / 120.0);

    // Apply viscous damping friction
    velX *= FRICTION;
    velY *= FRICTION;

    // Integrate position
    posX += velX;
    posY += velY;

    bool collision = false;

    // Boundary collisions with restitution
    if (posX - BALL_RADIUS < ARENA_MIN_X) {
        posX = ARENA_MIN_X + BALL_RADIUS;
        velX *= BOUNCE;
        collision = true;
    } else if (posX + BALL_RADIUS > ARENA_MAX_X) {
        posX = ARENA_MAX_X - BALL_RADIUS;
        velX *= BOUNCE;
        collision = true;
    }

    if (posY - BALL_RADIUS < ARENA_MIN_Y) {
        posY = ARENA_MIN_Y + BALL_RADIUS;
        velY *= BOUNCE;
        collision = true;
    } else if (posY + BALL_RADIUS > ARENA_MAX_Y) {
        posY = ARENA_MAX_Y - BALL_RADIUS;
        velY *= BOUNCE;
        collision = true;
    }

    // Audio & optical bounce feedback
    if (collision && (abs(velX) > 1.5 || abs(velY) > 1.5)) {
        k10.rgb->write(-1, 0xFF4444);
        music.playTone(1200, 30);
    } else {
        k10.rgb->write(-1, 0x00FF88);
    }

    // 2. Render Graphics
    k10.canvas->canvasClear();

    // Header
    k10.canvas->canvasText("ACCEL BALL PHYSICS", 16, 14, 0xFEE715,
                           k10.canvas->eCNAndENFont24, 20, false);
    k10.canvas->canvasLine(15, 42, 225, 42, 0x334155);

    // Arena container border
    k10.canvas->canvasRectangle(ARENA_MIN_X, ARENA_MIN_Y, ARENA_MAX_X - ARENA_MIN_X, ARENA_MAX_Y - ARENA_MIN_Y, 0x00E5FF, 0x111827, true);

    // Arena center target
    k10.canvas->canvasCircle(120, 165, 20, 0x1E293B, 0x111827, false);
    k10.canvas->canvasLine(115, 165, 125, 165, 0x334155);
    k10.canvas->canvasLine(120, 160, 120, 170, 0x334155);

    // Rolling Ball: Outer glow and core
    k10.canvas->canvasCircle((int)posX, (int)posY, (int)BALL_RADIUS, 0x00FF88, 0x00FF88, true);
    k10.canvas->canvasCircle((int)posX - 3, (int)posY - 3, 3, 0xFFFFFF, 0xFFFFFF, true);

    // Footer stats
    k10.canvas->canvasLine(15, 285, 225, 285, 0x334155);
    String speedStr = "Speed: " + String((int)sqrt(velX * velX + velY * velY)) + " px/s  |  Tilt to roll";
    k10.canvas->canvasText(speedStr, 20, 296, 0x38BDF8,
                           k10.canvas->eCNAndENFont16, 26, false);

    k10.canvas->updateCanvas();
    delay(20);
}
```
