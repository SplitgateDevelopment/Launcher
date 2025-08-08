#pragma once

#include <windows.h>
#include <fcntl.h>
#include <io.h>
#include <iostream>

namespace Logger
{
    FILE* consoleStream = new FILE();
    FILE* logFile = new FILE();
    HANDLE g_Handle;
    HWND g_hWnd;

    BOOL ResetConsoleColor() {
        return SetConsoleTextAttribute(g_Handle, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    }

    BOOL SetConsoleColor(const std::string title) {
        if (title == "INFO") return SetConsoleTextAttribute(g_Handle, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
        if (title == "ERROR") return SetConsoleTextAttribute(g_Handle, FOREGROUND_RED | FOREGROUND_INTENSITY);
        if (title == "SUCCESS") return SetConsoleTextAttribute(g_Handle, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
        if (title == "RPC") return SetConsoleTextAttribute(g_Handle, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);

        return SetConsoleTextAttribute(g_Handle, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    }

    void Log(const std::string title, const std::string message)
    {
        time_t now = time(0);
        tm tstruct;
        char buf[80];
        localtime_s(&tstruct, &now);
        strftime(buf, sizeof(buf), "[%H:%M:%S]", &tstruct);

        SetConsoleColor(title);
        std::cout << buf << " [" << title << "] " << message << std::endl;
        ResetConsoleColor();

        fprintf(logFile, "%s [%s] %s\n", buf, title.c_str(), message.c_str());
        fflush(logFile);

        return;
    }

    void SetConsoleVisibility(bool bShow)
    {
        ShowWindow(g_hWnd, bShow ? SW_SHOW : SW_HIDE);
    };

    void DestroyConsole() {
        Log("INFO", "Destroying console");
        SetConsoleVisibility(false);

        fclose(consoleStream);
        fclose(logFile);
        DestroyWindow(g_hWnd);
        FreeConsole();
    }

    void CreateConsole()
    {
        if (!AllocConsole()) return;

        g_Handle = GetStdHandle(STD_OUTPUT_HANDLE);
        g_hWnd = GetConsoleWindow();

        fs::path logFilePath = SettingsHelper::GetAppPath() / "splitgate.log";
        fopen_s(&logFile, logFilePath.string().c_str(), "w");

        freopen_s(&consoleStream, "CONOUT$", "w", stdout);
        freopen_s(&consoleStream, "CONIN$", "r", stdin);
        freopen_s(&consoleStream, "CONOUT$", "w", stderr);

        SetConsoleTitleA("Splitgate Internal");
        SetConsoleVisibility(true);

        Log("SUCCESS", "Created console");
        Log("INFO", std::format("Logging to {}", logFilePath.string()));
    };
};