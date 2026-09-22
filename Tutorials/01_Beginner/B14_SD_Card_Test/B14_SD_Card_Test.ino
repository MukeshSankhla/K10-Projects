#include "unihiker_k10.h"
#include "FS.h"
#include "SD.h"

UNIHIKER_K10 k10;
uint8_t screen_dir = 2; // Portrait orientation (240x320)

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

// Non-blocking button edge-detection tracker
bool checkButtonAPressed() {
    static bool lastState = false;
    static unsigned long lastDebounceTime = 0;
    bool reading = k10.buttonA->isPressed();
    bool pressedEvent = false;

    if (reading != lastState) {
        lastDebounceTime = millis();
    }
    if ((millis() - lastDebounceTime) > 35) {
        static bool stableState = false;
        if (reading != stableState) {
            stableState = reading;
            if (stableState) {
                pressedEvent = true;
            }
        }
    }
    lastState = reading;
    return pressedEvent;
}

// Function to scan and display SD card status & file listings
void checkSDCard() {
    k10.canvas->canvasClear();
    // Retro Matrix Carbon background
    k10.setScreenBackground(0x0D1117);

    // 1. Header Banner
    k10.canvas->canvasRectangle(0, 0, 240, 42, 0x161B22, 0x161B22, true);
    k10.canvas->canvasLine(0, 42, 240, 42, 0x00FF87);
    k10.canvas->canvasText("SD STORAGE", 52, 10, 0x00FF87,
                           k10.canvas->eCNAndENFont24, 15, false);

    // 2. Detect SD card
    bool isConnected = SD.begin();

    if (!isConnected || SD.cardType() == CARD_NONE) {
        // Not connected card
        k10.canvas->canvasRectangle(14, 54, 212, 130, 0xEF4444, 0x1C1317, true);
        k10.canvas->canvasText("Status: Disconnected", 26, 68, 0xEF4444,
                               k10.canvas->eCNAndENFont16, 22, false);
        k10.canvas->canvasText("No FAT32 MicroSD detected", 26, 94, 0x94A3B8,
                               k10.canvas->eCNAndENFont16, 24, false);
        k10.canvas->canvasText("Insert card into K10 slot", 26, 120, 0x94A3B8,
                               k10.canvas->eCNAndENFont16, 24, false);
        k10.canvas->canvasText("Press [A] to scan again", 26, 148, 0x60EFFF,
                               k10.canvas->eCNAndENFont16, 22, false);
    } else {
        String cardType = getCardType();
        File root = SD.open("/");

        if (!root || !root.isDirectory()) {
            k10.canvas->canvasRectangle(14, 54, 212, 90, 0xEF4444, 0x1C1317, true);
            k10.canvas->canvasText("Status: Read Error", 26, 68, 0xEF4444,
                                   k10.canvas->eCNAndENFont16, 22, false);
            k10.canvas->canvasText("Cannot open root dir", 26, 94, 0x94A3B8,
                                   k10.canvas->eCNAndENFont16, 22, false);
        } else {
            int fileCount = 0;
            int yPos = 88;
            const int maxDisplayFiles = 8;
            File file = root.openNextFile();

            // Status header pill
            k10.canvas->canvasRectangle(14, 50, 212, 30, 0x30363D, 0x161B22, true);
            String statStr = "Online (" + cardType + ")";
            k10.canvas->canvasText(statStr, 24, 56, 0x00FF87,
                                   k10.canvas->eCNAndENFont16, 20, false);

            while (file) {
                fileCount++;
                if (fileCount <= maxDisplayFiles) {
                    String fileName = String(file.name());
                    if (fileName.startsWith("/")) {
                        fileName = fileName.substring(1);
                    }
                    if (file.isDirectory()) {
                        fileName = "/" + fileName;
                    }
                    // Truncate long file names safely to 22 characters
                    if (fileName.length() > 22) {
                        fileName = fileName.substring(0, 19) + "...";
                    }
                    k10.canvas->canvasText(fileName, 24, yPos, 0x60EFFF,
                                           k10.canvas->eCNAndENFont16, 24, false);
                    yPos += 20;
                }
                file = root.openNextFile();
            }

            if (fileCount == 0) {
                k10.canvas->canvasText("Card is empty (No files)", 24, 100, 0xFEE715,
                                       k10.canvas->eCNAndENFont16, 24, false);
            }
        }
    }

    // 3. Centered Navigation Footer Bar (Zero-overflow)
    k10.canvas->canvasLine(15, 276, 225, 276, 0x30363D);
    k10.canvas->canvasRectangle(20, 280, 200, 32, 0x30363D, 0x161B22, true);
    k10.canvas->canvasText("[A] Re-Scan Storage", 42, 288, 0x00FF87,
                           k10.canvas->eCNAndENFont16, 20, false);

    k10.canvas->updateCanvas();
}

void setup() {
    k10.begin();
    k10.initScreen(screen_dir);
    k10.creatCanvas();

    k10.rgb->brightness(5);
    k10.rgb->write(-1, 0x00FF87); // Matrix Mint glow

    checkSDCard();
}

void loop() {
    // Non-blocking Button A: Trigger SD Card Re-Scan
    if (checkButtonAPressed()) {
        checkSDCard();
    }

    delay(25); // Responsive loop tick
}
