#include "unihiker_k10.h"

UNIHIKER_K10 k10;

void setup() {
    k10.begin();
    k10.initScreen(2); // Portrait orientation (240x320)
    k10.creatCanvas();
    k10.setScreenBackground(0x000000); // Clean black background

    // Header Title (24px font)
    k10.canvas->canvasText("UNIHIKER K10", 35, 30, 0xFEE715,
                           k10.canvas->eCNAndENFont24, 20, false);

    // Subtitle (24px font)
    k10.canvas->canvasText("Hello, World!", 35, 65, 0xFFFFFF,
                           k10.canvas->eCNAndENFont24, 20, false);

    // Section label (16px font)
    k10.canvas->canvasText("Level 1: Beginner", 25, 120, 0x00E5FF,
                           k10.canvas->eCNAndENFont16, 20, false);

    // Multi-line description text
    k10.canvas->canvasText("Ready to create amazing", 25, 150, 0xCCCCCC,
                           k10.canvas->eCNAndENFont16, 25, false);
    k10.canvas->canvasText("embedded projects!", 25, 175, 0xCCCCCC,
                           k10.canvas->eCNAndENFont16, 20, false);

    // Guide label
    k10.canvas->canvasText("System Uptime:", 25, 230, 0x94A3B8,
                           k10.canvas->eCNAndENFont16, 20, false);

    k10.canvas->updateCanvas();
}

void loop() {
    static int seconds = 0;
    seconds++;

    String timeStr = "Uptime: " + String(seconds) + " s";
    // Clear only the counter text area
    k10.canvas->clearLocalCanvas(25, 260, 190, 24);
    k10.canvas->canvasText(timeStr, 25, 260, 0x10B981,
                           k10.canvas->eCNAndENFont24, 20, false);
    k10.canvas->updateCanvas();

    delay(1000);
}
