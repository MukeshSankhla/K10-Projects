#include "unihiker_k10.h"

UNIHIKER_K10 k10;
uint8_t screen_dir = 2; // Portrait orientation (240x320)
AHT20 aht20;

// Clean Minimalist Light Theme Palette
const uint32_t COLOR_BG        = 0xF8FAFC; // Soft Slate Off-White
const uint32_t COLOR_CARD      = 0xFFFFFF; // Pure White Card Fill
const uint32_t COLOR_BORDER    = 0xE2E8F0; // Delicate 1px Border
const uint32_t COLOR_TEXT_PRI  = 0x0F172A; // Deep Slate Charcoal
const uint32_t COLOR_TEXT_SEC  = 0x334155; // Slate Secondary
const uint32_t COLOR_TEXT_MUTED= 0x64748B; // Slate Muted Label
const uint32_t COLOR_AMBER     = 0xD97706; // Amber
const uint32_t COLOR_CORAL     = 0xE11D48; // Coral
const uint32_t COLOR_TEAL      = 0x0D9488; // Teal
const uint32_t COLOR_BLUE      = 0x0284C7; // Blue
const uint32_t COLOR_GREEN     = 0x16A34A; // Emerald Brand Dot

void drawStaticSensorCard(int x, int y, int w, int h, const char* title) {
    k10.canvas->canvasRectangle(x, y, w, h, COLOR_BORDER, COLOR_CARD, true);
    k10.canvas->canvasText(title, x + 10, y + 8, COLOR_TEXT_MUTED, k10.canvas->eCNAndENFont16, 12, false);
}

void updateSensorCardValues(int x, int y, int w, const String& valStr, uint32_t accentColor) {
    k10.canvas->canvasRectangle(x + 2, y + 26, w - 4, 30, COLOR_CARD, COLOR_CARD, true);
    k10.canvas->canvasText(valStr, x + 10, y + 28, accentColor, k10.canvas->eCNAndENFont16, 14, false);
}

void drawIMUPodStaticChrome() {
    k10.canvas->canvasRectangle(10, 194, 220, 82, COLOR_BORDER, COLOR_CARD, true);
    k10.canvas->canvasText("3-AXIS ACCELEROMETER", 20, 202, COLOR_TEXT_MUTED, k10.canvas->eCNAndENFont16, 22, false);
}

void updateIMUVectors(int accX, int accY, int accZ) {
    k10.canvas->canvasRectangle(12, 224, 216, 48, COLOR_CARD, COLOR_CARD, true);

    String axStr = "X: " + String(accX) + " mG";
    String ayStr = "Y: " + String(accY) + " mG";
    String azStr = "Z: " + String(accZ) + " mG";

    k10.canvas->canvasText(axStr, 20, 228, COLOR_BLUE, k10.canvas->eCNAndENFont16, 12, false);
    k10.canvas->canvasText(ayStr, 20, 250, COLOR_TEXT_SEC, k10.canvas->eCNAndENFont16, 12, false);
    k10.canvas->canvasText(azStr, 130, 228, COLOR_AMBER, k10.canvas->eCNAndENFont16, 12, false);
}

void drawScreenChrome() {
    k10.canvas->canvasClear();
    k10.canvas->canvasRectangle(0, 0, 240, 320, COLOR_BG, COLOR_BG, true);

    // App Header Bar (y: 0 to 40)
    k10.canvas->canvasRectangle(0, 0, 240, 40, COLOR_CARD, COLOR_CARD, true);
    k10.canvas->canvasLine(0, 40, 240, 40, COLOR_BORDER);
    k10.canvas->canvasText("Sensor Dashboard", 14, 12, COLOR_TEXT_PRI, k10.canvas->eCNAndENFont16, 50, false);
    // Green brand dot
    k10.canvas->canvasCircle(224, 20, 4, COLOR_GREEN, COLOR_GREEN, true);

    // 3 Environmental Cards (y: 48 to 186)
    drawStaticSensorCard(10, 48, 106, 64, "LIGHT");
    drawStaticSensorCard(124, 48, 106, 64, "TEMP");
    drawStaticSensorCard(10, 120, 220, 66, "HUMIDITY");

    // IMU Card
    drawIMUPodStaticChrome();

    // Footer Bar (y: 284 to 320)
    k10.canvas->canvasRectangle(0, 284, 240, 36, COLOR_CARD, COLOR_CARD, true);
    k10.canvas->canvasLine(0, 284, 240, 284, COLOR_BORDER);
    k10.canvas->canvasText("Live Multi-Sensor Bus Feed", 14, 294, COLOR_TEXT_MUTED, k10.canvas->eCNAndENFont16, 50, false);
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

    int accX = k10.getAccelerometerX();
    int accY = k10.getAccelerometerY();
    int accZ = k10.getAccelerometerZ();

    updateSensorCardValues(10, 48, 106, String(light) + " Lux", COLOR_AMBER);
    updateSensorCardValues(124, 48, 106, String(temp, 1) + " C", COLOR_CORAL);
    updateSensorCardValues(10, 120, 220, String(humid, 1) + " % RH", COLOR_TEAL);

    updateIMUVectors(accX, accY, accZ);

    k10.canvas->updateCanvas();
    delay(100);
}
