#include "unihiker_k10.h"

UNIHIKER_K10 k10;
uint8_t screen_dir = 2; // Portrait orientation (240x320)

void drawAxisPod(int x, int y, int w, int h, const char* label, int val, uint32_t color) {
    k10.canvas->canvasRectangle(x, y, w, h, 0x1E3A5F, 0x0D1B2A, true);

    // Label on the left
    k10.canvas->canvasText(label, x + 10, y + 6, 0x94A3B8,
                           k10.canvas->eCNAndENFont16, 10, false);

    // Readout on the right
    String valStr = String(val) + " mG";
    int valX = x + w - (int)(valStr.length() * 8) - 10;
    k10.canvas->canvasText(valStr, valX, y + 6, color,
                           k10.canvas->eCNAndENFont16, 12, false);

    // Zero-center bipolar bar (Center at x + w/2)
    int centerX = x + (w / 2);
    int barY = y + 30;
    k10.canvas->canvasRectangle(x + 10, barY, w - 20, 6, 0x1E3A5F, 0x08121E, true);
    k10.canvas->canvasLine(centerX, barY - 2, centerX, barY + 8, 0x94A3B8); // Center zero mark

    int deflection = map(constrain(val, -1200, 1200), -1200, 1200, -(w/2 - 14), (w/2 - 14));
    if (deflection > 0) {
        k10.canvas->canvasRectangle(centerX, barY, deflection, 6, color, color, true);
    } else if (deflection < 0) {
        k10.canvas->canvasRectangle(centerX + deflection, barY, -deflection, 6, color, color, true);
    }
}

void setup() {
    k10.begin();
    k10.initScreen(screen_dir);
    k10.creatCanvas();
    // Gunmetal Navy background
    k10.setScreenBackground(0x08121E);

    k10.rgb->brightness(5);
    k10.rgb->write(-1, 0x20BF55); // Radar green glow
}

void loop() {
    int ax = k10.getAccelerometerX();
    int ay = k10.getAccelerometerY();
    int az = k10.getAccelerometerZ();
    int strength = k10.getStrength();

    k10.canvas->canvasClear();

    // 1. Avionics Header Banner
    k10.canvas->canvasRectangle(0, 0, 240, 42, 0x0F2033, 0x0F2033, true);
    k10.canvas->canvasLine(0, 42, 240, 42, 0x00F5D4);
    k10.canvas->canvasText("IMU TELEMETRY", 46, 10, 0x00F5D4,
                           k10.canvas->eCNAndENFont24, 15, false);

    // 2. Three Bipolar Axis Pods
    drawAxisPod(14, 50, 212, 48, "Axis X", ax, 0xFF6B35);
    drawAxisPod(14, 104, 212, 48, "Axis Y", ay, 0x20BF55);
    drawAxisPod(14, 158, 212, 48, "Axis Z", az, 0x00F5D4);

    // 3. Magnitude Readout Box
    k10.canvas->canvasRectangle(14, 212, 212, 54, 0x1E3A5F, 0x0D1B2A, true);
    k10.canvas->canvasText("Vector Magnitude:", 24, 220, 0x94A3B8,
                           k10.canvas->eCNAndENFont16, 20, false);
    String strVal = String(strength) + " mG (Total G-Force)";
    k10.canvas->canvasText(strVal, 24, 240, 0xFEE715,
                           k10.canvas->eCNAndENFont16, 24, false);

    // 4. Centered Footer Bar (Zero-overflow)
    k10.canvas->canvasLine(15, 276, 225, 276, 0x1E3A5F);
    k10.canvas->canvasText("SC7A20H Motion Sensor", 34, 290, 0x64748B,
                           k10.canvas->eCNAndENFont16, 24, false);

    k10.canvas->updateCanvas();
    delay(80);
}
