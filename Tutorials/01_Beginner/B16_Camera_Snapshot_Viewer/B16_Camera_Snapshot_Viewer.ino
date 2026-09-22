#include "unihiker_k10.h"

UNIHIKER_K10 k10;
uint8_t screen_dir = 2; // Portrait orientation (240x320)

bool isViewingPhoto = false;

#define COLOR_BG        0xF8FAFC
#define COLOR_CARD      0xFFFFFF
#define COLOR_BORDER    0xE2E8F0
#define COLOR_TEXT_PRI  0x0F172A
#define COLOR_TEXT_MUTED 0x64748B
#define COLOR_PRIMARY   0x2563EB // Sapphire Blue
#define COLOR_SUCCESS   0x059669 // Emerald Green
#define COLOR_RETICLE   0x60A5FA // Soft Sky Blue

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

bool checkButtonBPressed() {
    static bool lastState = false;
    static unsigned long lastDebounceTime = 0;
    bool reading = k10.buttonB->isPressed();
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

void drawCameraHUD(const char* statusMessage, uint32_t statusColor) {
    k10.canvas->canvasClear();
    k10.canvas->canvasSetLineWidth(1);

    // 1. Top Viewfinder Header Card
    k10.canvas->canvasRectangle(12, 10, 216, 38, COLOR_BORDER, COLOR_CARD, true);
    k10.canvas->canvasCircle(28, 29, 4, COLOR_PRIMARY, COLOR_PRIMARY, true);
    k10.canvas->canvasText("Camera Viewfinder", 44, 21, COLOR_TEXT_PRI,
                           k10.canvas->eCNAndENFont16, 20, false);

    // 2. Status Banner
    k10.canvas->canvasRectangle(12, 54, 216, 26, COLOR_BORDER, COLOR_CARD, true);
    int statusX = 120 - (int)(strlen(statusMessage) * 4);
    k10.canvas->canvasText(statusMessage, statusX, 58, statusColor,
                           k10.canvas->eCNAndENFont16, 24, false);

    // 3. Delicate 1px Reticle & Framing Corners
    k10.canvas->canvasLine(112, 160, 128, 160, COLOR_RETICLE);
    k10.canvas->canvasLine(120, 152, 120, 168, COLOR_RETICLE);

    // Subtle corner markers
    k10.canvas->canvasLine(30, 90, 45, 90, COLOR_BORDER);
    k10.canvas->canvasLine(30, 90, 30, 105, COLOR_BORDER);
    k10.canvas->canvasLine(210, 90, 195, 90, COLOR_BORDER);
    k10.canvas->canvasLine(210, 90, 210, 105, COLOR_BORDER);
    k10.canvas->canvasLine(30, 230, 45, 230, COLOR_BORDER);
    k10.canvas->canvasLine(30, 230, 30, 215, COLOR_BORDER);
    k10.canvas->canvasLine(210, 230, 195, 230, COLOR_BORDER);
    k10.canvas->canvasLine(210, 230, 210, 215, COLOR_BORDER);

    // 4. Centered Control Footer
    k10.canvas->canvasRectangle(12, 270, 216, 40, COLOR_BORDER, COLOR_CARD, true);
    k10.canvas->canvasText("[A] Shutter", 28, 281, COLOR_PRIMARY,
                           k10.canvas->eCNAndENFont16, 12, false);
    k10.canvas->canvasLine(120, 276, 120, 304, COLOR_BORDER);
    k10.canvas->canvasText("[B] Gallery", 136, 281, COLOR_TEXT_PRI,
                           k10.canvas->eCNAndENFont16, 12, false);

    k10.canvas->updateCanvas();
}

void showSavedPhoto() {
    k10.setBgCamerImage(false);
    k10.canvas->canvasClear();

    // Draw the BMP image from SD Card
    k10.canvas->canvasDrawImage(0, 0, "S:/photo.bmp");

    // Header Card
    k10.canvas->canvasSetLineWidth(1);
    k10.canvas->canvasRectangle(12, 10, 216, 36, COLOR_BORDER, COLOR_CARD, true);
    k10.canvas->canvasText("Saved: S:/photo.bmp", 34, 18, COLOR_TEXT_PRI,
                           k10.canvas->eCNAndENFont16, 24, false);

    // Footer Return Card
    k10.canvas->canvasRectangle(12, 272, 216, 38, COLOR_BORDER, COLOR_CARD, true);
    k10.canvas->canvasText("[A] Return to Camera", 42, 282, COLOR_SUCCESS,
                           k10.canvas->eCNAndENFont16, 22, false);

    k10.canvas->updateCanvas();
}

void setup() {
    k10.begin();
    k10.initScreen(screen_dir);
    k10.initSDFile();       // Mount SD Card
    k10.initBgCamerImage(); // Initialize camera sensor
    k10.creatCanvas();      // Create overlay canvas buffer

    k10.rgb->brightness(5);
    k10.rgb->write(-1, 0x000000);

    // Start live camera stream
    k10.setBgCamerImage(true);

    drawCameraHUD("Live Preview", COLOR_SUCCESS);
}

void loop() {
    if (checkButtonAPressed()) {
        if (isViewingPhoto) {
            isViewingPhoto = false;
            k10.setBgCamerImage(true);
            drawCameraHUD("Live Preview", COLOR_SUCCESS);
        } else {
            k10.rgb->write(-1, 0xFFFFFF); // Flash on
            drawCameraHUD("Saving frame...", 0xE11D48);

            k10.photoSaveToTFCard("S:/photo.bmp");

            delay(150);
            k10.rgb->write(-1, 0x000000); // Flash off
            drawCameraHUD("Captured: photo.bmp", COLOR_PRIMARY);
        }
    }

    if (checkButtonBPressed()) {
        if (!isViewingPhoto) {
            isViewingPhoto = true;
            showSavedPhoto();
        }
    }

    delay(20);
}
