#include "BleHID.h"

#include <NimBLEDevice.h>
#include <NimBLEHIDDevice.h>

#include <HIDTypes.h>
#include <HIDKeyboardTypes.h>

static const uint8_t HID_REPORT_MAP[] = {
    0x05, 0x01, // Usage Page (Generic Desktop)
    0x09, 0x06, // Usage (Keyboard)
    0xA1, 0x01, // Collection (Application)

    0x85, 0x01, // Report ID (1)

    0x05, 0x07, // Usage Page (Keyboard)
    0x19, 0xE0, // Usage Minimum (224)
    0x29, 0xE7, // Usage Maximum (231)

    0x15, 0x00,
    0x25, 0x01,

    0x75, 0x01,
    0x95, 0x08,
    0x81, 0x02,

    0x95, 0x01,
    0x75, 0x08,
    0x81, 0x01,

    0x95, 0x06,
    0x75, 0x08,

    0x15, 0x00,
    0x25, 0x65,

    0x05, 0x07,
    0x19, 0x00,
    0x29, 0x65,

    0x81, 0x00,

    0x95, 0x05,
    0x75, 0x01,

    0x05, 0x08,
    0x19, 0x01,
    0x29, 0x05,

    0x91, 0x02,

    0x95, 0x01,
    0x75, 0x03,

    0x91, 0x01,

    0xC0};

// Left Shift modifier bit, as used in the keyboard input report
// (byte 0 of the report — see HID_REPORT_MAP above).
static const uint8_t KEYBOARD_LEFT_SHIFT = 0x02;

// Translates one ASCII character into the USB HID keycode (and, if
// needed, the modifier bits) that produce it on a US keyboard layout.
// Returns false for characters with no direct single-key mapping.
static bool asciiToHid(uint8_t ascii, uint8_t &modifier, uint8_t &keycode)
{
    modifier = 0;
    keycode = 0;

    if (ascii >= 'a' && ascii <= 'z')
    {
        keycode = 0x04 + (ascii - 'a');
    }
    else if (ascii >= 'A' && ascii <= 'Z')
    {
        keycode = 0x04 + (ascii - 'A');
        modifier = KEYBOARD_LEFT_SHIFT;
    }
    else if (ascii >= '1' && ascii <= '9')
    {
        keycode = 0x1E + (ascii - '1');
    }
    else
    {
        switch (ascii)
        {
        case '0':
            keycode = 0x27;
            break;
        case '\n':
            keycode = 0x28;
            break; // Enter
        case '\r':
            keycode = 0x28;
            break; // Enter
        case 0x1B:
            keycode = 0x29;
            break; // Escape
        case '\b':
            keycode = 0x2A;
            break; // Backspace
        case '\t':
            keycode = 0x2B;
            break; // Tab
        case ' ':
            keycode = 0x2C;
            break; // Space

        case '-':
            keycode = 0x2D;
            break;
        case '_':
            keycode = 0x2D;
            modifier = KEYBOARD_LEFT_SHIFT;
            break;
        case '=':
            keycode = 0x2E;
            break;
        case '+':
            keycode = 0x2E;
            modifier = KEYBOARD_LEFT_SHIFT;
            break;
        case '[':
            keycode = 0x2F;
            break;
        case '{':
            keycode = 0x2F;
            modifier = KEYBOARD_LEFT_SHIFT;
            break;
        case ']':
            keycode = 0x30;
            break;
        case '}':
            keycode = 0x30;
            modifier = KEYBOARD_LEFT_SHIFT;
            break;
        case '\\':
            keycode = 0x31;
            break;
        case '|':
            keycode = 0x31;
            modifier = KEYBOARD_LEFT_SHIFT;
            break;
        case ';':
            keycode = 0x33;
            break;
        case ':':
            keycode = 0x33;
            modifier = KEYBOARD_LEFT_SHIFT;
            break;
        case '\'':
            keycode = 0x34;
            break;
        case '"':
            keycode = 0x34;
            modifier = KEYBOARD_LEFT_SHIFT;
            break;
        case '`':
            keycode = 0x35;
            break;
        case '~':
            keycode = 0x35;
            modifier = KEYBOARD_LEFT_SHIFT;
            break;
        case ',':
            keycode = 0x36;
            break;
        case '<':
            keycode = 0x36;
            modifier = KEYBOARD_LEFT_SHIFT;
            break;
        case '.':
            keycode = 0x37;
            break;
        case '>':
            keycode = 0x37;
            modifier = KEYBOARD_LEFT_SHIFT;
            break;
        case '/':
            keycode = 0x38;
            break;
        case '?':
            keycode = 0x38;
            modifier = KEYBOARD_LEFT_SHIFT;
            break;

        case '!':
            keycode = 0x1E;
            modifier = KEYBOARD_LEFT_SHIFT;
            break; // Shift+1
        case '@':
            keycode = 0x1F;
            modifier = KEYBOARD_LEFT_SHIFT;
            break; // Shift+2
        case '#':
            keycode = 0x20;
            modifier = KEYBOARD_LEFT_SHIFT;
            break; // Shift+3
        case '$':
            keycode = 0x21;
            modifier = KEYBOARD_LEFT_SHIFT;
            break; // Shift+4
        case '%':
            keycode = 0x22;
            modifier = KEYBOARD_LEFT_SHIFT;
            break; // Shift+5
        case '^':
            keycode = 0x23;
            modifier = KEYBOARD_LEFT_SHIFT;
            break; // Shift+6
        case '&':
            keycode = 0x24;
            modifier = KEYBOARD_LEFT_SHIFT;
            break; // Shift+7
        case '*':
            keycode = 0x25;
            modifier = KEYBOARD_LEFT_SHIFT;
            break; // Shift+8
        case '(':
            keycode = 0x26;
            modifier = KEYBOARD_LEFT_SHIFT;
            break; // Shift+9
        case ')':
            keycode = 0x27;
            modifier = KEYBOARD_LEFT_SHIFT;
            break; // Shift+0

        default:
            return false; // unsupported character
        }
    }

    return true;
}

