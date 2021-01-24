#if !defined(__UTILS_H__)
#define __UTILS_H__

#include <stdint.h>

extern uint16_t swap_16(uint16_t v);

extern uint16_t sign_extend(uint16_t x, int bit_count);

#define for_in_range(__type, __index, __start, __end) for (__type __index = (__start); __index < (__end); __index ++)

#endif // __UTILS_H__
