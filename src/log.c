#include "log.h"

#include <stdlib.h>

int LOG_OK = 0x00;
FILE *fileout = NULL;

void Log_init(const char *filename)
{
    fileout = fopen(filename, "w+");
    if (!fileout)
    {
        perror("Can't write log file");
        exit(1);
    }
    else
    {
        LOG_OK = 0x1;
    }
    atexit(Log_end);
}

void Log_end()
{
    if (fileout)
    {
        fclose(fileout);
    }
}