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
#include "firmware.h"

/**
 * @brief Struct to manage the state of the CPU
 * 
 */
typedef struct LC3Cpu_t
{
    /**
     * @brief PC(Program counter)
     * 
     */
    uint16_t PC;

    union
    {
        struct
        {
            /**
             * @brief Condition code - positive
             * 
             */
            uint8_t CC_P : 1;
            /**
             * @brief Condition code - zero
             * 
             */
            uint8_t CC_Z : 1;
            /**
             * @brief Condition code - negative
             * 
             */
            uint8_t CC_N : 1;
        };
        /**
         * @brief Condition codes register 
         * 
         */
        uint16_t CC;
    };

    /**
     * @brief Status of the cpu
     * 
     */
    struct
    {
        /**
         * @brief flag indicating if the cpu is executing
         * 
         */
        uint8_t running : 1;
        /**
         * @brief [EXPERIMENTAL] to solve some bugs, i'm thinking in implement a flag
         * that represents when the PC should increment after execute an instruction.
         * 
         */
        uint8_t incrementPC : 1;
    } stat;

    /**
     * @brief Pointer to the firmware instance to execute
     * 
     */
    LC3Firmware_t *firmware;

    /**
     * @brief Array of the directy accessible registers [R0...Rn]
     * 
     */
    uint16_t regs[REG_COUNT];
} LC3Cpu_t;

/**
 * @brief Map between opcode and their action
 * 
 */
typedef struct LC3OpcodeAction_t
{
    /**
     * @brief string name of the opcode
     * 
     */
    const char *name;

    /**
     * @brief opcode value
     * 
     */
    uint8_t opcode;

    /**
     * @brief ptr to the action
     * 
     */
    void (*action)(LC3Cpu_t *cpu, LC3Instruction_t inst);
} LC3OpcodeAction_t;

/**
 * @brief Array of opcodes and their actions ordered
 * 
 */
extern LC3OpcodeAction_t LC3Opcodes[OP_COUNT];

/**
 * @brief 
 * 
 * @param cpu pointer to the cpu instance
 * @param firmware firmware to attach to the cpu
 * @return uint8_t success value
 */
uint8_t LC3CpuInit(LC3Cpu_t *cpu, LC3Firmware_t *firmware);

/**
 * @brief 
 * 
 * @param cpu pointer to the cpu instance
 */
void LC3CpuExecute(LC3Cpu_t *cpu);

/**
 * @brief Reads the memory addressed by the PC register and return it casted
 * to LC3Instruction_t struct
 * 
 * @param cpu pointer to the cpu instance
 * @return LC3Instruction_t instruction readed
 */
LC3Instruction_t LC3CpuReadInstruction(LC3Cpu_t *cpu);

/**
 * @brief Write to the memory of the CPU
 * 
 * @param cpu pointer to the cpu instance
 * @param addr Memory address
 * @param value Value to be written
 * @return uint16_t word readed from memory
 */
uint16_t LC3CpuReadMemory(LC3Cpu_t *cpu, uint16_t addr);

/**
 * @brief Write to the memory of the CPU
 * 
 * @param cpu pointer to the cpu instance
 * @param addr Memory address
 * @param value Value to be written
 */
void LC3CpuWriteMemory(LC3Cpu_t *cpu, uint16_t addr, uint16_t value);

/**
 * @brief Updates the conditional control register of the cpu by a register
 * 
 * @param cpu pointer to the cpu instance
 * @param reg on this register will be updated the CC register
 */
void LC3CpuUpdateCCReg(LC3Cpu_t *cpu, uint16_t reg);

/**
 * @brief Executes the opcode BR
 * 
 * @param cpu pointer to the cpu instance
 * @param inst parameters for add instruction
 */
void LC3Inst_br(LC3Cpu_t *cpu, LC3Instruction_t inst);

/**
 * @brief Executes the opcode ADD
 * 
 * @param cpu pointer to the cpu instance
 * @param inst parameters for add instruction
 */
void LC3Inst_add(LC3Cpu_t *cpu, LC3Instruction_t inst);

/**
 * @brief Executes the opcode LD
 * 
 * @param cpu pointer to the cpu instance
 * @param inst parameters for add instruction
 */
void LC3Inst_ld(LC3Cpu_t *cpu, LC3Instruction_t inst);

/**
 * @brief Executes the opcode ST
 * 
 * @param cpu pointer to the cpu instance
 * @param inst parameters for add instruction
 */
void LC3Inst_st(LC3Cpu_t *cpu, LC3Instruction_t inst);

/**
 * @brief Executes the opcode JSR
 * 
 * @param cpu pointer to the cpu instance
 * @param inst parameters for add instruction
 */
void LC3Inst_jsr(LC3Cpu_t *cpu, LC3Instruction_t inst);

/**
 * @brief Executes the opcode AND
 * 
 * @param cpu pointer to the cpu instance
 * @param inst parameters for add instruction
 */
void LC3Inst_and(LC3Cpu_t *cpu, LC3Instruction_t inst);

/**
 * @brief Executes the opcode LDR
 * 
 * @param cpu pointer to the cpu instance
 * @param inst parameters for add instruction
 */
void LC3Inst_ldr(LC3Cpu_t *cpu, LC3Instruction_t inst);

/**
 * @brief Executes the opcode STR
 * 
 * @param cpu pointer to the cpu instance
 * @param inst parameters for add instruction
 */
void LC3Inst_str(LC3Cpu_t *cpu, LC3Instruction_t inst);

/**
 * @brief Executes the opcode NOT
 * 
 * @param cpu pointer to the cpu instance
 * @param inst parameters for add instruction
 */
void LC3Inst_not(LC3Cpu_t *cpu, LC3Instruction_t inst);

/**
 * @brief Executes the opcode LDI
 * 
 * @param cpu pointer to the cpu instance
 * @param inst parameters for add instruction
 */
void LC3Inst_ldi(LC3Cpu_t *cpu, LC3Instruction_t inst);

/**
 * @brief Executes the opcode STI
 * 
 * @param cpu pointer to the cpu instance
 * @param inst parameters for add instruction
 */
void LC3Inst_sti(LC3Cpu_t *cpu, LC3Instruction_t inst);

/**
 * @brief Executes the opcode JMP
 * 
 * @param cpu pointer to the cpu instance
 * @param inst parameters for add instruction
 */
void LC3Inst_jmp(LC3Cpu_t *cpu, LC3Instruction_t inst);

/**
 * @brief Executes the opcode LEA
 * 
 * @param cpu pointer to the cpu instance
 * @param inst parameters for add instruction
 */
void LC3Inst_lea(LC3Cpu_t *cpu, LC3Instruction_t inst);

/**
 * @brief Executes the opcode TRAP
 * 
 * @param cpu pointer to the cpu instance
 * @param inst parameters for add instruction
 * @return uint8_t because this opcode can stop the execution
 * returns 1 if execution can still run, eiter 0.
 */
void LC3Inst_trap(LC3Cpu_t *cpu, LC3Instruction_t inst);

#endif  // __CPU_H__
