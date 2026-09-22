## What this tutorial is??

This tutorial demonstrates peer-to-peer wireless packet transmission and reception between UNIHIKER K10 / ESP32 devices using **ESP-NOW**.

Features:
- **Connectionless Mesh / Broadcast Protocol**: Transmits and receives data packets with microsecond latency without connecting to a Wi-Fi router.
- **Random Text Transmission**: Press Button A to broadcast randomized tactical telemetry phrases (`"Hello K10 Mesh!"`, `"Telemetry Ping OK"`, etc.).
- **Military Olive & Tactical Lime UI**: Camouflage dark olive background (`0x081206`), tactical panel (`0x142410`), neon lime (`0x84CC16`), and radio green (`0x4ADE80`).
- **Two-Way Packet Counters**: Tracks transmitted (TX) packets and incoming (RX) packets with sender packet IDs.

---

## How it works

1. **Broadcast Peer Configuration**:
   - The sketch registers peer `FF:FF:FF:FF:FF:FF`, enabling universal broadcast so any nearby listening node receives packets without needing pre-paired MAC addresses.
2. **Callbacks**:
   - `onDataSent`: Evaluates delivery status and flashes RGB green or red.
   - `onDataRecv`: Unpacks incoming `struct_message` payloads into `rxData` and triggers the display update flag.
3. **Dynamic Partial Refresh**:
   - The chrome, MAC banner, and container frames are rendered once during `setup()`.
   - On send/receive events, only the interior message text and numeric counter areas are erased and updated.

---

## API and Functions detailes

| API / Method | Arguments | Return Type | Description |
| :--- | :--- | :--- | :--- |
| `esp_now_init()` | None | `esp_err_t` | Initializes ESP-NOW protocol stack. |
| `esp_now_register_send_cb(cb)` | Callback func | `esp_err_t` | Registers transmit status handler. |
| `esp_now_register_recv_cb(cb)` | Callback func | `esp_err_t` | Registers incoming packet handler. |
| `esp_now_send(peer, buf, len)` | Peer MAC, data, size | `esp_err_t` | Transmits raw payload over radio. |
| `k10.buttonA->isPressed()` | None | `bool` | Button to generate and send random message. |

---

## Full Code

