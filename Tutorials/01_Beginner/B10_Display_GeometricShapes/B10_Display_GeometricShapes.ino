#include "unihiker_k10.h"

UNIHIKER_K10 k10;

void setup() {
    k10.begin();
    k10.initScreen(2);               // 2 = Portrait orientation (240x320)
    k10.creatCanvas();               // Allocate 2D graphics canvas buffer in RAM
    k10.setScreenBackground(0x050515); // Deep space background

    // =========================================================================
    // 1. canvasLine API:
    //    k10.canvas->canvasLine(x1, y1, x2, y2, color);
    //    - x1, y1 : Starting coordinate pair
    //    - x2, y2 : Ending coordinate pair
    //    - color  : 24-bit Hex RGB color (0xRRGGBB)
    // =========================================================================
    k10.canvas->canvasLine(0, 0, 240, 320, 0x333366);
    k10.canvas->canvasLine(240, 0, 0, 320, 0x333366);

    // =========================================================================
    // 2. canvasRectangle API:
    //    k10.canvas->canvasRectangle(x, y, w, h, borderColor, fillColor, fill);
    //    - x, y        : Top-left coordinate
    //    - w, h        : Width and Height in pixels
    //    - borderColor : Outer edge outline color
    //    - fillColor   : Interior fill color
    //    - fill        : true = filled interior, false = transparent outline
    // =========================================================================
    // Filled rectangle with cyan border and deep blue interior
    k10.canvas->canvasRectangle(20, 30, 200, 70, 0x00FFFF, 0x003366, true);

    // =========================================================================
    // 3. canvasCircle API:
    //    k10.canvas->canvasCircle(x, y, radius, borderColor, fillColor, fill);
    //    - x, y        : Center point coordinate
    //    - radius      : Radius in pixels
    //    - borderColor : Circumference outline color
    //    - fillColor   : Interior disk color
    //    - fill        : true = solid filled disk, false = hollow ring
    // =========================================================================
    // Hollow circle: Outer pink neon ring
    k10.canvas->canvasCircle(120, 160, 45, 0xFF007F, 0x000000, false);

    // Solid circle: Inner yellow core
    k10.canvas->canvasCircle(120, 160, 20, 0xFFFF00, 0xFFFF00, true);

    // Hollow rectangle: Green bounding box
    k10.canvas->canvasRectangle(30, 230, 180, 60, 0x00FF88, 0x000000, false);

    // Push frame buffer to physical ST7789 display
    k10.canvas->updateCanvas();
}

void loop() {
    // Static vector shapes; loop remains idle
}
