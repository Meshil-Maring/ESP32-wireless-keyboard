#pragma once

#include <Arduino.h>

class KeyboardMatrix
{
public:
    static constexpr uint8_t ROW_COUNT = 4;
    static constexpr uint8_t COL_COUNT = 4;
    static constexpr uint8_t DEBOUNCE_THRESHOLD = 8;
    static constexpr uint8_t STABLE_SCAN_COUNT = 5;

    using KeyChangeCallback = void (*)(uint8_t row, uint8_t col, bool pressed);

    KeyboardMatrix();

    void begin();
    bool scan();
    bool isPressed(uint8_t row, uint8_t col) const;
    bool changed() const;
    void printState() const;
    void setKeyChangeCallback(KeyChangeCallback callback);

private:
    static const uint8_t rowPins[ROW_COUNT];
    static const uint8_t colPins[COL_COUNT];

    bool m_state[ROW_COUNT][COL_COUNT];
    uint8_t m_debounce[ROW_COUNT][COL_COUNT];
    uint8_t m_pending[ROW_COUNT][COL_COUNT];
    bool m_changed;
    KeyChangeCallback m_callback;

    void setRow(uint8_t row, bool active);
};