```cpp
#include "unihiker_k10.h"
#include <WiFi.h>
#include <esp_now.h>

UNIHIKER_K10 k10;
uint8_t screen_dir = 2; // Portrait (240x320)

// Broadcast MAC Address (FF:FF:FF:FF:FF:FF)
uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

// Random phrase bank for transmission
const char* phraseBank[] = {
    "Hello K10 Mesh!",
    "Telemetry Ping OK",
    "Sensor Link Active",
    "ESP-NOW Packet #",
    "Field Node Online",
    "Radio Beacon Sync",
    "Tactical Alert 01",
    "K10 Data Broadcast"
};
const int totalPhrases = sizeof(phraseBank) / sizeof(phraseBank[0]);

// Transmission packet structure
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

// Tactical Military Olive & Lime Theme
const uint32_t COLOR_BG        = 0x081206; // Deep Camo Dark Olive
const uint32_t COLOR_PANEL     = 0x142410; // Tactical Panel
const uint32_t COLOR_BORDER    = 0x2A4420; // Khaki Olive Border
const uint32_t COLOR_LIME      = 0x84CC16; // Neon Tactical Lime
const uint32_t COLOR_GREEN     = 0x4ADE80; // Bright Radio Green
const uint32_t COLOR_YELLOW    = 0xFACC15;
const uint32_t COLOR_WHITE     = 0xFFFFFF;
const uint32_t COLOR_MUTED     = 0x65A30D;

void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
    if (status == ESP_NOW_SEND_SUCCESS) {
        k10.rgb->write(-1, 0x00FF44);
    } else {
        k10.rgb->write(-1, 0xFF2200);
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

    // Tactical Header
    k10.canvas->canvasRectangle(0, 0, 240, 36, COLOR_PANEL, COLOR_PANEL, true);
    k10.canvas->canvasLine(0, 36, 240, 36, COLOR_LIME);
    k10.canvas->canvasText("ESP-NOW TACTICAL MESH", 10, 10, COLOR_LIME, k10.canvas->eCNAndENFont16, 50, false);

    // Mesh Channel & MAC Shell (y: 44 to 88)
    k10.canvas->canvasRectangle(8, 44, 224, 44, COLOR_BORDER, COLOR_PANEL, true);
    k10.canvas->canvasText("RADIO: 2.4GHz BROADCAST", 16, 52, COLOR_MUTED, k10.canvas->eCNAndENFont16, 50, false);
    String mac = "MY MAC: " + WiFi.macAddress();
    k10.canvas->canvasText(mac, 16, 68, COLOR_WHITE, k10.canvas->eCNAndENFont16, 50, false);

    // Transmit (TX) Pod (y: 96 to 176)
    k10.canvas->canvasRectangle(8, 96, 224, 80, COLOR_BORDER, COLOR_PANEL, true);
    k10.canvas->canvasText("TX OUTGOING TELEGRAM", 16, 104, COLOR_LIME, k10.canvas->eCNAndENFont16, 50, false);
    k10.canvas->canvasLine(16, 122, 224, 122, COLOR_BORDER);

    // Receive (RX) Pod (y: 184 to 264)
    k10.canvas->canvasRectangle(8, 184, 224, 80, COLOR_BORDER, COLOR_PANEL, true);
    k10.canvas->canvasText("RX INCOMING TELEGRAM", 16, 192, COLOR_GREEN, k10.canvas->eCNAndENFont16, 50, false);
    k10.canvas->canvasLine(16, 210, 224, 210, COLOR_BORDER);

    // Footer Control Prompt (y: 274 to 320)
    k10.canvas->canvasRectangle(0, 274, 240, 46, COLOR_PANEL, COLOR_PANEL, true);
    k10.canvas->canvasLine(0, 274, 240, 274, COLOR_BORDER);
    k10.canvas->canvasText("BTN A: Send Random Message", 16, 282, COLOR_LIME, k10.canvas->eCNAndENFont16, 50, false);
    k10.canvas->canvasText("Peer: Broadcast FF:FF:FF...", 16, 300, COLOR_MUTED, k10.canvas->eCNAndENFont16, 50, false);

    k10.canvas->updateCanvas();
}

// Partial refresh for TX card
void updateTxDisplay(String msg, uint32_t count) {
    k10.canvas->canvasRectangle(14, 126, 212, 46, COLOR_PANEL, COLOR_PANEL, true);
    k10.canvas->canvasText("MSG: " + msg, 16, 130, COLOR_WHITE, k10.canvas->eCNAndENFont16, 50, false);
    k10.canvas->canvasText("Packets Sent: " + String(count), 16, 150, COLOR_YELLOW, k10.canvas->eCNAndENFont16, 50, false);
    k10.canvas->updateCanvas();
}

// Partial refresh for RX card
void updateRxDisplay(String msg, uint32_t count, uint32_t senderCount) {
    k10.canvas->canvasRectangle(14, 214, 212, 46, COLOR_PANEL, COLOR_PANEL, true);
    k10.canvas->canvasText("MSG: " + msg, 16, 218, COLOR_WHITE, k10.canvas->eCNAndENFont16, 50, false);
    k10.canvas->canvasText("Packets Recv: " + String(count) + " (#" + String(senderCount) + ")", 16, 238, COLOR_GREEN, k10.canvas->eCNAndENFont16, 50, false);
    k10.canvas->updateCanvas();
}

void sendRandomMessage() {
    txCount++;
    int idx = random(0, totalPhrases);
    String phrase = String(phraseBank[idx]);
    if (idx == 3) {
        phrase += String(txCount);
    }

    phrase.toCharArray(txData.text, 32);
    txData.counter = txCount;
    txData.timestamp = millis();

    esp_now_send(broadcastAddress, (uint8_t *) &txData, sizeof(txData));
    updateTxDisplay(phrase, txCount);
}

void setup() {
    Serial.begin(115200);
    k10.begin();
    k10.initScreen(screen_dir);
    k10.creatCanvas();
    k10.rgb->brightness(6);

    WiFi.mode(WIFI_STA);

    if (esp_now_init() != ESP_OK) {
        Serial.println("ESP-NOW Init Failed");
        return;
    }

    esp_now_register_send_cb(onDataSent);
    esp_now_register_recv_cb(onDataRecv);

    // Register broadcast peer
    esp_now_peer_info_t peerInfo = {};
    memcpy(peerInfo.peer_addr, broadcastAddress, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;
    esp_now_add_peer(&peerInfo);

    drawStaticChrome();
    updateTxDisplay("Ready to broadcast...", 0);
    updateRxDisplay("Listening on mesh...", 0, 0);
}

void loop() {
    static bool lastBtnA = false;
    bool btnA = k10.buttonA->isPressed();
    if (btnA && !lastBtnA) {
        sendRandomMessage();
    }
    lastBtnA = btnA;

    if (rxFlag) {
        rxFlag = false;
        k10.rgb->write(-1, 0x0088FF);
        updateRxDisplay(String(rxData.text), rxCount, rxData.counter);
    }

    delay(30);
}
```
