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

// Clean Minimalist Light Theme Palette
const uint32_t COLOR_BG        = 0xF8FAFC; // Soft Slate Off-White
const uint32_t COLOR_CARD      = 0xFFFFFF; // Pure White Card Fill
const uint32_t COLOR_BORDER    = 0xE2E8F0; // Delicate 1px Border
const uint32_t COLOR_TEXT_PRI  = 0x0F172A; // Deep Slate Charcoal
const uint32_t COLOR_TEXT_SEC  = 0x334155; // Slate Secondary
const uint32_t COLOR_TEXT_MUTED= 0x64748B; // Slate Muted Label
const uint32_t COLOR_TEAL      = 0x0D9488; // Refined Teal
const uint32_t COLOR_CORAL     = 0xE11D48; // Clean Rose / Coral
const uint32_t COLOR_AMBER     = 0xD97706; // Warm Amber
const uint32_t COLOR_GREEN     = 0x16A34A; // Connected Green
const uint32_t COLOR_WHITE     = 0xFFFFFF;

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

    // App Header Bar (y: 0 to 40)
    k10.canvas->canvasRectangle(0, 0, 240, 40, COLOR_CARD, COLOR_CARD, true);
    k10.canvas->canvasLine(0, 40, 240, 40, COLOR_BORDER);
    k10.canvas->canvasText("Environmental BLE", 14, 12, COLOR_TEXT_PRI, k10.canvas->eCNAndENFont16, 50, false);
    // Teal brand dot
    k10.canvas->canvasCircle(224, 20, 4, COLOR_TEAL, COLOR_TEAL, true);

    // Device / GATT Service Info Card (y: 48 to 96)
    k10.canvas->canvasRectangle(10, 48, 220, 48, COLOR_BORDER, COLOR_CARD, true);
    k10.canvas->canvasText("BLE SENSOR SERVICE", 20, 56, COLOR_TEXT_MUTED, k10.canvas->eCNAndENFont16, 50, false);
    k10.canvas->canvasText("GATT 0x181A", 20, 72, COLOR_TEXT_SEC, k10.canvas->eCNAndENFont16, 50, false);

    // Temperature Metric Card (y: 104 to 160)
    k10.canvas->canvasRectangle(10, 104, 220, 56, COLOR_BORDER, COLOR_CARD, true);
    k10.canvas->canvasText("TEMPERATURE", 20, 112, COLOR_TEXT_MUTED, k10.canvas->eCNAndENFont16, 50, false);

    // Humidity Metric Card (y: 168 to 224)
    k10.canvas->canvasRectangle(10, 168, 220, 56, COLOR_BORDER, COLOR_CARD, true);
    k10.canvas->canvasText("HUMIDITY", 20, 176, COLOR_TEXT_MUTED, k10.canvas->eCNAndENFont16, 50, false);

    // Light Sensor Card (y: 232 to 284)
    k10.canvas->canvasRectangle(10, 232, 220, 52, COLOR_BORDER, COLOR_CARD, true);
    k10.canvas->canvasText("AMBIENT LIGHT", 20, 240, COLOR_TEXT_MUTED, k10.canvas->eCNAndENFont16, 50, false);

    // Footer Bar (y: 292 to 320)
    k10.canvas->canvasRectangle(0, 292, 240, 28, COLOR_CARD, COLOR_CARD, true);
    k10.canvas->canvasLine(0, 292, 240, 292, COLOR_BORDER);
    k10.canvas->canvasText("Standard GATT Environmental Sensing", 16, 298, COLOR_TEAL, k10.canvas->eCNAndENFont16, 50, false);

    k10.canvas->updateCanvas();
}

