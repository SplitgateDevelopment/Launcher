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

    void log(const string title, const string message) {
        time_t now = time(0);
        tm tstruct;
        char buf[80];
        localtime_s(&tstruct, &now);
        strftime(buf, sizeof(buf), "[%H:%M:%S]", &tstruct);
        std::cout << buf << " [" << title << "] " << message << std::endl;

        if (useMsgBox) MessageBoxA(0, message.c_str(), title.c_str(), MB_OK);
        return;
    }

private:
    HANDLE g_hChildStd_IN_Rd = NULL;
    HANDLE g_hChildStd_IN_Wr = NULL;
    HANDLE g_hChildStd_OUT_Rd = NULL;
    HANDLE g_hChildStd_OUT_Wr = NULL;

    void CreateConsole()
    {
        SECURITY_ATTRIBUTES saAttr;
        saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
        saAttr.bInheritHandle = TRUE;
        saAttr.lpSecurityDescriptor = NULL;

        // Create a pipe for the child process's STDOUT.
        if (!CreatePipe(&g_hChildStd_OUT_Rd, &g_hChildStd_OUT_Wr, &saAttr, 0))
            return;

        // Ensure the read handle to the pipe for STDOUT is not inherited.
        if (!SetHandleInformation(g_hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0))
            return;

        // Create a pipe for the child process's STDIN.
        if (!CreatePipe(&g_hChildStd_IN_Rd, &g_hChildStd_IN_Wr, &saAttr, 0))
            return;

        // Ensure the write handle to the pipe for STDIN is not inherited.
        if (!SetHandleInformation(g_hChildStd_IN_Wr, HANDLE_FLAG_INHERIT, 0))
            return;

        // Save the handle to the current STDOUT.
        HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);

        // Redirect unbuffered STDOUT to the pipe.
        int fd = _open_osfhandle((intptr_t)g_hChildStd_OUT_Wr, _O_TEXT);
        if (fd == -1)
            return;
        FILE* pf = _fdopen(fd, "w");
        *stdout = *pf;
        setvbuf(stdout, NULL, _IONBF, 0);

        // Save the handle to the current STDIN.
        HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);

        // Redirect unbuffered STDIN to the pipe.
        fd = _open_osfhandle((intptr_t)g_hChildStd_IN_Rd, _O_TEXT);
        if (fd == -1)
            return;
        pf = _fdopen(fd, "r");
        *stdin = *pf;
        setvbuf(stdin, NULL, _IONBF, 0);

        // Redirect the STDOUT handle to the console window.
        if (!SetStdHandle(STD_OUTPUT_HANDLE, g_hChildStd_OUT_Wr))
            return;

        // Redirect the STDIN handle to the console window.
        if (!SetStdHandle(STD_INPUT_HANDLE, g_hChildStd_IN_Rd))
            return;

        // Create the console window.
        if (!AllocConsole())
        {
            // If the AllocConsole function fails, restore the original handle.
            SetStdHandle(STD_OUTPUT_HANDLE, hStdout);
            SetStdHandle(STD_INPUT_HANDLE, hStdin);
            return;
        };

        FILE* file = new FILE();
        freopen_s(&file, "CONOUT$", "w", stdout);
    }

};