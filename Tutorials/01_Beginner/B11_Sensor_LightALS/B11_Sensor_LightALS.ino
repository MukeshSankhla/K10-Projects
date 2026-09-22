#include "unihiker_k10.h"

UNIHIKER_K10 k10;

#define COLOR_BG        0xF8FAFC
#define COLOR_CARD      0xFFFFFF
#define COLOR_BORDER    0xE2E8F0
#define COLOR_TEXT_PRI  0x0F172A
#define COLOR_TEXT_MUTED 0x64748B
#define COLOR_AMBER     0xD97706
#define COLOR_AMBER_BG  0xFEF3C7

void drawStaticChrome() {
    k10.canvas->canvasClear();
    k10.setScreenBackground(COLOR_BG);

    k10.canvas->canvasSetLineWidth(1);

    // Header Card
    k10.canvas->canvasRectangle(16, 16, 208, 44, COLOR_BORDER, COLOR_CARD, true);
    k10.canvas->canvasCircle(36, 38, 5, COLOR_AMBER, COLOR_AMBER, true);
    k10.canvas->canvasText("Light Sensor (ALS)", 52, 30, COLOR_TEXT_PRI,
                           k10.canvas->eCNAndENFont16, 20, false);

    // Main Reading Card Container
    k10.canvas->canvasRectangle(16, 68, 208, 170, COLOR_BORDER, COLOR_CARD, true);
    k10.canvas->canvasText("AMBIENT ILLUMINANCE", 28, 80, COLOR_TEXT_MUTED,
                           k10.canvas->eCNAndENFont16, 20, false);

    // Baseline track for level bar
    k10.canvas->canvasRectangle(28, 196, 184, 8, COLOR_BORDER, 0xF1F5F9, true);

    // Instructions Card
    k10.canvas->canvasRectangle(16, 246, 208, 54, COLOR_BORDER, COLOR_CARD, true);
    k10.canvas->canvasText("Cover sensor to test", 28, 256, COLOR_TEXT_PRI,
                           k10.canvas->eCNAndENFont16, 20, false);
    k10.canvas->canvasText("Updates continuously", 28, 276, COLOR_TEXT_MUTED,
                           k10.canvas->eCNAndENFont16, 20, false);

    k10.canvas->updateCanvas();
}

void updateReading(uint16_t lux) {
    // Dynamic partial refresh of Lux text & state badge
    k10.canvas->canvasRectangle(28, 102, 184, 86, COLOR_CARD, COLOR_CARD, true);

    // Numeric value
    String luxStr = String(lux) + " Lux";
    k10.canvas->canvasText(luxStr.c_str(), 28, 106, COLOR_AMBER,
                           k10.canvas->eCNAndENFont24, 20, false);

    // State Pill
    const char* envState = (lux < 50) ? "Low Light" : ((lux < 500) ? "Indoor Lighting" : "Bright Daylight");
    k10.canvas->canvasRectangle(28, 146, 140, 22, COLOR_BORDER, COLOR_AMBER_BG, true);
    k10.canvas->canvasText(envState, 34, 149, COLOR_AMBER,
                           k10.canvas->eCNAndENFont16, 20, false);

    // Level Bar fill
    k10.canvas->canvasRectangle(28, 196, 184, 8, COLOR_BORDER, 0xF1F5F9, true);
    int barW = map(constrain(lux, 0, 1000), 0, 1000, 0, 184);
    if (barW > 0) {
        k10.canvas->canvasRectangle(28, 196, barW, 8, COLOR_AMBER, COLOR_AMBER, true);
    }

    k10.canvas->updateCanvas();
}

void setup() {
    k10.begin();
    k10.initScreen(2); // Portrait orientation (240x320)
    k10.creatCanvas();

    drawStaticChrome();
}

void loop() {
    uint16_t lightLevel = k10.readALS();
    updateReading(lightLevel);
    delay(200);
}
