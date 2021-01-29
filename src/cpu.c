#include "cpu.h"

#include <stdio.h>
#include <stdlib.h>

#include "console.h"
#include "firmware.h"
#include "log.h"
#include "utils.h"

LC3OpcodeAction_t LC3Opcodes[OP_COUNT] =
    {
        {"BR  ", OP_BR, LC3Inst_br},
        {"ADD ", OP_ADD, LC3Inst_add},
        {"LD  ", OP_LD, LC3Inst_ld},
        {"ST  ", OP_ST, LC3Inst_st},
        {"JSR ", OP_JSR, LC3Inst_jsr},
        {"AND ", OP_AND, LC3Inst_and},
        {"LDR ", OP_LDR, LC3Inst_ldr},
        {"STR ", OP_STR, LC3Inst_str},
        {"JSRR", OP_JSRR, LC3Inst_jsr},
        {"NOT ", OP_NOT, LC3Inst_not},
        {"LDI ", OP_LDI, LC3Inst_ldi},
        {"STI ", OP_STI, LC3Inst_sti},
        {"JMP ", OP_JMP, LC3Inst_jmp},
        {"RES ", OP_RES, NULL},
        {"LEA ", OP_LEA, LC3Inst_lea},
        {"TRAP", OP_TRAP, LC3Inst_trap}};

uint8_t LC3CpuInit(LC3Cpu_t *cpu, LC3Firmware_t *firmware)
{
    LOG_LN("Initializing CPU");
    cpu->PC = PC_START_ADDRESS;
    cpu->CC = 0b000;
    cpu->stat.incrementPC = 0b1;
    cpu->stat.running = 0b1;
    if (!firmware)
    {
        return EXIT_FAILURE;
    }
    cpu->firmware = firmware;
    return EXIT_SUCCESS;
}

void LC3CpuExecute(LC3Cpu_t *cpu)
{
    while (cpu->stat.running)
    {
        LC3Instruction_t inst = LC3CpuReadInstruction(cpu);
        LOG(" PC: 0x%04X [0x%04X] -> %s ",
            cpu->PC,
            cpu->firmware->memory[cpu->PC],
            LC3Opcodes[inst.opcode].name);
        LC3Opcodes[inst.opcode].action(cpu, inst);
        if (cpu->stat.incrementPC)
        {
            cpu->PC++;
        }
    }
}

LC3Instruction_t LC3CpuReadInstruction(LC3Cpu_t *cpu)
{
    LC3Instruction_t *c = (LC3Instruction_t *)&cpu->firmware->memory[cpu->PC];
    return *c;
}

uint16_t LC3CpuReadMemory(LC3Cpu_t *cpu, uint16_t addr)
{
    if (addr == MMR_KBSR)
    {
        if (OSKeyboardIsKeyPressed())
        {
            cpu->firmware->memory[MMR_KBSR] = (1 << 15);
            cpu->firmware->memory[MMR_KBDR] = getchar();
        }
        else
        {
            cpu->firmware->memory[MMR_KBSR] = 0;
        }
    }
    return cpu->firmware->memory[addr];
}

void LC3CpuWriteMemory(LC3Cpu_t *cpu, uint16_t addr, uint16_t value)
{
    cpu->firmware->memory[addr] = value;
}

void LC3CpuUpdateCCReg(LC3Cpu_t *cpu, uint16_t reg)
{
    cpu->CC = 0b000;  // Clear register
    if (cpu->regs[reg] == 0)
    {
        cpu->CC_Z = 0b1;
    }
    else if (cpu->regs[reg] >> 15)
    {
        cpu->CC_N = 0b1;
    }
    else
    {
        cpu->CC_P = 0b1;
    }
}

void LC3Inst_br(LC3Cpu_t *cpu, LC3Instruction_t inst)
{
    // |   BR OPCODE   |   |   |   |           |
    // +---+---+---+---+---+---+---+-----------+
    // | 0 | 0 | 0 | 0 | n | z | p | PCOffset9 |
    // +---+---+---+---+---+---+---+-----------+
    uint8_t br = READ_BIT(inst.body, 11U) && cpu->CC_N;
    br |= READ_BIT(inst.body, 10U) && cpu->CC_Z;
    br |= READ_BIT(inst.body, 9U) && cpu->CC_P;
    if (br)
    {
        cpu->PC += sign_extend(inst.body & 0x1FF, 9U);
        LOG_TXT("# BR $0x%04X\n", cpu->PC);
    }
    else
    {
        LOG_TXT("# BR $0x%04X xxx\n", cpu->PC);
    }
}

