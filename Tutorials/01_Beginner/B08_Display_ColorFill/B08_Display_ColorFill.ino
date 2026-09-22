#include "unihiker_k10.h"

UNIHIKER_K10 k10;

void setup() {
    k10.begin();
    k10.initScreen(2); // Portrait orientation (240x320)
    k10.creatCanvas();
}

void loop() {
    // Cycle through 4 solid background colors every 1.5 seconds
    k10.setScreenBackground(0x008080); // Teal
    delay(1500);

    k10.setScreenBackground(0xFFBF00); // Amber
    delay(1500);

    k10.setScreenBackground(0x800080); // Purple
    delay(1500);

    k10.setScreenBackground(0x000080); // Navy
    delay(1500);
}
