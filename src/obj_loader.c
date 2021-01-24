#include "obj_loader.h"

#include <string.h>

#include "log.h"
#include "utils.h"

uint8_t loadObjFile(Lc3_CPU_t *cpu, const char *filename)
{
    LOG_LN("Reading objfile %s", filename);
    FILE *f = fopen(filename, "r");
    // In case of error loading the file return 0
    if (!f)
    {
        return 0x00;
    }

    uint16_t memOrig;
    // Read where the memory is going to be located
    fread(&memOrig, sizeof(MemSize_t), 1U, f);
    memOrig = swap_16(memOrig);
    LOG_LN("Memory start region: 0x%04X", memOrig);
    // Now read from file the program data by memory offset
    size_t progSize = fread(cpu->memory + memOrig, sizeof(MemSize_t), ADDRESS_MEMORY_LENGTH - memOrig, f);
    LOG_LN("Program size: %lu bytes", progSize);
    LOG_LN("Lc3 VM memory loaded!");

    fclose(f);
    return 0x01;
}