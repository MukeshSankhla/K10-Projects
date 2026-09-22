## What this tutorial is??

This tutorial demonstrates using the UNIHIKER K10 as a **Wireless Bluetooth Serial Terminal** over Bluetooth Low Energy (BLE) using the industry-standard Nordic UART Service (NUS).

Features:
- **Wireless Serial Link**: Connect any smartphone or computer using standard BLE terminal apps (e.g. *Serial Bluetooth Terminal* on Android or *nRF Connect* / *LightBlue* on iOS).
- **Retro Phosphor CRT UI**: Vintage terminal monitor aesthetic (`0x080502` phosphor black, `0xFFB000` bright amber, `0xB37400` dim amber, and `0x3E2008` bezel).
- **Console Log Buffer**: Displays the latest 4 incoming serial text lines with automatic vertical scrolling.
- **Two-Way Communication**: Button A transmits an echo telemetry string back to the connected BLE client.

---

## How it works

1. **BLE Architecture on ESP32-S3**:
   - The UNIHIKER K10 features an ESP32-S3 SoC with hardware Bluetooth 5.0 (LE). Classic Bluetooth BR/EDR (SPP) is not supported in hardware.
   - Serial communication is implemented using NUS (Nordic UART Service):
     - **Service UUID**: `6E400001-B5A3-F393-E0A9-E50E24DCCA9E`
     - **RX Characteristic**: `6E400002-B5A3-F393-E0A9-E50E24DCCA9E` (Client writes data to K10)
     - **TX Characteristic**: `6E400003-B5A3-F393-E0A9-E50E24DCCA9E` (K10 notifies data to client)
2. **Dynamic Partial Refresh**:
   - The CRT bezel, terminal header, console monitor frame, and button guides are drawn once in `setup()` (`drawStaticChrome()`).
   - The connection status pill (`ADVERTISING` vs `CONNECTED`) and the internal console text lines are erased and redrawn inside their bounding boxes without screen wiping.

---

## API and Functions detailes

| API / Method | Arguments | Return Type | Description |
| :--- | :--- | :--- | :--- |
| `BLEDevice::init("UNIHIKER_K10_BT")` | Device name | `void` | Initializes BLE stack. |
| `pServer->createService(UUID)` | Service UUID string | `BLEService*` | Creates BLE GATT service. |
| `pService->createCharacteristic(...)` | UUID, properties | `BLECharacteristic*` | Creates RX (write) and TX (notify) characteristics. |
| `pTxCharacteristic->notify()` | None | `void` | Pushes telemetry packets to connected central. |
| `k10.buttonA->isPressed()` | None | `bool` | Detects Button A press to transmit message. |
| `k10.canvas->updateCanvas()` | None | `void` | Flushes buffer to display. |

---

## Full Code

