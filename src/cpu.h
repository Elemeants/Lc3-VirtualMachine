/**
 * @file cpu.h
 * @author Daniel Polanco (jdanypa@gmail.com)
 * @brief Lc3 Cpu implementation, is based on this document
 * https://justinmeiners.github.io/lc3-vm/supplies/lc3-isa.pdf
 * @version 1.0
 * @date 2021-01-27
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#if !defined(__CPU_H__)
#define __CPU_H__

#include "defs.h"

/**
 * @brief Struct to manage the state of the CPU
 * 
 */
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

/**
 * @brief Initializes the Lc3 CPU.
 * 
 * The flow control starts loading to the memory the obj file located in the filename
 * then write the initial PC value, by default which is defined as PC_START_ADDRESS
 * 
 * @param cpu pointer to the cpu instance
 * @param filename path/filename to the obj file to read
 * @return uint8_t 0x1 - OK | 0x0 - ERROR
 */
uint8_t Lc3_initCPU(LC3_CPU_t *cpu, const char *filename);

/**
 * @brief Executes the actual instruction in the PC register
 * 
 * @param cpu pointer to the cpu instance
 * @return uint8_t running state, 1 - continue | 0 - stop.
 */
uint8_t Lc3_execInstruction(LC3_CPU_t *cpu);

/**
 * @brief Fetch the PC register to the next instruction
 * 
 * @param cpu pointer to the cpu instancecpu 
 */
void Lc3_fetchNext(LC3_CPU_t *cpu);

/**
 * @brief Utility to write to a register
 * 
 * @param cpu pointer to the cpu instance
 * @param reg Register of the cpu
 * @param value Value to be written
 */
void Lc3_WriteReg(LC3_CPU_t *cpu, Lc3Registers_e reg, uint16_t value);

/**
 * @brief Utility to read from a register
 * 
 * @param cpu pointer to the cpu instance
 * @param reg Register of the cpu
 * @return uint16_t 
 */
uint16_t Lc3_ReadReg(LC3_CPU_t *cpu, Lc3Registers_e reg);

/**
 * @brief Returns the actual program counter value from the CPU
 * 
 * @param cpu pointer to the cpu instance
 * @return uint16_t PC actual value
 */
uint16_t Lc3_ReadPC(LC3_CPU_t *cpu);

/**
 * @brief Write to the memory of the CPU
 * 
 * @param cpu pointer to the cpu instance
 * @param addr Memory address
 * @param value Value to be written
 */
void Lc3_WriteMem(LC3_CPU_t *cpu, uint16_t addr, uint16_t value);

/**
 * @brief Utility to read from the memory of the CPU
 * 
 * @param cpu pointer to the cpu instance
 * @param addr Memory address
 * @return uint16_t 
 */
uint16_t Lc3_ReadMem(LC3_CPU_t *cpu, uint16_t addr);

/**
 * @brief Returns if the result of the last operation of the CPU 
 * was positive using conditional control register
 * 
 * @param cpu pointer to the cpu instance
 * @return uint8_t 0x1 - true | 0x0 - false
 */
uint8_t Lc3_CC_isPositive(LC3_CPU_t *cpu);

/**
 * @brief Returns if the result of the last operation of the CPU 
 * was zero using conditional control register
 * 
 * @param cpu pointer to the cpu instance
 * @return uint8_t 0x1 - true | 0x0 - false
 */
uint8_t Lc3_CC_isZero(LC3_CPU_t *cpu);

/**
 * @brief Returns if the result of the last operation of the CPU 
 * was negative using conditional control register
 * 
 * @param cpu pointer to the cpu instance
 * @return uint8_t 0x1 - true | 0x0 - false
 */
uint8_t Lc3_CC_isNegative(LC3_CPU_t *cpu);

#endif  // __CPU_H__
