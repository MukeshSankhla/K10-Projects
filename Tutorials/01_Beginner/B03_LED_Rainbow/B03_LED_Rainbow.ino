#include "unihiker_k10.h"

UNIHIKER_K10 k10;

// Helper function to convert HSV (0-360 deg) to 24-bit RGB
uint32_t hsvToRgb(float h, float s, float v) {
    int i = int(h / 60.0) % 6;
    float f = (h / 60.0) - i;
    float p = v * (1.0 - s);
    float q = v * (1.0 - f * s);
    float t = v * (1.0 - (1.0 - f) * s);
    float r = 0, g = 0, b = 0;
    switch (i) {
        case 0: r = v; g = t; b = p; break;
        case 1: r = q; g = v; b = p; break;
        case 2: r = p; g = v; b = t; break;
        case 3: r = p; g = q; b = v; break;
        case 4: r = t; g = p; b = v; break;
        case 5: r = v; g = p; b = q; break;
    }
    return ((uint32_t(r * 255) << 16) | (uint32_t(g * 255) << 8) | uint32_t(b * 255));
}

void setup() {
    k10.begin();
    k10.rgb->brightness(5);
}

void loop() {
    static float hue = 0;
    // Update each LED with a slight phase shift (30 degrees apart)
    for (int i = 0; i < 3; i++) {
        float ledHue = fmod(hue + (i * 30.0), 360.0);
        uint32_t color = hsvToRgb(ledHue, 1.0, 1.0);
        k10.rgb->write(i, color);
    }
    hue += 2.0;
    if (hue >= 360.0) hue = 0;
    delay(20);
}
