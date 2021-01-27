#include "console.h"

// This keyboard platform specific implementation is based on
// https://justinmeiners.github.io/lc3-vm/index.html

static void HandleInterrupt(int signal)
{
    OSKeyboardRestoreBuffering();
    printf("\n");
    exit(-2);
}

#if __UNIX__
struct termios original_tio;
#elif _WIN32
HANDLE hStdin = INVALID_HANDLE_VALUE;
DWORD fdwMode, fdwOldMode;
#endif

void OSKeyboardInit()
{
    signal(SIGINT, HandleInterrupt);
    OSKeyboardDisableBuffering();
}

uint16_t OSKeyboardWaitKey()
{
#if __UNIX__
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(STDIN_FILENO, &readfds);
    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;
    return select(1, &readfds, NULL, NULL, &timeout) != 0;
#elif _WIN32
    return WaitForSingleObject(hStdin, 1000) == WAIT_OBJECT_0 && _kbhit();
#endif
}

void OSKeyboardDisableBuffering()
{
#if __UNIX__
    tcgetattr(STDIN_FILENO, &original_tio);
    struct termios new_tio = original_tio;
    new_tio.c_lflag &= ~ICANON & ~ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &new_tio);
#elif _WIN32
    hStdin = GetStdHandle(STD_INPUT_HANDLE);
    GetConsoleMode(hStdin, &fdwOldMode);
    fdwMode = fdwOldMode ^ ENABLE_ECHO_INPUT
              ^ ENABLE_LINE_INPUT;
    SetConsoleMode(hStdin, fdwMode);
    FlushConsoleInputBuffer(hStdin);
#endif
}

void OSKeyboardRestoreBuffering()
{
#if __UNIX__
    tcsetattr(STDIN_FILENO, TCSANOW, &original_tio);
#elif _WIN32
    SetConsoleMode(hStdin, fdwOldMode);
#endif
}
