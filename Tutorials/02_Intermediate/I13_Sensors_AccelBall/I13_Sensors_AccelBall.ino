#include "unihiker_k10.h"

UNIHIKER_K10 k10;
uint8_t screen_dir = 2; // Portrait orientation (240x320)
Music music;

// Ball physical state
float posX = 120.0;
float posY = 160.0;
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

void setup() {
    k10.begin();
    k10.initScreen(screen_dir);
    k10.creatCanvas();
    // Neon Pinball Arcade background
    k10.setScreenBackground(0x0A0618);

    k10.rgb->brightness(5);
    k10.rgb->write(-1, 0x00FF87);
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

    // 2. Render Graphics
    k10.canvas->canvasClear();

    // 1. Arcade Header Banner (Zero-overflow)
    k10.canvas->canvasRectangle(0, 0, 240, 42, 0x160D2E, 0x160D2E, true);
    k10.canvas->canvasLine(0, 42, 240, 42, 0xFF007F);
    k10.canvas->canvasText("PINBALL PHYSICS", 24, 10, 0xFFE600,
                           k10.canvas->eCNAndENFont24, 16, false);

    // 2. Arena container border with neon bumper rails
    k10.canvas->canvasRectangle(ARENA_MIN_X, ARENA_MIN_Y, ARENA_MAX_X - ARENA_MIN_X, ARENA_MAX_Y - ARENA_MIN_Y, 0x00FF87, 0x110822, true);

    // Arena center bonus bumper
    k10.canvas->canvasCircle(120, 162, 18, 0xFF007F, 0x220A33, true);
    k10.canvas->canvasCircle(120, 162, 6, 0xFFE600, 0xFFE600, true);

    // Rolling Pinball: Outer glow, core, and specular reflection
    k10.canvas->canvasCircle((int)posX, (int)posY, (int)BALL_RADIUS, 0xFFE600, 0xFFE600, true);
    k10.canvas->canvasCircle((int)posX - 3, (int)posY - 3, 2, 0xFFFFFF, 0xFFFFFF, true);

    // 3. Centered Speed Footer Telemetry (Zero-overflow)
    k10.canvas->canvasLine(15, 276, 225, 276, 0x241142);
    float currentSpeed = sqrt(velX * velX + velY * velY);
    String speedStr = "Speed: " + String((int)currentSpeed) + " px/s";
    int speedX = 120 - (int)(speedStr.length() * 4);
    k10.canvas->canvasText(speedStr, speedX, 290, 0x00F5D4,
                           k10.canvas->eCNAndENFont16, 20, false);

    k10.canvas->updateCanvas();
    delay(20);
}
