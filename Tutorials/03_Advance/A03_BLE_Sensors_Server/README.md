## What this tutorial is??

This tutorial demonstrates transforming the UNIHIKER K10 into a **BLE GATT Environmental Sensor Beacon**, continuously publishing live ambient sensor telemetry over standardized Bluetooth SIG services.

Features:
- **Standardized Environmental Sensing Profile**: Implements Environmental Sensing Service (`0x181A`) with Temperature (`0x2A6E`), Humidity (`0x2A6F`), and Ambient Light (`0x2A77`) characteristics.
- **Electric Indigo & Mint Telemetry Theme**: Modern sci-fi UI (`0x0A061E` space indigo background, `0x818CF8` lavender indigo, `0x2DD4BF` neon mint, and `0xFACC15` sensor gold).
- **Dynamic Onboard Reading**: Integrates the onboard AHT20 high-precision temperature/humidity sensor and ALS ambient light sensor.
- **BLE Notify Streaming**: Pushes new 1-second telemetry packets to any subscribed smartphone or BLE gateway.

---

## How it works

1. **GATT Environmental Sensing Architecture**:
   - The K10 registers a standard BLE GATT Server:
     - `0x181A`: Environmental Sensing Service
     - `0x2A6E`: Temperature Characteristic (Notify + Read)
     - `0x2A6F`: Humidity Characteristic (Notify + Read)
     - `0x2A77`: Light Intensity Characteristic (Notify + Read)
2. **Dynamic Partial Refresh**:
   - The shell, service headers, card labels, and units are drawn once during `setup()`.
   - The 1000ms loop updates only the numeric value boxes inside each card and the advertising/sync status pill.

---

## API and Functions detailes

| API / Method | Arguments | Return Type | Description |
| :--- | :--- | :--- | :--- |
| `aht20.getData(AHT20::eAHT20TempC)` | Metric enum | `float` | Reads temperature in Celsius. |
| `aht20.getData(AHT20::eAHT20HumiRH)` | Metric enum | `float` | Reads relative humidity (%). |
| `k10.readALS()` | None | `uint16_t` | Reads ambient light in Lux. |
| `pChar->notify()` | None | `void` | Broadcasts new reading to connected central. |
| `k10.canvas->updateCanvas()` | None | `void` | Flushes canvas buffer to screen. |

---

## Full Code

