#include "unihiker_k10.h"
#include "FS.h"
#include "SD.h"

UNIHIKER_K10 k10;

// Helper to get descriptive SD card format type
String getCardType() {
    uint8_t type = SD.cardType();
    switch (type) {
        case CARD_MMC:  return "MMC";
        case CARD_SD:   return "SDSC";
        case CARD_SDHC: return "SDHC";
        default:        return "Unknown";
    }
}

// Function to scan and display SD card status & file listings using small font
void checkSDCard() {
    // Clear canvas and reset background
    k10.canvas->canvasClear();
    k10.setScreenBackground(0x000000);

    // Header Title (24px bold)
    k10.canvas->canvasText("SD CARD TEST", 45, 14, 0xFEE715,
                           k10.canvas->eCNAndENFont24, 20, false);
    // Decorative separator rule
    k10.canvas->canvasLine(15, 42, 225, 42, 0x334155);

    // Initialize/detect SD card
    bool isConnected = SD.begin();

    if (!isConnected || SD.cardType() == CARD_NONE) {
        // SD card not detected or cannot be initialized (small 16px font)
        k10.canvas->canvasText("Status: Not Connected", 15, 55, 0xFF5555,
                               k10.canvas->eCNAndENFont16, 26, false);
        k10.canvas->canvasText("No SD card found", 15, 80, 0xFF5555,
                               k10.canvas->eCNAndENFont16, 26, false);
        k10.canvas->canvasText("Please insert a FAT32", 15, 120, 0x94A3B8,
                               k10.canvas->eCNAndENFont16, 26, false);
        k10.canvas->canvasText("MicroSD card and", 15, 140, 0x94A3B8,
                               k10.canvas->eCNAndENFont16, 26, false);
        k10.canvas->canvasText("press Button A to scan.", 15, 160, 0x00E5FF,
                               k10.canvas->eCNAndENFont16, 26, false);
    } else {
        // SD card detected successfully!
        String cardType = getCardType();
        File root = SD.open("/");

        if (!root || !root.isDirectory()) {
            k10.canvas->canvasText("Status: Read Error", 15, 55, 0xFF5555,
                                   k10.canvas->eCNAndENFont16, 26, false);
            k10.canvas->canvasText("Cannot open root dir", 15, 80, 0xFF5555,
                                   k10.canvas->eCNAndENFont16, 26, false);
        } else {
            int fileCount = 0;
            int yPos = 95;
            const int maxDisplayFiles = 10;
            File file = root.openNextFile();

            while (file) {
                fileCount++;
                if (fileCount <= maxDisplayFiles) {
                    String fileName = String(file.name());
                    if (fileName.startsWith("/")) {
                        fileName = fileName.substring(1);
                    }
                    if (file.isDirectory()) {
                        fileName = "[DIR] " + fileName;
                    }
                    // Display each file in small 16px font
                    k10.canvas->canvasText(fileName, 15, yPos, 0x00E5FF,
                                           k10.canvas->eCNAndENFont16, 26, false);
                    yPos += 19;
                }
                file = root.openNextFile();
            }

            if (fileCount == 0) {
                // Connected, but empty filesystem (small 16px font)
                k10.canvas->canvasText("Status: Connected (" + cardType + ")", 15, 55, 0x00FF00,
                                       k10.canvas->eCNAndENFont16, 26, false);
                k10.canvas->canvasText("SD card connected", 15, 85, 0x00FF00,
                                       k10.canvas->eCNAndENFont16, 26, false);
                k10.canvas->canvasText("but no files", 15, 105, 0xFEE715,
                                       k10.canvas->eCNAndENFont16, 26, false);
                k10.canvas->canvasText("Add files to card &", 15, 140, 0x94A3B8,
                                       k10.canvas->eCNAndENFont16, 26, false);
                k10.canvas->canvasText("press Button A to re-scan.", 15, 160, 0x00E5FF,
                                       k10.canvas->eCNAndENFont16, 26, false);
            } else {
                // Connected and files found (small 16px font)
                k10.canvas->canvasText("Status: Connected (" + cardType + ")", 15, 52, 0x00FF00,
                                       k10.canvas->eCNAndENFont16, 26, false);
                k10.canvas->canvasText("Files found (" + String(fileCount) + "):", 15, 72, 0xFEE715,
                                       k10.canvas->eCNAndENFont16, 26, false);
                if (fileCount > maxDisplayFiles) {
                    int remaining = fileCount - maxDisplayFiles;
                    k10.canvas->canvasText("...and " + String(remaining) + " more", 15, yPos, 0x94A3B8,
                                           k10.canvas->eCNAndENFont16, 26, false);
                }
            }
        }
    }

    // Bottom helper prompt in compact 16px font
    k10.canvas->canvasText("Press Button A: Re-scan", 15, 300, 0x64748B,
                           k10.canvas->eCNAndENFont16, 26, false);

    // Push canvas buffer to physical screen
    k10.canvas->updateCanvas();
}

void setup() {
    k10.begin();
    k10.initScreen(2);                 // Portrait orientation (240x320)
    k10.creatCanvas();                 // Create 2D canvas buffer
    k10.setScreenBackground(0x000000);   // Deep black background

    // Initial check on boot
    checkSDCard();
}

void loop() {
    // Press Button A anytime to re-scan the SD card
    if (k10.buttonA->isPressed()) {
        k10.canvas->canvasText("Scanning SD card...", 15, 300, 0xFFFF00,
                               k10.canvas->eCNAndENFont16, 26, false);
        k10.canvas->updateCanvas();
        delay(300);

        checkSDCard();

        // Wait for button release
        while (k10.buttonA->isPressed()) {
            delay(50);
        }
    }

    delay(50);
}
