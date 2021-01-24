#include "utils.h"

uint16_t swap_16(uint16_t v)
{
    return (v << 8) | (v >> 8);
}

uint16_t sign_extend(uint16_t x, int bit_count)
{
    if ((x >> (bit_count - 1)) & 1)
    {
        x |= (0xFFFF << bit_count);
    }
    return x;
}