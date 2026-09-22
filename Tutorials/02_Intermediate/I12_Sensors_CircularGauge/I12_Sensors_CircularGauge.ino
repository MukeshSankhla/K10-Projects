#include "unihiker_k10.h"

UNIHIKER_K10 k10;
uint8_t screen_dir = 2; // Portrait orientation (240x320)
AHT20 aht20;

int activeSensor = 0; // 0 = Temperature (10-45 C), 1 = Light Sensor (0-4000 lux)

void drawCircularGauge(int centerX, int centerY, int radius, float value, float minVal, float maxVal, const char* label, const char* unit, uint32_t activeColor) {
    // 1. Outer dial bezel
    k10.canvas->canvasCircle(centerX, centerY, radius + 8, 0x334155, 0x000000, false);
    k10.canvas->canvasCircle(centerX, centerY, radius, 0x1E293B, 0x111827, true);

    // 2. Dial tick marks
    float startAngle = 150.0 * (PI / 180.0);
    float sweepAngle = 240.0 * (PI / 180.0);

    for (int t = 0; t <= 10; t++) {
        float theta = startAngle + (t * sweepAngle / 10.0);
        int x1 = centerX + (int)(cos(theta) * (radius - 4));
        int y1 = centerY + (int)(sin(theta) * (radius - 4));
        int x2 = centerX + (int)(cos(theta) * (radius - 12));
        int y2 = centerY + (int)(sin(theta) * (radius - 12));

        k10.canvas->canvasLine(x1, y1, x2, y2, 0x64748B);
    }

    // 3. Indicator Needle
    float constrainedVal = constrain(value, minVal, maxVal);
    float normVal = (constrainedVal - minVal) / (maxVal - minVal);
    float needleAngle = startAngle + (normVal * sweepAngle);

    int needleX = centerX + (int)(cos(needleAngle) * (radius - 16));
    int needleY = centerY + (int)(sin(needleAngle) * (radius - 16));

    k10.canvas->canvasLine(centerX, centerY, needleX, needleY, activeColor);
    k10.canvas->canvasLine(centerX + 1, centerY, needleX, needleY, activeColor);
    k10.canvas->canvasCircle(centerX, centerY, 7, 0xFEE715, 0xFEE715, true);
    k10.canvas->canvasCircle(centerX, centerY, 3, 0x000000, 0x000000, true);

    // 4. Digital Readouts
    k10.canvas->canvasText(label, centerX - 40, centerY - 45, 0x94A3B8,
                           k10.canvas->eCNAndENFont16, 20, false);

    String valStr = String(value, 1);
    k10.canvas->canvasText(valStr, centerX - 24, centerY + 25, 0xFEE715,
                           k10.canvas->eCNAndENFont24, 10, false);
    k10.canvas->canvasText(unit, centerX - 12, centerY + 52, 0x00E5FF,
                           k10.canvas->eCNAndENFont16, 10, false);
}

void setup() {
    k10.begin();
    k10.initScreen(screen_dir);
    k10.creatCanvas();
    k10.setScreenBackground(0x0F172A);

    k10.rgb->brightness(5);
    k10.rgb->write(-1, 0x00E5FF);
}

void loop() {
    float tempC = aht20.getData(AHT20::eAHT20TempC);
    int lightALS = k10.readALS();

    k10.canvas->canvasClear();

    // 1. Header
    k10.canvas->canvasText("DIAL SENSOR", 48, 14, 0xFEE715,
                           k10.canvas->eCNAndENFont24, 20, false);
    k10.canvas->canvasLine(15, 44, 225, 44, 0x334155);

    // 2. Render Circular Gauge based on active sensor
    if (activeSensor == 0) {
        uint32_t needleColor = (tempC > 30.0) ? 0xFF4444 : 0x00FF88;
        drawCircularGauge(120, 160, 75, tempC, 10.0, 45.0, "Temperature", "deg C", needleColor);
    } else {
        drawCircularGauge(120, 160, 75, lightALS, 0, 4000, "Ambient Light", "lux", 0x38BDF8);
    }

    // 3. Navigation Controls
    k10.canvas->canvasLine(15, 275, 225, 275, 0x334155);
    k10.canvas->canvasText("Press [A]: Switch Sensor", 20, 288, 0x00FF88,
                           k10.canvas->eCNAndENFont16, 26, false);

    k10.canvas->updateCanvas();

    if (k10.buttonA->isPressed()) {
        activeSensor = (activeSensor + 1) % 2;
        while (k10.buttonA->isPressed()) {
            delay(50);
        }
    }

    delay(100);
}
