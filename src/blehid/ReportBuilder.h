#ifndef BLEHID_REPORT_BUILDER_H
#define BLEHID_REPORT_BUILDER_H

#include <Arduino.h>

struct KeyboardReport;

void buildReport(const KeyboardReport &report, uint8_t *output, size_t outputSize);

#endif
