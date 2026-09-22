#include "unihiker_k10.h"

UNIHIKER_K10 k10;
uint8_t screen_dir = 2; // Portrait orientation (240x320)

void setup() {
    k10.begin();
    k10.initScreen(screen_dir);
    k10.creatCanvas();
    // Modern Indigo Dark theme background
    k10.setScreenBackground(0x0F172A);

    // 1. Header Banner
    k10.canvas->canvasRectangle(0, 0, 240, 44, 0x1E1B4B, 0x1E1B4B, true);
    k10.canvas->canvasLine(0, 44, 240, 44, 0x6366F1);
    k10.canvas->canvasText("UNIHIKER K10", 42, 10, 0xFEE715,
                           k10.canvas->eCNAndENFont24, 15, false);

    // 2. Welcome Card
    k10.canvas->canvasRectangle(14, 56, 212, 60, 0x6366F1, 0x1E293B, true);
    k10.canvas->canvasText("Hello, World!", 26, 68, 0xF8FAFC,
                           k10.canvas->eCNAndENFont24, 15, false);
    k10.canvas->canvasText("Embedded GUI Architecture", 26, 96, 0x818CF8,
                           k10.canvas->eCNAndENFont16, 26, false);

    // 3. Feature Overview Card
    k10.canvas->canvasRectangle(14, 126, 212, 80, 0x334155, 0x1E293B, true);
    k10.canvas->canvasText("Resolution: 240 x 320 px", 24, 136, 0x00E5FF,
                           k10.canvas->eCNAndENFont16, 24, false);
    k10.canvas->canvasText("Color Depth: 24-bit RGB", 24, 158, 0x94A3B8,
                           k10.canvas->eCNAndENFont16, 24, false);
    k10.canvas->canvasText("Buffer: RAM Frame Canvas", 24, 180, 0x94A3B8,
                           k10.canvas->eCNAndENFont16, 24, false);

    // 4. Live Telemetry Card Base
    k10.canvas->canvasRectangle(14, 216, 212, 54, 0x10B981, 0x064E3B, true);
    k10.canvas->canvasText("Live Heartbeat Telemetry", 24, 224, 0xA7F3D0,
                           k10.canvas->eCNAndENFont16, 24, false);

    // 5. Centered Footer Bar
    k10.canvas->canvasLine(15, 282, 225, 282, 0x334155);
    k10.canvas->canvasText("Ready for Development", 36, 294, 0x64748B,
                           k10.canvas->eCNAndENFont16, 24, false);

    k10.canvas->updateCanvas();
}

void loop() {
    static int seconds = 0;
    seconds++;

    // Selective redraw of the dynamic counter card (eliminates autoClean full-screen wipe bug)
    k10.canvas->canvasRectangle(24, 244, 192, 20, 0x064E3B, 0x064E3B, true);

    String timeStr = "Uptime: " + String(seconds) + " s";
    k10.canvas->canvasText(timeStr, 24, 246, 0x34D399,
                           k10.canvas->eCNAndENFont16, 20, false);

    k10.canvas->updateCanvas();
    delay(1000); // 1-second cadence
}
