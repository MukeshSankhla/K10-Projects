#include "unihiker_k10.h"

UNIHIKER_K10 k10;
uint8_t screen_dir = 2; // Portrait orientation (240x320)

int qrMode = 0; // 0 = Official Website, 1 = Wi-Fi Setup

void renderQRCode(int mode) {
    k10.canvas->canvasClear();
    k10.setScreenBackground(0x0F172A);

    // 1. Header
    k10.canvas->canvasText("QR CODE DEMO", 45, 14, 0xFEE715,
                           k10.canvas->eCNAndENFont24, 20, false);
    k10.canvas->canvasLine(15, 44, 225, 44, 0x334155);

    // 2. White Card Container for QR Code (High contrast scanning)
    k10.canvas->canvasRectangle(20, 56, 200, 180, 0x38BDF8, 0xFFFFFF, true);

    if (mode == 0) {
        // Draw Website QR Code
        k10.canvasDrawCode("https://www.unihiker.com");

        k10.canvas->canvasText("Target: Official Website", 20, 245, 0x00E5FF,
                               k10.canvas->eCNAndENFont16, 26, false);
        k10.canvas->canvasText("https://www.unihiker.com", 20, 268, 0x94A3B8,
                               k10.canvas->eCNAndENFont16, 26, false);
    } else {
        // Draw Wi-Fi Configuration QR Code
        k10.canvasDrawCode("WIFI:S:UNIHIKER_K10;T:WPA;P:12345678;;");

        k10.canvas->canvasText("Target: Wi-Fi Credentials", 20, 245, 0x00FF88,
                               k10.canvas->eCNAndENFont16, 26, false);
        k10.canvas->canvasText("SSID: UNIHIKER_K10", 20, 268, 0x94A3B8,
                               k10.canvas->eCNAndENFont16, 26, false);
    }

    // 3. Navigation Controls
    k10.canvas->canvasLine(15, 292, 225, 292, 0x334155);
    k10.canvas->canvasText("Press [A]: Switch QR Code", 20, 300, 0xFEE715,
                           k10.canvas->eCNAndENFont16, 26, false);

    k10.canvas->updateCanvas();
}

void setup() {
    k10.begin();
    k10.initScreen(screen_dir);
    k10.creatCanvas();

    k10.rgb->brightness(5);
    k10.rgb->write(-1, 0x00E5FF);

    renderQRCode(qrMode);
}

void loop() {
    if (k10.buttonA->isPressed()) {
        qrMode = (qrMode + 1) % 2;
        renderQRCode(qrMode);

        while (k10.buttonA->isPressed()) {
            delay(50);
        }
    }
    delay(20);
}
