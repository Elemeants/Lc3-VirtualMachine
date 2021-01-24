#include <stdio.h>

#include "cpu.h"
#include "log.h"
#include "obj_loader.h"

LC3_CPU_t cpu;
char *filename = NULL;

int main(int argc, char const *argv[])
{
    LOG_LN("Hello world! This Virtual machine for Lc3 is been building...");

    if (argc < 2)
    {
        LOG_LN("Usage: lc3vm [obj-file]");
        return 1;
    }

    if (!Lc3_initCPU(&cpu, argv[1]))
    {
        return 2;
    }

    while (1)
    {
        uint8_t status = Lc3_execInstruction(&cpu);
        if (!status)
        {
            break;
        }
        Lc3_fetchNext(&cpu);
    }

    return 0;
}
