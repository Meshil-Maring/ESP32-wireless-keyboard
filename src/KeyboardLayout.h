#pragma once

#include "KeyboardMatrix.h"
#include "KeyboardKeys.h"

#include <Arduino.h>

constexpr uint8_t MATRIX_LAYOUT[KeyboardMatrix::ROW_COUNT][KeyboardMatrix::COL_COUNT] = {
    {'q', 'w', 'e', 'r'},
    {'a', 's', 'd', 'f'},
    {'z', 'x', 'c', 'v'},
    {'1', '2', '3', '4'},
};

inline uint8_t keycodeFromMatrix(uint8_t row, uint8_t col)
{
    if (row >= KeyboardMatrix::ROW_COUNT || col >= KeyboardMatrix::COL_COUNT)
    {
        return 0;
    }

    return MATRIX_LAYOUT[row][col];
}
