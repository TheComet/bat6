#include "libq.h"
#include <stddef.h>

_Q16 _Q16mpy(_Q16 x, _Q16 y)
{
    short a = x >> 16;
    unsigned short b = x & 0xFFFF;
    short c = y >> 16;
    unsigned short d = y & 0xFFFF;
    int result_low = (b * d);
    int result_mid = (a * d) + (b * c);
    int result_high = (a * c);
    int result = (result_high << 16) + result_mid + (result_low >> 16);
    return (_Q16)result;
}
