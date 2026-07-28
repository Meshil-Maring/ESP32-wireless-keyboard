#ifndef BLEHID_KEY_TRANSLATOR_H
#define BLEHID_KEY_TRANSLATOR_H

#include <Arduino.h>

bool asciiToHid(uint8_t ascii, uint8_t &modifier, uint8_t &keycode);

#endif
