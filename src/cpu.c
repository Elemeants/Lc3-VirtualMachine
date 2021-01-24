#include "cpu.h"

#include <stdio.h>

#include "log.h"
#include "obj_loader.h"

uint8_t Lc3_initCPU(LC3_CPU_t *cpu, const char *filename)
{
    LOG_LN("Initializing CPU");
    if (!loadObjFile(cpu, filename))
    {
        LOG_LN("Can't load objfile, exit...");
        return 0;
    }

    cpu->regs[REG_PC] = PC_START_ADDRESS;
    LOG_LN("Set CPU Program counter to 0x%04X", cpu->regs[REG_PC]);
    return 1;
}

uint8_t Lc3_execInstruction(LC3_CPU_t *cpu)
{
    uint16_t instruction = cpu->memory[cpu->regs[REG_PC]];
    Lc3Opcodes_e opcode = (Lc3Opcodes_e)((instruction & 0xF000) >> 12);
    LOG(" PC: 0x%04X [0x%04X] -> ", cpu->regs[REG_PC], instruction);
    switch (opcode)
    {
    case OP_BR:
        printf("BR\n");
        break;
    case OP_ADD:
        printf("ADD\n");
        break;
    case OP_LD:
        printf("LD\n");
        break;
    case OP_ST:
        printf("ST\n");
        break;
    case OP_JSR:
        printf("JSR\n");
        break;
    case OP_AND:
        printf("AND\n");
        break;
    case OP_LDR:
        printf("LDR\n");
        break;
    case OP_STR:
        printf("STR\n");
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
        break;
    case OP_JMP:
        printf("JMP\n");
        break;
    case OP_RES:
        return 0;
        printf("RES\n");
        break;
    case OP_LEA:
        printf("LEA\n");
        break;
    case OP_TRAP:
        printf("TRAP\n");
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