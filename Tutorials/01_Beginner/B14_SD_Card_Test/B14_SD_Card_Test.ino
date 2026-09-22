#include "unihiker_k10.h"
#include "FS.h"
#include "SD.h"

UNIHIKER_K10 k10;

String getCardType() {
    uint8_t type = SD.cardType();
    switch (type) {
        case CARD_MMC:  return "MMC";
        case CARD_SD:   return "SDSC";
        case CARD_SDHC: return "SDHC";
        default:        return "Unknown";
    }
}

void checkSDCard() {
    k10.canvas->canvasClear();
    k10.canvas->canvasText("SD Card Storage", 1, 0x00F0FF);

    if (!SD.begin() || SD.cardType() == CARD_NONE) {
        k10.canvas->canvasText("Status: No Card", 3, 0xFF4444);
        k10.canvas->canvasText("Insert FAT32 MicroSD", 5, 0xFFFFFF);
        k10.canvas->canvasText("Press [A] to re-scan", 7, 0xFEE715);
    } else {
        k10.canvas->canvasText("Status: Connected!", 3, 0x00FF87);
        k10.canvas->canvasText("Type: " + getCardType(), 4, 0xFFFFFF);

        File root = SD.open("/");
        if (!root || !root.isDirectory()) {
            k10.canvas->canvasText("Error reading files", 6, 0xFF4444);
        } else {
            k10.canvas->canvasText("Files on SD Card:", 6, 0xFFFFFF);
            int row = 7;
            File file = root.openNextFile();
            while (file && row <= 11) {
                String name = String(file.name());
                if (file.isDirectory()) {
                    name = "/" + name;
                }
                k10.canvas->canvasText(name, row, 0x60EFFF);
                row++;
                file = root.openNextFile();
            }
            if (row == 7) {
                k10.canvas->canvasText("(No files found)", 7, 0x888888);
            }
        }
        k10.canvas->canvasText("Press [A] to re-scan", 13, 0x888888);
    }
    k10.canvas->updateCanvas();
}

void setup() {
    k10.begin();
    k10.initScreen(2); // Portrait orientation (240x320)
    k10.creatCanvas();
    k10.setScreenBackground(0x000000);

    checkSDCard();
}

void loop() {
    // When button A is pressed, re-scan
    if (k10.buttonA->isPressed()) {
        checkSDCard();
        delay(300); // Simple debounce
    }
    delay(50);
}
