#include "obj_loader.h"

#include <string.h>

#include "log.h"
#include "utils.h"

uint8_t loadObjFile(LC3_CPU_t *cpu, const char *filename)
{
    LOG_LN("Reading objfile %s", filename);
    FILE *f = fopen(filename, "rb");
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
    LOG_LN("Program size: %lu bytes", progSize * sizeof(MemSize_t));
    // TODO: This can be separated in two functions, and the first part be printed into the console for info.

    LOG_LN("================================= Program memory ==================================");
    LOG_TXT("            0     1     2     3     4     5     6     7     8     9     A     B     C     D     E     F\n");
    for (uint16_t i = memOrig; i < (memOrig + progSize); i += 0x10)
    {
        LOG_TXT(" 0x%04X:", i);

        for (uint8_t x = 0; x < 0x10; x++)
        {
            if ((i + x) >= (memOrig + progSize))
            {
                break;
            }
            cpu->memory[i + x] = swap_16(cpu->memory[i + x]);
            LOG_TXT(" %04X ", cpu->memory[i + x]);
        }
        LOG_TXT("\n");
    }
    LOG_LN("===================================================================================");

    LOG_LN("Lc3 VM memory loaded!");

    fclose(f);
    return 0x01;
}
