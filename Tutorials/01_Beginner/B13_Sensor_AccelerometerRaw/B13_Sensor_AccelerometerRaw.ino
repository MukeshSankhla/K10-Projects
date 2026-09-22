#include "unihiker_k10.h"

UNIHIKER_K10 k10;

void setup() {
    k10.begin();
    k10.initScreen(2); // Portrait orientation (240x320)
    k10.creatCanvas();
    k10.setScreenBackground(0x000000); // Black background

    // Title
    k10.canvas->canvasText("Accelerometer (IMU)", 1, 0x00F0FF);
}

void loop() {
    int ax = k10.getAccelerometerX();
    int ay = k10.getAccelerometerY();
    int az = k10.getAccelerometerZ();
    int strength = k10.getStrength();

    k10.canvas->canvasText("Axis X:", 3, 0xFFFFFF);
    k10.canvas->canvasText(String(ax) + " mG", 4, 0xFF6B35);

    k10.canvas->canvasText("Axis Y:", 5, 0xFFFFFF);
    k10.canvas->canvasText(String(ay) + " mG", 6, 0x20BF55);

    k10.canvas->canvasText("Axis Z:", 7, 0xFFFFFF);
    k10.canvas->canvasText(String(az) + " mG", 8, 0x00F5D4);

    k10.canvas->canvasText("Total Strength:", 10, 0xFFFFFF);
    k10.canvas->canvasText(String(strength) + " mG", 11, 0xFEE715);

    k10.canvas->canvasText("Tilt board to test", 13, 0x888888);

    k10.canvas->updateCanvas();
    delay(100);
}
