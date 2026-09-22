#include "unihiker_k10.h"

UNIHIKER_K10 k10;
AHT20 aht20;

#define COLOR_BG        0xF8FAFC
#define COLOR_CARD      0xFFFFFF
#define COLOR_BORDER    0xE2E8F0
#define COLOR_TEXT_PRI  0x0F172A
#define COLOR_TEXT_MUTED 0x64748B
#define COLOR_TEMP      0xE11D48 // Rose
#define COLOR_HUMID     0x0284C7 // Sky Blue

void drawStaticChrome() {
    k10.canvas->canvasClear();
    k10.setScreenBackground(COLOR_BG);

    k10.canvas->canvasSetLineWidth(1);

    // Header Card
    k10.canvas->canvasRectangle(16, 16, 208, 44, COLOR_BORDER, COLOR_CARD, true);
    k10.canvas->canvasCircle(36, 38, 5, COLOR_TEMP, COLOR_TEMP, true);
    k10.canvas->canvasText("Climate Sensors", 52, 30, COLOR_TEXT_PRI,
                           k10.canvas->eCNAndENFont16, 20, false);

    // Temperature Card Shell
    k10.canvas->canvasRectangle(16, 68, 208, 108, COLOR_BORDER, COLOR_CARD, true);
    k10.canvas->canvasText("TEMPERATURE", 28, 80, COLOR_TEXT_MUTED,
                           k10.canvas->eCNAndENFont16, 20, false);

    // Humidity Card Shell
    k10.canvas->canvasRectangle(16, 186, 208, 114, COLOR_BORDER, COLOR_CARD, true);
    k10.canvas->canvasText("RELATIVE HUMIDITY", 28, 198, COLOR_TEXT_MUTED,
                           k10.canvas->eCNAndENFont16, 20, false);

    k10.canvas->updateCanvas();
}

void updateMetrics(float tempC, float tempF, float humidity) {
    // Dynamic partial refresh for Temperature
    k10.canvas->canvasRectangle(28, 102, 184, 66, COLOR_CARD, COLOR_CARD, true);
    String tStr = String(tempC, 1) + " C";
    k10.canvas->canvasText(tStr.c_str(), 28, 104, COLOR_TEMP,
                           k10.canvas->eCNAndENFont24, 20, false);
    String fStr = "(" + String(tempF, 1) + " F)";
    k10.canvas->canvasText(fStr.c_str(), 28, 140, COLOR_TEXT_MUTED,
                           k10.canvas->eCNAndENFont16, 20, false);

    // Dynamic partial refresh for Humidity
    k10.canvas->canvasRectangle(28, 220, 184, 72, COLOR_CARD, COLOR_CARD, true);
    String hStr = String(humidity, 1) + " %RH";
    k10.canvas->canvasText(hStr.c_str(), 28, 222, COLOR_HUMID,
                           k10.canvas->eCNAndENFont24, 20, false);

    const char* status = (humidity < 30) ? "Dry Air" : ((humidity > 60) ? "High Humidity" : "Optimal Comfort");
    k10.canvas->canvasRectangle(28, 262, 130, 22, COLOR_BORDER, 0xF0F9FF, true);
    k10.canvas->canvasText(status, 36, 265, COLOR_HUMID,
                           k10.canvas->eCNAndENFont16, 20, false);

    k10.canvas->updateCanvas();
}

void setup() {
    k10.begin();
    k10.initScreen(2); // Portrait orientation (240x320)
    k10.creatCanvas();

    drawStaticChrome();
}

void loop() {
    float tempC = aht20.getData(AHT20::eAHT20TempC);
    float tempF = aht20.getData(AHT20::eAHT20TempF);
    float humidity = aht20.getData(AHT20::eAHT20HumiRH);

    updateMetrics(tempC, tempF, humidity);
    delay(500);
}
