#include "unihiker_k10.h"

UNIHIKER_K10 k10;
uint8_t screen_dir = 2; // Portrait orientation (240x320)

// Helper to draw a modern SCADA industrial linear gauge
void drawLinearGauge(int x, int y, int width, int height, int value, int minVal, int maxVal, const char* label, uint32_t barColor) {
    // 1. Label on the left
    k10.canvas->canvasText(label, x, y - 20, 0xF1F5F9,
                           k10.canvas->eCNAndENFont16, 18, false);

    // Dynamic right-aligned percentage readout
    String valStr = String(value) + "%";
    int readoutX = x + width - (int)(valStr.length() * 8);
    k10.canvas->canvasText(valStr, readoutX, y - 20, barColor,
                           k10.canvas->eCNAndENFont16, 8, false);

    // 2. Gauge Track Background & Border
    k10.canvas->canvasRectangle(x, y, width, height, 0x334155, 0x131C24, true);

    // 3. Filled Bar Indicator
    int constrainedVal = constrain(value, minVal, maxVal);
    int fillWidth = map(constrainedVal, minVal, maxVal, 0, width - 4);

    if (fillWidth > 0) {
        k10.canvas->canvasRectangle(x + 2, y + 2, fillWidth, height - 4, barColor, barColor, true);
    }

    // 4. Tick marks along bottom
    for (int t = 0; t <= 4; t++) {
        int tickX = x + (t * (width - 1) / 4);
        k10.canvas->canvasLine(tickX, y + height, tickX, y + height + 4, 0x475569);
    }
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
}

void loop() {
    k10.canvas->canvasClear();

    // 1. SCADA Header Banner
    k10.canvas->canvasRectangle(0, 0, 240, 42, 0x131D24, 0x131D24, true);
    k10.canvas->canvasLine(0, 42, 240, 42, 0x0284C7);
    k10.canvas->canvasText("SCADA GAUGES", 48, 10, 0x38BDF8,
                           k10.canvas->eCNAndENFont24, 15, false);

    // 2. Gauge 1: Hydraulic Pressure (Cyan)
    drawLinearGauge(20, 84, 200, 22, demoPercent, 0, 100, "Hydraulic Line", 0x00E5FF);

    // 3. Gauge 2: Core Thermal Monitor (Dynamic color: Green -> Amber -> Red)
    uint32_t tempColor = (demoPercent > 75) ? 0xEF4444 : ((demoPercent > 45) ? 0xF59E0B : 0x10B981);
    drawLinearGauge(20, 154, 200, 22, demoPercent, 0, 100, "Core Temp", tempColor);

    // 4. Gauge 3: Inverted Battery Storage (Amber/Blue)
    int batteryLevel = 100 - demoPercent;
    drawLinearGauge(20, 224, 200, 20, batteryLevel, 0, 100, "Storage Bank", 0x38BDF8);

    // 5. Centered Footer Bus Status
    k10.canvas->canvasLine(15, 276, 225, 276, 0x1E293B);
    k10.canvas->canvasText("Telemetry Bus: Active", 36, 290, 0x64748B,
                           k10.canvas->eCNAndENFont16, 24, false);

    k10.canvas->updateCanvas();

    // Animate demo percentage value
    demoPercent += step;
    if (demoPercent >= 100 || demoPercent <= 0) {
        step = -step;
    }

    delay(30);
}
