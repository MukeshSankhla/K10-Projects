#include "unihiker_k10.h"

UNIHIKER_K10 k10;
AHT20 aht20;

void setup() {
    k10.begin();
    k10.initScreen(2); // Portrait orientation (240x320)
    k10.creatCanvas();
    k10.setScreenBackground(0x000000); // Black background

    // Title
    k10.canvas->canvasText("Temp & Humidity", 1, 0x00F0FF);
}

void loop() {
    float tempC = aht20.getData(AHT20::eAHT20TempC);
    float tempF = aht20.getData(AHT20::eAHT20TempF);
    float humidity = aht20.getData(AHT20::eAHT20HumiRH);

    k10.canvas->canvasText("Temperature (C):", 3, 0xFFFFFF);
    k10.canvas->canvasText(String(tempC, 1) + " C", 4, 0xFF6B6B);

    k10.canvas->canvasText("Temperature (F):", 6, 0xFFFFFF);
    k10.canvas->canvasText(String(tempF, 1) + " F", 7, 0xF59E0B);

    k10.canvas->canvasText("Humidity:", 9, 0xFFFFFF);
    k10.canvas->canvasText(String(humidity, 1) + " %", 10, 0x00B4D8);

    k10.canvas->updateCanvas();
    delay(500);
}
