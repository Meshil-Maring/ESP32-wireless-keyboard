#ifndef BLEHID_BLE_SERVER_CALLBACKS_H
#define BLEHID_BLE_SERVER_CALLBACKS_H

#include <NimBLEServer.h>

class BleHID;

class BleServerCallbacks : public NimBLEServerCallbacks
{
public:
    explicit BleServerCallbacks(BleHID &hid);

    void onConnect(NimBLEServer *server, NimBLEConnInfo &connInfo) override;
    void onDisconnect(NimBLEServer *server, NimBLEConnInfo &connInfo, int reason) override;
    void onAuthenticationComplete(NimBLEConnInfo &connInfo) override;

private:
    BleHID &m_hid;
};

#endif
