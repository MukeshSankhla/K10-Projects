#include "unihiker_k10.h"
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

UNIHIKER_K10 k10;
uint8_t screen_dir = 2; // Portrait (240x320)

// Nordic UART Service (NUS) UUIDs
#define SERVICE_UUID           "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

BLEServer *pServer = NULL;
BLECharacteristic *pTxCharacteristic = NULL;
bool deviceConnected = false;
bool oldDeviceConnected = false;

// Terminal Log ring buffer (last 4 lines)
String logLines[4] = {"Ready to connect", "Open Serial App", "", ""};
bool logUpdated = true;

// Clean Minimalist Light Theme Palette
const uint32_t COLOR_BG        = 0xF8FAFC; // Soft Slate Off-White
const uint32_t COLOR_CARD      = 0xFFFFFF; // Pure White Card
const uint32_t COLOR_CONSOLE   = 0xF1F5F9; // Soft Console Paper Fill
const uint32_t COLOR_BORDER    = 0xE2E8F0; // Delicate 1px Border
const uint32_t COLOR_BORDER_CON= 0xCBD5E1; // Console 1px Border
const uint32_t COLOR_TEXT_PRI  = 0x0F172A; // Deep Slate Charcoal
const uint32_t COLOR_TEXT_SEC  = 0x334155; // Slate Secondary
const uint32_t COLOR_TEXT_MUTED= 0x64748B; // Slate Muted Label
const uint32_t COLOR_INDIGO    = 0x4F46E5; // Clean Indigo Accent
const uint32_t COLOR_GREEN     = 0x16A34A; // Connected Green
const uint32_t COLOR_RED       = 0xDC2626; // Disconnected Red
const uint32_t COLOR_WHITE     = 0xFFFFFF;

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
        deviceConnected = true;
    }

    void onDisconnect(BLEServer* pServer) {
        deviceConnected = false;
    }
};

class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
        std::string rxValue = pCharacteristic->getValue();
        if (rxValue.length() > 0) {
            String incoming = "";
            for (size_t i = 0; i < rxValue.length(); i++) {
                if (rxValue[i] >= 32 && rxValue[i] <= 126) {
                    incoming += (char)rxValue[i];
                }
            }
            if (incoming.length() > 0) {
                logLines[0] = logLines[1];
                logLines[1] = logLines[2];
                logLines[2] = logLines[3];
                logLines[3] = "RX: " + incoming;
                logUpdated = true;
            }
        }
    }
};

void drawStaticChrome() {
    k10.canvas->canvasClear();
    k10.canvas->canvasRectangle(0, 0, 240, 320, COLOR_BG, COLOR_BG, true);

    // App Header Bar (y: 0 to 40)
    k10.canvas->canvasRectangle(0, 0, 240, 40, COLOR_CARD, COLOR_CARD, true);
    k10.canvas->canvasLine(0, 40, 240, 40, COLOR_BORDER);
    k10.canvas->canvasText("Bluetooth Terminal", 14, 12, COLOR_TEXT_PRI, k10.canvas->eCNAndENFont16, 50, false);
    // Indigo brand accent indicator
    k10.canvas->canvasCircle(224, 20, 4, COLOR_INDIGO, COLOR_INDIGO, true);

    // Device Info Card (y: 48 to 110)
    k10.canvas->canvasRectangle(10, 48, 220, 62, COLOR_BORDER, COLOR_CARD, true);
    k10.canvas->canvasText("BLUETOOTH DEVICE", 20, 56, COLOR_TEXT_MUTED, k10.canvas->eCNAndENFont16, 50, false);
    k10.canvas->canvasText("UNIHIKER K10 BLE", 20, 74, COLOR_TEXT_PRI, k10.canvas->eCNAndENFont16, 50, false);

    // Terminal Container (y: 118 to 278)
    k10.canvas->canvasRectangle(10, 118, 220, 160, COLOR_BORDER, COLOR_CARD, true);
    k10.canvas->canvasText("DATA LOG", 20, 126, COLOR_TEXT_MUTED, k10.canvas->eCNAndENFont16, 50, false);

    // Inner console paper box (y: 144 to 268)
    k10.canvas->canvasRectangle(18, 144, 204, 124, COLOR_BORDER_CON, COLOR_CONSOLE, true);

    // Footer Info Bar (y: 286 to 320)
    k10.canvas->canvasRectangle(0, 286, 240, 34, COLOR_CARD, COLOR_CARD, true);
    k10.canvas->canvasLine(0, 286, 240, 286, COLOR_BORDER);
    k10.canvas->canvasText("[A] Send Ping    Nordic UART", 14, 294, COLOR_INDIGO, k10.canvas->eCNAndENFont16, 50, false);

    k10.canvas->updateCanvas();
}

