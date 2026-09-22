#include "unihiker_k10.h"

UNIHIKER_K10 k10;

#define COLOR_BG        0xF8FAFC
#define COLOR_CARD      0xFFFFFF
#define COLOR_BORDER    0xE2E8F0
#define COLOR_TEXT_PRI  0x0F172A
#define COLOR_TEXT_MUTED 0x64748B
#define COLOR_AXIS_X    0x4F46E5 // Indigo
#define COLOR_AXIS_Y    0x059669 // Emerald
#define COLOR_AXIS_Z    0x0284C7 // Sky

void drawStaticChrome() {
    k10.canvas->canvasClear();
    k10.setScreenBackground(COLOR_BG);

    k10.canvas->canvasSetLineWidth(1);

    // Header Card
    k10.canvas->canvasRectangle(16, 16, 208, 44, COLOR_BORDER, COLOR_CARD, true);
    k10.canvas->canvasCircle(36, 38, 5, COLOR_AXIS_X, COLOR_AXIS_X, true);
    k10.canvas->canvasText("IMU Accelerometer", 52, 30, COLOR_TEXT_PRI,
                           k10.canvas->eCNAndENFont16, 20, false);

    // Main Telemetry Card
    k10.canvas->canvasRectangle(16, 68, 208, 232, COLOR_BORDER, COLOR_CARD, true);

    // Row Labels
    k10.canvas->canvasText("Axis X", 28, 80, COLOR_TEXT_MUTED, k10.canvas->eCNAndENFont16, 10, false);
    k10.canvas->canvasRectangle(28, 118, 184, 6, COLOR_BORDER, 0xF1F5F9, true);

    k10.canvas->canvasText("Axis Y", 28, 134, COLOR_TEXT_MUTED, k10.canvas->eCNAndENFont16, 10, false);
    k10.canvas->canvasRectangle(28, 172, 184, 6, COLOR_BORDER, 0xF1F5F9, true);

    k10.canvas->canvasText("Axis Z", 28, 188, COLOR_TEXT_MUTED, k10.canvas->eCNAndENFont16, 10, false);
    k10.canvas->canvasRectangle(28, 226, 184, 6, COLOR_BORDER, 0xF1F5F9, true);

    // Total G-Force Footer Line
    k10.canvas->canvasLine(28, 246, 212, 246, COLOR_BORDER);
    k10.canvas->canvasText("Total Vector:", 28, 256, COLOR_TEXT_MUTED, k10.canvas->eCNAndENFont16, 15, false);

    k10.canvas->updateCanvas();
}

void updateAxisData(int ax, int ay, int az, int strength) {
    // 1. Partial refresh Axis X (y: 96..116)
    k10.canvas->canvasRectangle(28, 96, 184, 20, COLOR_CARD, COLOR_CARD, true);
    String xStr = String(ax) + " mG";
    k10.canvas->canvasText(xStr.c_str(), 28, 96, COLOR_AXIS_X, k10.canvas->eCNAndENFont16, 20, false);
    // Track dot
    k10.canvas->canvasRectangle(28, 118, 184, 6, COLOR_BORDER, 0xF1F5F9, true);
    int dotX = map(constrain(ax, -1000, 1000), -1000, 1000, 30, 210);
    k10.canvas->canvasCircle(dotX, 121, 3, COLOR_AXIS_X, COLOR_AXIS_X, true);

    // 2. Partial refresh Axis Y (y: 150..170)
    k10.canvas->canvasRectangle(28, 150, 184, 20, COLOR_CARD, COLOR_CARD, true);
    String yStr = String(ay) + " mG";
    k10.canvas->canvasText(yStr.c_str(), 28, 150, COLOR_AXIS_Y, k10.canvas->eCNAndENFont16, 20, false);
    // Track dot
    k10.canvas->canvasRectangle(28, 172, 184, 6, COLOR_BORDER, 0xF1F5F9, true);
    int dotY = map(constrain(ay, -1000, 1000), -1000, 1000, 30, 210);
    k10.canvas->canvasCircle(dotY, 175, 3, COLOR_AXIS_Y, COLOR_AXIS_Y, true);

    // 3. Partial refresh Axis Z (y: 204..224)
    k10.canvas->canvasRectangle(28, 204, 184, 20, COLOR_CARD, COLOR_CARD, true);
    String zStr = String(az) + " mG";
    k10.canvas->canvasText(zStr.c_str(), 28, 204, COLOR_AXIS_Z, k10.canvas->eCNAndENFont16, 20, false);
    // Track dot
    k10.canvas->canvasRectangle(28, 226, 184, 6, COLOR_BORDER, 0xF1F5F9, true);
    int dotZ = map(constrain(az, -1000, 1000), -1000, 1000, 30, 210);
    k10.canvas->canvasCircle(dotZ, 229, 3, COLOR_AXIS_Z, COLOR_AXIS_Z, true);

    // 4. Partial refresh Total Vector
    k10.canvas->canvasRectangle(124, 254, 88, 20, COLOR_CARD, COLOR_CARD, true);
    String sStr = String(strength) + " mG";
    k10.canvas->canvasText(sStr.c_str(), 124, 256, COLOR_TEXT_PRI, k10.canvas->eCNAndENFont16, 15, false);

    k10.canvas->updateCanvas();
}

void setup() {
    k10.begin();
    k10.initScreen(2); // Portrait orientation (240x320)
    k10.creatCanvas();

    drawStaticChrome();
}

void loop() {
    int ax = k10.getAccelerometerX();
    int ay = k10.getAccelerometerY();
    int az = k10.getAccelerometerZ();
    int strength = k10.getStrength();

    updateAxisData(ax, ay, az, strength);
    delay(100);
}
