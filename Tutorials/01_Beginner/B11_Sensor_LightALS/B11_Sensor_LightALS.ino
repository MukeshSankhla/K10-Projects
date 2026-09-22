#include "unihiker_k10.h"

UNIHIKER_K10 k10;

void setup() {
    k10.begin();
    k10.initScreen(2);               // 2 = Portrait orientation
    k10.creatCanvas();               // Create canvas
    k10.setScreenBackground(0x000000); // Black background

    // Row 1: Title
    k10.canvas->canvasText("LIGHT SENSOR", 1, 0xFEE715);
    k10.canvas->updateCanvas();
}

void loop() {
    // Read onboard Ambient Light Sensor (0 - 1000+ lux)
    uint16_t lightLevel = k10.readALS();

    // Row 3: Display raw sensor reading
    k10.canvas->canvasText("Light: " + String(lightLevel) + " Lux", 3, 0x00E5FF);

    // Row 5: Display condition description
    if (lightLevel < 50) {
        k10.canvas->canvasText("State: Dark", 5, 0x888888);
    } else if (lightLevel < 300) {
        k10.canvas->canvasText("State: Dim Light", 5, 0xFFA500);
    } else if (lightLevel < 800) {
        k10.canvas->canvasText("State: Normal", 5, 0x00FF00);
    } else {
        k10.canvas->canvasText("State: Very Bright", 5, 0xFFFFFF);
    }

    // Refresh display
    k10.canvas->updateCanvas();
    delay(200);
}
