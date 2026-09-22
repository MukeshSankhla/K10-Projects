#include "unihiker_k10.h"

UNIHIKER_K10 k10;
uint8_t screen_dir = 2; // Portrait orientation (240x320)

// Helper to convert RGB components into 24-bit Hex RGB
uint32_t rgbToColor(uint8_t r, uint8_t g, uint8_t b) {
    return ((uint32_t)r << 16) | ((uint32_t)g << 8) | (uint32_t)b;
}

// Structure to model a star in 3D projection
struct Star {
    float x;
    float y;
    float z;
};

const int NUM_STARS = 60;
Star stars[NUM_STARS];

void initStars() {
    for (int i = 0; i < NUM_STARS; i++) {
        stars[i].x = random(-120, 120);
        stars[i].y = random(-160, 160);
        stars[i].z = random(20, 240);
    }
}

void setup() {
    k10.begin();
    k10.initScreen(screen_dir);
    k10.creatCanvas();
    k10.setScreenBackground(0x050515);

    initStars();

    k10.rgb->brightness(5);
    k10.rgb->write(-1, 0x1E1B4B); // Deep purple aura
}

void loop() {
    k10.canvas->canvasClear();

    // 1. Header Banner
    k10.canvas->canvasText("DRAW POINT DEMO", 28, 14, 0xFEE715,
                           k10.canvas->eCNAndENFont24, 20, false);
    k10.canvas->canvasText("3D Starfield & Particle Points", 18, 42, 0x94A3B8,
                           k10.canvas->eCNAndENFont16, 30, false);
    k10.canvas->canvasLine(15, 62, 225, 62, 0x334155);

    // 2. Render 3D Starfield Points
    for (int i = 0; i < NUM_STARS; i++) {
        stars[i].z -= 4.0; // Move star closer to observer

        // Recycle star when it passes the screen plane
        if (stars[i].z <= 2.0) {
            stars[i].x = random(-120, 120);
            stars[i].y = random(-160, 160);
            stars[i].z = 240;
        }

        // Perspective 3D to 2D projection
        int screenX = 120 + (int)((stars[i].x * 120.0) / stars[i].z);
        int screenY = 170 + (int)((stars[i].y * 120.0) / stars[i].z);

        if (screenX >= 5 && screenX < 235 && screenY >= 70 && screenY < 280) {
            // Brightness attenuates with distance
            uint8_t brightness = map((int)stars[i].z, 2, 240, 255, 60);
            uint32_t starColor = rgbToColor(brightness, brightness, 255);

            // Draw primary point
            k10.canvas->canvasPoint(screenX, screenY, starColor);

            // If close, draw cross cluster for enhanced glow
            if (stars[i].z < 60) {
                k10.canvas->canvasPoint(screenX + 1, screenY, starColor);
                k10.canvas->canvasPoint(screenX - 1, screenY, starColor);
                k10.canvas->canvasPoint(screenX, screenY + 1, starColor);
                k10.canvas->canvasPoint(screenX, screenY - 1, starColor);
            }
        }
    }

    // 3. Footer Stats
    k10.canvas->canvasLine(15, 285, 225, 285, 0x334155);
    k10.canvas->canvasText("Active Points: 60  |  API: canvasPoint", 15, 295, 0x00E5FF,
                           k10.canvas->eCNAndENFont16, 30, false);

    k10.canvas->updateCanvas();
    delay(20);
}
