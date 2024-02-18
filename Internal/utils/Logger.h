#pragma once

#include <windows.h>
#include <fcntl.h>
#include <io.h>
#include <iostream>

using namespace std;

FILE* logFile = new FILE();

namespace Logger
{
    BOOL SetConsoleColor(const string title) {
        if (title == "INFO") return SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE | FOREGROUND_INTENSITY);
        if (title == "ERROR") return SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY);
        if (title == "SUCCESS") return SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_INTENSITY);
        if (title == "RPC") return SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);

        return SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    }

    void Log(const string title, const string message)
    {
        time_t now = time(0);
        tm tstruct;
        char buf[80];
        localtime_s(&tstruct, &now);
        strftime(buf, sizeof(buf), "[%H:%M:%S]", &tstruct);

        SetConsoleColor(title);
        std::cout << buf << " [" << title << "] " << message << std::endl;

        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        return;
    }


    void DestroyConsole() {
        Log("INFO", "Destroying console");
        if (FreeConsole())
        {
            fflush(stdout);
            fflush(stderr);
        };
    }

    void CreateConsole()
    {
        if (!AllocConsole()) return;
        freopen_s(&logFile, "CONIN$", "r", stdin);
        freopen_s(&logFile, "CONOUT$", "w", stderr);
        freopen_s(&logFile, "CONOUT$", "w", stdout);

        SetConsoleTitleA("Splitgate Internal");
    };
};