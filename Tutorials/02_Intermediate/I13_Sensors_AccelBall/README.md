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
3. **Dynamic Partial Sprite Refresh**:
   - Static elements (arcade banner, arena boundary rails, center bumper, footer divider) are rendered once during `setup()` (`drawScreenChrome()`).
   - In `loop()`, only the old ball position is erased with the arena background color (`0x110822`), the center bumper is repaired if touched, the new ball is rendered, and the speed footer text is updated (`updateDynamicEntities()`). This provides silky smooth 50 FPS movement with zero full-screen clearing or flicker.

---

## API and Functions detailes

| API / Method | Arguments | Return Type | Description |
| :--- | :--- | :--- | :--- |
| `k10.getAccelerometerX()` | None | `int` | Reads X-axis tilt acceleration. |
| `k10.getAccelerometerY()` | None | `int` | Reads Y-axis tilt acceleration. |
| `music.playTone(freq, dur)` | `uint16_t, uint32_t` | `void` | Generates impact sound upon wall collision. |
| `k10.canvas->canvasCircle(...)` | Coordinates & styling | `void` | Renders the rolling physics ball and erases old sprite. |
| `k10.canvas->updateCanvas()` | None | `void` | Flushes canvas buffer to display. |

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
float lastPosX = 120.0;
float lastPosY = 160.0;
float velX = 0.0;
float velY = 0.0;
const float BALL_RADIUS = 10.0;
const float FRICTION = 0.96;
const float BOUNCE = -0.75; // Restitution coefficient

// Bounding box for the playing arena (212x220 px)
const int ARENA_MIN_X = 14;
const int ARENA_MAX_X = 226;
const int ARENA_MIN_Y = 52;
const int ARENA_MAX_Y = 272;

// Render center bumper widget
void drawCenterBumper() {
    k10.canvas->canvasCircle(120, 162, 18, 0xFF007F, 0x220A33, true);
    k10.canvas->canvasCircle(120, 162, 6, 0xFFE600, 0xFFE600, true);
}

// 1. Render static layout (Arcade banner, arena border, bumper, footer line) once
void drawScreenChrome() {
    // Arcade Header Banner (Zero-overflow)
    k10.canvas->canvasRectangle(0, 0, 240, 42, 0x160D2E, 0x160D2E, true);
    k10.canvas->canvasLine(0, 42, 240, 42, 0xFF007F);
    k10.canvas->canvasText("PINBALL PHYSICS", 24, 10, 0xFFE600,
                           k10.canvas->eCNAndENFont24, 16, false);

    // Arena container border with neon bumper rails
    k10.canvas->canvasRectangle(ARENA_MIN_X, ARENA_MIN_Y, ARENA_MAX_X - ARENA_MIN_X, ARENA_MAX_Y - ARENA_MIN_Y, 0x00FF87, 0x110822, true);

    // Arena center bonus bumper
    drawCenterBumper();

    // Footer divider line
    k10.canvas->canvasLine(15, 276, 225, 276, 0x241142);
}

// 2. Dynamic Partial Refresh: erase ball, draw ball, and update speed text
void updateDynamicEntities() {
    // Erase old ball position using arena background color (0x110822)
    k10.canvas->canvasCircle((int)lastPosX, (int)lastPosY, (int)BALL_RADIUS + 2, 0x110822, 0x110822, true);

    // If previous or current position intersected center bumper, repair bumper
    float distLast = sqrt(pow(lastPosX - 120.0, 2) + pow(lastPosY - 162.0, 2));
    float distNew = sqrt(pow(posX - 120.0, 2) + pow(posY - 162.0, 2));
    if (distLast < 32.0 || distNew < 32.0) {
        drawCenterBumper();
    }

    // Draw Rolling Pinball at new position
    k10.canvas->canvasCircle((int)posX, (int)posY, (int)BALL_RADIUS, 0xFFE600, 0xFFE600, true);
    k10.canvas->canvasCircle((int)posX - 3, (int)posY - 3, 2, 0xFFFFFF, 0xFFFFFF, true);

    // Erase and redraw speed footer telemetry
    k10.canvas->canvasRectangle(50, 286, 140, 24, 0x0A0618, 0x0A0618, true);
    float currentSpeed = sqrt(velX * velX + velY * velY);
    String speedStr = "Speed: " + String((int)currentSpeed) + " px/s";
    int speedX = 120 - (int)(speedStr.length() * 4);
    k10.canvas->canvasText(speedStr, speedX, 290, 0x00F5D4,
                           k10.canvas->eCNAndENFont16, 20, false);

    lastPosX = posX;
    lastPosY = posY;
}

void setup() {
    k10.begin();
    k10.initScreen(screen_dir);
    k10.creatCanvas();
    // Neon Pinball Arcade background
    k10.setScreenBackground(0x0A0618);

    k10.rgb->brightness(5);
    k10.rgb->write(-1, 0x00FF87);

    // Initial paint: static chrome + initial ball + speed
    drawScreenChrome();
    updateDynamicEntities();
    k10.canvas->updateCanvas();
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

    // Optical & Audio collision feedback
    if (collision && (abs(velX) > 1.5 || abs(velY) > 1.5)) {
        k10.rgb->write(-1, 0xFF007F); // Neon pink collision flash
        music.playTone(1200, 30);
    } else {
        k10.rgb->write(-1, 0x00FF87);
    }

    // 2. Dynamic Partial Refresh: update ball and speed readout without screen clear
    updateDynamicEntities();

    // Flush canvas without full-screen flicker
    k10.canvas->updateCanvas();
    delay(20);
}
```
