/**
 * @file utils.h
 * @author Daniel Polanco (jdanypa@gmail.com)
 * @brief 
 * @version 1.0
 * @date 2021-01-27
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#if !defined(__UTILS_H__)
#define __UTILS_H__

#include <stdint.h>

/**
 * @brief Swaps a 16bit number in binary format, used to change the endianness
 * Example:
 *  0xAAFF -> 0xFFAA
 *  0x1234 -> 0x3412
 * 
 * @param v Value to swap
 * @return uint16_t value swaped
 */
extern uint16_t swap_16(uint16_t v);

/**
 * @brief Is the operation, in computer arithmetic, of increasing the number of
 * bits of a binary number while preserving the number's sign (positive/negative) and value
 * 
 * @param x original value
 * @param bit_count no of bits in the original value
 * @return uint16_t value in 16bits with the same value.
 */
extern uint16_t sign_extend(uint16_t x, int bit_count);

#endif // __UTILS_H__
