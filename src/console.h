/**
 * @file console.h
 * @author Daniel Polanco (jdanypa@gmail.com)
 * @brief This header contains VM OS specific keyboard functionality
 * @version 1.0
 * @date 2021-01-27
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#if !defined(__CONSOLE_H__)
#define __CONSOLE_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <signal.h>
#if __unix__
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/termios.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#elif _WIN32
#include <conio.h>
#include <windows.h>
#else
#error Not implemented OS Arch
#endif

/**
 * @brief Initialize the keyboard configuration
 * 
 */
void OSKeyboardInit();

/**
 * @brief Restores restore the terminal settings back to normal.
 * 
 */
void OSKeyboardRestoreBuffering();

/**
 * @brief Modify the terminal to allow the Lc3 input behave nicely
 * 
 */
void OSKeyboardDisableBuffering();

/**
 * @brief Checks for a polling state to check if a key was pressed
 * 
 * @return uint16_t 0x1 if a key was pressed, or 0x0 if not
 */
uint16_t OSKeyboardIsKeyPressed();

#endif  // __CONSOLE_H__
