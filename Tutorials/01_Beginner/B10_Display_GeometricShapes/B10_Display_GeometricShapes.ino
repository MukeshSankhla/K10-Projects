#include "unihiker_k10.h"

UNIHIKER_K10 k10;
uint8_t screen_dir = 2; // Portrait orientation (240x320)

void setup() {
    k10.begin();
    k10.initScreen(screen_dir);
    k10.creatCanvas();
    // Neon Synthwave Obsidian background
    k10.setScreenBackground(0x08071A);

    // 1. Cyberpunk Header Banner
    k10.canvas->canvasRectangle(0, 0, 240, 42, 0x150E2E, 0x150E2E, true);
    k10.canvas->canvasLine(0, 42, 240, 42, 0xFF007F);
    k10.canvas->canvasText("VECTOR SHAPES", 42, 10, 0x00F0FF,
                           k10.canvas->eCNAndENFont24, 15, false);

    // 2. Geometric Stage Arena (x=14, y=52, w=212, h=218)
    k10.canvas->canvasRectangle(14, 52, 212, 218, 0x2A194C, 0x100A24, true);

    // Decorative diagonals
    k10.canvas->canvasLine(14, 52, 226, 270, 0x1E1238);
    k10.canvas->canvasLine(226, 52, 14, 270, 0x1E1238);

    // Outer Neon Rounded Card
    k10.canvas->canvasRectangle(26, 64, 188, 56, 0x00F0FF, 0x0C223A, true);
    k10.canvas->canvasText("Linear Bounding Box", 38, 82, 0x00F0FF,
                           k10.canvas->eCNAndENFont16, 22, false);

    // Concentric Neon Circular Targets (Center at x=120, y=175)
    k10.canvas->canvasCircle(120, 175, 42, 0xFF007F, 0x000000, false);
    k10.canvas->canvasCircle(120, 175, 28, 0xBD00FF, 0x1D0630, true);
    k10.canvas->canvasCircle(120, 175, 14, 0xFFE600, 0xFFE600, true);
    k10.canvas->canvasCircle(120, 175, 5, 0x000000, 0x000000, true);

    // Lower Neon Lime Floating Card
    k10.canvas->canvasRectangle(26, 230, 188, 30, 0x39FF14, 0x0A2610, true);
    k10.canvas->canvasText("Raster Graphic Nodes", 44, 237, 0x39FF14,
                           k10.canvas->eCNAndENFont16, 22, false);

    // 3. Centered Footer Label (Zero-overflow)
    k10.canvas->canvasLine(15, 278, 225, 278, 0x2A194C);
    k10.canvas->canvasText("2D Vector Graphics Demo", 32, 290, 0x94A3B8,
                           k10.canvas->eCNAndENFont16, 24, false);

    k10.canvas->updateCanvas();

    // Magenta ambient LED
    k10.rgb->brightness(5);
    k10.rgb->write(-1, 0xFF007F);
}

void loop() {
    // Static vector shapes demonstration
    delay(200);
}
