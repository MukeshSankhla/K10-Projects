#include "unihiker_k10.h"

UNIHIKER_K10 k10;
uint8_t screen_dir = 2; // Portrait orientation (240x320)

bool isViewingPhoto = false;

// Non-blocking button edge-detection trackers
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

// Draw Leica Pro Viewfinder HUD
void drawCameraHUD(const char* statusMessage, uint32_t statusColor) {
    k10.canvas->canvasClear();

    // 1. Top Viewfinder Header Bar
    k10.canvas->canvasRectangle(0, 0, 240, 42, 0x0A0A0A, 0x0A0A0A, true);
    k10.canvas->canvasCircle(24, 21, 6, 0xEA2B2B, 0xEA2B2B, true); // Leica Red Dot
    k10.canvas->canvasText("PRO CAMERA", 52, 10, 0xFAFAFA,
                           k10.canvas->eCNAndENFont24, 15, false);

    // 2. Status Banner
    k10.canvas->canvasRectangle(12, 48, 216, 26, 0x18181B, 0x18181B, true);
    int statusX = 120 - (int)(strlen(statusMessage) * 4);
    k10.canvas->canvasText(statusMessage, statusX, 53, statusColor,
                           k10.canvas->eCNAndENFont16, 24, false);

    // 3. Viewfinder Focus Reticle & Corner Framing Brackets
    k10.canvas->canvasLine(110, 160, 130, 160, 0x00E5FF);
    k10.canvas->canvasLine(120, 150, 120, 170, 0x00E5FF);
    // Framing corners
    k10.canvas->canvasLine(30, 90, 45, 90, 0x71717A);
    k10.canvas->canvasLine(30, 90, 30, 105, 0x71717A);
    k10.canvas->canvasLine(210, 90, 195, 90, 0x71717A);
    k10.canvas->canvasLine(210, 90, 210, 105, 0x71717A);
    k10.canvas->canvasLine(30, 230, 45, 230, 0x71717A);
    k10.canvas->canvasLine(30, 230, 30, 215, 0x71717A);
    k10.canvas->canvasLine(210, 230, 195, 230, 0x71717A);
    k10.canvas->canvasLine(210, 230, 210, 215, 0x71717A);

    // 4. Centered Two-Column Control Bar (Zero-overflow)
    k10.canvas->canvasRectangle(0, 272, 240, 48, 0x0A0A0A, 0x0A0A0A, true);
    k10.canvas->canvasLine(0, 272, 240, 272, 0x27272A);
    k10.canvas->canvasText("[A] Shutter", 22, 286, 0x00E5FF,
                           k10.canvas->eCNAndENFont16, 12, false);
    k10.canvas->canvasText("[B] Gallery", 132, 286, 0x38BDF8,
                           k10.canvas->eCNAndENFont16, 12, false);

    k10.canvas->updateCanvas();
}

// Show the saved photo on canvas
void showSavedPhoto() {
    k10.setBgCamerImage(false);
    k10.canvas->canvasClear();

    // Draw the BMP image from SD Card
    k10.canvas->canvasDrawImage(0, 0, "S:/photo.bmp");

    // Top Header Badge
    k10.canvas->canvasRectangle(0, 0, 240, 34, 0x000000, 0x000000, true);
    k10.canvas->canvasText("Saved: S:/photo.bmp", 32, 10, 0xFEE715,
                           k10.canvas->eCNAndENFont16, 24, false);

    // Bottom Control Badge
    k10.canvas->canvasRectangle(0, 280, 240, 40, 0x000000, 0x000000, true);
    k10.canvas->canvasText("[A] Return to Camera", 38, 292, 0x00FF88,
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

    drawCameraHUD("Lens: Live Viewfinder", 0x10B981);
}

void loop() {
    // Non-blocking Button A Action (Capture or Return)
    if (checkButtonAPressed()) {
        if (isViewingPhoto) {
            isViewingPhoto = false;
            k10.setBgCamerImage(true);
            drawCameraHUD("Lens: Live Viewfinder", 0x10B981);
        } else {
            // Shutter Flash & Snapshot
            k10.rgb->write(-1, 0xFFFFFF); // Flash on
            drawCameraHUD("Capturing frame...", 0xFF4444);

            k10.photoSaveToTFCard("S:/photo.bmp");

            delay(150);
            k10.rgb->write(-1, 0x000000); // Flash off
            drawCameraHUD("Saved: S:/photo.bmp", 0x00E5FF);
        }
    }

    // Non-blocking Button B Action (Gallery)
    if (checkButtonBPressed()) {
        if (!isViewingPhoto) {
            isViewingPhoto = true;
            showSavedPhoto();
        }
    }

    delay(20); // Responsive loop tick
}
