#include <stdio.h>

#include "console.h"
#include "cpu.h"
#include "log.h"
#include "obj_loader.h"

LC3_CPU_t cpu;
char *filename = NULL;

int main(int argc, char const *argv[])
{
    printf("Hello world! This Virtual machine for Lc3 is been building...\n");

    if (argc < 2)
    {
        printf("Usage: lc3vm [obj-file]\n");
        return 1;
    }

    Log_init("lc3vm.log");

    OSKeyboardInit();

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
