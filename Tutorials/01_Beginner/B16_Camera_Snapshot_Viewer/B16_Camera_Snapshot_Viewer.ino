#include "unihiker_k10.h"

UNIHIKER_K10 k10;
uint8_t screen_dir = 2; // Portrait orientation (240x320)

bool isViewingPhoto = false;

// Draw Camera Viewfinder HUD (Top & Bottom bars for maximum text readability)
void drawCameraHUD(String statusMessage, uint32_t statusColor) {
    k10.canvas->canvasClear();

    // 1. Top HUD Header Bar
    k10.canvas->canvasRectangle(0, 0, 240, 48, 0x000000, 0x000000, true);
    k10.canvas->canvasText("K10 CAMERA", 52, 12, 0xFEE715,
                           k10.canvas->eCNAndENFont24, 20, false);

    // 2. Dynamic Status Banner (Under header)
    k10.canvas->canvasRectangle(10, 52, 220, 24, 0x1E293B, 0x1E293B, true);
    k10.canvas->canvasText(statusMessage, 18, 56, statusColor,
                           k10.canvas->eCNAndENFont16, 26, false);

    // 3. Viewfinder Focus Reticle (Center crosshair marks)
    k10.canvas->canvasLine(110, 160, 130, 160, 0x00E5FF);
    k10.canvas->canvasLine(120, 150, 120, 170, 0x00E5FF);

    // 4. Bottom Controls Bar
    k10.canvas->canvasRectangle(0, 258, 240, 62, 0x000000, 0x000000, true);
    k10.canvas->canvasText("[A] Capture Photo", 20, 268, 0x00E5FF,
                           k10.canvas->eCNAndENFont16, 26, false);
    k10.canvas->canvasText("[B] View Saved Photo", 20, 292, 0x38BDF8,
                           k10.canvas->eCNAndENFont16, 26, false);

    k10.canvas->updateCanvas();
}

// Show the saved photo on canvas with an overlay prompt to return
void showSavedPhoto() {
    // Temporarily disable live camera feed so photo is visible
    k10.setBgCamerImage(false);
    k10.canvas->canvasClear();

    // Draw the BMP image from SD Card onto the canvas
    k10.canvas->canvasDrawImage(0, 0, "S:/photo.bmp");

    // Top & Bottom info badges over the image
    k10.canvas->canvasRectangle(0, 0, 240, 32, 0x000000, 0x000000, true);
    k10.canvas->canvasText("PHOTO: S:/photo.bmp", 20, 8, 0xFEE715,
                           k10.canvas->eCNAndENFont16, 26, false);

    k10.canvas->canvasRectangle(0, 285, 240, 35, 0x000000, 0x000000, true);
    k10.canvas->canvasText("[A] Return to Camera", 20, 295, 0x00FF88,
                           k10.canvas->eCNAndENFont16, 26, false);

    k10.canvas->updateCanvas();
}

void setup() {
    k10.begin();
    k10.initScreen(screen_dir);
    k10.initSDFile();          // Mount SD Card
    k10.initBgCamerImage();    // Initialize camera sensor
    k10.creatCanvas();         // Create overlay canvas buffer

    // Initialize RGB LED (Flash simulator)
    k10.rgb->brightness(5);
    k10.rgb->write(-1, 0x000000);

    // Start live camera stream in the background
    k10.setBgCamerImage(true);

    // Initial HUD overlay
    drawCameraHUD("Mode: Live Viewfinder", 0x00FF88);
}

void loop() {
    // ==========================================
    // Button A Action:
    // - In Live Mode: Snap photo to SD Card
    // - In Photo View Mode: Return to Live Camera
    // ==========================================
    if (k10.buttonA->isPressed()) {
        if (isViewingPhoto) {
            // Return to live camera feed
            isViewingPhoto = false;
            k10.setBgCamerImage(true);
            drawCameraHUD("Mode: Live Viewfinder", 0x00FF88);
        } else {
            // Camera Shutter Flash & Save status
            k10.rgb->write(-1, 0xFFFFFF); // White shutter flash
            drawCameraHUD("Capturing photo...", 0xFF4444);

            // Save snapshot to SD Card
            k10.photoSaveToTFCard("S:/photo.bmp");

            delay(200);
            k10.rgb->write(-1, 0x000000); // Shutter flash off
            drawCameraHUD("Photo Saved: photo.bmp", 0x00E5FF);
        }

        // Debounce wait for button release
        while (k10.buttonA->isPressed()) {
            delay(50);
        }
    }

    // ==========================================
    // Button B Action:
    // - View the captured photo from SD Card
    // ==========================================
    else if (k10.buttonB->isPressed()) {
        if (!isViewingPhoto) {
            isViewingPhoto = true;
            showSavedPhoto();
        }

        // Debounce wait for button release
        while (k10.buttonB->isPressed()) {
            delay(50);
        }
    }

    delay(20);
}
