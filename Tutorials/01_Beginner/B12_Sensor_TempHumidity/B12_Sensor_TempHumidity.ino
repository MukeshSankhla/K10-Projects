#include "unihiker_k10.h"

UNIHIKER_K10 k10;
AHT20 aht20;

void setup() {
    k10.begin();
    k10.initScreen(2);               // 2 = Portrait orientation
    k10.creatCanvas();               // Create canvas
    k10.setScreenBackground(0x000000); // Black background

    // Row 1: Title
    k10.canvas->canvasText("AHT20 CLIMATE", 1, 0xFEE715);
    k10.canvas->updateCanvas();
}

void loop() {
    // Read temperature and humidity from onboard AHT20 sensor
    float tempC = aht20.getData(AHT20::eAHT20TempC);
    float tempF = aht20.getData(AHT20::eAHT20TempF);
    float humidity = aht20.getData(AHT20::eAHT20HumiRH);

    // Row 3: Temperature in Celsius
    k10.canvas->canvasText("Temp: " + String(tempC, 1) + " C", 3, 0xFF5555);

    // Row 5: Temperature in Fahrenheit
    k10.canvas->canvasText("Temp: " + String(tempF, 1) + " F", 5, 0xFFA500);

    // Row 7: Humidity percentage
    k10.canvas->canvasText("Humi: " + String(humidity, 1) + " %", 7, 0x00E5FF);

    // Refresh display
    k10.canvas->updateCanvas();
    delay(500);
}
