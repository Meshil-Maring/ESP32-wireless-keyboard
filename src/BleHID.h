#ifndef BLE_HID_H
#define BLE_HID_H
#include "KeyboardKeys.h"

#include <Arduino.h>

class NimBLEServer;
class NimBLEHIDDevice;
class NimBLECharacteristic;
class BleServerCallbacks;

struct KeyboardReport
{
    uint8_t modifiers = 0;
    uint8_t reserved = 0;
    uint8_t keys[6] = {0};
};

class BleHID
{
    friend class BleServerCallbacks;

private:
    bool m_connected = false;

private:
    KeyboardReport m_report;

private:
    void sendReport();
    bool pressKey(uint8_t key);
    bool releaseKey(uint8_t key);
    void releaseAll();
    void setModifierState(uint8_t modifier, bool pressed);

private:
    NimBLEServer *server;
    NimBLEHIDDevice *hid;

    NimBLECharacteristic *inputReport;
    NimBLECharacteristic *outputReport;

    const char *deviceName;

public:
    BleHID();

    bool begin(const char *deviceName = "ESP32 Keyboard");

    bool isConnected() const
    {
        return m_connected;
    }

    bool press(uint8_t key);
    bool release(uint8_t key);
    bool pressMany(const uint8_t *keys, size_t count);
    bool pressModifier(uint8_t modifier);
    bool releaseModifier(uint8_t modifier);
    bool pressCombo(uint8_t modifier, uint8_t key);

    size_t write(uint8_t character);
    size_t print(const char *text);
    size_t println(const char *text);
    size_t println();
};

#endif