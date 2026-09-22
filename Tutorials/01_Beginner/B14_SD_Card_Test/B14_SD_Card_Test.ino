#include "unihiker_k10.h"
#include "FS.h"
#include "SD.h"

UNIHIKER_K10 k10;

#define COLOR_BG        0xF8FAFC
#define COLOR_CARD      0xFFFFFF
#define COLOR_BORDER    0xE2E8F0
#define COLOR_TEXT_PRI  0x0F172A
#define COLOR_TEXT_MUTED 0x64748B
#define COLOR_EMERALD   0x059669
#define COLOR_EMERALD_BG 0xDCFCE7
#define COLOR_ROSE      0xE11D48
#define COLOR_ROSE_BG   0xFFE4E6

String getCardType() {
    uint8_t type = SD.cardType();
    switch (type) {
        case CARD_MMC:  return "MMC";
        case CARD_SD:   return "SDSC";
        case CARD_SDHC: return "SDHC";
        default:        return "Unknown";
    }
}

void drawStaticChrome() {
    k10.canvas->canvasClear();
    k10.setScreenBackground(COLOR_BG);

    k10.canvas->canvasSetLineWidth(1);

    // Header Card
    k10.canvas->canvasRectangle(16, 16, 208, 44, COLOR_BORDER, COLOR_CARD, true);
    k10.canvas->canvasCircle(36, 38, 5, COLOR_EMERALD, COLOR_EMERALD, true);
    k10.canvas->canvasText("SD Card Storage", 52, 30, COLOR_TEXT_PRI,
                           k10.canvas->eCNAndENFont16, 20, false);

    // Status Card Shell
    k10.canvas->canvasRectangle(16, 68, 208, 54, COLOR_BORDER, COLOR_CARD, true);
    k10.canvas->canvasText("STORAGE STATUS", 28, 78, COLOR_TEXT_MUTED,
                           k10.canvas->eCNAndENFont16, 20, false);

    // Files Card Shell
    k10.canvas->canvasRectangle(16, 130, 208, 126, COLOR_BORDER, COLOR_CARD, true);
    k10.canvas->canvasText("ROOT DIRECTORY", 28, 140, COLOR_TEXT_MUTED,
                           k10.canvas->eCNAndENFont16, 20, false);

    // Footer Card
    k10.canvas->canvasRectangle(16, 264, 208, 40, COLOR_BORDER, COLOR_CARD, true);
    k10.canvas->canvasText("[A] Re-scan Storage", 44, 276, COLOR_EMERALD,
                           k10.canvas->eCNAndENFont16, 20, false);

    k10.canvas->updateCanvas();
}

void checkSDCard() {
    // Dynamic refresh of status area (y: 94..118)
    k10.canvas->canvasRectangle(26, 94, 188, 24, COLOR_CARD, COLOR_CARD, true);

    // Dynamic refresh of file area (y: 156..248)
    k10.canvas->canvasRectangle(26, 156, 188, 94, COLOR_CARD, COLOR_CARD, true);

    if (!SD.begin() || SD.cardType() == CARD_NONE) {
        k10.canvas->canvasRectangle(28, 96, 100, 20, COLOR_BORDER, COLOR_ROSE_BG, true);
        k10.canvas->canvasText("No Card", 38, 98, COLOR_ROSE, k10.canvas->eCNAndENFont16, 15, false);

        k10.canvas->canvasText("Insert FAT32 MicroSD", 28, 166, COLOR_TEXT_PRI, k10.canvas->eCNAndENFont16, 20, false);
        k10.canvas->canvasText("Press [A] to check", 28, 192, COLOR_TEXT_MUTED, k10.canvas->eCNAndENFont16, 20, false);
    } else {
        k10.canvas->canvasRectangle(28, 96, 96, 20, COLOR_BORDER, COLOR_EMERALD_BG, true);
        k10.canvas->canvasText("Connected", 34, 98, COLOR_EMERALD, k10.canvas->eCNAndENFont16, 15, false);

        String typeStr = "Type: " + getCardType();
        k10.canvas->canvasText(typeStr.c_str(), 132, 98, COLOR_TEXT_MUTED, k10.canvas->eCNAndENFont16, 10, false);

        File root = SD.open("/");
        if (!root || !root.isDirectory()) {
            k10.canvas->canvasText("Read error", 28, 166, COLOR_ROSE, k10.canvas->eCNAndENFont16, 15, false);
        } else {
            int yPos = 160;
            int count = 0;
            File file = root.openNextFile();
            while (file && count < 4) {
                String name = String(file.name());
                if (file.isDirectory()) name = "/" + name;
                k10.canvas->canvasText(name.c_str(), 28, yPos, COLOR_TEXT_PRI, k10.canvas->eCNAndENFont16, 20, false);
                yPos += 20;
                count++;
                file = root.openNextFile();
            }
            if (count == 0) {
                k10.canvas->canvasText("(No files found)", 28, 166, COLOR_TEXT_MUTED, k10.canvas->eCNAndENFont16, 20, false);
            }
        }
    }
    k10.canvas->updateCanvas();
}

void setup() {
    k10.begin();
    k10.initScreen(2); // Portrait orientation (240x320)
    k10.creatCanvas();

    drawStaticChrome();
    checkSDCard();
}

void loop() {
    if (k10.buttonA->isPressed()) {
        checkSDCard();
        delay(300); // Simple debounce
    }
    delay(50);
}
