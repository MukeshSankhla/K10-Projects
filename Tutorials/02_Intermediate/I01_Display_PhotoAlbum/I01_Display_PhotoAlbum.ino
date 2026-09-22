#include "unihiker_k10.h"

UNIHIKER_K10 k10;
uint8_t screen_dir = 2; // Portrait orientation (240x320)

// Array of photo filenames on the MicroSD card
const char* photoAlbum[] = {
    "S:/photo.bmp",
    "S:/photo1.bmp",
    "S:/photo2.bmp"
};
const int totalPhotos = sizeof(photoAlbum) / sizeof(photoAlbum[0]);
int currentPhotoIndex = 0;

// Render modern photo frame HUD with image title & control legend
void displayAlbumPhoto(int index) {
    k10.canvas->canvasClear();
    k10.setScreenBackground(0x0F172A);

    // 1. Top Header Banner
    k10.canvas->canvasRectangle(0, 0, 240, 42, 0x1E293B, 0x1E293B, true);
    k10.canvas->canvasText("PHOTO ALBUM", 48, 10, 0xFEE715,
                           k10.canvas->eCNAndENFont24, 20, false);

    // 2. Photo Display Area (Centered in 220x200 frame)
    k10.canvas->canvasRectangle(10, 50, 220, 200, 0x334155, 0x000000, true);

    // Draw the BMP image from SD Card
    k10.canvas->canvasDrawImage(10, 50, photoAlbum[index]);

    // 3. Photo Meta & Index Badge
    String indexStr = "Photo " + String(index + 1) + " of " + String(totalPhotos);
    k10.canvas->canvasText(indexStr, 20, 225, 0x00E5FF,
                           k10.canvas->eCNAndENFont16, 26, false);

    // 4. Bottom Controls Bar
    k10.canvas->canvasLine(10, 260, 230, 260, 0x334155);
    k10.canvas->canvasText("[A] Prev Photo", 20, 272, 0x38BDF8,
                           k10.canvas->eCNAndENFont16, 26, false);
    k10.canvas->canvasText("[B] Next Photo", 130, 272, 0x38BDF8,
                           k10.canvas->eCNAndENFont16, 26, false);
    k10.canvas->canvasText(photoAlbum[index], 20, 298, 0x64748B,
                           k10.canvas->eCNAndENFont16, 26, false);

    k10.canvas->updateCanvas();
}

void setup() {
    k10.begin();
    k10.initScreen(screen_dir);
    k10.creatCanvas();
    k10.initSDFile(); // Mount MicroSD card

    k10.rgb->brightness(5);
    k10.rgb->write(-1, 0x00E5FF); // Soft cyan ambient glow

    // Display initial photo
    displayAlbumPhoto(currentPhotoIndex);
}

void loop() {
    // Button A: Previous Photo
    if (k10.buttonA->isPressed()) {
        currentPhotoIndex--;
        if (currentPhotoIndex < 0) {
            currentPhotoIndex = totalPhotos - 1;
        }
        displayAlbumPhoto(currentPhotoIndex);

        while (k10.buttonA->isPressed()) {
            delay(50);
        }
    }
    // Button B: Next Photo
    else if (k10.buttonB->isPressed()) {
        currentPhotoIndex++;
        if (currentPhotoIndex >= totalPhotos) {
            currentPhotoIndex = 0;
        }
        displayAlbumPhoto(currentPhotoIndex);

        while (k10.buttonB->isPressed()) {
            delay(50);
        }
    }

    delay(30);
}
