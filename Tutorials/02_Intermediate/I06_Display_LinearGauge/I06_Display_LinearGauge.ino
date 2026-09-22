#include "unihiker_k10.h"

UNIHIKER_K10 k10;
uint8_t screen_dir = 2; // Portrait orientation (240x320)

// 1. Draw static gauge elements (title label, track border, tick marks) once
void drawGaugeStaticChrome(int x, int y, int width, int height, const char* label) {
    // Label on the left
    k10.canvas->canvasText(label, x, y - 20, 0xF1F5F9,
                           k10.canvas->eCNAndENFont16, 18, false);

    // Gauge Track Background & Border
    k10.canvas->canvasRectangle(x, y, width, height, 0x334155, 0x131C24, true);

    // Tick marks along bottom
    for (int t = 0; t <= 4; t++) {
        int tickX = x + (t * (width - 1) / 4);
        k10.canvas->canvasLine(tickX, y + height, tickX, y + height + 4, 0x475569);
    }
}

// 2. Dynamic Partial Refresh: update ONLY numeric percentage readout and inner fill bar
void updateLinearGaugeValue(int x, int y, int width, int height, int value, int minVal, int maxVal, uint32_t barColor) {
    // Clear previous right-aligned percentage readout area (width 50px, height 18px)
    k10.canvas->canvasRectangle(x + width - 50, y - 20, 50, 18, 0x0C1217, 0x0C1217, true);

    // Draw new percentage readout
    String valStr = String(value) + "%";
    int readoutX = x + width - (int)(valStr.length() * 8);
    k10.canvas->canvasText(valStr, readoutX, y - 20, barColor,
                           k10.canvas->eCNAndENFont16, 8, false);

    // Clear previous inner bar area with track background
    k10.canvas->canvasRectangle(x + 2, y + 2, width - 4, height - 4, 0x131C24, 0x131C24, true);

    // Draw updated filled bar indicator
    int constrainedVal = constrain(value, minVal, maxVal);
    int fillWidth = map(constrainedVal, minVal, maxVal, 0, width - 4);

    if (fillWidth > 0) {
        k10.canvas->canvasRectangle(x + 2, y + 2, fillWidth, height - 4, barColor, barColor, true);
    }
}

// Render static screen layout (Header banner and footer status) once
void drawScreenChrome() {
    // 1. SCADA Header Banner
    k10.canvas->canvasRectangle(0, 0, 240, 42, 0x131D24, 0x131D24, true);
    k10.canvas->canvasLine(0, 42, 240, 42, 0x0284C7);
    k10.canvas->canvasText("SCADA GAUGES", 48, 10, 0x38BDF8,
                           k10.canvas->eCNAndENFont24, 15, false);

    // 2. Centered Footer Bus Status
    k10.canvas->canvasLine(15, 276, 225, 276, 0x1E293B);
    k10.canvas->canvasText("Telemetry Bus: Active", 36, 290, 0x64748B,
                           k10.canvas->eCNAndENFont16, 24, false);

    // 3. Static Gauge Containers and Labels
    drawGaugeStaticChrome(20, 84, 200, 22, "Hydraulic Line");
    drawGaugeStaticChrome(20, 154, 200, 22, "Core Temp");
    drawGaugeStaticChrome(20, 224, 200, 20, "Storage Bank");
}

int demoPercent = 0;
int step = 2;

void setup() {
    k10.begin();
    k10.initScreen(screen_dir);
    k10.creatCanvas();
    // Cyber-Industrial SCADA theme
    k10.setScreenBackground(0x0C1217);

    k10.rgb->brightness(5);
    k10.rgb->write(-1, 0x00E5FF);

    // Initial paint: static chrome + initial gauge states
    drawScreenChrome();
    updateLinearGaugeValue(20, 84, 200, 22, demoPercent, 0, 100, 0x00E5FF);
    updateLinearGaugeValue(20, 154, 200, 22, demoPercent, 0, 100, 0x10B981);
    updateLinearGaugeValue(20, 224, 200, 20, 100 - demoPercent, 0, 100, 0x38BDF8);
    k10.canvas->updateCanvas();
}

void loop() {
    // Dynamic Partial Refresh: update ONLY the gauge fill bars and percentage text readouts
    updateLinearGaugeValue(20, 84, 200, 22, demoPercent, 0, 100, 0x00E5FF);

    uint32_t tempColor = (demoPercent > 75) ? 0xEF4444 : ((demoPercent > 45) ? 0xF59E0B : 0x10B981);
    updateLinearGaugeValue(20, 154, 200, 22, demoPercent, 0, 100, tempColor);

    int batteryLevel = 100 - demoPercent;
    updateLinearGaugeValue(20, 224, 200, 20, batteryLevel, 0, 100, 0x38BDF8);

    // Push updated components to display without any full-screen flicker
    k10.canvas->updateCanvas();

    // Animate demo percentage value
    demoPercent += step;
    if (demoPercent >= 100 || demoPercent <= 0) {
        step = -step;
    }

    delay(30);
}
