#include "unihiker_k10.h"

UNIHIKER_K10 k10;
uint8_t screen_dir = 2; // Portrait orientation (240x320)
Music music;

// Helper function to render a clean, high-contrast UI dashboard
void drawUI(String statusText, uint32_t statusColor) {
    // Clear canvas
    k10.canvas->canvasClear();
    k10.setScreenBackground(0x0F172A); // Deep slate dark background

    // 1. Header Title (24px bold)
    k10.canvas->canvasText("VOICE RECORDER", 32, 18, 0xFEE715,
                           k10.canvas->eCNAndENFont24, 20, false);

    // Decorative divider line
    k10.canvas->canvasLine(15, 48, 225, 48, 0x334155);

    // 2. Target File Information Card
    k10.canvas->canvasRectangle(15, 60, 210, 68, 0x1E293B, 0x1E293B, true);
    k10.canvas->canvasText("Target: S:/sound.wav", 25, 72, 0x94A3B8,
                           k10.canvas->eCNAndENFont16, 26, false);
    k10.canvas->canvasText("Duration: 3 Seconds", 25, 96, 0x94A3B8,
                           k10.canvas->eCNAndENFont16, 26, false);

    // 3. Dynamic Status Panel
    k10.canvas->canvasRectangle(15, 142, 210, 60, statusColor, 0x111827, true);
    k10.canvas->canvasText(statusText, 25, 162, statusColor,
                           k10.canvas->eCNAndENFont16, 26, false);

    // 4. Button Control Guide (Bottom panel)
    k10.canvas->canvasLine(15, 220, 225, 220, 0x334155);
    k10.canvas->canvasText("Button [A]: Record (3s)", 20, 235, 0x00E5FF,
                           k10.canvas->eCNAndENFont16, 26, false);
    k10.canvas->canvasText("Button [B]: Play Audio", 20, 262, 0x38BDF8,
                           k10.canvas->eCNAndENFont16, 26, false);
    k10.canvas->canvasText("Requires FAT32 MicroSD card", 20, 292, 0x64748B,
                           k10.canvas->eCNAndENFont16, 26, false);

    // Push canvas buffer to physical screen
    k10.canvas->updateCanvas();
}

void setup() {
    k10.begin();
    k10.initScreen(screen_dir);
    k10.creatCanvas();
    k10.initSDFile(); // Mount SD/TF Card

    // Initialize RGB LED to OFF
    k10.rgb->brightness(5);
    k10.rgb->write(-1, 0x000000);

    // Initial Standby Screen
    drawUI("Status: READY", 0x00FF88);
}

void loop() {
    // ==========================================
    // Button A: Record Audio for 3 Seconds
    // ==========================================
    if (k10.buttonA->isPressed()) {
        // Red indicator LED & Recording status
        k10.rgb->write(-1, 0xFF0000);
        drawUI("Status: RECORDING (3s)...", 0xFF4444);

        // Record audio from microphone to TF card
        music.recordSaveToTFCard("S:/sound.wav", 3);

        // Turn off LED & show completion status
        k10.rgb->write(-1, 0x000000);
        drawUI("Status: RECORD SAVED!", 0x00E5FF);

        // Debounce wait for button release
        while (k10.buttonA->isPressed()) {
            delay(50);
        }
    }
    // ==========================================
    // Button B: Playback Recorded Audio
    // ==========================================
    else if (k10.buttonB->isPressed()) {
        // Green indicator LED & Playing status
        k10.rgb->write(-1, 0x00FF00);
        drawUI("Status: PLAYING AUDIO...", 0xFEE715);

        // Play WAV file through onboard speaker
        music.playTFCardAudio("S:/sound.wav");
        delay(3000); // Wait for playback duration

        // Turn off LED & show completion status
        k10.rgb->write(-1, 0x000000);
        drawUI("Status: PLAYBACK DONE", 0x00FF88);

        // Debounce wait for button release
        while (k10.buttonB->isPressed()) {
            delay(50);
        }
    }

    delay(20);
}
