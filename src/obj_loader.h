#if !defined(__OBJ_LOADER_H__)
#define __OBJ_LOADER_H__

#include <stdint.h>
#include <stdio.h>

#include "cpu.h"

extern uint8_t loadObjFile(LC3_CPU_t *cpu, const char *filename);

#endif  // __OBJ_LOADER_H__
