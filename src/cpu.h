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
} LC3_CPU_t;

uint8_t Lc3_initCPU(LC3_CPU_t *cpu, const char *filename);

uint8_t Lc3_execInstruction(LC3_CPU_t *cpu);

void Lc3_fetchNext(LC3_CPU_t *cpu);

#endif  // __CPU_H__
