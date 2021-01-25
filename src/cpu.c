#include "cpu.h"

#include <stdio.h>

#include "log.h"
#include "obj_loader.h"
#include "utils.h"

#define FETCH_INSTRUCTION_CPU(__cpu) (__cpu->memory[__cpu->regs[REG_PC]])

void Lc3_CPU_UpdateCCReg(LC3_CPU_t *cpu, Lc3Registers_e reg);

void Lc3_OP_Add(LC3_CPU_t *cpu);
void Lc3_OP_And(LC3_CPU_t *cpu);
void Lc3_OP_Not(LC3_CPU_t *cpu);
void Lc3_OP_ST(LC3_CPU_t *cpu);
void Lc3_OP_STI(LC3_CPU_t *cpu);
void Lc3_OP_STR(LC3_CPU_t *cpu);
void Lc3_OP_JMP(LC3_CPU_t *cpu);
void Lc3_OP_JSR(LC3_CPU_t *cpu);
void Lc3_OP_LD(LC3_CPU_t *cpu);
void Lc3_OP_LDI(LC3_CPU_t *cpu);
void Lc3_OP_LDR(LC3_CPU_t *cpu);
void Lc3_OP_LEA(LC3_CPU_t *cpu);
void Lc3_OP_BR(LC3_CPU_t *cpu);
uint8_t Lc3_OP_Trap(LC3_CPU_t *cpu);

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
}

uint16_t Lc3_ReadReg(LC3_CPU_t *cpu, Lc3Registers_e reg)
{
    return cpu->regs[reg];
}

uint16_t Lc3_ReadPC(LC3_CPU_t *cpu)
{
    return cpu->regs[REG_PC];
}

uint8_t Lc3_CC_isPositive(LC3_CPU_t *cpu)
{
    return cpu->regs[REG_CC] == CC_P;
}

uint8_t Lc3_CC_isZero(LC3_CPU_t *cpu)
{
    return cpu->regs[REG_CC] == CC_Z;
}

uint8_t Lc3_CC_isNegative(LC3_CPU_t *cpu)
{
    return cpu->regs[REG_CC] == CC_N;
}

uint8_t Lc3_initCPU(LC3_CPU_t *cpu, const char *filename)
{
    LOG_LN("Initializing CPU");
    if (!loadObjFile(cpu, filename))
    {
        LOG_LN("Can't load objfile, exit...");
        return 0;
    }

    Lc3_WriteReg(cpu, REG_PC, PC_START_ADDRESS);
    LOG_LN("Set CPU Program counter to 0x%04X", cpu->regs[REG_PC]);
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
        printf("BR   ");
        Lc3_OP_BR(cpu);
        break;
    case OP_ADD:
        printf("ADD  ");
        Lc3_OP_Add(cpu);
        break;
    case OP_LD:
        printf("LD   ");
        Lc3_OP_LD(cpu);
        break;
    case OP_ST:
        printf("ST   ");
        Lc3_OP_ST(cpu);
        break;
    case OP_JSR:
        printf("JSR  ");
        Lc3_OP_JSR(cpu);
        break;
    case OP_AND:
        printf("AND  ");
        Lc3_OP_And(cpu);
        break;
    case OP_LDR:
        printf("LDR  ");
        Lc3_OP_LDR(cpu);
        break;
    case OP_STR:
        printf("STR  ");
        Lc3_OP_STR(cpu);
        break;
    case OP_JSRR:
        printf("JSRR ");
        Lc3_OP_JSR(cpu);
        break;
    case OP_NOT:
        printf("NOT  ");
        Lc3_OP_Not(cpu);
        break;
    case OP_LDI:
        printf("LDI  ");
        Lc3_OP_LDI(cpu);
        break;
    case OP_STI:
        printf("STI  ");
        Lc3_OP_STI(cpu);
        break;
    case OP_JMP:
        printf("JMP  ");
        Lc3_OP_JMP(cpu);
        break;
    case OP_RES:
        printf("RES ERROR Invalid opcode\n");
        return 0;
    case OP_LEA:
        printf("LEA  ");
        Lc3_OP_LEA(cpu);
        break;
    case OP_TRAP:
        printf("TRAP\n");
        return Lc3_OP_Trap(cpu);
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
        printf(" # [R%u <- R%u + %u] = %u\n", dr, sr1, param, cpu->regs[dr]);
    }
    else
    {
        uint16_t sr2 = instruction & 0x7;
        Lc3_WriteReg(cpu, dr, Lc3_ReadReg(cpu, sr1) + Lc3_ReadReg(cpu, sr2));
        printf(" # [R%u <- R%u + R%u] = %u\n", dr, sr1, sr2, cpu->regs[dr]);
    }
    Lc3_CPU_UpdateCCReg(cpu, dr);
}

