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
const float BALL_RADIUS = 9.0;
const float FRICTION = 0.96;
const float BOUNCE = -0.75;

// Playing Arena boundaries (x: 10..230, y: 48..276)
const int ARENA_MIN_X = 10;
const int ARENA_MAX_X = 230;
const int ARENA_MIN_Y = 48;
const int ARENA_MAX_Y = 276;

// Clean Minimalist Light Theme Palette
const uint32_t COLOR_BG        = 0xF8FAFC; // Soft Slate Off-White
const uint32_t COLOR_CARD      = 0xFFFFFF; // Pure White Card Fill
const uint32_t COLOR_BORDER    = 0xE2E8F0; // Delicate 1px Border
const uint32_t COLOR_BORDER_PEG= 0xCBD5E1; // Peg Border
const uint32_t COLOR_TEXT_PRI  = 0x0F172A; // Deep Slate Charcoal
const uint32_t COLOR_TEXT_MUTED= 0x64748B; // Slate Muted Label
const uint32_t COLOR_SAPPHIRE  = 0x2563EB; // Sapphire Brand Accent
const uint32_t COLOR_AMBER     = 0xD97706; // Amber Peg Center

void drawCenterBumper() {
    k10.canvas->canvasCircle(120, 162, 18, COLOR_BORDER_PEG, COLOR_CARD, true);
    k10.canvas->canvasCircle(120, 162, 6, COLOR_AMBER, COLOR_AMBER, true);
}

void drawScreenChrome() {
    k10.canvas->canvasClear();
    k10.canvas->canvasRectangle(0, 0, 240, 320, COLOR_BG, COLOR_BG, true);

    // App Header Bar (y: 0 to 40)
    k10.canvas->canvasRectangle(0, 0, 240, 40, COLOR_CARD, COLOR_CARD, true);
    k10.canvas->canvasLine(0, 40, 240, 40, COLOR_BORDER);
    k10.canvas->canvasText("Marble Arena", 14, 12, COLOR_TEXT_PRI, k10.canvas->eCNAndENFont16, 50, false);
    // Sapphire brand dot
    k10.canvas->canvasCircle(224, 20, 4, COLOR_SAPPHIRE, COLOR_SAPPHIRE, true);

    // Arena container border
    k10.canvas->canvasRectangle(ARENA_MIN_X, ARENA_MIN_Y, ARENA_MAX_X - ARENA_MIN_X, ARENA_MAX_Y - ARENA_MIN_Y, COLOR_BORDER, COLOR_CARD, true);

    drawCenterBumper();

    // Footer Bar (y: 284 to 320)
    k10.canvas->canvasRectangle(0, 284, 240, 36, COLOR_CARD, COLOR_CARD, true);
    k10.canvas->canvasLine(0, 284, 240, 284, COLOR_BORDER);
    k10.canvas->canvasText("Tilt board to roll the marble", 14, 294, COLOR_TEXT_MUTED, k10.canvas->eCNAndENFont16, 50, false);
}

void updateDynamicEntities() {
    // Erase old ball position with pure white card color
    k10.canvas->canvasCircle((int)lastPosX, (int)lastPosY, (int)BALL_RADIUS + 2, COLOR_CARD, COLOR_CARD, true);

    // Repair center bumper if ball overlapped it
    float distLast = sqrt(pow(lastPosX - 120.0, 2) + pow(lastPosY - 162.0, 2));
    float distNew  = sqrt(pow(posX - 120.0, 2) + pow(posY - 162.0, 2));
    if (distLast < 32.0 || distNew < 32.0) {
        drawCenterBumper();
    }

    // Draw marble at new position
    k10.canvas->canvasCircle((int)posX, (int)posY, (int)BALL_RADIUS, COLOR_SAPPHIRE, COLOR_SAPPHIRE, true);
    k10.canvas->canvasCircle((int)posX - 2, (int)posY - 2, 3, COLOR_CARD, COLOR_CARD, true);

    lastPosX = posX;
    lastPosY = posY;
}

void setup() {
    k10.begin();
    k10.initScreen(screen_dir);
    k10.creatCanvas();
    k10.setScreenBackground(COLOR_BG);

    k10.rgb->brightness(5);
    k10.rgb->write(-1, 0x2563EB);

    drawScreenChrome();
    k10.canvas->updateCanvas();
}

void loop() {
    float ax = k10.getAccelerometerX() / 1000.0;
    float ay = k10.getAccelerometerY() / 1000.0;

    velX -= ax * 1.8;
    velY += ay * 1.8;

    velX *= FRICTION;
    velY *= FRICTION;

    posX += velX;
    posY += velY;

    bool collision = false;

    // Arena walls collision
    if (posX < (ARENA_MIN_X + BALL_RADIUS + 1)) {
        posX = ARENA_MIN_X + BALL_RADIUS + 1;
        velX *= BOUNCE;
        collision = true;
    }
    if (posX > (ARENA_MAX_X - BALL_RADIUS - 1)) {
        posX = ARENA_MAX_X - BALL_RADIUS - 1;
        velX *= BOUNCE;
        collision = true;
    }
    if (posY < (ARENA_MIN_Y + BALL_RADIUS + 1)) {
        posY = ARENA_MIN_Y + BALL_RADIUS + 1;
        velY *= BOUNCE;
        collision = true;
    }
    if (posY > (ARENA_MAX_Y - BALL_RADIUS - 1)) {
        posY = ARENA_MAX_Y - BALL_RADIUS - 1;
        velY *= BOUNCE;
        collision = true;
    }

    // Center Bumper collision
    float dx = posX - 120.0;
    float dy = posY - 162.0;
    float dist = sqrt((dx * dx) + (dy * dy));
    float minDist = 18.0 + BALL_RADIUS;

    if (dist < minDist && dist > 0.001) {
        float nx = dx / dist;
        float ny = dy / dist;

        posX = 120.0 + (nx * (minDist + 1.0));
        posY = 162.0 + (ny * (minDist + 1.0));

        float dot = (velX * nx) + (velY * ny);
        velX = (velX - 2.0 * dot * nx) * 0.85;
        velY = (velY - 2.0 * dot * ny) * 0.85;
        collision = true;
    }

    if (collision) {
        music.playTone(880, 20);
    }

    updateDynamicEntities();
    k10.canvas->updateCanvas();
    delay(25);
}
