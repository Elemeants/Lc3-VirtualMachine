/**
 * @file obj_loader.h
 * @author Daniel Polanco (jdanypa@gmail.com)
 * @brief 
 * @version 1.0
 * @date 2021-01-27
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#if !defined(__OBJ_LOADER_H__)
#define __OBJ_LOADER_H__

#include <stdint.h>
#include <stdio.h>

#include "cpu.h"

/**
 * @brief Loads and dumps the program file into the CPU
 * 
 * @param cpu pointer to the cpu instance
 * @param filename path/filename to the obj file
 * @return uint8_t 0x1 - Ok | 0x0 - Error
 */
extern uint8_t loadObjFile(LC3_CPU_t *cpu, const char *filename);

#endif  // __OBJ_LOADER_H__