// Partial refresh for Connection State Pill
void updateConnectionState(bool connected) {
    // Clear status pill area in header (x: 140..214, y: 54..74)
    k10.canvas->canvasRectangle(140, 54, 80, 22, COLOR_CARD, COLOR_CARD, true);

    if (connected) {
        k10.canvas->canvasRectangle(144, 54, 76, 20, COLOR_GREEN, COLOR_GREEN, true);
        k10.canvas->canvasText("CONNECTED", 148, 56, COLOR_WHITE, k10.canvas->eCNAndENFont16, 50, false);
        k10.rgb->write(-1, 0x16A34A);
    } else {
        k10.canvas->canvasRectangle(144, 54, 76, 20, COLOR_BORDER, COLOR_BG, true);
        k10.canvas->canvasText("STANDBY", 154, 56, COLOR_TEXT_MUTED, k10.canvas->eCNAndENFont16, 50, false);
        k10.rgb->write(-1, 0x4F46E5);
    }
    k10.canvas->updateCanvas();
}

// Partial refresh for Terminal Console Lines
void updateTerminalLogs() {
    // Erase inner console area
    k10.canvas->canvasRectangle(20, 146, 200, 120, COLOR_CONSOLE, COLOR_CONSOLE, true);

    for (int i = 0; i < 4; i++) {
        if (logLines[i].length() > 0) {
            uint32_t lineCol = COLOR_TEXT_SEC;
            if (logLines[i].startsWith("RX:")) lineCol = COLOR_INDIGO;
            if (logLines[i].startsWith("TX:")) lineCol = COLOR_GREEN;

            k10.canvas->canvasText(logLines[i], 24, 152 + (i * 26), lineCol, k10.canvas->eCNAndENFont16, 50, false);
        }
    }
    k10.canvas->updateCanvas();
}

void setup() {
    Serial.begin(115200);
    k10.begin();
    k10.initScreen(screen_dir);
    k10.creatCanvas();
    k10.rgb->brightness(5);

    BLEDevice::init("UNIHIKER K10 BLE");
    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks());

    BLEService *pService = pServer->createService(SERVICE_UUID);

    pTxCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_UUID_TX,
        BLECharacteristic::PROPERTY_NOTIFY
    );
    pTxCharacteristic->addDescriptor(new BLE2902());

    BLECharacteristic *pRxCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_UUID_RX,
        BLECharacteristic::PROPERTY_WRITE
    );
    pRxCharacteristic->setCallbacks(new MyCallbacks());

    pService->start();

    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setScanResponse(true);
    pAdvertising->setMinPreferred(0x06);
    pAdvertising->setMinPreferred(0x12);
    BLEDevice::startAdvertising();

    drawStaticChrome();
    updateConnectionState(false);
    updateTerminalLogs();
}

void loop() {
    if (!deviceConnected && oldDeviceConnected) {
        delay(300);
        pServer->startAdvertising();
        oldDeviceConnected = deviceConnected;
        updateConnectionState(false);
    }

    if (deviceConnected && !oldDeviceConnected) {
        oldDeviceConnected = deviceConnected;
        updateConnectionState(true);
    }

    if (logUpdated) {
        logUpdated = false;
        updateTerminalLogs();
    }

    // Button A sends Ping packet over BLE
    if (k10.buttonA->isPressed()) {
        delay(200);
        if (deviceConnected && pTxCharacteristic != NULL) {
            String msg = "PING #" + String(millis() / 1000);
            pTxCharacteristic->setValue(msg.c_str());
            pTxCharacteristic->notify();

            logLines[0] = logLines[1];
            logLines[1] = logLines[2];
            logLines[2] = logLines[3];
            logLines[3] = "TX: " + msg;
            updateTerminalLogs();
        } else {
            logLines[0] = logLines[1];
            logLines[1] = logLines[2];
            logLines[2] = logLines[3];
            logLines[3] = "TX: Not connected";
            updateTerminalLogs();
        }
    }

    delay(20);
}
