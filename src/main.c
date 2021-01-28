/**
 * @file main.c
 * @author Daniel Polanco (jdanypa@gmail.com)
 * @version 1.0
 * @date 2021-01-27
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#include <stdio.h>

#include "console.h"
#include "cpu.h"
#include "log.h"
#include "obj_loader.h"

/**
 * @brief Mame of the log output file
 * 
 */
#define LOG_OUTPUT_FILENAME "lc3vm.log"

LC3_CPU_t cpu;
char *filename = NULL;

int main(int argc, char const *argv[])
{
    if (argc < 2)
    {
        printf("Usage: lc3vm [obj-file]\n");
        return 1;
    }

    Log_init(LOG_OUTPUT_FILENAME);

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
