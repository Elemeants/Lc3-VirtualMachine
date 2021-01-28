#include "cpu.h"

#include <stdio.h>

#include "console.h"
#include "log.h"
#include "obj_loader.h"
#include "utils.h"

/**
 * @brief Returns the actual instruction on the CPU
 * 
 */
#define FETCH_INSTRUCTION_CPU(__cpu) (__cpu->memory[__cpu->regs[REG_PC]])

/**
 * @brief [INTERNAL] Updates the conditional control register of the cpu by a register
 * 
 * @param cpu pointer to the cpu instance
 * @param reg on this register will be updated the CC register
 */
void Lc3_CPU_UpdateCCReg(LC3_CPU_t *cpu, Lc3Registers_e reg);

/**
 * @brief [INTERNAL] Executes the opcode ADD
 * 
 * @param cpu pointer to the cpu instance
 */
void Lc3_OP_Add(LC3_CPU_t *cpu);

/**
 * @brief [INTERNAL] Executes the opcode AND
 * 
 * @param cpu pointer to the cpu instance
 */
void Lc3_OP_And(LC3_CPU_t *cpu);

/**
 * @brief [INTERNAL] Executes the opcode NOT
 * 
 * @param cpu pointer to the cpu instance
 */
void Lc3_OP_Not(LC3_CPU_t *cpu);

/**
 * @brief [INTERNAL] Executes the opcode ST
 * 
 * @param cpu pointer to the cpu instance
 */
void Lc3_OP_ST(LC3_CPU_t *cpu);

/**
 * @brief [INTERNAL] Executes the opcode STI
 * 
 * @param cpu pointer to the cpu instance
 */
void Lc3_OP_STI(LC3_CPU_t *cpu);

/**
 * @brief [INTERNAL] Executes the opcode STR
 * 
 * @param cpu pointer to the cpu instance
 */
void Lc3_OP_STR(LC3_CPU_t *cpu);

/**
 * @brief [INTERNAL] Executes the opcode JMP
 * 
 * @param cpu pointer to the cpu instance
 */
void Lc3_OP_JMP(LC3_CPU_t *cpu);

/**
 * @brief [INTERNAL] Executes the opcode JSR
 * 
 * @param cpu pointer to the cpu instance
 */
void Lc3_OP_JSR(LC3_CPU_t *cpu);

/**
 * @brief [INTERNAL] Executes the opcode LD
 * 
 * @param cpu pointer to the cpu instance
 */
void Lc3_OP_LD(LC3_CPU_t *cpu);

/**
 * @brief [INTERNAL] Executes the opcode LDI
 * 
 * @param cpu pointer to the cpu instance
 */
void Lc3_OP_LDI(LC3_CPU_t *cpu);

/**
 * @brief [INTERNAL] Executes the opcode LDR
 * 
 * @param cpu pointer to the cpu instance
 */
void Lc3_OP_LDR(LC3_CPU_t *cpu);

/**
 * @brief [INTERNAL] Executes the opcode LEA
 * 
 * @param cpu pointer to the cpu instance
 */
void Lc3_OP_LEA(LC3_CPU_t *cpu);

/**
 * @brief [INTERNAL] Executes the opcode BR
 * 
 * @param cpu pointer to the cpu instance
 */
void Lc3_OP_BR(LC3_CPU_t *cpu);

/**
 * @brief [INTERNAL] Executes the opcode TRAP
 * 
 * @param cpu pointer to the cpu instance
 * @return uint8_t because this opcode can stop the execution
 * returns 1 if execution can still run, eiter 0.
 */
uint8_t Lc3_OP_Trap(LC3_CPU_t *cpu);

void Lc3_WriteMem(LC3_CPU_t *cpu, uint16_t addr, uint16_t value)
{
    cpu->memory[addr] = value;
}

