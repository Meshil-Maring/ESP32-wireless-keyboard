#include "KeyTranslator.h"

static const uint8_t KEYBOARD_LEFT_SHIFT = 0x02;

bool asciiToHid(uint8_t ascii, uint8_t &modifier, uint8_t &keycode)
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
        case '\r':
            keycode = 0x28;
            break;
        case 0x1B:
            keycode = 0x29;
            break;
        case '\b':
            keycode = 0x2A;
            break;
        case '\t':
            keycode = 0x2B;
            break;
        case ' ':
            keycode = 0x2C;
            break;
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
            break;
        case '@':
            keycode = 0x1F;
            modifier = KEYBOARD_LEFT_SHIFT;
            break;
        case '#':
            keycode = 0x20;
            modifier = KEYBOARD_LEFT_SHIFT;
            break;
        case '$':
            keycode = 0x21;
            modifier = KEYBOARD_LEFT_SHIFT;
            break;
        case '%':
            keycode = 0x22;
            modifier = KEYBOARD_LEFT_SHIFT;
            break;
        case '^':
            keycode = 0x23;
            modifier = KEYBOARD_LEFT_SHIFT;
            break;
        case '&':
            keycode = 0x24;
            modifier = KEYBOARD_LEFT_SHIFT;
            break;
        case '*':
            keycode = 0x25;
            modifier = KEYBOARD_LEFT_SHIFT;
            break;
        case '(':
            keycode = 0x26;
            modifier = KEYBOARD_LEFT_SHIFT;
            break;
        case ')':
            keycode = 0x27;
            modifier = KEYBOARD_LEFT_SHIFT;
            break;
        default:
            return false;
        }
    }

    return true;
}
