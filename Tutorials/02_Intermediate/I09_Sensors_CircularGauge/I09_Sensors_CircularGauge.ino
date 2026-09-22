#include "unihiker_k10.h"

UNIHIKER_K10 k10;
uint8_t screen_dir = 2; // Portrait orientation (240x320)
AHT20 aht20;

int activeSensor = 0; // 0 = Temperature (0-50 C), 1 = Light Sensor (0-1000 lux)

const float START_ANGLE = 150.0 * (PI / 180.0);
const float SWEEP_ANGLE = 240.0 * (PI / 180.0);

// Clean Minimalist Light Theme Palette
const uint32_t COLOR_BG        = 0xF8FAFC; // Soft Slate Off-White
const uint32_t COLOR_CARD      = 0xFFFFFF; // Pure White Card Fill
const uint32_t COLOR_BORDER    = 0xE2E8F0; // Delicate 1px Border
const uint32_t COLOR_BORDER_DIAL=0xCBD5E1; // Dial 1px Border
const uint32_t COLOR_TEXT_PRI  = 0x0F172A; // Deep Slate Charcoal
const uint32_t COLOR_TEXT_MUTED= 0x64748B; // Slate Muted Label
const uint32_t COLOR_CORAL     = 0xE11D48; // Coral
const uint32_t COLOR_AMBER     = 0xD97706; // Amber
const uint32_t COLOR_WHITE     = 0xFFFFFF;

bool checkButtonAPressed() {
    static bool lastState = false;
    static unsigned long lastDebounceTime = 0;
    bool reading = k10.buttonA->isPressed();
    bool pressedEvent = false;

    if (reading != lastState) {
        lastDebounceTime = millis();
    }
    if ((millis() - lastDebounceTime) > 35) {
        static bool stableState = false;
        if (reading != stableState) {
            stableState = reading;
            if (stableState) {
                pressedEvent = true;
            }
        }
    }
    lastState = reading;
    return pressedEvent;
}

void drawCircularGaugeStaticChrome(int centerX, int centerY, int radius) {
    k10.canvas->canvasCircle(centerX, centerY, radius + 2, COLOR_BORDER_DIAL, COLOR_CARD, true);

    for (int t = 0; t <= 10; t++) {
        float theta = START_ANGLE + (t * SWEEP_ANGLE / 10.0);
        int x1 = centerX + (int)(cos(theta) * (radius - 2));
        int y1 = centerY + (int)(sin(theta) * (radius - 2));
        int x2 = centerX + (int)(cos(theta) * (radius - 8));
        int y2 = centerY + (int)(sin(theta) * (radius - 8));

        k10.canvas->canvasLine(x1, y1, x2, y2, 0x94A3B8);
    }
}

void updateGaugeLabels(int centerX, int centerY, const char* label, const char* unit) {
    k10.canvas->canvasRectangle(centerX - 60, centerY - 48, 120, 18, COLOR_CARD, COLOR_CARD, true);
    k10.canvas->canvasRectangle(centerX - 40, centerY + 46, 80, 18, COLOR_CARD, COLOR_CARD, true);

    int labelX = centerX - (int)(strlen(label) * 4);
    k10.canvas->canvasText(label, labelX, centerY - 44, COLOR_TEXT_MUTED, k10.canvas->eCNAndENFont16, 16, false);

    int unitX = centerX - (int)(strlen(unit) * 4);
    k10.canvas->canvasText(unit, unitX, centerY + 46, COLOR_TEXT_MUTED, k10.canvas->eCNAndENFont16, 10, false);
}

void updateCircularGaugeNeedle(int centerX, int centerY, int radius, float value, float minVal, float maxVal, uint32_t activeColor) {
    k10.canvas->canvasCircle(centerX, centerY, radius - 10, COLOR_CARD, COLOR_CARD, true);

    float constrainedVal = constrain(value, minVal, maxVal);
    float normVal = (constrainedVal - minVal) / (maxVal - minVal);
    float needleAngle = START_ANGLE + (normVal * SWEEP_ANGLE);

    int needleX = centerX + (int)(cos(needleAngle) * (radius - 14));
    int needleY = centerY + (int)(sin(needleAngle) * (radius - 14));

    k10.canvas->canvasLine(centerX, centerY, needleX, needleY, activeColor);
    k10.canvas->canvasLine(centerX + 1, centerY, needleX, needleY, activeColor);

    k10.canvas->canvasCircle(centerX, centerY, 6, COLOR_TEXT_PRI, COLOR_TEXT_PRI, true);
    k10.canvas->canvasCircle(centerX, centerY, 3, COLOR_CARD, COLOR_CARD, true);

    String valStr = (activeSensor == 0) ? String(constrainedVal, 1) : String((int)constrainedVal);
    int textX = centerX - (int)(valStr.length() * 8);
    k10.canvas->canvasText(valStr, textX, centerY + 22, COLOR_TEXT_PRI, k10.canvas->eCNAndENFont24, 6, false);
}

void drawScreenChrome() {
    k10.canvas->canvasClear();
    k10.canvas->canvasRectangle(0, 0, 240, 320, COLOR_BG, COLOR_BG, true);

    // App Header Bar (y: 0 to 40)
    k10.canvas->canvasRectangle(0, 0, 240, 40, COLOR_CARD, COLOR_CARD, true);
    k10.canvas->canvasLine(0, 40, 240, 40, COLOR_BORDER);
    k10.canvas->canvasText("Sensor Dial", 14, 12, COLOR_TEXT_PRI, k10.canvas->eCNAndENFont16, 50, false);
    // Coral brand dot
    k10.canvas->canvasCircle(224, 20, 4, COLOR_CORAL, COLOR_CORAL, true);

    // Card Container (y: 48 to 278)
    k10.canvas->canvasRectangle(10, 48, 220, 230, COLOR_BORDER, COLOR_CARD, true);

    drawCircularGaugeStaticChrome(120, 160, 80);

    // Footer Bar (y: 284 to 320)
    k10.canvas->canvasRectangle(0, 284, 240, 36, COLOR_CARD, COLOR_CARD, true);
    k10.canvas->canvasLine(0, 284, 240, 284, COLOR_BORDER);
    k10.canvas->canvasText("[A] Switch Sensor Dial", 14, 294, COLOR_CORAL, k10.canvas->eCNAndENFont16, 50, false);
}

void setup() {
    k10.begin();
    k10.initScreen(screen_dir);
    k10.creatCanvas();
    k10.setScreenBackground(COLOR_BG);

    k10.rgb->brightness(5);
    k10.rgb->write(-1, 0xE11D48);

    drawScreenChrome();
    updateGaugeLabels(120, 160, "Temperature", "deg C");
    k10.canvas->updateCanvas();
}

void loop() {
    if (checkButtonAPressed()) {
        activeSensor = 1 - activeSensor;
        if (activeSensor == 0) {
            updateGaugeLabels(120, 160, "Temperature", "deg C");
            k10.rgb->write(-1, 0xE11D48);
        } else {
            updateGaugeLabels(120, 160, "Ambient Light", "Lux");
            k10.rgb->write(-1, 0xD97706);
        }
    }

    if (activeSensor == 0) {
        float temp = aht20.getData(AHT20::eAHT20TempC);
        updateCircularGaugeNeedle(120, 160, 80, temp, 0.0, 50.0, COLOR_CORAL);
    } else {
        uint16_t light = k10.readALS();
        updateCircularGaugeNeedle(120, 160, 80, (float)light, 0.0, 1000.0, COLOR_AMBER);
    }

    k10.canvas->updateCanvas();
    delay(50);
}
