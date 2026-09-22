#include "unihiker_k10.h"

UNIHIKER_K10 k10;
uint8_t screen_dir = 2; // Portrait orientation (240x320)
AHT20 aht20;

void drawSensorCard(int x, int y, int w, int h, const char* title, String value1, String value2, uint32_t accentColor) {
    // Card background & left accent border
    k10.canvas->canvasRectangle(x, y, w, h, 0x1E293B, 0x101726, true);
    k10.canvas->canvasLine(x, y, x, y + h, accentColor);
    k10.canvas->canvasLine(x + 1, y, x + 1, y + h, accentColor);

    // Card Title
    k10.canvas->canvasText(title, x + 8, y + 6, 0x94A3B8,
                           k10.canvas->eCNAndENFont16, 12, false);

    // Primary Value
    k10.canvas->canvasText(value1, x + 8, y + 26, accentColor,
                           k10.canvas->eCNAndENFont16, 14, false);

    // Secondary Value / Condition Badge
    if (value2.length() > 0) {
        k10.canvas->canvasText(value2, x + 8, y + 44, 0x64748B,
                               k10.canvas->eCNAndENFont16, 12, false);
    }
}

void setup() {
    k10.begin();
    k10.initScreen(screen_dir);
    k10.creatCanvas();
    // Spacecraft Dark Void background
    k10.setScreenBackground(0x080B14);

    k10.rgb->brightness(5);
    k10.rgb->write(-1, 0x00F0FF); // Cyan HUD glow
}

void loop() {
    // Read hardware sensors
    float tempC = aht20.getData(AHT20::eAHT20TempC);
    float humi = aht20.getData(AHT20::eAHT20HumiRH);
    int lightALS = k10.readALS();
    int accX = k10.getAccelerometerX();
    int accY = k10.getAccelerometerY();
    int accZ = k10.getAccelerometerZ();
    int strength = k10.getStrength();

    k10.canvas->canvasClear();

    // 1. Mission Control Header Banner
    k10.canvas->canvasRectangle(0, 0, 240, 42, 0x0F1524, 0x0F1524, true);
    k10.canvas->canvasLine(0, 42, 240, 42, 0x00F0FF);
    k10.canvas->canvasText("TELEMETRY HUD", 46, 10, 0x00F0FF,
                           k10.canvas->eCNAndENFont24, 15, false);

    // 2. Render 4 Symmetrical Sensor Pods (w=100 each, Left=14, Right=126)
    // Pod 1: Temperature
    String tempStr1 = String(tempC, 1) + " C";
    String tempStr2 = (tempC > 30.0) ? "Warm" : "Nominal";
    drawSensorCard(14, 50, 100, 66, "Temp", tempStr1, tempStr2, 0xFF5555);

    // Pod 2: Humidity
    String humiStr1 = String(humi, 1) + " %";
    String humiStr2 = (humi > 60.0) ? "Humid" : "Optimal";
    drawSensorCard(126, 50, 100, 66, "Humidity", humiStr1, humiStr2, 0x38BDF8);

    // Pod 3: Ambient Light
    String lightStr1 = String(lightALS) + " lx";
    String lightStr2 = (lightALS > 1000) ? "Bright" : "Indoor";
    drawSensorCard(14, 124, 100, 66, "Light", lightStr1, lightStr2, 0xFACC15);

    // Pod 4: Accelerometer Magnitude
    String strStr1 = String(strength) + " mg";
    String strStr2 = "1.0G Ref";
    drawSensorCard(126, 124, 100, 66, "G-Force", strStr1, strStr2, 0x00FF9D);

    // 3. Wide IMU 3-Axis Breakdown Pod
    k10.canvas->canvasRectangle(14, 198, 212, 66, 0x1E293B, 0x101726, true);
    k10.canvas->canvasText("3-Axis Vectors (mG)", 24, 204, 0x94A3B8,
                           k10.canvas->eCNAndENFont16, 22, false);

    String axStr = "X: " + String(accX);
    String ayStr = "Y: " + String(accY);
    String azStr = "Z: " + String(accZ);
    k10.canvas->canvasText(axStr, 24, 226, 0x00F0FF, k10.canvas->eCNAndENFont16, 12, false);
    k10.canvas->canvasText(ayStr, 24, 244, 0x38BDF8, k10.canvas->eCNAndENFont16, 12, false);
    k10.canvas->canvasText(azStr, 126, 226, 0xFACC15, k10.canvas->eCNAndENFont16, 12, false);

    // 4. Centered Footer Bar (Zero-overflow)
    k10.canvas->canvasLine(15, 276, 225, 276, 0x1E293B);
    k10.canvas->canvasText("Mission Control Node", 42, 290, 0x64748B,
                           k10.canvas->eCNAndENFont16, 24, false);

    k10.canvas->updateCanvas();
    delay(100);
}
