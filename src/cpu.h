#if !defined(__CPU_H__)
#define __CPU_H__

#include "defs.h"

typedef struct
{
    /**
     * @brief CPU Lc3 registers
     * 
     */
    MemSize_t regs[REG_COUNT];

    /**
     * @brief Array of memory for Lc3 CPU
     * 
     */
    MemSize_t memory[ADDRESS_MEMORY_LENGTH];
} Lc3_CPU_t;

#endif  // __CPU_H__
