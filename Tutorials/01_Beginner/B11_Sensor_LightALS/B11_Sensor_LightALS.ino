#include "unihiker_k10.h"

UNIHIKER_K10 k10;
uint8_t screen_dir = 2; // Portrait orientation (240x320)

void setup() {
    k10.begin();
    k10.initScreen(screen_dir);
    k10.creatCanvas();
    // Solar Gold Warm Charcoal background
    k10.setScreenBackground(0x141008);

    k10.rgb->brightness(5);
    k10.rgb->write(-1, 0xFFB703); // Solar Gold glow
}

void loop() {
    uint16_t lightLevel = k10.readALS();

    k10.canvas->canvasClear();

    // 1. Solar Header Banner
    k10.canvas->canvasRectangle(0, 0, 240, 42, 0x221708, 0x221708, true);
    k10.canvas->canvasLine(0, 42, 240, 42, 0xFFB703);
    k10.canvas->canvasText("LIGHT SENSOR", 46, 10, 0xFFB703,
                           k10.canvas->eCNAndENFont24, 15, false);

    // 2. Primary Lux Telemetry Card
    k10.canvas->canvasRectangle(14, 54, 212, 105, 0x3D290F, 0x1F1409, true);
    k10.canvas->canvasText("Ambient Illumination", 36, 64, 0xD4A373,
                           k10.canvas->eCNAndENFont16, 22, false);

    String luxStr = String(lightLevel) + " Lux";
    int valX = 120 - (int)(luxStr.length() * 7);
    k10.canvas->canvasText(luxStr, valX, 95, 0xFFD166,
                           k10.canvas->eCNAndENFont24, 12, false);

    // Lux Dynamic Scale Bar (0 - 3000 lux)
    int fillW = map(constrain(lightLevel, 0, 3000), 0, 3000, 0, 188);
    k10.canvas->canvasRectangle(26, 134, 188, 10, 0x3D290F, 0x0E0A05, true);
    if (fillW > 0) {
        k10.canvas->canvasRectangle(26, 134, fillW, 10, 0xFB8500, 0xFB8500, true);
    }

    // 3. Condition State Card
    k10.canvas->canvasRectangle(14, 172, 212, 85, 0x3D290F, 0x1F1409, true);
    k10.canvas->canvasText("Operating Environment:", 28, 184, 0x94A3B8,
                           k10.canvas->eCNAndENFont16, 22, false);

    String stateStr = "";
    uint32_t stateColor = 0xFFFFFF;

    if (lightLevel < 50) {
        stateStr = "State: Night / Dark";
        stateColor = 0x94A3B8;
    } else if (lightLevel < 300) {
        stateStr = "State: Dim Indoors";
        stateColor = 0xF59E0B;
    } else if (lightLevel < 1000) {
        stateStr = "State: Optimal Room";
        stateColor = 0x10B981;
    } else {
        stateStr = "State: Direct Sunlight";
        stateColor = 0xFFD166;
    }

    int stateX = 120 - (int)(stateStr.length() * 4);
    k10.canvas->canvasText(stateStr, stateX, 215, stateColor,
                           k10.canvas->eCNAndENFont16, 24, false);

    // 4. Centered Footer Bar (Zero-overflow)
    k10.canvas->canvasLine(15, 276, 225, 276, 0x3D290F);
    k10.canvas->canvasText("LTR-303ALS Optical Sensor", 24, 290, 0x8D6E63,
                           k10.canvas->eCNAndENFont16, 26, false);

    k10.canvas->updateCanvas();
    delay(150);
}
