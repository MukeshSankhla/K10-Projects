#include "unihiker_k10.h"
#include <WiFi.h>
#include <esp_now.h>

UNIHIKER_K10 k10;
uint8_t screen_dir = 2; // Portrait (240x320)

// Broadcast MAC Address (FF:FF:FF:FF:FF:FF)
uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

// Friendly message phrase bank
const char* phraseBank[] = {
    "Hello UNIHIKER!",
    "Wireless Ping OK",
    "Sensor Link Active",
    "ESP-NOW Packet",
    "Local Node Online",
    "Radio Sync Check",
    "Device Broadcast",
    "Mesh Message Test"
};
const int totalPhrases = sizeof(phraseBank) / sizeof(phraseBank[0]);

// Message packet structure
typedef struct struct_message {
    char text[32];
    uint32_t counter;
    uint32_t timestamp;
} struct_message;

struct_message txData;
struct_message rxData;

uint32_t txCount = 0;
uint32_t rxCount = 0;
bool rxFlag = false;

// Clean Minimalist Light Theme Palette
const uint32_t COLOR_BG        = 0xF8FAFC; // Soft Slate Off-White
const uint32_t COLOR_CARD      = 0xFFFFFF; // Pure White Card Fill
const uint32_t COLOR_CONSOLE   = 0xF1F5F9; // Soft Paper Box
const uint32_t COLOR_BORDER    = 0xE2E8F0; // Delicate 1px Border
const uint32_t COLOR_BORDER_CON= 0xCBD5E1; // Paper 1px Border
const uint32_t COLOR_TEXT_PRI  = 0x0F172A; // Deep Slate Charcoal
const uint32_t COLOR_TEXT_SEC  = 0x334155; // Slate Secondary
const uint32_t COLOR_TEXT_MUTED= 0x64748B; // Slate Muted Label
const uint32_t COLOR_OLIVE     = 0x65A30D; // Clean Olive Brand Accent
const uint32_t COLOR_BLUE      = 0x2563EB; // Transmission Blue
const uint32_t COLOR_GREEN     = 0x16A34A; // Received Green
const uint32_t COLOR_WHITE     = 0xFFFFFF;

void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
    if (status == ESP_NOW_SEND_SUCCESS) {
        k10.rgb->write(-1, 0x16A34A);
    } else {
        k10.rgb->write(-1, 0xDC2626);
    }
}

void onDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
    if (len == sizeof(struct_message)) {
        memcpy(&rxData, incomingData, sizeof(rxData));
        rxCount++;
        rxFlag = true;
    }
}

void drawStaticChrome() {
    k10.canvas->canvasClear();
    k10.canvas->canvasRectangle(0, 0, 240, 320, COLOR_BG, COLOR_BG, true);

    // App Header Bar (y: 0 to 40)
    k10.canvas->canvasRectangle(0, 0, 240, 40, COLOR_CARD, COLOR_CARD, true);
    k10.canvas->canvasLine(0, 40, 240, 40, COLOR_BORDER);
    k10.canvas->canvasText("ESP-NOW Mesh", 14, 12, COLOR_TEXT_PRI, k10.canvas->eCNAndENFont16, 50, false);
    // Olive brand dot
    k10.canvas->canvasCircle(224, 20, 4, COLOR_OLIVE, COLOR_OLIVE, true);

    // MAC Info Card (y: 48 to 96)
    k10.canvas->canvasRectangle(10, 48, 220, 48, COLOR_BORDER, COLOR_CARD, true);
    k10.canvas->canvasText("DEVICE MAC ADDRESS", 20, 56, COLOR_TEXT_MUTED, k10.canvas->eCNAndENFont16, 50, false);
    String mac = WiFi.macAddress();
    k10.canvas->canvasText(mac, 20, 72, COLOR_TEXT_PRI, k10.canvas->eCNAndENFont16, 50, false);

    // TX Message Card (y: 104 to 184)
    k10.canvas->canvasRectangle(10, 104, 220, 80, COLOR_BORDER, COLOR_CARD, true);
    k10.canvas->canvasText("TRANSMITTED (TX)", 20, 112, COLOR_TEXT_MUTED, k10.canvas->eCNAndENFont16, 50, false);

    // Inner paper box for TX (y: 132 to 176)
    k10.canvas->canvasRectangle(18, 132, 204, 44, COLOR_BORDER_CON, COLOR_CONSOLE, true);

    // RX Message Card (y: 192 to 276)
    k10.canvas->canvasRectangle(10, 192, 220, 84, COLOR_BORDER, COLOR_CARD, true);
    k10.canvas->canvasText("RECEIVED (RX)", 20, 200, COLOR_TEXT_MUTED, k10.canvas->eCNAndENFont16, 50, false);

    // Inner paper box for RX (y: 220 to 268)
    k10.canvas->canvasRectangle(18, 220, 204, 48, COLOR_BORDER_CON, COLOR_CONSOLE, true);

    // Footer Info Bar (y: 286 to 320)
    k10.canvas->canvasRectangle(0, 286, 240, 34, COLOR_CARD, COLOR_CARD, true);
    k10.canvas->canvasLine(0, 286, 240, 286, COLOR_BORDER);
    k10.canvas->canvasText("[A] Send Broadcast Packet", 14, 294, COLOR_OLIVE, k10.canvas->eCNAndENFont16, 50, false);

    k10.canvas->updateCanvas();
}