class BleServerCallbacks : public NimBLEServerCallbacks
{
public:
    explicit BleServerCallbacks(BleHID &hid)
        : m_hid(hid)
    {
    }

    void onConnect(NimBLEServer *server,
                   NimBLEConnInfo &connInfo) override
    {
        (void)server;

        m_hid.m_connected = true;

        Serial.println("BLE Connected");

        // HID input reports must go over an encrypted/bonded link.
        // Ask for it immediately instead of waiting for the host -
        // without this, some hosts connect, fail the security
        // handshake, and disconnect in an endless loop.
        NimBLEDevice::startSecurity(connInfo.getConnHandle());
    }

    void onDisconnect(NimBLEServer *server,
                      NimBLEConnInfo &,
                      int reason) override
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

    void onAuthenticationComplete(NimBLEConnInfo &connInfo) override
    {
        if (!connInfo.isEncrypted())
        {
            // Pairing/bonding failed - drop it cleanly instead of
            // leaving a half-working connection that will just
            // flap on and off.
            Serial.println("Pairing/bonding failed - disconnecting.");
            NimBLEDevice::getServer()->disconnect(connInfo.getConnHandle());
            return;
        }

        Serial.println("Bonded and link encrypted successfully.");
    }

private:
    BleHID &m_hid;
};

BleHID::BleHID()
    : server(nullptr),
      hid(nullptr),
      inputReport(nullptr),
      outputReport(nullptr),
      deviceName(nullptr)
{
}

void BleHID::setModifierState(uint8_t modifier, bool pressed)
{
    if (modifier == 0)
        return;

    if (pressed)
    {
        m_report.modifiers |= modifier;
    }
    else
    {
        m_report.modifiers &= ~modifier;
    }

    sendReport();
}

bool BleHID::pressModifier(uint8_t modifier)
{
    setModifierState(modifier, true);
    return true;
}

bool BleHID::releaseModifier(uint8_t modifier)
{
    setModifierState(modifier, false);
    return true;
}

bool BleHID::begin(const char *deviceName)
{
    Serial.println("=================================");
    Serial.println("Initializing NimBLE...");
    Serial.println("=================================");

    NimBLEDevice::init(deviceName);

    // Require bonding + encryption for this device, with no
    // passkey exchange (the ESP32 has no display/buttons here, so
    // pairing falls back to "Just Works"). This is what was
    // missing and is the root cause of the connect/disconnect loop.
    NimBLEDevice::setSecurityAuth(true, true, true);
    NimBLEDevice::setSecurityIOCap(BLE_HS_IO_NO_INPUT_OUTPUT);

    this->deviceName = deviceName;
    server = NimBLEDevice::createServer();
    server->setCallbacks(new BleServerCallbacks(*this));
    hid = new NimBLEHIDDevice(server);

    inputReport = hid->getInputReport(1);
    outputReport = hid->getOutputReport(1);

    hid->getBootInput();
    hid->getBootOutput();

    hid->setManufacturer("Meshil");
    hid->setPnp(0x02, 0x1234, 0x5678, 0x0100);
    hid->setHidInfo(0x00, 0x01);

    hid->setReportMap(
        const_cast<uint8_t *>(HID_REPORT_MAP),
        sizeof(HID_REPORT_MAP));

    hid->setBatteryLevel(100);

    // Start all GATT services
    server->start();

    NimBLEAdvertising *advertising = NimBLEDevice::getAdvertising();

    // Advertise the HID Service (0x1812)
    advertising->addServiceUUID(hid->getHidService()->getUUID());

    // Also advertise the Battery Service (0x180F)
    advertising->addServiceUUID(hid->getBatteryService()->getUUID());

    // Tell the host this is a keyboard, for the right device icon
    // and so some OSes handle the HID connection correctly.
    advertising->setAppearance(HID_KEYBOARD);

    advertising->setName(deviceName);
    advertising->start();

    Serial.println("Bluetooth advertising started.");
    Serial.print("Device Name: ");
    Serial.println(deviceName);

    return true;
}
/*
============================
 SEND REPORT
============================
*/

