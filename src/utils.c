#include "utils.h"

uint16_t swap_16(uint16_t v)
{
    return (v << 8) | (v >> 8);
}