// Partial refresh for TX Card
void updateTXDisplay(const String& msg, uint32_t count) {
    // Clear TX counter badge
    k10.canvas->canvasRectangle(150, 110, 74, 16, COLOR_CARD, COLOR_CARD, true);
    String cStr = "#" + String(count);
    k10.canvas->canvasText(cStr, 178, 112, COLOR_BLUE, k10.canvas->eCNAndENFont16, 50, false);

    // Clear TX inner paper box
    k10.canvas->canvasRectangle(20, 134, 200, 40, COLOR_CONSOLE, COLOR_CONSOLE, true);
    k10.canvas->canvasText(msg, 24, 144, COLOR_TEXT_SEC, k10.canvas->eCNAndENFont16, 50, false);
    k10.canvas->updateCanvas();
}

// Partial refresh for RX Card
void updateRXDisplay(const String& msg, uint32_t count) {
    // Clear RX counter badge
    k10.canvas->canvasRectangle(150, 198, 74, 16, COLOR_CARD, COLOR_CARD, true);
    String cStr = "#" + String(count);
    k10.canvas->canvasText(cStr, 178, 200, COLOR_GREEN, k10.canvas->eCNAndENFont16, 50, false);

    // Clear RX inner paper box
    k10.canvas->canvasRectangle(20, 222, 200, 44, COLOR_CONSOLE, COLOR_CONSOLE, true);
    k10.canvas->canvasText(msg, 24, 234, COLOR_GREEN, k10.canvas->eCNAndENFont16, 50, false);
    k10.canvas->updateCanvas();
}

void setup() {
    Serial.begin(115200);
    k10.begin();
    k10.initScreen(screen_dir);
    k10.creatCanvas();
    k10.rgb->brightness(5);

    WiFi.mode(WIFI_STA);

    if (esp_now_init() != ESP_OK) {
        Serial.println(F("ESP-NOW init failed"));
        return;
    }

    esp_now_register_send_cb(onDataSent);
    esp_now_register_recv_cb(onDataRecv);

    esp_now_peer_info_t peerInfo;
    memset(&peerInfo, 0, sizeof(peerInfo));
    memcpy(peerInfo.peer_addr, broadcastAddress, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;
    esp_now_add_peer(&peerInfo);

    drawStaticChrome();
    updateTXDisplay("Press Button A to send", 0);
    updateRXDisplay("Waiting for packets...", 0);
}

void loop() {
    if (k10.buttonA->isPressed()) {
        delay(200);
        txCount++;
        int idx = random(0, totalPhrases);
        String phrase = String(phraseBank[idx]) + " [" + String(txCount) + "]";

        memset(&txData, 0, sizeof(txData));
        strncpy(txData.text, phrase.c_str(), sizeof(txData.text) - 1);
        txData.counter = txCount;
        txData.timestamp = millis();

        esp_now_send(broadcastAddress, (uint8_t *)&txData, sizeof(txData));
        updateTXDisplay(phrase, txCount);
    }

    if (rxFlag) {
        rxFlag = false;
        String incoming = String(rxData.text);
        updateRXDisplay(incoming, rxCount);
        k10.rgb->write(-1, 0x16A34A);
    }

    delay(20);
}