void Lc3_OP_And(LC3_CPU_t *cpu)
{
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
        printf(" # [R%u <- R%u & %u] = %u\n", dr, sr1, param, cpu->regs[dr]);
    }
    else
    {
        uint16_t sr2 = instruction & 0x7;
        Lc3_WriteReg(cpu, dr, Lc3_ReadReg(cpu, sr1) & Lc3_ReadReg(cpu, sr2));
        printf(" # [R%u <- R%u & R%u] = %u\n", dr, sr1, sr2, cpu->regs[dr]);
    }
    Lc3_CPU_UpdateCCReg(cpu, dr);
}

void Lc3_OP_Not(LC3_CPU_t *cpu)
{
    // |   NOT OPCODE  | dr | sr |               |
    // +---+---+---+---+----+----+---+---+---+---+
    // | 1 | 0 | 0 | 1 | DR | SR | 1 | 1 | 1 | 1 |
    // +---+---+---+---+----+----+---+---+---+---+
    //
    uint16_t instruction = FETCH_INSTRUCTION_CPU(cpu);
    uint16_t dr = (instruction >> 9) & 0x7;
    uint16_t sr = (instruction >> 6) & 0x7;
    Lc3_WriteReg(cpu, dr, ~Lc3_ReadReg(cpu, sr));
    printf(" # [R%u <- ~R%u] = 0x%04X\n", dr, sr, cpu->regs[dr]);
    Lc3_CPU_UpdateCCReg(cpu, dr);
}

void Lc3_OP_ST(LC3_CPU_t *cpu)
{
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
    printf(" # [R%u -> $0x%04X] = 0x%04X\n", sr, pc_offset, mem);
}

void Lc3_OP_STI(LC3_CPU_t *cpu)
{
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
    printf(" # [R%u -> $0x%04X] = 0x%04X\n", sr, mem_target, mem);
}

void Lc3_OP_STR(LC3_CPU_t *cpu)
{
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
    printf(" # [R%u -> $0x%04X] = 0x%04X\n", sr, base + offset, Lc3_ReadReg(cpu, sr));
}

void Lc3_OP_JMP(LC3_CPU_t *cpu)
{
    // |  JMP OPCODE   |           |           |                       |
    // +---+---+---+---+---+---+---+-----------+---+---+---+---+---+---+
    // | 1 | 1 | 0 | 0 | 0 | 0 | 0 | BaseR     | 0 | 0 | 0 | 0 | 0 | 0 |
    // +---+---+---+---+---+---+---+-----------+---+---+---+---+---+---+
    // | 1 | 1 | 0 | 0 | 0 | 0 | 0 | 1 | 1 | 1 | 0 | 0 | 0 | 0 | 0 | 0 | -> RET
    // +---+---+---+---+---+---+---+-----------+---+---+---+---+---+---+
    //
    uint16_t instruction = FETCH_INSTRUCTION_CPU(cpu);
    uint16_t reg = (instruction >> 6) & 0x7;
    Lc3_WriteReg(cpu, REG_PC, Lc3_ReadReg(cpu, reg));
    printf(" # [PC <- $0x%04X]\n", Lc3_ReadPC(cpu));
}

void Lc3_OP_JSR(LC3_CPU_t *cpu)
{
    // |  JSR OPCODE   | J |       |       |                       |
    // +---+---+---+---+---+---+---+-------+---+---+---+---+---+---+
    // | 0 | 1 | 0 | 0 | 1 |              PCOffset11               |
    // +---+---+---+---+---+---+---+-------+---+---+---+---+---+---+
    // | 0 | 1 | 0 | 0 | 0 | 0 | 0 | BaseR | 0 | 0 | 0 | 0 | 0 | 0 |
    // +---+---+---+---+---+---+---+-------+---+---+---+---+---+---+
    //
    uint16_t instruction = FETCH_INSTRUCTION_CPU(cpu);
    Lc3_WriteReg(cpu, REG_R7, Lc3_ReadPC(cpu));

    if ((instruction >> 11) & 0b1)
    {
        uint16_t pc_offset = instruction & 0x3FF;
        Lc3_WriteReg(cpu, REG_PC, Lc3_ReadPC(cpu) + sign_extend(pc_offset, 11U));
    }
    else
    {
        uint8_t br = (instruction >> 6) & 0x7;
        Lc3_WriteReg(cpu, REG_PC, Lc3_ReadReg(cpu, br));
    }
    printf(" # [PC <- $0x%04X]\n", Lc3_ReadPC(cpu));
}

