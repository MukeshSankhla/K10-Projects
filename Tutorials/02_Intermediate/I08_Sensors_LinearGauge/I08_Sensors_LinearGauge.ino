#include "unihiker_k10.h"

UNIHIKER_K10 k10;
uint8_t screen_dir = 2; // Portrait orientation (240x320)
AHT20 aht20;

// Clean Minimalist Light Theme Palette
const uint32_t COLOR_BG        = 0xF8FAFC; // Soft Slate Off-White
const uint32_t COLOR_CARD      = 0xFFFFFF; // Pure White Card Fill
const uint32_t COLOR_TRACK     = 0xF1F5F9; // Soft Track Fill
const uint32_t COLOR_BORDER    = 0xE2E8F0; // Delicate 1px Border
const uint32_t COLOR_BORDER_TRK= 0xCBD5E1; // Track Border
const uint32_t COLOR_TEXT_PRI  = 0x0F172A; // Deep Slate Charcoal
const uint32_t COLOR_TEXT_MUTED= 0x64748B; // Slate Muted Label
const uint32_t COLOR_AMBER     = 0xD97706; // Amber
const uint32_t COLOR_CORAL     = 0xE11D48; // Coral
const uint32_t COLOR_TEAL      = 0x0D9488; // Teal
const uint32_t COLOR_GREEN     = 0x16A34A; // Success Green

void drawGaugeCard(int cardY, const char* label) {
    k10.canvas->canvasRectangle(10, cardY, 220, 68, COLOR_BORDER, COLOR_CARD, true);
    k10.canvas->canvasText(label, 20, cardY + 12, COLOR_TEXT_MUTED, k10.canvas->eCNAndENFont16, 18, false);
    k10.canvas->canvasRectangle(20, cardY + 36, 200, 18, COLOR_BORDER_TRK, COLOR_TRACK, true);
}

void updateLinearGauge(int cardY, float value, float minVal, float maxVal, const char* unit, uint32_t barColor) {
    // Clear numeric readout area (x: 140..220, y: cardY + 10)
    k10.canvas->canvasRectangle(140, cardY + 10, 80, 20, COLOR_CARD, COLOR_CARD, true);

    String valStr = String(value, 1) + " " + unit;
    int readoutX = 220 - (int)(valStr.length() * 8);
    k10.canvas->canvasText(valStr, readoutX, cardY + 12, COLOR_TEXT_PRI, k10.canvas->eCNAndENFont16, 12, false);

    // Clear and redraw inner track bar
    k10.canvas->canvasRectangle(22, cardY + 38, 196, 14, COLOR_TRACK, COLOR_TRACK, true);

    float constrainedVal = constrain(value, minVal, maxVal);
    int fillW = (int)(((constrainedVal - minVal) / (maxVal - minVal)) * 196);
    if (fillW > 0) {
        k10.canvas->canvasRectangle(22, cardY + 38, fillW, 14, barColor, barColor, true);
    }
}

void drawScreenChrome() {
    k10.canvas->canvasClear();
    k10.canvas->canvasRectangle(0, 0, 240, 320, COLOR_BG, COLOR_BG, true);

    // App Header Bar (y: 0 to 40)
    k10.canvas->canvasRectangle(0, 0, 240, 40, COLOR_CARD, COLOR_CARD, true);
    k10.canvas->canvasLine(0, 40, 240, 40, COLOR_BORDER);
    k10.canvas->canvasText("Sensor Gauges", 14, 12, COLOR_TEXT_PRI, k10.canvas->eCNAndENFont16, 50, false);
    // Green brand dot
    k10.canvas->canvasCircle(224, 20, 4, COLOR_GREEN, COLOR_GREEN, true);

    // 3 Gauge Cards
    drawGaugeCard(48, "Ambient Light");
    drawGaugeCard(124, "Temperature");
    drawGaugeCard(200, "Humidity");

    // Footer Bar (y: 284 to 320)
    k10.canvas->canvasRectangle(0, 284, 240, 36, COLOR_CARD, COLOR_CARD, true);
    k10.canvas->canvasLine(0, 284, 240, 284, COLOR_BORDER);
    k10.canvas->canvasText("Onboard Sensors Real-Time Telemetry", 14, 294, COLOR_TEXT_MUTED, k10.canvas->eCNAndENFont16, 50, false);
}

void setup() {
    k10.begin();
    k10.initScreen(screen_dir);
    k10.creatCanvas();
    k10.setScreenBackground(COLOR_BG);

    k10.rgb->brightness(5);
    k10.rgb->write(-1, 0x16A34A);

    drawScreenChrome();
    k10.canvas->updateCanvas();
}

void loop() {
    uint16_t light = k10.readALS();
    float temp = aht20.getData(AHT20::eAHT20TempC);
    float humid = aht20.getData(AHT20::eAHT20HumiRH);

    updateLinearGauge(48, (float)light, 0, 1000, "Lux", COLOR_AMBER);
    updateLinearGauge(124, temp, 0, 50, "C", COLOR_CORAL);
    updateLinearGauge(200, humid, 0, 100, "%", COLOR_TEAL);

    k10.canvas->updateCanvas();
    delay(200);
}
