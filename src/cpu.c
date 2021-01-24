#include "cpu.h"

#include <stdio.h>

#include "log.h"
#include "obj_loader.h"
#include "utils.h"

#define FETCH_INSTRUCTION_CPU(__cpu) (cpu->memory[cpu->regs[REG_PC]])

void Lc3_CPU_UpdateCCReg(LC3_CPU_t *cpu, Lc3Registers_e reg);

void Lc3_OP_Add(LC3_CPU_t *cpu);
void Lc3_OP_And(LC3_CPU_t *cpu);
void Lc3_OP_Not(LC3_CPU_t *cpu);
void Lc3_OP_ST(LC3_CPU_t *cpu);
void Lc3_OP_STI(LC3_CPU_t *cpu);
void Lc3_OP_STR(LC3_CPU_t *cpu);
void Lc3_OP_Trap(LC3_CPU_t *cpu);

void Lc3_WriteMem(LC3_CPU_t *cpu, uint16_t addr, uint16_t value)
{
    cpu->memory[addr] = value;
}

uint16_t Lc3_ReadMem(LC3_CPU_t *cpu, uint16_t addr)
{
    return cpu->memory[addr];
}

void Lc3_WriteReg(LC3_CPU_t *cpu, Lc3Registers_e reg, uint16_t value)
{
    cpu->regs[reg] = value;
    Lc3_CPU_UpdateCCReg(cpu, reg);
}

uint16_t Lc3_ReadReg(LC3_CPU_t *cpu, Lc3Registers_e reg)
{
    return cpu->regs[reg];
}

uint16_t Lc3_ReadPC(LC3_CPU_t *cpu)
{
    return cpu->regs[REG_PC];
}

uint8_t Lc3_initCPU(LC3_CPU_t *cpu, const char *filename)
{
    LOG_LN("Initializing CPU");
    if (!loadObjFile(cpu, filename))
    {
        LOG_LN("Can't load objfile, exit...");
        return 0;
    }

    LOG_LN("Set CPU Program counter to 0x%04X", cpu->regs[REG_PC]);
    Lc3_WriteReg(cpu, REG_PC, PC_START_ADDRESS);
    return 1;
}

uint8_t Lc3_execInstruction(LC3_CPU_t *cpu)
{
    uint16_t instruction = FETCH_INSTRUCTION_CPU(cpu);
    Lc3Opcodes_e opcode = (Lc3Opcodes_e)((instruction & 0xF000) >> 12);
    LOG(" PC: 0x%04X [0x%04X] -> ", cpu->regs[REG_PC], instruction);
    switch (opcode)
    {
    case OP_BR:
        printf("BR\n");
        break;
    case OP_ADD:
        printf("ADD\n");
        Lc3_OP_Add(cpu);
        break;
    case OP_LD:
        printf("LD\n");
        break;
    case OP_ST:
        printf("ST\n");
        Lc3_OP_ST(cpu);
        break;
    case OP_JSR:
        printf("JSR\n");
        break;
    case OP_AND:
        printf("AND\n");
        Lc3_OP_And(cpu);
        break;
    case OP_LDR:
        printf("LDR\n");
        break;
    case OP_STR:
        printf("STR\n");
        Lc3_OP_STR(cpu);
        break;
    case OP_JSRR:
        printf("JSRR\n");
        break;
    case OP_NOT:
        printf("NOT\n");
        break;
    case OP_LDI:
        printf("LDI\n");
        break;
    case OP_STI:
        printf("STI\n");
        Lc3_OP_STI(cpu);
        break;
    case OP_JMP:
        printf("JMP\n");
        break;
    case OP_RES:
        printf("RES\n");
        return 0;
    case OP_LEA:
        printf("LEA\n");
        break;
    case OP_TRAP:
        printf("TRAP\n");
        Lc3_OP_Trap(cpu);
        return 0;
    default:
        printf("ERROR Invalid opcode 0x%X", (uint8_t)opcode);
        return 0;
    }
    return 1;
}

void Lc3_fetchNext(LC3_CPU_t *cpu)
{
    cpu->regs[REG_PC]++;
}