```cpp
#include "unihiker_k10.h"
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

UNIHIKER_K10 k10;
AHT20 aht20;
uint8_t screen_dir = 2; // Portrait (240x320)

// Environmental Sensing Standard GATT UUIDs
#define ENV_SERVICE_UUID           "0000181A-0000-1000-8000-00805F9B34FB"
#define TEMP_CHAR_UUID             "00002A6E-0000-1000-8000-00805F9B34FB"
#define HUMID_CHAR_UUID            "00002A6F-0000-1000-8000-00805F9B34FB"
#define LIGHT_CHAR_UUID            "00002A77-0000-1000-8000-00805F9B34FB"

BLEServer *pServer = NULL;
BLECharacteristic *pTempChar = NULL;
BLECharacteristic *pHumidChar = NULL;
BLECharacteristic *pLightChar = NULL;

bool isConnected = false;
bool oldConnected = false;
unsigned long lastTelemetryUpdate = 0;

// Electric Indigo & Mint Telemetry Theme
const uint32_t COLOR_BG         = 0x0A061E; // Deep Space Indigo
const uint32_t COLOR_PANEL      = 0x1E1238; // Indigo Card Panel
const uint32_t COLOR_BORDER     = 0x362261; // Panel Border
const uint32_t COLOR_INDIGO_BRT = 0x818CF8; // Bright Lavender Indigo
const uint32_t COLOR_MINT       = 0x2DD4BF; // Neon Mint Cyan
const uint32_t COLOR_YELLOW     = 0xFACC15; // Golden Yellow
const uint32_t COLOR_WHITE      = 0xFFFFFF;
const uint32_t COLOR_MUTED      = 0x94A3B8;

class BeaconServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
        isConnected = true;
    }
    void onDisconnect(BLEServer* pServer) {
        isConnected = false;
    }
};

void drawStaticChrome() {
    k10.canvas->canvasClear();
    k10.canvas->canvasRectangle(0, 0, 240, 320, COLOR_BG, COLOR_BG, true);

    // Header Bar
    k10.canvas->canvasRectangle(0, 0, 240, 36, COLOR_PANEL, COLOR_PANEL, true);
    k10.canvas->canvasLine(0, 36, 240, 36, COLOR_INDIGO_BRT);
    k10.canvas->canvasText("BLE SENSOR BEACON", 12, 10, COLOR_INDIGO_BRT, k10.canvas->eCNAndENFont16, 50, false);

    // Connection & Device Info Shell (y: 44 to 88)
    k10.canvas->canvasRectangle(8, 44, 224, 44, COLOR_BORDER, COLOR_PANEL, true);
    k10.canvas->canvasText("DEVICE: K10_ENV_GATT", 16, 52, COLOR_WHITE, k10.canvas->eCNAndENFont16, 50, false);
    k10.canvas->canvasText("SERVICE: 0x181A (ENV)", 16, 68, COLOR_MUTED, k10.canvas->eCNAndENFont16, 50, false);

    // Temperature Metric Card (y: 96 to 154)
    k10.canvas->canvasRectangle(8, 96, 224, 58, COLOR_BORDER, COLOR_PANEL, true);
    k10.canvas->canvasText("AIR TEMPERATURE", 16, 104, COLOR_MUTED, k10.canvas->eCNAndENFont16, 50, false);
    k10.canvas->canvasText("C", 195, 122, COLOR_INDIGO_BRT, k10.canvas->eCNAndENFont24, 50, false);

    // Humidity Metric Card (y: 160 to 218)
    k10.canvas->canvasRectangle(8, 160, 224, 58, COLOR_BORDER, COLOR_PANEL, true);
    k10.canvas->canvasText("RELATIVE HUMIDITY", 16, 168, COLOR_MUTED, k10.canvas->eCNAndENFont16, 50, false);
    k10.canvas->canvasText("%", 195, 186, COLOR_MINT, k10.canvas->eCNAndENFont24, 50, false);

    // Ambient Light Metric Card (y: 224 to 282)
    k10.canvas->canvasRectangle(8, 224, 224, 58, COLOR_BORDER, COLOR_PANEL, true);
    k10.canvas->canvasText("AMBIENT LIGHT LEVEL", 16, 232, COLOR_MUTED, k10.canvas->eCNAndENFont16, 50, false);
    k10.canvas->canvasText("Lx", 192, 250, COLOR_YELLOW, k10.canvas->eCNAndENFont24, 50, false);

    // Footer Bar (y: 290 to 320)
    k10.canvas->canvasRectangle(0, 290, 240, 30, COLOR_PANEL, COLOR_PANEL, true);
    k10.canvas->canvasLine(0, 290, 240, 290, COLOR_BORDER);
    k10.canvas->canvasText("STATUS: BROADCASTING", 16, 296, COLOR_MINT, k10.canvas->eCNAndENFont16, 50, false);

    k10.canvas->updateCanvas();
}

// Partial refresh for connection status badge in header
void updateConnectionState(bool connected) {
    k10.canvas->canvasRectangle(170, 52, 54, 28, COLOR_PANEL, COLOR_PANEL, true);
    if (connected) {
        k10.canvas->canvasRectangle(172, 54, 50, 22, 0x10B981, 0x10B981, true);
        k10.canvas->canvasText("SYNC", 178, 57, COLOR_WHITE, k10.canvas->eCNAndENFont16, 50, false);
        k10.rgb->write(-1, 0x00FF99);
    } else {
        k10.canvas->canvasRectangle(172, 54, 50, 22, COLOR_BORDER, COLOR_BORDER, true);
        k10.canvas->canvasText("ADV", 182, 57, COLOR_INDIGO_BRT, k10.canvas->eCNAndENFont16, 50, false);
        k10.rgb->write(-1, 0x8800FF);
    }
    k10.canvas->updateCanvas();
}

// Partial refresh for telemetry values
void updateSensorTelemetry(float temp, float hum, int light) {
    k10.canvas->canvasRectangle(16, 120, 160, 28, COLOR_PANEL, COLOR_PANEL, true);
    k10.canvas->canvasText(String(temp, 1), 18, 122, COLOR_WHITE, k10.canvas->eCNAndENFont24, 50, false);

    k10.canvas->canvasRectangle(16, 184, 160, 28, COLOR_PANEL, COLOR_PANEL, true);
    k10.canvas->canvasText(String(hum, 1), 18, 186, COLOR_WHITE, k10.canvas->eCNAndENFont24, 50, false);

    k10.canvas->canvasRectangle(16, 248, 160, 28, COLOR_PANEL, COLOR_PANEL, true);
    k10.canvas->canvasText(String(light), 18, 250, COLOR_WHITE, k10.canvas->eCNAndENFont24, 50, false);

    k10.canvas->updateCanvas();
}

void setup() {
    Serial.begin(115200);
    k10.begin();
    k10.initScreen(screen_dir);
    k10.creatCanvas();
    k10.rgb->brightness(6);

    // Initialize BLE Server
    BLEDevice::init("K10_ENV_BEACON");
    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new BeaconServerCallbacks());

    BLEService *pService = pServer->createService(ENV_SERVICE_UUID);

    pTempChar = pService->createCharacteristic(TEMP_CHAR_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);
    pTempChar->addDescriptor(new BLE2902());

    pHumidChar = pService->createCharacteristic(HUMID_CHAR_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);
    pHumidChar->addDescriptor(new BLE2902());

    pLightChar = pService->createCharacteristic(LIGHT_CHAR_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);
    pLightChar->addDescriptor(new BLE2902());

    pService->start();
    pServer->getAdvertising()->start();

    drawStaticChrome();
    updateConnectionState(false);
}

void loop() {
    if (isConnected != oldConnected) {
        oldConnected = isConnected;
        updateConnectionState(isConnected);
        if (!isConnected) {
            pServer->startAdvertising();
        }
    }

    unsigned long now = millis();
    if (now - lastTelemetryUpdate >= 1000) {
        lastTelemetryUpdate = now;

        float temp = aht20.getData(AHT20::eAHT20TempC);
        float hum = aht20.getData(AHT20::eAHT20HumiRH);
        int light = k10.readALS();

        if (isConnected) {
            String tStr = String(temp, 2);
            pTempChar->setValue((uint8_t*)tStr.c_str(), tStr.length());
            pTempChar->notify();

            String hStr = String(hum, 2);
            pHumidChar->setValue((uint8_t*)hStr.c_str(), hStr.length());
            pHumidChar->notify();

            String lStr = String(light);
            pLightChar->setValue((uint8_t*)lStr.c_str(), lStr.length());
            pLightChar->notify();
        }

        updateSensorTelemetry(temp, hum, light);
    }

    delay(30);
}
```