void BleHID::sendReport()
{
    if (!m_connected || inputReport == nullptr)
        return;

    uint8_t report[8] = {0};

    report[0] = m_report.modifiers;
    report[1] = m_report.reserved;

    for (uint8_t i = 0; i < 6; ++i)
    {
        report[2 + i] = m_report.keys[i];
    }

    inputReport->setValue(report, sizeof(report));
    inputReport->notify();
}

/*
============================
 PRESSKEY KEY
============================
*/

bool BleHID::pressKey(uint8_t key)
{
    // Already pressed - nothing to do, but it's not a failure
    for (uint8_t i = 0; i < 6; i++)
    {
        if (m_report.keys[i] == key)
            return true;
    }

    // Find the first empty slot
    for (uint8_t i = 0; i < 6; i++)
    {
        if (m_report.keys[i] == 0)
        {
            m_report.keys[i] = key;
            sendReport();
            return true;
        }
    }

    // No free slot (6-key rollover limit)
    return false;
}

/*
============================
 RELEASE KEY
============================
*/

bool BleHID::releaseKey(uint8_t key)
{
    for (uint8_t i = 0; i < 6; i++)
    {
        if (m_report.keys[i] == key)
        {
            // Shift remaining keys left
            for (uint8_t j = i; j < 5; j++)
            {
                m_report.keys[j] = m_report.keys[j + 1];
            }

            m_report.keys[5] = 0;

            sendReport();
            return true;
        }
    }

    return false;
}

/*
============================
 PRESS (public)
============================
*/

bool BleHID::press(uint8_t key)
{
    if (key == KEY_LEFT_CTRL || key == KEY_LEFT_SHIFT || key == KEY_LEFT_ALT ||
        key == KEY_LEFT_GUI || key == KEY_RIGHT_CTRL || key == KEY_RIGHT_SHIFT ||
        key == KEY_RIGHT_ALT || key == KEY_RIGHT_GUI)
    {
        return pressModifier(key);
    }

    uint8_t usage = 0;

    // ASCII keys
    if (key < 0x80)
    {
        KEYMAP map = keymap[key];

        if (map.usage == 0)
            return false;

        if (map.modifier)
            pressModifier(map.modifier);

        usage = map.usage;
    }
    else
    {
        switch (key)
        {
        case KEY_ENTER:
            usage = 0x28;
            break;

        case KEY_ESC:
            usage = 0x29;
            break;

        case KEY_BACKSPACE:
            usage = 0x2A;
            break;

        case KEY_TAB:
            usage = 0x2B;
            break;

        case KEY_DELETE:
            usage = 0x4C;
            break;

        default:
            return false;
        }
    }

    return pressKey(usage);
}

/*
============================
 RELEASE (public)
============================
*/

bool BleHID::release(uint8_t key)
{
    if (key == KEY_LEFT_CTRL || key == KEY_LEFT_SHIFT || key == KEY_LEFT_ALT ||
        key == KEY_LEFT_GUI || key == KEY_RIGHT_CTRL || key == KEY_RIGHT_SHIFT ||
        key == KEY_RIGHT_ALT || key == KEY_RIGHT_GUI)
    {
        return releaseModifier(key);
    }

    uint8_t usage = 0;

    if (key < 0x80)
    {
        KEYMAP map = keymap[key];

        if (map.usage == 0)
            return false;

        if (map.modifier)
            releaseModifier(map.modifier);

        usage = map.usage;
    }
    else
    {
        switch (key)
        {
        case KEY_ENTER:
            usage = 0x28;
            break;

        case KEY_ESC:
            usage = 0x29;
            break;

        case KEY_BACKSPACE:
            usage = 0x2A;
            break;

        case KEY_TAB:
            usage = 0x2B;
            break;

        case KEY_DELETE:
            usage = 0x4C;
            break;

        default:
            return false;
        }
    }

    return releaseKey(usage);
}

/*
============================
 RELEASE ALL KEY
============================
*/
void BleHID::releaseAll()
{
    m_report.modifiers = 0;

    for (uint8_t i = 0; i < 6; i++)
    {
        m_report.keys[i] = 0;
    }

    sendReport();
}

size_t BleHID::write(uint8_t character)
{
    uint8_t modifier = 0;
    uint8_t keycode = 0;

    if (!asciiToHid(character, modifier, keycode))
        return 0; // no HID mapping for this character

    if (modifier)
        pressModifier(modifier);

    pressKey(keycode);
    delay(10);
    releaseKey(keycode);

    if (modifier)
        releaseModifier(modifier);

    return 1;
}

/*
============================
 PRINT
============================
*/

size_t BleHID::print(const char *text)
{
    if (text == nullptr)
        return 0;

    size_t count = 0;

    while (*text)
    {
        write(*text++);
        count++;
    }

    return count;
}

/*
============================
 PRINT LINE
============================
*/

size_t BleHID::println(const char *text)
{
    size_t count = print(text);

    write('\r');
    write('\n');

    return count + 2;
}