#include "unihiker_k10.h"

UNIHIKER_K10 k10;

void setup() {
    k10.begin();
    k10.initScreen(2);               // 2 = Portrait orientation
    k10.creatCanvas();               // Create canvas
    k10.setScreenBackground(0x000000); // Black background

    // Row 1: Title
    k10.canvas->canvasText("6-AXIS ACCEL", 1, 0xFEE715);
    k10.canvas->updateCanvas();
}

void loop() {
    // Read raw 3-axis acceleration in milli-gravities (mG)
    int ax = k10.getAccelerometerX();
    int ay = k10.getAccelerometerY();
    int az = k10.getAccelerometerZ();

    // Row 3: X-axis acceleration
    k10.canvas->canvasText("Accel X: " + String(ax), 3, 0xFF5555);

    // Row 5: Y-axis acceleration
    k10.canvas->canvasText("Accel Y: " + String(ay), 5, 0x00FF00);

    // Row 7: Z-axis acceleration
    k10.canvas->canvasText("Accel Z: " + String(az), 7, 0x00E5FF);

    // Refresh display
    k10.canvas->updateCanvas();
    delay(100);
}