void Lc3_OP_LD(LC3_CPU_t *cpu)
{
    // |   LD OPCODE   |    |           |
    // +---+---+---+---+----+-----------+
    // | 0 | 0 | 1 | 0 | DR | PCOffset9 |
    // +---+---+---+---+----+-----------+
    uint16_t instruction = FETCH_INSTRUCTION_CPU(cpu);
    uint8_t reg = (instruction >> 9) & 0x7;
    uint16_t mem = Lc3_ReadMem(cpu, sign_extend(instruction & 0xFF, 8) + Lc3_ReadPC(cpu) + 1U);
    Lc3_WriteReg(cpu, reg, mem);
    printf(" # [R%u <- $0x%04X]\n", reg, mem);
    Lc3_CPU_UpdateCCReg(cpu, reg);
}

void Lc3_OP_LDI(LC3_CPU_t *cpu)
{
    // |   LDI OPCODE  |    |           |
    // +---+---+---+---+----+-----------+
    // | 1 | 0 | 1 | 0 | DR | PCOffset9 |
    // +---+---+---+---+----+-----------+
    uint16_t instruction = FETCH_INSTRUCTION_CPU(cpu);
    uint8_t reg = (instruction >> 9) & 0x7;
    uint16_t i_mem = Lc3_ReadMem(cpu, sign_extend(instruction & 0xFF, 8) + Lc3_ReadPC(cpu) + 1U);
    uint16_t mem = Lc3_ReadMem(cpu, i_mem);
    Lc3_WriteReg(cpu, reg, mem);
    printf(" # [R%u <- $0x%04X]\n", reg, mem);
    Lc3_CPU_UpdateCCReg(cpu, reg);
}

void Lc3_OP_LDR(LC3_CPU_t *cpu)
{
    // |   LDR OPCODE  |    |       |         |
    // +---+---+---+---+----+-----------------+
    // | 0 | 1 | 1 | 0 | DR | BaseR | Offset6 |
    // +---+---+---+---+----+-----------------+
    uint16_t instruction = FETCH_INSTRUCTION_CPU(cpu);
    uint8_t reg = (instruction >> 9) & 0x7;
    uint8_t base = (instruction >> 6) & 0x7;
    uint8_t offset = sign_extend(instruction & 0x3F, 6U);
    uint16_t mem = Lc3_ReadMem(cpu, Lc3_ReadReg(cpu, base) + offset);
    Lc3_WriteReg(cpu, reg, mem);
    printf(" # [R%u <- $0x%04X]\n", reg, mem);
    Lc3_CPU_UpdateCCReg(cpu, reg);
}

void Lc3_OP_LEA(LC3_CPU_t *cpu)
{
    // |   LEA OPCODE  |    |           |
    // +---+---+---+---+----+-----------+
    // | 1 | 1 | 1 | 0 | DR | PCOffset9 |
    // +---+---+---+---+----+-----------+
    uint16_t instruction = FETCH_INSTRUCTION_CPU(cpu);
    uint8_t reg = (instruction >> 9) & 0x7;
    uint8_t offset = sign_extend(instruction & 0xFF, 8U);
    Lc3_WriteReg(cpu, reg, offset + Lc3_ReadPC(cpu) + 1U);
    printf(" # [R%u <- $0x%04X]\n", reg, Lc3_ReadReg(cpu, reg));
    Lc3_CPU_UpdateCCReg(cpu, reg);
}

void Lc3_OP_BR(LC3_CPU_t *cpu)
{
    // |   BR OPCODE   |   |   |   |           |
    // +---+---+---+---+---+---+---+-----------+
    // | 0 | 0 | 0 | 0 | n | z | p | PCOffset9 |
    // +---+---+---+---+---+---+---+-----------+
    uint16_t instruction = FETCH_INSTRUCTION_CPU(cpu);
    uint8_t n = ((instruction >> 11) & 0b1) && Lc3_CC_isNegative(cpu);
    uint8_t z = ((instruction >> 10) & 0b1) && Lc3_CC_isZero(cpu);
    uint8_t p = ((instruction >> 9) & 0b1) && Lc3_CC_isPositive(cpu);
    if (n || z || p)
    {
        Lc3_WriteReg(cpu, REG_PC, sign_extend(instruction & 0xFF, 8U) + Lc3_ReadPC(cpu) + 1U);
        printf(" # [BR -> $0x%04X]\n", Lc3_ReadPC(cpu));
    }
    else
    {
        printf(" # [Not allowed]\n");
    }
}

uint8_t Lc3_OP_Trap(LC3_CPU_t *cpu)
{
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
        return 1;
    case TRAP_OUT:
        return 1;
    case TRAP_PUTS:
        return 1;
    case TRAP_IN:
        return 1;
    case TRAP_PUTSP:
        return 1;
    case TRAP_HALT:
        return 0;
    }
    return 0;
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