// Partial refresh for connection status badge
void updateConnectionState(bool connected) {
    // Clear badge region in device card (x: 136..220, y: 56..84)
    k10.canvas->canvasRectangle(136, 56, 88, 24, COLOR_CARD, COLOR_CARD, true);

    if (connected) {
        k10.canvas->canvasRectangle(140, 58, 80, 20, COLOR_GREEN, COLOR_GREEN, true);
        k10.canvas->canvasText("CONNECTED", 146, 60, COLOR_WHITE, k10.canvas->eCNAndENFont16, 50, false);
        k10.rgb->write(-1, 0x16A34A);
    } else {
        k10.canvas->canvasRectangle(140, 58, 80, 20, COLOR_BORDER, COLOR_BG, true);
        k10.canvas->canvasText("BROADCAST", 146, 60, COLOR_TEXT_MUTED, k10.canvas->eCNAndENFont16, 50, false);
        k10.rgb->write(-1, 0x0D9488);
    }
    k10.canvas->updateCanvas();
}

// Partial refresh for sensor telemetry values
void updateSensorTelemetry(float temp, float humid, uint16_t light) {
    // Clear temp value box
    k10.canvas->canvasRectangle(20, 130, 190, 22, COLOR_CARD, COLOR_CARD, true);
    String tempStr = String(temp, 1) + "  deg C";
    k10.canvas->canvasText(tempStr, 20, 130, COLOR_CORAL, k10.canvas->eCNAndENFont16, 50, false);

    // Clear humid value box
    k10.canvas->canvasRectangle(20, 194, 190, 22, COLOR_CARD, COLOR_CARD, true);
    String humidStr = String(humid, 1) + "  % RH";
    k10.canvas->canvasText(humidStr, 20, 194, COLOR_TEAL, k10.canvas->eCNAndENFont16, 50, false);

    // Clear light value box
    k10.canvas->canvasRectangle(20, 256, 190, 20, COLOR_CARD, COLOR_CARD, true);
    String lightStr = String(light) + "  Lux";
    k10.canvas->canvasText(lightStr, 20, 256, COLOR_AMBER, k10.canvas->eCNAndENFont16, 50, false);

    k10.canvas->updateCanvas();
}

void setup() {
    Serial.begin(115200);
    k10.begin();
    k10.initScreen(screen_dir);
    k10.creatCanvas();
    k10.rgb->brightness(5);

    BLEDevice::init("K10-ENV-NODE");
    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new BeaconServerCallbacks());

    BLEService *pService = pServer->createService(ENV_SERVICE_UUID);

    pTempChar = pService->createCharacteristic(
        TEMP_CHAR_UUID,
        BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY
    );
    pTempChar->addDescriptor(new BLE2902());

    pHumidChar = pService->createCharacteristic(
        HUMID_CHAR_UUID,
        BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY
    );
    pHumidChar->addDescriptor(new BLE2902());

    pLightChar = pService->createCharacteristic(
        LIGHT_CHAR_UUID,
        BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY
    );
    pLightChar->addDescriptor(new BLE2902());

    pService->start();

    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(ENV_SERVICE_UUID);
    pAdvertising->setScanResponse(true);
    BLEDevice::startAdvertising();

    drawStaticChrome();
    updateConnectionState(false);
    updateSensorTelemetry(0.0, 0.0, 0);
}

void loop() {
    if (!isConnected && oldConnected) {
        delay(300);
        pServer->startAdvertising();
        oldConnected = isConnected;
        updateConnectionState(false);
    }
    if (isConnected && !oldConnected) {
        oldConnected = isConnected;
        updateConnectionState(true);
    }

    unsigned long now = millis();
    if (now - lastTelemetryUpdate >= 1000) {
        lastTelemetryUpdate = now;

        float temp = aht20.getData(AHT20::eAHT20TempC);
        float humid = aht20.getData(AHT20::eAHT20HumiRH);
        uint16_t light = k10.readALS();

        updateSensorTelemetry(temp, humid, light);

        if (isConnected) {
            int16_t bleTemp = (int16_t)(temp * 100);
            pTempChar->setValue((uint8_t*)&bleTemp, 2);
            pTempChar->notify();

            uint16_t bleHumid = (uint16_t)(humid * 100);
            pHumidChar->setValue((uint8_t*)&bleHumid, 2);
            pHumidChar->notify();

            pLightChar->setValue((uint8_t*)&light, 2);
            pLightChar->notify();
        }
    }

    delay(20);
}
