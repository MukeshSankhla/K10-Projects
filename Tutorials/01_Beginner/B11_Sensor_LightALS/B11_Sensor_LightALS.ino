#include "unihiker_k10.h"

UNIHIKER_K10 k10;

void setup() {
    k10.begin();
    k10.initScreen(2); // Portrait orientation (240x320)
    k10.creatCanvas();
    k10.setScreenBackground(0x000000); // Black background

    // Title on Row 1
    k10.canvas->canvasText("Light Sensor (ALS)", 1, 0x00F0FF);
}

void loop() {
    uint16_t lightLevel = k10.readALS();

    // Display Lux reading
    k10.canvas->canvasText("Ambient Light:", 3, 0xFFFFFF);
    k10.canvas->canvasText(String(lightLevel) + " Lux", 4, 0xFFE600);

    // Display Environment State
    k10.canvas->canvasText("Environment:", 6, 0xFFFFFF);
    if (lightLevel < 50) {
        k10.canvas->canvasText("Dark / Night", 7, 0x94A3B8);
    } else if (lightLevel < 500) {
        k10.canvas->canvasText("Indoors / Normal", 7, 0x00FF87);
    } else {
        k10.canvas->canvasText("Bright / Sunlight", 7, 0xFFB703);
    }

    k10.canvas->canvasText("Cover sensor to test", 10, 0x888888);

    k10.canvas->updateCanvas();
    delay(200);
}