void Lc3_OP_Add(LC3_CPU_t *cpu)
{
    //  INSTRUCTION FORM
    // |   ADD OPCODE  | dr | sr1 | in|    param    |
    // +---+---+---+---+----+-----+---+---+---+-----+
    // | 0 | 0 | 0 | 1 | DR | SR1 | 0 | 0 | 0 | SR2 |
    // +---+---+---+---+----+-----+---+---+---+-----+
    // | 0 | 0 | 0 | 1 | DR | SR1 | 1 |  Inmediate  |
    // +---+---+---+---+----+-----+---+---+---+-----+
    //
    // If in (inmediate) flag is active then the param value is directly added
    uint16_t instruction = FETCH_INSTRUCTION_CPU(cpu);
    uint16_t dr = (instruction >> 9) & 0x7;
    uint16_t sr1 = (instruction >> 6) & 0x7;
    if ((instruction >> 5) & 0x1)
    {
        uint16_t param = sign_extend(instruction & 0x1F, 5);
        Lc3_WriteReg(cpu, dr, Lc3_ReadReg(cpu, sr1) + param);
        LOG_LN("                        # [R%u <- R%u + %u] = %u", dr, sr1, param, cpu->regs[dr]);
    }
    else
    {
        uint16_t sr2 = instruction & 0x7;
        Lc3_WriteReg(cpu, dr, Lc3_ReadReg(cpu, sr1) + Lc3_ReadReg(cpu, sr2));
        LOG_LN("                        # [R%u <- R%u + R%u] = %u", dr, sr1, sr2, cpu->regs[dr]);
    }
}

void Lc3_OP_And(LC3_CPU_t *cpu)
{
    //  INSTRUCTION FORM
    // |   AND OPCODE  | dr | sr1 | in|    param    |
    // +---+---+---+---+----+-----+---+---+---+-----+
    // | 0 | 1 | 0 | 1 | DR | SR1 | 0 | 0 | 0 | SR2 |
    // +---+---+---+---+----+-----+---+---+---+-----+
    // | 0 | 1 | 0 | 1 | DR | SR1 | 1 |  Inmediate  |
    // +---+---+---+---+----+-----+---+---+---+-----+
    //
    // If in (inmediate) flag is active then the param value is directly added

    uint16_t instruction = FETCH_INSTRUCTION_CPU(cpu);
    uint16_t dr = (instruction >> 9) & 0x7;
    uint16_t sr1 = (instruction >> 6) & 0x7;
    if ((instruction >> 5) & 0x1)
    {
        uint16_t param = sign_extend(instruction & 0x1F, 5);
        Lc3_WriteReg(cpu, dr, Lc3_ReadReg(cpu, sr1) & param);
        LOG_LN("                        # [R%u <- R%u & %u] = %u", dr, sr1, param, cpu->regs[dr]);
    }
    else
    {
        uint16_t sr2 = instruction & 0x7;
        Lc3_WriteReg(cpu, dr, Lc3_ReadReg(cpu, sr1) & Lc3_ReadReg(cpu, sr2));
        LOG_LN("                        # [R%u <- R%u & R%u] = %u", dr, sr1, sr2, cpu->regs[dr]);
    }
}

void Lc3_OP_Not(LC3_CPU_t *cpu)
{
    //  INSTRUCTION FORM
    // |   NOT OPCODE  | dr | sr |               |
    // +---+---+---+---+----+----+---+---+---+---+
    // | 1 | 0 | 0 | 1 | DR | SR | 1 | 1 | 1 | 1 |
    // +---+---+---+---+----+----+---+---+---+---+
    //
    uint16_t instruction = FETCH_INSTRUCTION_CPU(cpu);
    uint16_t dr = (instruction >> 9) & 0x7;
    uint16_t sr = (instruction >> 6) & 0x7;
    Lc3_WriteReg(cpu, dr, ~Lc3_ReadReg(cpu, sr));
    LOG_LN("                        # [R%u <- ~R%u] = 0x%04X", dr, sr, cpu->regs[dr]);
}