```cpp
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
String logLines[4] = {"", "", "", ""};
bool logUpdated = false;

// Retro Amber Phosphor CRT Theme
const uint32_t COLOR_CRT_BG    = 0x080502; // Dark CRT Phosphor Black
const uint32_t COLOR_CRT_PANEL = 0x1A0E05; // Deep Amber Panel
const uint32_t COLOR_CRT_BORDER= 0x3E2008; // Amber CRT Bezel
const uint32_t COLOR_AMBER_BRT = 0xFFB000; // Bright Phosphor Amber
const uint32_t COLOR_AMBER_DIM = 0xB37400; // Dim Phosphor Amber
const uint32_t COLOR_GREEN     = 0x22C55E; // Connected Green
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
                logLines[3] = "RX> " + incoming;
                logUpdated = true;
            }
        }
    }
};

void drawStaticChrome() {
    k10.canvas->canvasClear();
    k10.canvas->canvasRectangle(0, 0, 240, 320, COLOR_CRT_BG, COLOR_CRT_BG, true);

    // Amber CRT Terminal Header
    k10.canvas->canvasRectangle(0, 0, 240, 36, COLOR_CRT_PANEL, COLOR_CRT_PANEL, true);
    k10.canvas->canvasLine(0, 36, 240, 36, COLOR_AMBER_BRT);
    k10.canvas->canvasText("K10 BLE SERIAL TERMINAL", 10, 10, COLOR_AMBER_BRT, k10.canvas->eCNAndENFont16, 50, false);

    // Status Badge Frame (y: 44 to 78)
    k10.canvas->canvasRectangle(8, 44, 224, 34, COLOR_CRT_BORDER, COLOR_CRT_PANEL, true);

    // CRT Log Shell (y: 84 to 258)
    k10.canvas->canvasRectangle(8, 84, 224, 176, COLOR_CRT_BORDER, COLOR_CRT_PANEL, true);
    k10.canvas->canvasText("CONSOLE MONITOR", 16, 92, COLOR_AMBER_DIM, k10.canvas->eCNAndENFont16, 50, false);
    k10.canvas->canvasLine(16, 110, 224, 110, COLOR_CRT_BORDER);

    // Transmit Guide Footer (y: 266 to 320)
    k10.canvas->canvasRectangle(8, 266, 224, 46, COLOR_CRT_BORDER, COLOR_CRT_PANEL, true);
    k10.canvas->canvasText("BTN A: Send Echo Telemetry", 16, 274, COLOR_AMBER_BRT, k10.canvas->eCNAndENFont16, 50, false);
    k10.canvas->canvasText("NUS: Nordic UART Service", 16, 292, COLOR_AMBER_DIM, k10.canvas->eCNAndENFont16, 50, false);

    k10.canvas->updateCanvas();
}

// Partial refresh for connection status pill
void updateConnectionStatus(bool connected) {
    k10.canvas->canvasRectangle(12, 48, 216, 26, COLOR_CRT_PANEL, COLOR_CRT_PANEL, true);

    if (connected) {
        k10.canvas->canvasRectangle(14, 51, 80, 20, COLOR_GREEN, COLOR_GREEN, true);
        k10.canvas->canvasText("CONNECTED", 20, 53, COLOR_WHITE, k10.canvas->eCNAndENFont16, 50, false);
        k10.canvas->canvasText("Link: Active", 102, 53, COLOR_AMBER_BRT, k10.canvas->eCNAndENFont16, 50, false);
        k10.rgb->write(-1, 0x00FF66);
    } else {
        k10.canvas->canvasRectangle(14, 51, 100, 20, COLOR_CRT_BORDER, COLOR_CRT_BORDER, true);
        k10.canvas->canvasText("ADVERTISING", 20, 53, COLOR_AMBER_BRT, k10.canvas->eCNAndENFont16, 50, false);
        k10.canvas->canvasText("Waiting...", 122, 53, COLOR_AMBER_DIM, k10.canvas->eCNAndENFont16, 50, false);
        k10.rgb->write(-1, 0xFF9900);
    }
    k10.canvas->updateCanvas();
}

// Partial refresh for terminal log lines
void updateTerminalDisplay() {
    k10.canvas->canvasRectangle(12, 116, 216, 138, COLOR_CRT_PANEL, COLOR_CRT_PANEL, true);

    for (int i = 0; i < 4; i++) {
        if (logLines[i].length() > 0) {
            int yPos = 120 + (i * 32);
            k10.canvas->canvasText(logLines[i], 16, yPos, (i == 3) ? COLOR_AMBER_BRT : COLOR_AMBER_DIM, k10.canvas->eCNAndENFont16, 50, false);
        }
    }
    k10.canvas->updateCanvas();
}

void setup() {
    Serial.begin(115200);
    k10.begin();
    k10.initScreen(screen_dir);
    k10.creatCanvas();
    k10.rgb->brightness(6);

    // Initialize BLE
    BLEDevice::init("UNIHIKER_K10_BT");
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
    pServer->getAdvertising()->start();

    logLines[3] = "RX> Ready for connect...";

    drawStaticChrome();
    updateConnectionStatus(false);
    updateTerminalDisplay();
}

void loop() {
    if (deviceConnected != oldDeviceConnected) {
        oldDeviceConnected = deviceConnected;
        updateConnectionStatus(deviceConnected);
        if (!deviceConnected) {
            pServer->startAdvertising();
        }
    }

    if (logUpdated) {
        logUpdated = false;
        updateTerminalDisplay();
    }

    static bool lastBtnA = false;
    bool btnA = k10.buttonA->isPressed();
    if (btnA && !lastBtnA) {
        if (deviceConnected && pTxCharacteristic) {
            String msg = "K10 Echo: millis=" + String(millis()) + "\n";
            pTxCharacteristic->setValue((uint8_t*)msg.c_str(), msg.length());
            pTxCharacteristic->notify();

            logLines[0] = logLines[1];
            logLines[1] = logLines[2];
            logLines[2] = logLines[3];
            logLines[3] = "TX> Sent echo tele";
            updateTerminalDisplay();
        }
    }
    lastBtnA = btnA;

    delay(30);
}
```
