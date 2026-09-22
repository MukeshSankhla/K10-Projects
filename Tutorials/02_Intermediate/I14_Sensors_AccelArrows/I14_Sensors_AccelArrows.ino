#include "unihiker_k10.h"

UNIHIKER_K10 k10;
uint8_t screen_dir = 2; // Portrait orientation (240x320)

// Helper to draw a bold directional arrow pointing in one of 4 cardinal directions
void drawDirectionArrow(int centerX, int centerY, int direction, uint32_t color) {
    // direction: 0 = Flat/Center, 1 = Forward (Up), 2 = Back (Down), 3 = Left, 4 = Right
    if (direction == 0) {
        // Level Circle
        k10.canvas->canvasCircle(centerX, centerY, 24, color, 0x1E293B, true);
        k10.canvas->canvasCircle(centerX, centerY, 8, color, color, true);
    } else if (direction == 1) { // Forward (Up)
        k10.canvas->canvasLine(centerX, centerY - 45, centerX - 30, centerY - 15, color);
        k10.canvas->canvasLine(centerX, centerY - 45, centerX + 30, centerY - 15, color);
        k10.canvas->canvasLine(centerX, centerY - 45, centerX, centerY + 45, color);
        k10.canvas->canvasLine(centerX - 1, centerY - 45, centerX - 1, centerY + 45, color);
        k10.canvas->canvasLine(centerX + 1, centerY - 45, centerX + 1, centerY + 45, color);
    } else if (direction == 2) { // Back (Down)
        k10.canvas->canvasLine(centerX, centerY + 45, centerX - 30, centerY + 15, color);
        k10.canvas->canvasLine(centerX, centerY + 45, centerX + 30, centerY + 15, color);
        k10.canvas->canvasLine(centerX, centerY - 45, centerX, centerY + 45, color);
        k10.canvas->canvasLine(centerX - 1, centerY - 45, centerX - 1, centerY + 45, color);
        k10.canvas->canvasLine(centerX + 1, centerY - 45, centerX + 1, centerY + 45, color);
    } else if (direction == 3) { // Left
        k10.canvas->canvasLine(centerX - 45, centerY, centerX - 15, centerY - 30, color);
        k10.canvas->canvasLine(centerX - 45, centerY, centerX - 15, centerY + 30, color);
        k10.canvas->canvasLine(centerX - 45, centerY, centerX + 45, centerY, color);
        k10.canvas->canvasLine(centerX - 45, centerY - 1, centerX + 45, centerY - 1, color);
        k10.canvas->canvasLine(centerX - 45, centerY + 1, centerX + 45, centerY + 1, color);
    } else if (direction == 4) { // Right
        k10.canvas->canvasLine(centerX + 45, centerY, centerX + 15, centerY - 30, color);
        k10.canvas->canvasLine(centerX + 45, centerY, centerX + 15, centerY + 30, color);
        k10.canvas->canvasLine(centerX - 45, centerY, centerX + 45, centerY, color);
        k10.canvas->canvasLine(centerX - 45, centerY - 1, centerX + 45, centerY - 1, color);
        k10.canvas->canvasLine(centerX - 45, centerY + 1, centerX + 45, centerY + 1, color);
    }
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
    int accX = k10.getAccelerometerX();
    int accY = k10.getAccelerometerY();

    int direction = 0; // 0 = Center
    String dirLabel = "Level (Flat)";
    uint32_t arrowColor = 0x00FF88;

    // Evaluate tilt direction using accelerometer gravity vectors
    if (k10.isGesture(TiltForward) || accY < -300) {
        direction = 1;
        dirLabel = "Tilt FORWARD";
        arrowColor = 0x00E5FF;
    } else if (k10.isGesture(TiltBack) || accY > 300) {
        direction = 2;
        dirLabel = "Tilt BACKWARD";
        arrowColor = 0xFEE715;
    } else if (k10.isGesture(TiltLeft) || accX < -300) {
        direction = 3;
        dirLabel = "Tilt LEFT";
        arrowColor = 0x38BDF8;
    } else if (k10.isGesture(TiltRight) || accX > 300) {
        direction = 4;
        dirLabel = "Tilt RIGHT";
        arrowColor = 0xFF007F;
    }

    k10.canvas->canvasClear();

    // 1. Header
    k10.canvas->canvasText("TILT DETECTOR", 32, 14, 0xFEE715,
                           k10.canvas->eCNAndENFont24, 20, false);
    k10.canvas->canvasLine(15, 44, 225, 44, 0x334155);

    // 2. Compass Outer Ring
    k10.canvas->canvasCircle(120, 160, 78, 0x334155, 0x111827, true);
    k10.canvas->canvasCircle(120, 160, 72, 0x1E293B, 0x111827, false);

    // Render Arrow
    drawDirectionArrow(120, 160, direction, arrowColor);

    // 3. Direction readout banner
    k10.canvas->canvasRectangle(20, 245, 200, 32, 0x38BDF8, 0x1E293B, true);
    k10.canvas->canvasText(dirLabel, 30, 252, arrowColor,
                           k10.canvas->eCNAndENFont16, 20, false);

    // 4. Raw Accel Readout
    String rawStr = "X: " + String(accX) + "  Y: " + String(accY);
    k10.canvas->canvasText(rawStr, 35, 292, 0x64748B,
                           k10.canvas->eCNAndENFont16, 25, false);

    k10.canvas->updateCanvas();
    delay(50);
}