void Lc3_OP_ST(LC3_CPU_t *cpu)
{
    //  INSTRUCTION FORM
    // |   ST OPCODE   | sr |           |
    // +---+---+---+---+----+---+---+---+
    // | 1 | 0 | 0 | 1 | SR | PCoffset9 |
    // +---+---+---+---+----+---+---+---+
    //
    uint16_t instruction = FETCH_INSTRUCTION_CPU(cpu);
    uint16_t sr = (instruction >> 9) & 0x7;
    uint16_t pc_offset = sign_extend(instruction & 0x1FF, 9U) + Lc3_ReadPC(cpu) + 1U;  // We add 1 for the next fetch status
    uint16_t mem = Lc3_ReadReg(cpu, sr);
    Lc3_WriteMem(cpu, pc_offset, mem);
    LOG_LN("                        # [R%u -> $0x%04X] = 0x%04X", sr, pc_offset, mem);
}

void Lc3_OP_STI(LC3_CPU_t *cpu)
{
    //  INSTRUCTION FORM
    // |  STI OPCODE   | sr |           |
    // +---+---+---+---+----+---+---+---+
    // | 1 | 0 | 1 | 1 | SR | PCoffset9 |
    // +---+---+---+---+----+---+---+---+
    //
    uint16_t instruction = FETCH_INSTRUCTION_CPU(cpu);
    uint16_t sr = (instruction >> 9) & 0x7;
    uint16_t pc_offset = sign_extend(instruction & 0x1FF, 9U) + Lc3_ReadPC(cpu) + 1U;  // We add 1 for the next fetch status
    uint16_t mem = Lc3_ReadReg(cpu, sr);
    uint16_t mem_target = Lc3_ReadMem(cpu, pc_offset);
    Lc3_WriteMem(cpu, mem_target, mem);
    LOG_LN("                        # [R%u -> $0x%04X] = 0x%04X", sr, mem_target, mem);
}

void Lc3_OP_STR(LC3_CPU_t *cpu)
{
    //  INSTRUCTION FORM
    // |  STR OPCODE   |    |       |           |
    // +---+---+---+---+----+---+---+---+---+---+
    // | 0 | 1 | 1 | 1 | SR | BaseR | offset6   |
    // +---+---+---+---+----+---+---+---+---+---+
    //
    uint16_t instruction = FETCH_INSTRUCTION_CPU(cpu);
    uint16_t sr = (instruction >> 9) & 0x7;
    uint16_t base = (instruction >> 6) & 0x7;
    uint16_t offset = sign_extend(instruction & 0x3F, 6U);
    Lc3_WriteMem(cpu, base + offset, Lc3_ReadReg(cpu, sr));
    LOG_LN("                        # [R%u -> $0x%04X] = 0x%04X", sr, base + offset, Lc3_ReadReg(cpu, sr));
}

void Lc3_OP_Trap(LC3_CPU_t *cpu)
{
    //  INSTRUCTION FORM
    // |  TRAP OPCODE  |               | trap vector |
    // +---+---+---+---+---+---+---+---+-------------+
    // | 1 | 1 | 1 | 1 | 0 | 0 | 0 | 0 |   vector8   |
    // +---+---+---+---+---+---+---+---+-------------+
    //
    uint16_t instruction = FETCH_INSTRUCTION_CPU(cpu);
    uint16_t trp_v = instruction & 0xFF;

    // First load PC incremented to R7
    Lc3_WriteReg(cpu, REG_R7, Lc3_ReadPC(cpu) + 1U);
    switch (trp_v)
    {
    case TRAP_GETC:
        break;
    case TRAP_OUT:
        break;
    case TRAP_PUTS:
        break;
    case TRAP_IN:
        break;
    case TRAP_PUTSP:
        break;
    case TRAP_HALT:
        break;
    }
}

void Lc3_CPU_UpdateCCReg(LC3_CPU_t *cpu, Lc3Registers_e reg)
{
    if (cpu->regs[reg] == 0)
    {
        cpu->regs[REG_CC] = CC_Z;
    }
    else if (cpu->regs[reg] >> 15)
    {
        cpu->regs[REG_CC] = CC_N;
    }
    else
    {
        cpu->regs[REG_CC] = CC_P;
    }
}
