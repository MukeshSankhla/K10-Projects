#include "unihiker_k10.h"

UNIHIKER_K10 k10;
uint8_t screen_dir = 2; // Portrait orientation (240x320)

// Clean Minimalist Light Theme Palette
const uint32_t COLOR_BG        = 0xF8FAFC; // Soft Slate Off-White
const uint32_t COLOR_CARD      = 0xFFFFFF; // Pure White Card Fill
const uint32_t COLOR_BORDER    = 0xE2E8F0; // Delicate 1px Border
const uint32_t COLOR_BORDER_DIAL=0xCBD5E1; // Dial Ring 1px Border
const uint32_t COLOR_TEXT_PRI  = 0x0F172A; // Deep Slate Charcoal
const uint32_t COLOR_TEXT_SEC  = 0x334155; // Slate Secondary
const uint32_t COLOR_TEXT_MUTED= 0x64748B; // Slate Muted Label
const uint32_t COLOR_TEAL      = 0x0D9488; // Teal Brand Accent
const uint32_t COLOR_GREEN     = 0x16A34A; // Level Green
const uint32_t COLOR_BLUE      = 0x2563EB; // Direction Blue

void drawDirectionArrow(int centerX, int centerY, int direction, uint32_t color) {
    if (direction == 0) {
        // Level Target Circle
        k10.canvas->canvasCircle(centerX, centerY, 16, COLOR_GREEN, COLOR_GREEN, true);
        k10.canvas->canvasCircle(centerX, centerY, 6, COLOR_CARD, COLOR_CARD, true);
    } else if (direction == 1) { // Up / Forward
        k10.canvas->canvasLine(centerX, centerY - 32, centerX - 18, centerY - 10, color);
        k10.canvas->canvasLine(centerX, centerY - 32, centerX + 18, centerY - 10, color);
        k10.canvas->canvasLine(centerX, centerY - 32, centerX, centerY + 32, color);
    } else if (direction == 2) { // Down / Back
        k10.canvas->canvasLine(centerX, centerY + 32, centerX - 18, centerY + 10, color);
        k10.canvas->canvasLine(centerX, centerY + 32, centerX + 18, centerY + 10, color);
        k10.canvas->canvasLine(centerX, centerY - 32, centerX, centerY + 32, color);
    } else if (direction == 3) { // Left
        k10.canvas->canvasLine(centerX - 32, centerY, centerX - 10, centerY - 18, color);
        k10.canvas->canvasLine(centerX - 32, centerY, centerX - 10, centerY + 18, color);
        k10.canvas->canvasLine(centerX - 32, centerY, centerX + 32, centerY, color);
    } else if (direction == 4) { // Right
        k10.canvas->canvasLine(centerX + 32, centerY, centerX + 10, centerY - 18, color);
        k10.canvas->canvasLine(centerX + 32, centerY, centerX + 10, centerY + 18, color);
        k10.canvas->canvasLine(centerX - 32, centerY, centerX + 32, centerY, color);
    }
}

void drawScreenChrome() {
    k10.canvas->canvasClear();
    k10.canvas->canvasRectangle(0, 0, 240, 320, COLOR_BG, COLOR_BG, true);

    // App Header Bar (y: 0 to 40)
    k10.canvas->canvasRectangle(0, 0, 240, 40, COLOR_CARD, COLOR_CARD, true);
    k10.canvas->canvasLine(0, 40, 240, 40, COLOR_BORDER);
    k10.canvas->canvasText("Tilt Compass", 14, 12, COLOR_TEXT_PRI, k10.canvas->eCNAndENFont16, 50, false);
    // Teal brand dot
    k10.canvas->canvasCircle(224, 20, 4, COLOR_TEAL, COLOR_TEAL, true);

    // Compass Card Frame (y: 48 to 226)
    k10.canvas->canvasRectangle(10, 48, 220, 178, COLOR_BORDER, COLOR_CARD, true);

    // Status Banner Card (y: 234 to 278)
    k10.canvas->canvasRectangle(10, 234, 220, 44, COLOR_BORDER, COLOR_CARD, true);

    // Footer Bar (y: 284 to 320)
    k10.canvas->canvasRectangle(0, 284, 240, 36, COLOR_CARD, COLOR_CARD, true);
    k10.canvas->canvasLine(0, 284, 240, 284, COLOR_BORDER);
    k10.canvas->canvasText("3-Axis Accelerometer Spirit Level", 14, 294, COLOR_TEXT_MUTED, k10.canvas->eCNAndENFont16, 50, false);
}

void updateRadarReticle(int centerX, int centerY, int direction, uint32_t arrowColor) {
    // Clear inner scope area
    k10.canvas->canvasCircle(centerX, centerY, 68, COLOR_CARD, COLOR_CARD, true);

    // Concentric range rings
    k10.canvas->canvasCircle(centerX, centerY, 66, COLOR_BORDER_DIAL, COLOR_CARD, false);
    k10.canvas->canvasCircle(centerX, centerY, 44, COLOR_BORDER, COLOR_CARD, false);
    k10.canvas->canvasCircle(centerX, centerY, 22, COLOR_BORDER, COLOR_CARD, false);

    // Crosshairs
    k10.canvas->canvasLine(centerX - 66, centerY, centerX + 66, centerY, COLOR_BORDER);
    k10.canvas->canvasLine(centerX, centerY - 66, centerX, centerY + 66, COLOR_BORDER);

    drawDirectionArrow(centerX, centerY, direction, arrowColor);
}

void updateTelemetryCards(const char* label, uint32_t labelColor, float ax, float ay) {
    // Clear status banner
    k10.canvas->canvasRectangle(12, 236, 216, 40, COLOR_CARD, COLOR_CARD, true);

    int labelX = 120 - (int)(strlen(label) * 4);
    k10.canvas->canvasText(label, labelX, 242, labelColor, k10.canvas->eCNAndENFont16, 20, false);

    String tiltStr = "Pitch: " + String(ay, 2) + "  Roll: " + String(ax, 2);
    int tiltX = 120 - (int)(tiltStr.length() * 4);
    k10.canvas->canvasText(tiltStr, tiltX, 260, COLOR_TEXT_MUTED, k10.canvas->eCNAndENFont16, 26, false);

    k10.canvas->updateCanvas();
}

void setup() {
    k10.begin();
    k10.initScreen(screen_dir);
    k10.creatCanvas();
    k10.setScreenBackground(COLOR_BG);

    k10.rgb->brightness(5);
    k10.rgb->write(-1, 0x0D9488);

    drawScreenChrome();
    k10.canvas->updateCanvas();
}

void loop() {
    float ax = k10.getAccelerometerX() / 1000.0;
    float ay = k10.getAccelerometerY() / 1000.0;

    int direction = 0;
    const char* label = "LEVEL - BALANCED";
    uint32_t arrowColor = COLOR_GREEN;

    if (ay > 0.30) {
        direction = 1;
        label = "TILT: FORWARD";
        arrowColor = COLOR_BLUE;
    } else if (ay < -0.30) {
        direction = 2;
        label = "TILT: BACKWARD";
        arrowColor = COLOR_BLUE;
    } else if (ax > 0.30) {
        direction = 3;
        label = "TILT: LEFT";
        arrowColor = COLOR_BLUE;
    } else if (ax < -0.30) {
        direction = 4;
        label = "TILT: RIGHT";
        arrowColor = COLOR_BLUE;
    }

    updateRadarReticle(120, 137, direction, arrowColor);
    updateTelemetryCards(label, arrowColor, ax, ay);

    delay(50);
}
