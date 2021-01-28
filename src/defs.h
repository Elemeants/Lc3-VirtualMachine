/**
 * @file defs.h
 * @author Daniel Polanco (jdanypa@gmail.com)
 * @brief Contains the definitions of the Lc3
 * @version 1.0
 * @date 2021-01-27
 * 
 * @copyright Copyright (c) 2021
 * 
 */
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

/**
 * @brief Available registers on Lc3
 * 
 */
typedef enum
{
    REG_R0,
    REG_R1,
    REG_R2,
    REG_R3,
    REG_R4,
    REG_R5,
    REG_R6,
    REG_R7,
    REG_R8,
    REG_PC,   // Program counter
    REG_IR,   // Instruction register
    REG_PSR,  // Processor status word
    REG_CC,   // Condition codes (Positive, Negative, Zero)
    REG_MAR,  // Memory Address Register
    REG_MDR,  // Memory Data Register
    REG_COUNT
} Lc3Registers_e;

/**
 * @brief Available states of the control condition register
 * 
 */
typedef enum
{
    CC_P = 1 << 0,  // Positive
    CC_Z = 1 << 1,  // Zero
    CC_N = 1 << 2,  // Negative
} Lc3ControlConditions_e;

/**
 * @brief Available trap codes for the Lc3
 * 
 */
typedef enum
{
    TRAP_GETC = 0x20,   // read a char from keyboard
    TRAP_OUT = 0x21,    // print a char
    TRAP_PUTS = 0x22,   // prints a string
    TRAP_IN = 0x23,     // read a char and echo it to terminal
    TRAP_PUTSP = 0x24,  // output a byte string
    TRAP_HALT = 0x25    // halt the program
} Lc3TrapCodes_e;

/**
 * @brief Memory mapped registers on Lc3
 * 
 */
typedef enum
{
    MMR_KBSR = 0xFE00, // Keyboard status register
    MMR_KBDR = 0XFE02, // Keyboard data register
    MMR_DSR = 0xFE04,  // Display status register
    MMR_DDR = 0xFE06,  // Display data register
    MMR_MRC = 0xFFFE   // Machine control register
} Lc3MMRCodes_e;

#endif  // __DEFS_H__
