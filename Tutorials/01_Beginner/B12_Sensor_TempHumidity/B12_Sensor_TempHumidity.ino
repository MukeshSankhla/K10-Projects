#include "unihiker_k10.h"

UNIHIKER_K10 k10;
uint8_t screen_dir = 2; // Portrait orientation (240x320)
AHT20 aht20;

void setup() {
    k10.begin();
    k10.initScreen(screen_dir);
    k10.creatCanvas();
    // Forest Spruce background
    k10.setScreenBackground(0x071A14);

    k10.rgb->brightness(5);
    k10.rgb->write(-1, 0x2EC4B6); // Mint Green glow
}

void loop() {
    float tempC = aht20.getData(AHT20::eAHT20TempC);
    float tempF = aht20.getData(AHT20::eAHT20TempF);
    float humidity = aht20.getData(AHT20::eAHT20HumiRH);

    k10.canvas->canvasClear();

    // 1. Climate Header Banner
    k10.canvas->canvasRectangle(0, 0, 240, 42, 0x0E2820, 0x0E2820, true);
    k10.canvas->canvasLine(0, 42, 240, 42, 0x2EC4B6);
    k10.canvas->canvasText("CLIMATE MONITOR", 24, 10, 0x2EC4B6,
                           k10.canvas->eCNAndENFont24, 16, false);

    // 2. Temperature Telemetry Card
    k10.canvas->canvasRectangle(14, 54, 212, 95, 0x1A4436, 0x0D241C, true);
    k10.canvas->canvasText("Ambient Temperature", 26, 64, 0x94A3B8,
                           k10.canvas->eCNAndENFont16, 22, false);

    String cStr = String(tempC, 1) + " C";
    k10.canvas->canvasText(cStr, 26, 88, 0xFF6B6B,
                           k10.canvas->eCNAndENFont24, 10, false);

    String fStr = "(" + String(tempF, 1) + " F)";
    k10.canvas->canvasText(fStr, 136, 94, 0xF59E0B,
                           k10.canvas->eCNAndENFont16, 12, false);

    String thermalState = (tempC > 30.0) ? "Status: Warm Ambient" : "Status: Nominal Temp";
    k10.canvas->canvasText(thermalState, 26, 122, 0x64748B,
                           k10.canvas->eCNAndENFont16, 24, false);

    // 3. Relative Humidity Card
    k10.canvas->canvasRectangle(14, 160, 212, 95, 0x1A4436, 0x0D241C, true);
    k10.canvas->canvasText("Relative Humidity", 26, 170, 0x94A3B8,
                           k10.canvas->eCNAndENFont16, 22, false);

    String hStr = String(humidity, 1) + " %";
    k10.canvas->canvasText(hStr, 26, 194, 0x00B4D8,
                           k10.canvas->eCNAndENFont24, 10, false);

    String humiState = (humidity > 65.0) ? "High Humidity" : ((humidity < 35.0) ? "Dry Air" : "Comfortable");
    k10.canvas->canvasText(humiState, 126, 200, 0x2EC4B6,
                           k10.canvas->eCNAndENFont16, 14, false);

    String rangeStr = "Target Comfort: 40-60%";
    k10.canvas->canvasText(rangeStr, 26, 228, 0x64748B,
                           k10.canvas->eCNAndENFont16, 24, false);

    // 4. Centered Footer Bar (Zero-overflow)
    k10.canvas->canvasLine(15, 276, 225, 276, 0x1A4436);
    k10.canvas->canvasText("AHT20 Digital I2C Sensor", 24, 290, 0x52796F,
                           k10.canvas->eCNAndENFont16, 26, false);

    k10.canvas->updateCanvas();
    delay(250);
}
