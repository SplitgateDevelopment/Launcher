#pragma once

#include <windows.h>
#include <fcntl.h>
#include <io.h>
#include <iostream>

using namespace std;

namespace Logger
{
    FILE* logFile = new FILE();
    HANDLE g_Handle;
    HWND g_hWnd;

    BOOL ResetConsoleColor() {
        return SetConsoleTextAttribute(g_Handle, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    }

    BOOL SetConsoleColor(const string title) {
        if (title == "INFO") return SetConsoleTextAttribute(g_Handle, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
        if (title == "ERROR") return SetConsoleTextAttribute(g_Handle, FOREGROUND_RED | FOREGROUND_INTENSITY);
        if (title == "SUCCESS") return SetConsoleTextAttribute(g_Handle, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
        if (title == "RPC") return SetConsoleTextAttribute(g_Handle, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);

        return SetConsoleTextAttribute(g_Handle, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
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
        ResetConsoleColor();

        return;
    }


    void DestroyConsole() {
        Log("INFO", "Destroying console");

        fclose(logFile);
        DestroyWindow(g_hWnd);
        FreeConsole();
    }

    void CreateConsole()
    {
        if (!AllocConsole()) return;

        g_Handle = GetStdHandle(STD_OUTPUT_HANDLE);
        g_hWnd = GetConsoleWindow();

        freopen_s(&logFile, "CONOUT$", "w", stdout);
        freopen_s(&logFile, "CONIN$", "r", stdin);
        freopen_s(&logFile, "CONOUT$", "w", stderr);

        SetConsoleTitleA("Splitgate Internal");
        ShowWindow(g_hWnd, SW_SHOW);
    };
};