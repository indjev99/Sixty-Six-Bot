#ifdef _WIN32
    #define OS_WINDOWS
#else
    #ifdef __WIN32__
        #define OS_WINDOWS
    #else
        #ifdef __WINDOWS__
            #define OS_WINDOWS
        #else
            #define OS_UNIX
        #endif
    #endif
#endif

#include "printing.h"
#include <iostream>
#include <string>

static const std::string UNIX_COLOR_CODES[NUM_COLORS] = {
    "30",
    "34",
    "31",
    "35",
    "32",
    "36",
    "33",
    "37"
};

static const std::string UNIX_PREF = "\33[";
static const std::string UNIX_DEFAULT = "0m";
static const std::string UNIX_BRIGHT = ";1m";
static const std::string UNIX_ONEUP = "A";
static const int UNIX_WIDTH = 80;

static const int WINDOWS_DEFAULT = 7;
static const int WINDOWS_BRIGHT = 8;

static void clearLine(int width)
{
    for (int i = 0; i < width; ++i)
    {
        std::cout << " ";
    }
    std::cout << "\r";
}

#ifdef OS_UNIX

void setColor(int color)
{
    std::cout << UNIX_PREF + UNIX_COLOR_CODES[color] + UNIX_BRIGHT;
}

void resetColor()
{
    std::cout << UNIX_PREF + UNIX_DEFAULT;
}

void moveUpOneLine()
{
    std::cout << UNIX_PREF + UNIX_ONEUP;
    clearLine(UNIX_WIDTH - 1);
}

#endif

#ifdef OS_WINDOWS
#include <windows.h>

void setColor(int color)
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color + WINDOWS_BRIGHT);
}

void resetColor()
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, WINDOWS_DEFAULT);
}

void moveUpOneLine()
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    CONSOLE_SCREEN_BUFFER_INFO info;
    GetConsoleScreenBufferInfo(hConsole, &info);

    COORD target;
    target.X = 0;
    target.Y = info.dwCursorPosition.Y - 1;

    SetConsoleCursorPosition(hConsole, target);
    clearLine(info.srWindow.Right);
}

#endif
