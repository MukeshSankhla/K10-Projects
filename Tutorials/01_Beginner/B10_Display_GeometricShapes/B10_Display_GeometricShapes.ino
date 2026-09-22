#include "unihiker_k10.h"

UNIHIKER_K10 k10;

void setup() {
    k10.begin();
    k10.initScreen(2); // Portrait orientation (240x320)
    k10.creatCanvas();
    k10.setScreenBackground(0x000000); // Black background

    // Title
    k10.canvas->canvasText("Geometric Shapes", 1, 0x00F0FF);

    // 1. Line demonstration
    k10.canvas->canvasText("1. Line:", 3, 0xFFFFFF);
    k10.canvas->canvasLine(20, 90, 220, 90, 0xFF0055);

    // 2. Rectangle demonstration
    k10.canvas->canvasText("2. Rectangles:", 5, 0xFFFFFF);
    // Outline rectangle
    k10.canvas->canvasRectangle(20, 140, 90, 50, 0x00FF87, 0x000000, false);
    // Filled rectangle
    k10.canvas->canvasRectangle(130, 140, 90, 50, 0x00FF87, 0x00AA55, true);

    // 3. Circle demonstration
    k10.canvas->canvasText("3. Circles:", 9, 0xFFFFFF);
    // Outline circle (center at x=65, y=250, radius=25)
    k10.canvas->canvasCircle(65, 250, 25, 0xFFB703, 0x000000, false);
    // Filled circle (center at x=175, y=250, radius=25)
    k10.canvas->canvasCircle(175, 250, 25, 0xFFB703, 0xFFB703, true);

    k10.canvas->updateCanvas();
}

void loop() {
    // Static display
    delay(500);
}
