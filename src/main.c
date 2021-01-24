#include <stdio.h>

#include "log.h"
#include "obj_loader.h"
#include "cpu.h"

Lc3_CPU_t cpu;
char *filename = NULL;

int main(int argc, char const *argv[])
{
    LOG_LN("Hello world! This Virtual machine for Lc3 is been building...");

    if (argc < 2)
    {
        LOG_LN("Usage: lc3vm [obj-file]");
        return 1;
    }

    if (!loadObjFile(&cpu, argv[1]))
    {
        LOG_LN("Can't load objfile, exit...");
        return 2;
    }

    return 0;
}
