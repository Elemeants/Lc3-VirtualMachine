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
#include <stdlib.h>

#include "console.h"
#include "cpu.h"
#include "firmware.h"
#include "log.h"

/**
 * @brief Mame of the log output file
 * 
 */
#define LOG_OUTPUT_FILENAME "lc3vm.log"

LC3Firmware_t firmware;
LC3Cpu_t cpu;

int main(int argc, char const *argv[])
{
    if (argc < 2)
    {
        printf("Usage: lc3vm [obj-file]\n");
        return 1;
    }

    Log_init(LOG_OUTPUT_FILENAME);

    OSKeyboardInit();

    loadFirmwareFromFile(argv[1], &firmware);
    dumpFirmware(&firmware);

    LC3CpuInit(&cpu, &firmware);

    LC3CpuExecute(&cpu);

    return EXIT_SUCCESS;
}
