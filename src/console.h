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

void OSKeyboardInit();
void OSKeyboardRestoreBuffering();
void OSKeyboardDisableBuffering();
uint16_t OSKeyboardWaitKey();

#endif  // __CONSOLE_H__
