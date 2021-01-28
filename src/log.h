#if !defined(__LOG_H__)
#define __LOG_H__

#include <stdio.h>

#define __FILENAME__ (__builtin_strrchr(__FILE__, '/') ? __builtin_strrchr(__FILE__, '/') + 1 : __FILE__)

#define LOG_TXT(format, ...) fprintf(fileout, format, ## __VA_ARGS__)
#define LOG(format, ...) fprintf(fileout, " -> [%s:%d] " format, __FILENAME__, __LINE__, ##__VA_ARGS__)
#define LOG_LN(format, ...) fprintf(fileout, " -> [%s:%d] " format "\n", __FILENAME__, __LINE__, ##__VA_ARGS__)

extern int LOG_OK;

extern FILE *fileout;

void Log_init(const char *filename);

void Log_end();

#endif  // __LOG_H__
