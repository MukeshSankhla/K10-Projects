#include "unihiker_k10.h"

UNIHIKER_K10 k10;
uint8_t screen_dir = 2; // Portrait orientation (240x320)

// Helper to draw a circular dial gauge
void drawCircularGauge(int centerX, int centerY, int radius, int value, int minVal, int maxVal, const char* unit, uint32_t activeColor) {
    // 1. Outer dial bezel ring
    k10.canvas->canvasCircle(centerX, centerY, radius + 8, 0x334155, 0x000000, false);
    k10.canvas->canvasCircle(centerX, centerY, radius + 7, 0x1E293B, 0x000000, false);

    // 2. Dial face background
    k10.canvas->canvasCircle(centerX, centerY, radius, 0x1E293B, 0x111827, true);

    // 3. Draw tick marks around 240-degree arc (from 150 deg to 390 deg)
    float startAngle = 150.0 * (PI / 180.0);
    float sweepAngle = 240.0 * (PI / 180.0);

    for (int t = 0; t <= 12; t++) {
        float theta = startAngle + (t * sweepAngle / 12.0);
        int x1 = centerX + (int)(cos(theta) * (radius - 4));
        int y1 = centerY + (int)(sin(theta) * (radius - 4));
        int x2 = centerX + (int)(cos(theta) * (radius - 14));
        int y2 = centerY + (int)(sin(theta) * (radius - 14));

        k10.canvas->canvasLine(x1, y1, x2, y2, 0x64748B);
    }

    // 4. Draw Indicator Needle
    int constrainedVal = constrain(value, minVal, maxVal);
    float normVal = (float)(constrainedVal - minVal) / (float)(maxVal - minVal);
    float needleAngle = startAngle + (normVal * sweepAngle);

    int needleX = centerX + (int)(cos(needleAngle) * (radius - 16));
    int needleY = centerY + (int)(sin(needleAngle) * (radius - 16));

    // Needle vector line
    k10.canvas->canvasLine(centerX, centerY, needleX, needleY, activeColor);
    k10.canvas->canvasLine(centerX + 1, centerY, needleX, needleY, activeColor);
    k10.canvas->canvasLine(centerX, centerY + 1, needleX, needleY, activeColor);

    // Center pivot cap
    k10.canvas->canvasCircle(centerX, centerY, 8, 0xFEE715, 0xFEE715, true);
    k10.canvas->canvasCircle(centerX, centerY, 4, 0x000000, 0x000000, true);

    // 5. Digital Readout below pivot
    String valStr = String(value);
    k10.canvas->canvasText(valStr, centerX - 18, centerY + 28, 0xFEE715,
                           k10.canvas->eCNAndENFont24, 10, false);
    k10.canvas->canvasText(unit, centerX - 12, centerY + 54, 0x94A3B8,
                           k10.canvas->eCNAndENFont16, 10, false);
}

int gaugeVal = 20;
int gaugeSpeed = 2;

void setup() {
    k10.begin();
    k10.initScreen(screen_dir);
    k10.creatCanvas();
    k10.setScreenBackground(0x0F172A);

    k10.rgb->brightness(5);
    k10.rgb->write(-1, 0x00FF88);
}

void loop() {
    k10.canvas->canvasClear();

    // 1. Header Banner
    k10.canvas->canvasText("CIRCULAR GAUGE", 26, 14, 0xFEE715,
                           k10.canvas->eCNAndENFont24, 20, false);
    k10.canvas->canvasLine(15, 44, 225, 44, 0x334155);

    // 2. Render Main Dial Gauge
    uint32_t needleColor = (gaugeVal > 80) ? 0xFF4444 : ((gaugeVal > 50) ? 0xFEE715 : 0x00E5FF);
    drawCircularGauge(120, 160, 75, gaugeVal, 0, 100, "KM / H", needleColor);

    // 3. Footer Stats
    k10.canvas->canvasLine(15, 280, 225, 280, 0x334155);
    k10.canvas->canvasText("Radial Needle & Dial Physics", 20, 292, 0x00FF88,
                           k10.canvas->eCNAndENFont16, 26, false);

    k10.canvas->updateCanvas();

    gaugeVal += gaugeSpeed;
    if (gaugeVal >= 100 || gaugeVal <= 0) {
        gaugeSpeed = -gaugeSpeed;
    }

    delay(30);
}
