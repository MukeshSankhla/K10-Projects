#include "unihiker_k10.h"

UNIHIKER_K10 k10;

void setup() {
  k10.begin();
  k10.initScreen(2); // Screen direction: 2 = Portrait mode (240x320)
  k10.creatCanvas(); // Allocate internal frame buffer in RAM
  k10.setScreenBackground(0x101820); // Set sleek dark background color

  // =========================================================================
  // canvasText API Parameter Guide:
  // k10.canvas->canvasText(text, x, y, color, font, count, autoClean);
  //
  //  1. text      : String / const char* / float to display
  //  2. x         : Horizontal pixel position (0 to 240 in portrait mode)
  //  3. y         : Vertical pixel position (0 to 320 in portrait mode)
  //  4. color     : 24-bit Hex RGB color code (0xRRGGBB)
  //  5. font      : eCNAndENFont24 (24px bold) or eCNAndENFont16 (16px compact)
  //  6. count     : Line wrap threshold (characters per line before wrapping)
  //  7. autoClean : If true, auto-erases background before drawing (no
  //  ghosting)
  // =========================================================================

  // Header in bright gold (24px font)
  k10.canvas->canvasText("UNIHIKER K10", 40, 35, 0xFEE715,
                         k10.canvas->eCNAndENFont24, 20, true);

  // Subtitle in crisp white (24px font)
  k10.canvas->canvasText("Hello, World!", 40, 65, 0xFFFFFF,
                         k10.canvas->eCNAndENFont24, 20, true);

  // Section label in vibrant cyan (16px font)
  k10.canvas->canvasText("Level 1: Beginner", 25, 120, 0x00E5FF,
                         k10.canvas->eCNAndENFont16, 20, true);

  // Multi-line description text in soft silver
  k10.canvas->canvasText("Ready to create amazing", 25, 150, 0xCCCCCC,
                         k10.canvas->eCNAndENFont16, 25, true);
  k10.canvas->canvasText("embedded projects!", 25, 175, 0xCCCCCC,
                         k10.canvas->eCNAndENFont16, 20, true);

  // Guide for the live counter below
  k10.canvas->canvasText("Live autoClean Demo:", 25, 230, 0x94A3B8,
                         k10.canvas->eCNAndENFont16, 20, true);

  // Push the initial canvas buffer to the physical screen
  k10.canvas->updateCanvas();
}

void loop() {
  // Dynamic text demonstration:
  // With autoClean = true, the counter updates cleanly without ghosting or
  // flickering!
  static int seconds = 0;
  seconds++;

  String timeStr = "Uptime: " + String(seconds) + " s";
  k10.canvas->canvasText(timeStr, 25, 260, 0x10B981, k10.canvas->eCNAndENFont24,
                         20, true);
  k10.canvas->updateCanvas();

  delay(1000); // Update once every second
}
