#include "KeyboardMatrix.h"

const uint8_t KeyboardMatrix::rowPins[KeyboardMatrix::ROW_COUNT] = {25, 26, 27, 14};
const uint8_t KeyboardMatrix::colPins[KeyboardMatrix::COL_COUNT] = {32, 33, 34, 35};

KeyboardMatrix::KeyboardMatrix()
    : m_state{},
      m_debounce{},
      m_changed(false),
      m_callback(nullptr)
{
}

void KeyboardMatrix::begin()
{
    // Start with all rows tri-stated (Hi-Z). We'll drive a row low when scanning it.
    for (uint8_t row = 0; row < ROW_COUNT; ++row)
    {
        pinMode(rowPins[row], INPUT);
    }

    for (uint8_t col = 0; col < COL_COUNT; ++col)
    {
        pinMode(colPins[col], INPUT_PULLUP);
    }
}

bool KeyboardMatrix::scan()
{
    m_changed = false;

    // We'll collect debounced changes first, then apply them while
    // checking for ghosting patterns. This reduces false positives
    // caused by capacitive coupling when touching the board.
    struct Change
    {
        uint8_t row;
        uint8_t col;
        bool pressed;
    };

    Change changes[ROW_COUNT * COL_COUNT];
    uint8_t changeCount = 0;

    for (uint8_t r = 0; r < ROW_COUNT; ++r)
    {
        // Tri-state all rows first
        for (uint8_t rr = 0; rr < ROW_COUNT; ++rr)
            setRow(rr, false); // ensure Hi-Z

        // Activate the current row (drive low)
        setRow(r, true);
        delayMicroseconds(30);

        for (uint8_t c = 0; c < COL_COUNT; ++c)
        {
            bool rawPressed = digitalRead(colPins[c]) == LOW;
            // Require stability across multiple full scan cycles to avoid
            // transient capacitive/contact spikes. m_pending counts how many
            // consecutive full scans have seen rawPressed != current state.
            if (rawPressed == m_state[r][c])
            {
                m_pending[r][c] = 0;
                m_debounce[r][c] = 0;
                continue;
            }

            if (++m_pending[r][c] >= STABLE_SCAN_COUNT)
            {
                // record the candidate change
                if (changeCount < (ROW_COUNT * COL_COUNT))
                {
                    changes[changeCount++] = {r, c, rawPressed};
                }
                m_pending[r][c] = 0;
                m_debounce[r][c] = 0;
            }
        }

        // Release current row back to Hi-Z
        setRow(r, false);
    }

    if (changeCount == 0)
        return false;

    // Build a tentative state applying the candidate changes
    bool tentative[ROW_COUNT][COL_COUNT];
    for (uint8_t rr = 0; rr < ROW_COUNT; ++rr)
        for (uint8_t cc = 0; cc < COL_COUNT; ++cc)
            tentative[rr][cc] = m_state[rr][cc];

    for (uint8_t i = 0; i < changeCount; ++i)
    {
        tentative[changes[i].row][changes[i].col] = changes[i].pressed;
    }

    // Simple ghost detection: look for a 2x2 corner with exactly 3 pressed
    bool ghost = false;
    uint8_t ghost_r1 = 0, ghost_r2 = 0, ghost_c1 = 0, ghost_c2 = 0;
    for (uint8_t r1 = 0; r1 < ROW_COUNT && !ghost; ++r1)
    {
        for (uint8_t r2 = r1 + 1; r2 < ROW_COUNT && !ghost; ++r2)
        {
            for (uint8_t c1 = 0; c1 < COL_COUNT && !ghost; ++c1)
            {
                for (uint8_t c2 = c1 + 1; c2 < COL_COUNT && !ghost; ++c2)
                {
                    uint8_t corners = 0;
                    corners += tentative[r1][c1] ? 1 : 0;
                    corners += tentative[r1][c2] ? 1 : 0;
                    corners += tentative[r2][c1] ? 1 : 0;
                    corners += tentative[r2][c2] ? 1 : 0;

                    if (corners == 3)
                    {
                        ghost = true;
                        ghost_r1 = r1;
                        ghost_r2 = r2;
                        ghost_c1 = c1;
                        ghost_c2 = c2;
                    }
                }
            }
        }
    }

    if (ghost)
    {
        // Log suppressed ghost event to help debugging
        Serial.print("Ghost detected at rows ");
        Serial.print(ghost_r1);
        Serial.print(",");
        Serial.print(ghost_r2);
        Serial.print(" cols ");
        Serial.print(ghost_c1);
        Serial.print(",");
        Serial.println(ghost_c2);

        Serial.print("Suppressed candidate changes: ");
        Serial.println(changeCount);
        for (uint8_t i = 0; i < changeCount; ++i)
        {
            Serial.print("  (");
            Serial.print(changes[i].row);
            Serial.print(",");
            Serial.print(changes[i].col);
            Serial.print(") -> ");
            Serial.println(changes[i].pressed ? "pressed" : "released");
            m_debounce[changes[i].row][changes[i].col] = 0;
        }

        return false;
    }

    // No ghost detected: apply changes and call callbacks
    for (uint8_t i = 0; i < changeCount; ++i)
    {
        uint8_t rr = changes[i].row;
        uint8_t cc = changes[i].col;
        bool pressed = changes[i].pressed;

        m_state[rr][cc] = pressed;
        m_changed = true;

        if (m_callback != nullptr)
            m_callback(rr, cc, pressed);
    }

    return m_changed;
}

bool KeyboardMatrix::isPressed(uint8_t row, uint8_t col) const
{
    if (row >= ROW_COUNT || col >= COL_COUNT)
        return false;

    return m_state[row][col];
}

bool KeyboardMatrix::changed() const
{
    return m_changed;
}

void KeyboardMatrix::printState() const
{
    for (uint8_t row = 0; row < ROW_COUNT; ++row)
    {
        Serial.print("Row ");
        Serial.print(row);
        Serial.print(": ");

        for (uint8_t col = 0; col < COL_COUNT; ++col)
        {
            Serial.print(m_state[row][col] ? '1' : '0');
            if (col + 1 < COL_COUNT)
                Serial.print(' ');
        }

        Serial.println();
    }
}

void KeyboardMatrix::setKeyChangeCallback(KeyChangeCallback callback)
{
    m_callback = callback;
}

void KeyboardMatrix::setRow(uint8_t row, bool active)
{
    if (active)
    {
        pinMode(rowPins[row], OUTPUT);
        digitalWrite(rowPins[row], LOW); // drive active row low
    }
    else
    {
        pinMode(rowPins[row], INPUT); // Hi-Z
    }
}
