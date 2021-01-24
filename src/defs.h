#if !defined(__DEFS_H__)
#define __DEFS_H__

#include <stdint.h>

/**
 * @brief Each memory location can store 16 bits
 * 
 */
typedef uint16_t MemSize_t;

/**
 * @brief The LC3 have 2^16 addresseable memory, so we write it as HEX value
 * 
 */
#define ADDRESS_MEMORY_LENGTH 0xFFFF

/**
 * @brief Default program counter value
 * 
 */
#define PC_START_ADDRESS 0x3000

/**
 * This enum is ordered by the current opcode (4bit) values.
 * 
 */
typedef enum
{
    OP_BR,    // Branch
    OP_ADD,   // Add
    OP_LD,    // Load
    OP_ST,    // Store
    OP_JSR,   // Jump Register
    OP_AND,   // And
    OP_LDR,   // Load Register
    OP_STR,   // Store Register
    OP_JSRR,  // Jump register Indirect
    OP_NOT,   // Not
    OP_LDI,   // Load Indirect
    OP_STI,   // Store Indirect
    OP_JMP,   // Jump
    OP_RES,   // Not Used
    OP_LEA,   // Load effective address
    OP_TRAP   // Trap
} Lc3Opcodes_e;

typedef enum
{
    REG_PC,   // Program counter
    REG_IR,   // Instruction register
    REG_PSR,  // Processor status word
    REG_CC,   // Condition codes (Positive, Negative, Zero)
    REG_MAR,  // Memory Address Register
    REG_MDR,  // Memory Data Register
    REG_R0,
    REG_R1,
    REG_R2,
    REG_R3,
    REG_R4,
    REG_R5,
    REG_R6,
    REG_R7,
    REG_R8,
    REG_COUNT
} Lc3Registers_e;

#endif  // __DEFS_H__