void LC3Inst_add(LC3Cpu_t *cpu, LC3Instruction_t inst)
{
    // |   ADD OPCODE  | dr | sr1 | in|    param    |
    // +---+---+---+---+----+-----+---+---+---+-----+
    // | 0 | 0 | 0 | 1 | DR | SR1 | 0 | 0 | 0 | SR2 |
    // +---+---+---+---+----+-----+---+---+---+-----+
    // | 0 | 0 | 0 | 1 | DR | SR1 | 1 |  Inmediate  |
    // +---+---+---+---+----+-----+---+---+---+-----+
    // If in (inmediate) flag is active then the param value is directly added
    uint16_t dr = READ_3BITS(inst.body, 9U);
    uint16_t sr = READ_3BITS(inst.body, 6U);
    uint16_t v = cpu->regs[sr];
    if ((inst.body >> 5) & 0x1)  // bit inmediate
    {
        uint16_t p = sign_extend(inst.body & 0x1F, 5);
        cpu->regs[dr] = v + p;
        LOG_TXT("# R%u <- R%u + 0x%04X = 0x%04X\n", dr, sr, p, cpu->regs[dr]);
    }
    else
    {
        uint16_t p = cpu->regs[inst.body & 0x7];
        cpu->regs[dr] = v + p;
        LOG_TXT("# R%u <- R%u + R%u = 0x%04X\n", dr, sr, p, cpu->regs[dr]);
    }
    LC3CpuUpdateCCReg(cpu, dr);
}

void LC3Inst_ld(LC3Cpu_t *cpu, LC3Instruction_t inst)
{
    // |    LD OPCODE  |    |           |
    // +---+---+---+---+----+-----------+
    // | 1 | 0 | 1 | 0 | DR | PCOffset9 |
    // +---+---+---+---+----+-----------+
    uint8_t reg = READ_3BITS(inst.body, 9U);
    uint16_t addr = sign_extend(inst.body & 0x1FF, 9U) + cpu->PC + 1U;
    uint16_t mem = LC3CpuReadMemory(cpu, addr);
    cpu->regs[reg] = mem;
    LOG_TXT("# R%u <- $0x%04X = 0x%04X\n", reg, addr, mem);
    LC3CpuUpdateCCReg(cpu, reg);
}

void LC3Inst_st(LC3Cpu_t *cpu, LC3Instruction_t inst)
{
    // |   ST OPCODE   | sr |           |
    // +---+---+---+---+----+---+---+---+
    // | 1 | 0 | 0 | 1 | SR | PCoffset9 |
    // +---+---+---+---+----+---+---+---+
    uint16_t reg = READ_3BITS(inst.body, 9U);
    uint16_t pc_offset = sign_extend(inst.body & 0x1FF, 9U) + cpu->PC + 1u;
    uint16_t mem = cpu->regs[reg];
    LC3CpuWriteMemory(cpu, pc_offset, mem);
    LOG_TXT("# (R%u + offset) -> $0x%04X = 0x%04X\n", reg, pc_offset, mem);
}

void LC3Inst_jsr(LC3Cpu_t *cpu, LC3Instruction_t inst)
{
    // |  JSR OPCODE   | J |       |       |                       |
    // +---+---+---+---+---+---+---+-------+---+---+---+---+---+---+
    // | 0 | 1 | 0 | 0 | 1 |              PCOffset11               |
    // +---+---+---+---+---+---+---+-------+---+---+---+---+---+---+
    // | 0 | 1 | 0 | 0 | 0 | 0 | 0 | BaseR | 0 | 0 | 0 | 0 | 0 | 0 |
    // +---+---+---+---+---+---+---+-------+---+---+---+---+---+---+
    cpu->regs[REG_R7] = cpu->PC;

    if (READ_BIT(inst.body, 11U))  // bit 11 | 0 = JSRR | 1 = JSR
    {
        cpu->PC += sign_extend(inst.body & 0x3FF, 11U);
    }
    else
    {
        cpu->PC = READ_3BITS(inst.body, 6);
    }
    LOG_TXT("# PC <- $0x%04X, R7 <- $0x%04X\n", cpu->PC + 1U, cpu->regs[REG_R7]);
}

