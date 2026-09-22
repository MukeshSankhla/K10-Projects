#include "unihiker_k10.h"

UNIHIKER_K10 k10;
uint8_t screen_dir = 2; // Portrait orientation (240x320)

struct ColorPreset {
    const char* name;
    uint32_t hexValue;
    const char* hexStr;
};

const ColorPreset presets[] = {
    {"Nordic Teal",    0x008080, "#008080"},
    {"Warm Amber",     0xF59E0B, "#F59E0B"},
    {"Royal Purple",   0x7C3AED, "#7C3AED"},
    {"Cobalt Navy",    0x1D4ED8, "#1D4ED8"},
    {"Emerald Marine", 0x059669, "#059669"},
    {"Crimson Rose",   0xDC2626, "#DC2626"}
};
const int totalPresets = sizeof(presets) / sizeof(presets[0]);

void displayColorCard(int index) {
    k10.setScreenBackground(presets[index].hexValue);
    k10.canvas->canvasClear();

    // 1. Semi-translucent dark HUD card centered in the screen
    k10.canvas->canvasRectangle(24, 110, 192, 100, 0xFFFFFF, 0x0F172A, true);

    // Color Name (Centered)
    String nameStr = String(presets[index].name);
    int nameX = 120 - (int)(nameStr.length() * 4);
    k10.canvas->canvasText(nameStr, nameX, 126, 0xF8FAFC,
                           k10.canvas->eCNAndENFont16, 20, false);

    // Hex Code Badge
    String hexStr = String(presets[index].hexStr);
    int hexX = 120 - (int)(hexStr.length() * 4);
    k10.canvas->canvasText(hexStr, hexX, 154, 0x38BDF8,
                           k10.canvas->eCNAndENFont16, 12, false);

    // Index Counter
    String countStr = String(index + 1) + " of " + String(totalPresets);
    int countX = 120 - (int)(countStr.length() * 4);
    k10.canvas->canvasText(countStr, countX, 180, 0x94A3B8,
                           k10.canvas->eCNAndENFont16, 12, false);

    k10.canvas->updateCanvas();

    // Mirror on RGB LED
    k10.rgb->write(-1, presets[index].hexValue);
}

void setup() {
    k10.begin();
    k10.initScreen(screen_dir);
    k10.creatCanvas();

    k10.rgb->brightness(5);
}

void loop() {
    for (int i = 0; i < totalPresets; i++) {
        displayColorCard(i);
        delay(1500);
    }
}
