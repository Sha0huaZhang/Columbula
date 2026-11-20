#if defined(_WIN32)

#include <io.h>
#include <stdio.h>
#include <windows.h>

#include "platform.h"

int plat_is_tty(void)
{
    return _isatty(_fileno(stdout)) != 0;
}

void plat_nap(long ms)
{
    Sleep((DWORD)ms);
}

void plat_vt_on(void)
{
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode;

    if (h == INVALID_HANDLE_VALUE || !GetConsoleMode(h, &mode)) return;
    SetConsoleMode(h, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
}

long long plat_now_ms(void)
{
    return (long long)GetTickCount64();
}

#else

#define _POSIX_C_SOURCE 200809L

#include <time.h>
#include <unistd.h>

#include "platform.h"

int plat_is_tty(void)
{
    return isatty(STDOUT_FILENO) != 0;
}

void plat_nap(long ms)
{
    struct timespec ts;

    ts.tv_sec = ms / 1000;
    ts.tv_nsec = (ms % 1000) * 1000000L;
    nanosleep(&ts, NULL);
}

void plat_vt_on(void)
{
    /* POSIX 终端本来就认 ANSI 转义序列 */
}

long long plat_now_ms(void)
{
    struct timespec ts;

    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (long long)ts.tv_sec * 1000LL + ts.tv_nsec / 1000000L;
}

#endif