void LC3Inst_and(LC3Cpu_t *cpu, LC3Instruction_t inst)
{
    // |   AND OPCODE  | dr | sr1 | in|    param    |
    // +---+---+---+---+----+-----+---+---+---+-----+
    // | 0 | 1 | 0 | 1 | DR | SR1 | 0 | 0 | 0 | SR2 |
    // +---+---+---+---+----+-----+---+---+---+-----+
    // | 0 | 1 | 0 | 1 | DR | SR1 | 1 |  Inmediate  |
    // +---+---+---+---+----+-----+---+---+---+-----+
    // If in (inmediate) flag is active then the param value is directly added

    uint16_t dr = READ_3BITS(inst.body, 9U);
    uint16_t sr = READ_3BITS(inst.body, 6U);
    uint16_t v = cpu->regs[sr];
    if (READ_BIT(inst.body, 5))  // bit inmediate
    {
        uint16_t p = sign_extend(inst.body & 0x1F, 5);
        cpu->regs[dr] = v & p;
        LOG_TXT("# R%u <- R%u & 0x%04X = 0x%04X\n", dr, sr, p, cpu->regs[dr]);
    }
    else
    {
        uint16_t p = cpu->regs[READ_3BITS(inst.body, 0)];
        cpu->regs[dr] = v & p;
        LOG_TXT("# R%u <- R%u & R%u = 0x%04X\n", dr, sr, READ_3BITS(inst.body, 0), cpu->regs[dr]);
    }
    LC3CpuUpdateCCReg(cpu, dr);
}

void LC3Inst_ldr(LC3Cpu_t *cpu, LC3Instruction_t inst)
{
    // |   LDR OPCODE  |    |       |         |
    // +---+---+---+---+----+-----------------+
    // | 0 | 1 | 1 | 0 | DR | BaseR | Offset6 |
    // +---+---+---+---+----+-----------------+
    uint16_t reg = READ_3BITS(inst.body, 9U);
    uint16_t base = READ_3BITS(inst.body, 6U);

    uint8_t offset = sign_extend(inst.body & 0x3F, 6U);
    uint16_t addr = cpu->regs[base] + offset;
    uint16_t mem = LC3CpuReadMemory(cpu, addr);
    cpu->regs[reg] = mem;
    LOG_TXT("# R%u <- $0x%04X = 0x%04X\n", reg, addr, mem);
    LC3CpuUpdateCCReg(cpu, reg);
}

void LC3Inst_str(LC3Cpu_t *cpu, LC3Instruction_t inst)
{
    // |  STR OPCODE   |    |       |           |
    // +---+---+---+---+----+---+---+---+---+---+
    // | 0 | 1 | 1 | 1 | SR | BaseR | offset6   |
    // +---+---+---+---+----+---+---+---+---+---+
    uint16_t sr = READ_3BITS(inst.body, 9U);
    uint16_t addr = cpu->regs[READ_3BITS(inst.body, 6U)];
    addr += sign_extend(inst.body & 0x3F, 6U);
    LC3CpuWriteMemory(cpu, addr, cpu->regs[sr]);
    LOG_TXT("# R%u -> $0x%04X = 0x%04X\n", sr, addr, cpu->regs[sr]);
}

void LC3Inst_not(LC3Cpu_t *cpu, LC3Instruction_t inst)
{
    // |   NOT OPCODE  | dr | sr |               |
    // +---+---+---+---+----+----+---+---+---+---+
    // | 1 | 0 | 0 | 1 | DR | SR | 1 | 1 | 1 | 1 |
    // +---+---+---+---+----+----+---+---+---+---+
    uint16_t dr = READ_3BITS(inst.body, 9U);
    uint16_t sr = READ_3BITS(inst.body, 6U);
    cpu->regs[dr] = ~cpu->regs[sr];
    LOG_TXT("# R%u <- 0x%04X\n", dr, cpu->regs[dr]);
    LC3CpuUpdateCCReg(cpu, dr);
}

void LC3Inst_ldi(LC3Cpu_t *cpu, LC3Instruction_t inst)
{
    // |   LDI OPCODE  |    |           |
    // +---+---+---+---+----+-----------+
    // | 1 | 0 | 1 | 0 | DR | PCOffset9 |
    // +---+---+---+---+----+-----------+
    uint8_t reg = READ_3BITS(inst.body, 9U);
    uint16_t addr = sign_extend(inst.body & 0xFF, 8U) + cpu->PC + 1U;
    uint16_t i_mem = LC3CpuReadMemory(cpu, addr);
    uint16_t mem = LC3CpuReadMemory(cpu, i_mem);
    cpu->regs[reg] = mem;
    LOG_TXT("# R%u <- $0x%04X (0x%04X)\n", reg, i_mem, mem);
    LC3CpuUpdateCCReg(cpu, reg);
}

void LC3Inst_sti(LC3Cpu_t *cpu, LC3Instruction_t inst)
{
    // |  STI OPCODE   | sr |           |
    // +---+---+---+---+----+---+---+---+
    // | 1 | 0 | 1 | 1 | SR | PCoffset9 |
    // +---+---+---+---+----+---+---+---+
    uint16_t sr = READ_3BITS(inst.body, 9U);
    uint16_t pc_offset = sign_extend(inst.body & 0x1FF, 9U) + cpu->PC;
    uint16_t mem = cpu->regs[sr];
    uint16_t mem_target = LC3CpuReadMemory(cpu, pc_offset);
    LC3CpuWriteMemory(cpu, mem_target, mem);
    LOG_TXT("# (R%u + 0x%04X) -> $0x%04X = 0x%04X\n", sr, pc_offset, mem_target, mem);
}

