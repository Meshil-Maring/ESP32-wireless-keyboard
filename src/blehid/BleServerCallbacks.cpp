#include "BleServerCallbacks.h"

#include <NimBLEDevice.h>
#include <Arduino.h>

#include "../BleHID.h"

BleServerCallbacks::BleServerCallbacks(BleHID &hid)
    : m_hid(hid)
{
}

void BleServerCallbacks::onConnect(NimBLEServer *server, NimBLEConnInfo &connInfo)
{
    (void)server;

    m_hid.m_connected = true;

    Serial.println("BLE Connected");

    NimBLEDevice::startSecurity(connInfo.getConnHandle());
}

void BleServerCallbacks::onDisconnect(NimBLEServer *server, NimBLEConnInfo &, int reason)
{
    (void)server;

    m_hid.m_connected = false;

    Serial.print("BLE Disconnected. Reason: ");
    Serial.println(reason);

    if (server != nullptr)
    {
        server->startAdvertising();
    }
}

void BleServerCallbacks::onAuthenticationComplete(NimBLEConnInfo &connInfo)
{
    if (!connInfo.isEncrypted())
    {
        Serial.println("Pairing/bonding failed - disconnecting.");
        NimBLEDevice::getServer()->disconnect(connInfo.getConnHandle());
        return;
    }

    Serial.println("Bonded and link encrypted successfully.");
}
