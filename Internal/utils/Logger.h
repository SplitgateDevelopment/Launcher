#pragma once

#include <windows.h>
#include <fcntl.h>
#include <io.h>
#include <iostream>

using namespace std;

struct LoggerOpts {
    BOOL useMsgBox = FALSE;
};

class Logger
{
public:
    BOOL useMsgBox = FALSE;
    Logger(LoggerOpts options)
    {
        useMsgBox = options.useMsgBox;
        CreateConsole();
    }

    ~Logger() {
        DestroyConsole();
    };

    void log(const string title, const string message)
    {
        time_t now = time(0);
        tm tstruct;
        char buf[80];
        localtime_s(&tstruct, &now);
        strftime(buf, sizeof(buf), "[%H:%M:%S]", &tstruct);

        setColor(title);
        std::cout << buf << " [" << title << "] " << message << std::endl;

        if (useMsgBox) MessageBoxA(0, message.c_str(), title.c_str(), MB_OK);

        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        return;
    }


    void DestroyConsole() {
        log("INFO", "Destroying console");
        if (FreeConsole())
        {
            fflush(stdout);
            fflush(stderr);
        };
    }

private:
    FILE* file = new FILE();

    void CreateConsole()
    {
        if (!AllocConsole()) return;
        freopen_s(&file, "CONIN$", "r", stdin);
        freopen_s(&file, "CONOUT$", "w", stderr);
        freopen_s(&file, "CONOUT$", "w", stdout);
    };

    BOOL setColor(const string title) {
        if (title == "INFO") return SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE | FOREGROUND_INTENSITY);
        if (title == "ERROR") return SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY);
        if (title == "SUCCESS") return SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_INTENSITY);
        if (title == "RPC") return SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);

        return SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    }
};