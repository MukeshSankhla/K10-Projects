#include "unihiker_k10.h"

UNIHIKER_K10 k10;
uint8_t screen_dir = 2; // Portrait orientation (240x320)

int currentMode = 0; // 0 = Funnel Hyperboloid, 1 = Rotating Ray Web
float angle = 0;

void setup() {
    k10.begin();
    k10.initScreen(screen_dir);
    k10.creatCanvas();
    k10.setScreenBackground(0x050510);

    k10.rgb->brightness(5);
    k10.rgb->write(-1, 0x00FF88); // Emerald green aura
}

void loop() {
    k10.canvas->canvasClear();

    // 1. Header
    k10.canvas->canvasText("VECTOR LINE ART", 25, 14, 0xFEE715,
                           k10.canvas->eCNAndENFont24, 20, false);
    k10.canvas->canvasLine(15, 44, 225, 44, 0x334155);

    // 2. Draw Vector Graphic depending on mode
    if (currentMode == 0) {
        k10.canvas->canvasText("Pattern: Perspective Funnel", 20, 52, 0x94A3B8,
                               k10.canvas->eCNAndENFont16, 26, false);

        // Funnel hyperboloid line web
        for (int i = 0; i <= 240; i += 12) {
            k10.canvas->canvasLine(i, 75, 240 - i, 265, 0x00E5FF);
            k10.canvas->canvasLine(0, 75 + (i * 190 / 240), 240, 265 - (i * 190 / 240), 0xFF007F);
        }
    } else {
        k10.canvas->canvasText("Pattern: Dynamic Radial Star", 20, 52, 0x94A3B8,
                               k10.canvas->eCNAndENFont16, 26, false);

        int centerX = 120;
        int centerY = 170;
        int radius = 80;

        for (int i = 0; i < 24; i++) {
            float theta = angle + (i * (2 * PI / 24));
            int x1 = centerX + (int)(cos(theta) * radius);
            int y1 = centerY + (int)(sin(theta) * radius);

            int x2 = centerX + (int)(cos(theta + PI / 2) * (radius / 2));
            int y2 = centerY + (int)(sin(theta + PI / 2) * (radius / 2));

            k10.canvas->canvasLine(centerX, centerY, x1, y1, 0x38BDF8);
            k10.canvas->canvasLine(x1, y1, x2, y2, 0xFEE715);
        }
        angle += 0.04;
    }

    // 3. Footer Control Bar
    k10.canvas->canvasLine(15, 275, 225, 275, 0x334155);
    k10.canvas->canvasText("Press [A] to Toggle Pattern", 20, 288, 0x00FF88,
                           k10.canvas->eCNAndENFont16, 26, false);

    k10.canvas->updateCanvas();

    // Toggle pattern mode
    if (k10.buttonA->isPressed()) {
        currentMode = (currentMode + 1) % 2;
        while (k10.buttonA->isPressed()) {
            delay(50);
        }
    }

    delay(30);
}
