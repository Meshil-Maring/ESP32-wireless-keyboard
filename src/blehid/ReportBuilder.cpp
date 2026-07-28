#include "ReportBuilder.h"
#include "../BleHID.h"

void buildReport(const KeyboardReport &report, uint8_t *output, size_t outputSize)
{
    if (output == nullptr || outputSize < 8)
        return;

    output[0] = report.modifiers;
    output[1] = report.reserved;

    for (uint8_t i = 0; i < 6; ++i)
    {
        output[2 + i] = report.keys[i];
    }
}