uint16_t Lc3_ReadMem(LC3_CPU_t *cpu, uint16_t addr)
{
    if (addr == MMR_KBSR)
    {
        if (OSKeyboardIsKeyPressed())
        {
            cpu->memory[MMR_KBSR] = (1 << 15);
            cpu->memory[MMR_KBDR] = getchar();
            LOG_TXT(" KeyboardEvent ");
        }
        else
        {
            cpu->memory[MMR_KBSR] = 0;
        }
    }
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
        return 0;  // If can't load stop the program
    }

    Lc3_WriteReg(cpu, REG_PC, PC_START_ADDRESS);
    LOG_LN("Set CPU Program counter to 0x%04X", cpu->regs[REG_PC]);
    return 1;
}

uint8_t Lc3_execInstruction(LC3_CPU_t *cpu)
{
    // TODO: Can optimize this function...
    uint16_t instruction = FETCH_INSTRUCTION_CPU(cpu);
    Lc3Opcodes_e opcode = (Lc3Opcodes_e)((instruction & 0xF000) >> 12);
    LOG(" PC: 0x%04X [0x%04X] -> ", cpu->regs[REG_PC], instruction);
    switch (opcode)
    {
    case OP_BR:
        LOG_TXT("BR   ");
        Lc3_OP_BR(cpu);
        break;
    case OP_ADD:
        LOG_TXT("ADD  ");
        Lc3_OP_Add(cpu);
        break;
    case OP_LD:
        LOG_TXT("LD   ");
        Lc3_OP_LD(cpu);
        break;
    case OP_ST:
        LOG_TXT("ST   ");
        Lc3_OP_ST(cpu);
        break;
    case OP_JSR:
        LOG_TXT("JSR  ");
        Lc3_OP_JSR(cpu);
        break;
    case OP_AND:
        LOG_TXT("AND  ");
        Lc3_OP_And(cpu);
        break;
    case OP_LDR:
        LOG_TXT("LDR  ");
        Lc3_OP_LDR(cpu);
        break;
    case OP_STR:
        LOG_TXT("STR  ");
        Lc3_OP_STR(cpu);
        break;
    case OP_JSRR:
        LOG_TXT("JSRR ");
        Lc3_OP_JSR(cpu);
        break;
    case OP_NOT:
        LOG_TXT("NOT  ");
        Lc3_OP_Not(cpu);
        break;
    case OP_LDI:
        LOG_TXT("LDI  ");
        Lc3_OP_LDI(cpu);
        break;
    case OP_STI:
        LOG_TXT("STI  ");
        Lc3_OP_STI(cpu);
        break;
    case OP_JMP:
        LOG_TXT("JMP  ");
        Lc3_OP_JMP(cpu);
        break;
    case OP_RES:
        // RES Opcode is reserved so throw a error message and stop the program
        LOG_TXT("RES ERROR Invalid opcode\n");
        return 0;
    case OP_LEA:
        LOG_TXT("LEA  ");
        Lc3_OP_LEA(cpu);
        break;
    case OP_TRAP:
        LOG_TXT("TRAP ");
        return Lc3_OP_Trap(cpu);
    default:
        LOG_TXT("ERROR Invalid opcode 0x%X", (uint8_t)opcode);
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
    if ((instruction >> 5) & 0x1)  // bit inmediate
    {
        uint16_t param = sign_extend(instruction & 0x1F, 5);
        Lc3_WriteReg(cpu, dr, Lc3_ReadReg(cpu, sr1) + param);
        LOG_TXT(" # [R%u <- R%u + 0x%04X] = 0x%04X\n", dr, sr1, param, cpu->regs[dr]);
    }
    else
    {
        uint16_t sr2 = instruction & 0x7;
        Lc3_WriteReg(cpu, dr, Lc3_ReadReg(cpu, sr1) + Lc3_ReadReg(cpu, sr2));
        LOG_TXT(" # [R%u <- R%u + R%u] = 0x%04X\n", dr, sr1, sr2, cpu->regs[dr]);
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
    if ((instruction >> 5) & 0x1)  // bit inmediate
    {
        uint16_t param = sign_extend(instruction & 0x1F, 5);
        Lc3_WriteReg(cpu, dr, Lc3_ReadReg(cpu, sr1) & param);
        LOG_TXT(" # [R%u <- R%u & 0x%04X] = 0x%04X\n", dr, sr1, param, cpu->regs[dr]);
    }
    else
    {
        uint16_t sr2 = instruction & 0x7;
        Lc3_WriteReg(cpu, dr, Lc3_ReadReg(cpu, sr1) & Lc3_ReadReg(cpu, sr2));
        LOG_TXT(" # [R%u <- R%u & R%u] = 0x%04X\n", dr, sr1, sr2, cpu->regs[dr]);
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
    LOG_TXT(" # [R%u <- ~R%u] = 0x%04X\n", dr, sr, cpu->regs[dr]);
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
    uint16_t pc_offset = sign_extend(instruction & 0x1FF, 9U) + Lc3_ReadPC(cpu) + 1u;
    uint16_t mem = Lc3_ReadReg(cpu, sr);
    Lc3_WriteMem(cpu, pc_offset, mem);
    LOG_TXT(" # [R%u -> $0x%04X] = 0x%04X\n", sr, pc_offset, mem);
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
    uint16_t pc_offset = sign_extend(instruction & 0x1FF, 9U) + Lc3_ReadPC(cpu);
    uint16_t mem = Lc3_ReadReg(cpu, sr);
    uint16_t mem_target = Lc3_ReadMem(cpu, pc_offset);
    Lc3_WriteMem(cpu, mem_target, mem);
    LOG_TXT(" # [R%u -> $0x%04X] = 0x%04X\n", sr, mem_target, mem);
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
    uint16_t base = Lc3_ReadReg(cpu, (instruction >> 6) & 0x7);
    uint16_t offset = sign_extend(instruction & 0x3F, 6U);
    uint16_t addr = base + offset;
    Lc3_WriteMem(cpu, addr, Lc3_ReadReg(cpu, sr));
    LOG_TXT(" # [R%u -> $0x%04X] = 0x%04X\n", sr, addr, Lc3_ReadReg(cpu, sr));
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
    Lc3_WriteReg(cpu, REG_PC, Lc3_ReadReg(cpu, reg));  // This -1 is because the next line is fetch next.
    if (reg == 0x7)                                    // In case RET opcode,
    {
        LOG_TXT(" # [PC <- $0x%04X R7]\n", Lc3_ReadPC(cpu));
    }
    else
    {
        // Because we increment PC finishing this function, we need to decrement by 1 the PC register now
        Lc3_WriteReg(cpu, REG_PC, Lc3_ReadPC(cpu) - 1U);
        LOG_TXT(" # [PC <- $0x%04X]\n", Lc3_ReadPC(cpu));
    }
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

    if ((instruction >> 11) & 0b1) // bit 11 | 0 = JSRR | 1 = JSR
    {
        uint16_t pc_offset = instruction & 0x3FF;
        Lc3_WriteReg(cpu, REG_PC, Lc3_ReadPC(cpu) + sign_extend(pc_offset, 11U));
    }
    else
    {
        uint8_t br = (instruction >> 6) & 0x7;
        Lc3_WriteReg(cpu, REG_PC, Lc3_ReadReg(cpu, br));
    }
    Lc3_WriteReg(cpu, REG_PC, Lc3_ReadPC(cpu));
    LOG_TXT(" # [PC <- $0x%04X | R7 <- $0x%04X]\n", Lc3_ReadPC(cpu), Lc3_ReadReg(cpu, REG_R7));
}

void Lc3_OP_LD(LC3_CPU_t *cpu)
{
    // |   LD OPCODE   |    |           |
    // +---+---+---+---+----+-----------+
    // | 0 | 0 | 1 | 0 | DR | PCOffset9 |
    // +---+---+---+---+----+-----------+
    uint16_t instruction = FETCH_INSTRUCTION_CPU(cpu);
    uint8_t reg = (instruction >> 9) & 0x7;
    uint16_t mem = Lc3_ReadMem(cpu, sign_extend(instruction & 0xFF, 8U) + Lc3_ReadPC(cpu) + 1);
    Lc3_WriteReg(cpu, reg, mem);
    LOG_TXT(" # [R%u <- 0x%04X]\n", reg, mem);
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
    uint16_t addr = sign_extend(instruction & 0xFF, 8) + Lc3_ReadPC(cpu) + 1U;
    uint16_t i_mem = Lc3_ReadMem(cpu, addr);
    uint16_t mem = Lc3_ReadMem(cpu, i_mem);
    Lc3_WriteReg(cpu, reg, mem);
    LOG_TXT(" # [R%u <- 0x%04X]\n", reg, mem);
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
    uint16_t addr = Lc3_ReadReg(cpu, base) + offset;
    uint16_t mem = Lc3_ReadMem(cpu, addr);
    Lc3_WriteReg(cpu, reg, mem);
    LOG_TXT(" # [R%u <- 0x%04X ($0x%04X)]\n", reg, mem, addr);
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
    LOG_TXT(" # [R%u <- 0x%04X]\n", reg, Lc3_ReadReg(cpu, reg));
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
        Lc3_WriteReg(cpu, REG_PC, sign_extend(instruction & 0xFF, 8U) + Lc3_ReadPC(cpu));
        LOG_TXT(" # [BR -> $0x%04X]\n", Lc3_ReadPC(cpu));
    }
    else
    {
        LOG_TXT(" # [Not allowed, BR -> $0x%04X]\n", Lc3_ReadPC(cpu));
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
    uint16_t tmp;
    uint8_t stop = 0x00;

    // First load PC incremented to R7
    Lc3_WriteReg(cpu, REG_R7, Lc3_ReadPC(cpu));
    switch (trp_v)
    {
    case TRAP_GETC:
        tmp = getchar();
        Lc3_WriteReg(cpu, REG_R0, tmp);
        LOG_TXT(" # [GETC key: %u]\n", Lc3_ReadReg(cpu, REG_R0));
        break;
    case TRAP_OUT:
        putc((char)Lc3_ReadReg(cpu, REG_R0), stdout);
        LOG_TXT(" # [OUT key: %u]\n", Lc3_ReadReg(cpu, REG_R0));
        break;
    case TRAP_PUTS:
        tmp = Lc3_ReadReg(cpu, REG_R0);
        while (1)
        {
            char mem = Lc3_ReadMem(cpu, tmp);
            if (mem == 0x0000)
            {
                break;
            }
            putc(mem, stdout);
            tmp++;
        }
        LOG_TXT(" # [PUTS]\n");
        break;
    case TRAP_IN:
        tmp = getchar();
        Lc3_WriteReg(cpu, REG_R0, tmp);
        putc(Lc3_ReadReg(cpu, REG_R0), stdout);
        LOG_TXT(" # [IN key: %u]\n", Lc3_ReadReg(cpu, REG_R0));
        break;
    case TRAP_PUTSP:
        tmp = Lc3_ReadReg(cpu, REG_R0);
        LOG_TXT(" # [PUTSP]\n");
        while (1)
        {
            uint16_t mem = Lc3_ReadMem(cpu, tmp);
            if (((mem & 0xFF) == 0x00) || ((mem >> 8) == 0x00))
            {
                break;
            }
            putc((char)(mem & 0xFF), stdout);
            putc((char)(mem >> 8), stdout);
            tmp++;
        }
        break;
    case TRAP_HALT:
        stop = 0x1;
        LOG_TXT(" # [HALT]\n");
        break;
    }
    fflush(stdout);
    Lc3_WriteReg(cpu, REG_PC, Lc3_ReadReg(cpu, REG_R7));
    return !stop;
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
