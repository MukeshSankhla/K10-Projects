#include "unihiker_k10.h"

UNIHIKER_K10 k10;

#define COLOR_BG        0xF8FAFC
#define COLOR_CARD      0xFFFFFF
#define COLOR_BORDER    0xE2E8F0
#define COLOR_TEXT_PRI  0x0F172A
#define COLOR_TEXT_MUTED 0x64748B
#define COLOR_INDIGO    0x4F46E5
#define COLOR_SKY       0x0284C7
#define COLOR_TEAL      0x0D9488

void setup() {
    k10.begin();
    k10.initScreen(2); // Portrait orientation (240x320)
    k10.creatCanvas();
    k10.setScreenBackground(COLOR_BG);

    // Enforce delicate 1px geometry
    k10.canvas->canvasSetLineWidth(1);

    // Header Card
    k10.canvas->canvasRectangle(16, 16, 208, 44, COLOR_BORDER, COLOR_CARD, true);
    k10.canvas->canvasCircle(36, 38, 5, COLOR_INDIGO, COLOR_INDIGO, true);
    k10.canvas->canvasText("Geometric Shapes", 52, 30, COLOR_TEXT_PRI,
                           k10.canvas->eCNAndENFont16, 20, false);

    // Section 1: Line Card
    k10.canvas->canvasRectangle(16, 68, 208, 58, COLOR_BORDER, COLOR_CARD, true);
    k10.canvas->canvasText("1. Thin Line (1px)", 28, 76, COLOR_TEXT_MUTED,
                           k10.canvas->eCNAndENFont16, 20, false);
    k10.canvas->canvasLine(28, 106, 204, 106, COLOR_INDIGO);

    // Section 2: Rectangles Card
    k10.canvas->canvasRectangle(16, 134, 208, 76, COLOR_BORDER, COLOR_CARD, true);
    k10.canvas->canvasText("2. Rectangles", 28, 142, COLOR_TEXT_MUTED,
                           k10.canvas->eCNAndENFont16, 20, false);
    // Outline rectangle (1px border)
    k10.canvas->canvasRectangle(28, 168, 88, 30, COLOR_BORDER, COLOR_CARD, false);
    k10.canvas->canvasText("Outline", 44, 175, COLOR_TEXT_MUTED, k10.canvas->eCNAndENFont16, 10, false);
    // Filled rectangle
    k10.canvas->canvasRectangle(124, 168, 88, 30, 0xBAE6FD, 0xE0F2FE, true);
    k10.canvas->canvasText("Filled", 146, 175, COLOR_SKY, k10.canvas->eCNAndENFont16, 10, false);

    // Section 3: Circles Card
    k10.canvas->canvasRectangle(16, 218, 208, 86, COLOR_BORDER, COLOR_CARD, true);
    k10.canvas->canvasText("3. Circles", 28, 226, COLOR_TEXT_MUTED,
                           k10.canvas->eCNAndENFont16, 20, false);
    // Outline circle
    k10.canvas->canvasCircle(72, 272, 20, COLOR_SKY, COLOR_CARD, false);
    // Filled circle
    k10.canvas->canvasCircle(168, 272, 20, COLOR_TEAL, COLOR_TEAL, true);

    k10.canvas->updateCanvas();
}

void loop() {
    delay(500);
}
