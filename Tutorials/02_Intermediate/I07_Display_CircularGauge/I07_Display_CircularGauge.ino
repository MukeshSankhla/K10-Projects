#include "unihiker_k10.h"

UNIHIKER_K10 k10;
uint8_t screen_dir = 2; // Portrait orientation (240x320)

const float START_ANGLE = 150.0 * (PI / 180.0);
const float SWEEP_ANGLE = 240.0 * (PI / 180.0);

// Clean Minimalist Light Theme Palette
const uint32_t COLOR_BG        = 0xF8FAFC; // Soft Slate Off-White
const uint32_t COLOR_CARD      = 0xFFFFFF; // Pure White Card Fill
const uint32_t COLOR_BORDER    = 0xE2E8F0; // Delicate 1px Border
const uint32_t COLOR_BORDER_DIAL=0xCBD5E1; // Subtle Dial 1px Border
const uint32_t COLOR_TEXT_PRI  = 0x0F172A; // Deep Slate Charcoal
const uint32_t COLOR_TEXT_MUTED= 0x64748B; // Slate Muted Label
const uint32_t COLOR_BLUE      = 0x2563EB; // Sapphire Brand Accent
const uint32_t COLOR_CORAL     = 0xE11D48; // Needle Rose / Coral
const uint32_t COLOR_WHITE     = 0xFFFFFF;

void drawCircularGaugeStaticChrome(int centerX, int centerY, int radius, const char* unit) {
    // Outer dial bezel (delicate 1px circle)
    k10.canvas->canvasCircle(centerX, centerY, radius + 2, COLOR_BORDER_DIAL, COLOR_CARD, true);

    // Draw tick marks around 240-degree arc
    for (int t = 0; t <= 12; t++) {
        float theta = START_ANGLE + (t * SWEEP_ANGLE / 12.0);
        int x1 = centerX + (int)(cos(theta) * (radius - 2));
        int y1 = centerY + (int)(sin(theta) * (radius - 2));
        int x2 = centerX + (int)(cos(theta) * (radius - 8));
        int y2 = centerY + (int)(sin(theta) * (radius - 8));

        uint32_t tickColor = (t >= 10) ? 0xDC2626 : 0x94A3B8;
        k10.canvas->canvasLine(x1, y1, x2, y2, tickColor);
    }

    // Static Unit Label below dial center
    int unitX = centerX - (int)(strlen(unit) * 4);
    k10.canvas->canvasText(unit, unitX, centerY + 46, COLOR_TEXT_MUTED, k10.canvas->eCNAndENFont16, 10, false);
}

void updateCircularGaugeNeedle(int centerX, int centerY, int radius, int value, int minVal, int maxVal) {
    // Clear inner dial face (radius - 10) with pure white card background
    k10.canvas->canvasCircle(centerX, centerY, radius - 10, COLOR_CARD, COLOR_CARD, true);

    // Calculate indicator needle vector
    int constrainedVal = constrain(value, minVal, maxVal);
    float normVal = (float)(constrainedVal - minVal) / (float)(maxVal - minVal);
    float needleAngle = START_ANGLE + (normVal * SWEEP_ANGLE);

    int needleX = centerX + (int)(cos(needleAngle) * (radius - 14));
    int needleY = centerY + (int)(sin(needleAngle) * (radius - 14));

    // Needle vector line
    k10.canvas->canvasLine(centerX, centerY, needleX, needleY, COLOR_CORAL);
    k10.canvas->canvasLine(centerX + 1, centerY, needleX, needleY, COLOR_CORAL);

    // Center pivot cap
    k10.canvas->canvasCircle(centerX, centerY, 6, COLOR_TEXT_PRI, COLOR_TEXT_PRI, true);
    k10.canvas->canvasCircle(centerX, centerY, 3, COLOR_CARD, COLOR_CARD, true);

    // Digital readout text
    String valStr = String(constrainedVal);
    int textX = centerX - (int)(valStr.length() * 8);
    k10.canvas->canvasText(valStr, textX, centerY + 22, COLOR_TEXT_PRI, k10.canvas->eCNAndENFont24, 6, false);
}

void drawScreenChrome() {
    k10.canvas->canvasClear();
    k10.canvas->canvasRectangle(0, 0, 240, 320, COLOR_BG, COLOR_BG, true);

    // App Header Bar (y: 0 to 40)
    k10.canvas->canvasRectangle(0, 0, 240, 40, COLOR_CARD, COLOR_CARD, true);
    k10.canvas->canvasLine(0, 40, 240, 40, COLOR_BORDER);
    k10.canvas->canvasText("Circular Gauge", 14, 12, COLOR_TEXT_PRI, k10.canvas->eCNAndENFont16, 50, false);
    // Blue brand dot
    k10.canvas->canvasCircle(224, 20, 4, COLOR_BLUE, COLOR_BLUE, true);

    // Card Container (y: 48 to 278)
    k10.canvas->canvasRectangle(10, 48, 220, 230, COLOR_BORDER, COLOR_CARD, true);

    // Static Gauge Bezel & Scale
    drawCircularGaugeStaticChrome(120, 160, 80, "KM / H");

    // Footer Bar (y: 284 to 320)
    k10.canvas->canvasRectangle(0, 284, 240, 36, COLOR_CARD, COLOR_CARD, true);
    k10.canvas->canvasLine(0, 284, 240, 284, COLOR_BORDER);
    k10.canvas->canvasText("High-Precision Analog Watch Dial", 14, 294, COLOR_TEXT_MUTED, k10.canvas->eCNAndENFont16, 50, false);
}

void setup() {
    k10.begin();
    k10.initScreen(screen_dir);
    k10.creatCanvas();
    k10.setScreenBackground(COLOR_BG);

    k10.rgb->brightness(5);
    k10.rgb->write(-1, 0x2563EB);

    drawScreenChrome();
    k10.canvas->updateCanvas();
}

void loop() {
    static int speed = 0;
    static int dir = 1;

    speed += dir * 2;
    if (speed >= 180) { speed = 180; dir = -1; }
    if (speed <= 0)   { speed = 0;   dir = 1; }

    updateCircularGaugeNeedle(120, 160, 80, speed, 0, 180);

    k10.canvas->updateCanvas();
    delay(40);
}
