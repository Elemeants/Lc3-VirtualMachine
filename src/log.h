/**
 * @file log.h
 * @author Daniel Polanco (jdanypa@gmail.com)
 * @brief Log utility functions
 * @version 1.0
 * @date 2021-01-27
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#if !defined(__LOG_H__)
#define __LOG_H__

#include <stdio.h>

/**
 * @brief Gets the filename where is called LOG macros
 * 
 */
#define __FILENAME__ (__builtin_strrchr(__FILE__, '/') ? __builtin_strrchr(__FILE__, '/') + 1 : __FILE__)

/**
 * @brief Logs a text, without start format
 * Common use is to add log messages to a previous message
 * 
 */
#define LOG_TXT(format, ...) fprintf(fileout, format, ## __VA_ARGS__)
/**
 * @brief Logs a message with start format but not new line
 * 
 */
#define LOG(format, ...) fprintf(fileout, " -> [%s:%d] " format, __FILENAME__, __LINE__, ##__VA_ARGS__)
/**
 * @brief Logs a message with start format and new line
 * 
 */
#define LOG_LN(format, ...) fprintf(fileout, " -> [%s:%d] " format "\n", __FILENAME__, __LINE__, ##__VA_ARGS__)

/**
 * @brief LOG_OK flag used to indicate if log file was successfully opened
 * 
 */
extern int LOG_OK;

/**
 * @brief File pointer to the log file
 * 
 */
extern FILE *fileout;

/**
 * @brief Initializes the log output, in case can't be opened exits the program
 * 
 * @param filename path/filename to the log file
 */
void Log_init(const char *filename);

/**
 * @brief This is not called directly, it is used by atexit function
 * 
 */
void Log_end();

#endif  // __LOG_H__
