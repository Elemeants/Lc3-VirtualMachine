#include "firmware.h"

#include <stdlib.h>
#include <string.h>

#include "log.h"
#include "utils.h"

/**
 * @brief Loads a objfile from a filename and build a instance of LC3Firmware_t
 * 
 * @param filename path/filename to the objfile
 * @param firmware Instance to build
 * @return uint8_t success flag
 */
uint8_t loadFirmwareFromFile(const char *filename, LC3Firmware_t *firmware)
{
    firmware->filename = filename;
    LOG_LN("Reading objfile %s", filename);
    FILE *f = fopen(filename, "rb");
    // In case of error loading the file return 0
    if (!f)
    {
        return EXIT_FAILURE;
    }
    // Read where the memory is going to be located
    uint16_t rWords = fread(&firmware->memOrig, sizeof(uint16_t), 1U, f);
    if (rWords != 1U)
    {
        fclose(f);
        return EXIT_FAILURE;
    }
    firmware->memOrig = swap_16(firmware->memOrig);
    LOG_LN("Memory start region: 0x%04X", firmware->memOrig);
    // Now read from file the program data by memory offset
    size_t progSize = fread(firmware->memory + firmware->memOrig, sizeof(uint16_t), UINT16_MAX - firmware->memOrig, f);
    firmware->size = progSize;
    LOG_LN("Program size: %u words", firmware->size);
    for (uint16_t x = 0; x < UINT16_MAX; x++)
    {
        firmware->memory[x] = swap_16(firmware->memory[x]);
    }
    LOG_LN("Lc3 VM memory loaded!");
    fclose(f);
    return EXIT_SUCCESS;
}

/**
 * @brief Prints fancy program bytes to the log file
 * 
 * @param firmware Firmware instance to dump
 */
void dumpFirmware(LC3Firmware_t *firmware)
{
    LOG_LN("================================= Program memory ==================================");
    LOG_TXT("            0     1     2     3     4     5     6     7     8     9     A     B     C     D     E     F\n");
    for (uint16_t i = firmware->memOrig; i < (firmware->memOrig + firmware->size); i += 0x10)
    {
        LOG_TXT(" 0x%04X:", i);

        for (uint8_t x = 0; x < 0x10; x++)
        {
            if ((i + x) >= (firmware->memOrig + firmware->size))
            {
                break;
            }
            LOG_TXT(" %04X ", firmware->memory[i + x]);
        }
        LOG_TXT("\n");
    }
    LOG_LN("===================================================================================");
}