void LC3Inst_jmp(LC3Cpu_t *cpu, LC3Instruction_t inst)
{
    // |  JMP OPCODE   |           |           |                       |
    // +---+---+---+---+---+---+---+-----------+---+---+---+---+---+---+
    // | 1 | 1 | 0 | 0 | 0 | 0 | 0 | BaseR     | 0 | 0 | 0 | 0 | 0 | 0 |
    // +---+---+---+---+---+---+---+-----------+---+---+---+---+---+---+
    // | 1 | 1 | 0 | 0 | 0 | 0 | 0 | 1 | 1 | 1 | 0 | 0 | 0 | 0 | 0 | 0 | -> RET
    // +---+---+---+---+---+---+---+-----------+---+---+---+---+---+---+
    uint16_t reg = READ_3BITS(inst.body, 6U);
    cpu->PC = cpu->regs[reg];
    if (reg == 0x7)  // In case RET opcode,
    {
        LOG_TXT("# PC <- $0x%04X <- R7\n", cpu->PC);
    }
    else
    {
        // Because we increment PC finishing this function, we need to decrement by 1 the PC register now
        LOG_TXT("# PC <- $0x%04X\n", cpu->PC);
        cpu->PC--;
    }
}

void LC3Inst_lea(LC3Cpu_t *cpu, LC3Instruction_t inst)
{
    // |   LEA OPCODE  |    |           |
    // +---+---+---+---+----+-----------+
    // | 1 | 1 | 1 | 0 | DR | PCOffset9 |
    // +---+---+---+---+----+-----------+
    uint8_t reg = READ_3BITS(inst.body, 9U);
    uint8_t offset = sign_extend(inst.body & 0xFF, 8U);
    cpu->regs[reg] = offset + cpu->PC + 1U;
    LOG_TXT("# R%u <- 0x%04X\n", reg, cpu->regs[reg]);
    LC3CpuUpdateCCReg(cpu, reg);
}

void LC3Inst_trap(LC3Cpu_t *cpu, LC3Instruction_t inst)
{
    // |  TRAP OPCODE  |               | trap vector |
    // +---+---+---+---+---+---+---+---+-------------+
    // | 1 | 1 | 1 | 1 | 0 | 0 | 0 | 0 |   vector8   |
    // +---+---+---+---+---+---+---+---+-------------+
    uint16_t _vector = inst.body & 0xFF;
    uint16_t tmp;

    // First load PC incremented to R7
    cpu->regs[REG_R7] = cpu->PC;
    switch (_vector)
    {
    case TRAP_GETC:
        cpu->regs[REG_R0] = getchar();
        LOG_TXT("# GETC key: %u\n", cpu->regs[REG_R0]);
        break;
    case TRAP_OUT:
        LOG_TXT("# OUT key: %u\n", cpu->regs[REG_R0]);
        putc((char)cpu->regs[REG_R0], stdout);
        break;
    case TRAP_PUTS:
        tmp = cpu->regs[REG_R0];
        LOG_TXT("# PUTS: ");
        while (1)
        {
            char mem = LC3CpuReadMemory(cpu, tmp);
            if (mem == 0x00)
            {
                break;
            }
            putc(mem, stdout);
            putc(mem, fileout);  // log
            tmp++;
        }
        LOG_TXT("\n");
        break;
    case TRAP_IN:
        cpu->regs[REG_R0] = getchar();
        putc(cpu->regs[REG_R0], stdout);
        LOG_TXT("# IN key: %u\n", cpu->regs[REG_R0]);
        break;
    case TRAP_PUTSP:
        tmp = cpu->regs[REG_R0];
        LOG_TXT("# PUTSP: ");
        while (1)
        {
            uint16_t mem = LC3CpuReadMemory(cpu, tmp);
            if (((mem & 0xFF) == 0x00) || ((mem >> 8) == 0x00))
            {
                break;
            }
            putc((char)(mem & 0xFF), stdout);
            putc((char)(mem >> 8), stdout);

            putc((char)(mem & 0xFF), fileout);  // log
            putc((char)(mem >> 8), fileout);    // log
            tmp++;
        }
        LOG_TXT("\n");
        break;
    case TRAP_HALT:
        cpu->stat.running = 0b0;
        LOG_TXT("# HALT\n");
        break;
    }
    fflush(stdout);
    cpu->PC = cpu->regs[REG_R7];
}
