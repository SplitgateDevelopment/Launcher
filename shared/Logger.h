#pragma once

#include <Windows.h>
#include <strsafe.h>
#include <iostream>
#include <fstream>
#include <format>
#include <chrono>
#include <string>

// Console-aware logger shared by the launcher and the injected DLL. It supports both
// consoles: the launcher attaches to the one it already has, while the DLL spawns a fresh
// one (AllocConsole + stdio redirect + show/hide). Output is colored per level and mirrored
// to a log file with a local-time [HH:MM:SS] prefix. Levels are free strings
// ("INFO"/"SUCCESS"/"ERROR"/"RPC", ...), so callers can add their own.
namespace Shared
{
	class Logger
	{
	public:
		Logger() = default;
		~Logger() { if (logFile) fclose(logFile); }

		// Owns C stdio handles — not copyable.
		Logger(const Logger&) = delete;
		Logger& operator=(const Logger&) = delete;

		// Launcher path: use the process's existing console and mirror to logPath.
		void attachConsole(const std::string& logPath)
		{
			consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
			openLogFile(logPath);
		}

		// DLL path: allocate a fresh console (redirecting stdio) and mirror to logPath.
		void createConsole(const std::string& title, const std::string& logPath)
		{
			if (!AllocConsole()) return;

			consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
			consoleWindow = GetConsoleWindow();

			openLogFile(logPath);

			freopen_s(&consoleStream, "CONOUT$", "w", stdout);
			freopen_s(&consoleStream, "CONIN$", "r", stdin);
			freopen_s(&consoleStream, "CONOUT$", "w", stderr);

			SetConsoleTitleA(title.c_str());
			setConsoleVisibility(true);
		}

		void destroyConsole()
		{
			log("INFO", "Destroying console");
			setConsoleVisibility(false);

			if (consoleStream) fclose(consoleStream);
			if (logFile) fclose(logFile);
			if (consoleWindow) DestroyWindow(consoleWindow);
			FreeConsole();

			consoleStream = nullptr;
			logFile = nullptr;
			consoleWindow = nullptr;
		}

		void setConsoleVisibility(bool show)
		{
			if (!consoleWindow) return;
			ShowWindow(consoleWindow, show ? SW_SHOW : SW_HIDE);
		}

		void log(const std::string& level, const std::string& message)
		{
			const std::string time = timestamp();

			setColor(level);
			std::cout << time << " [" << level << "] " << message << std::endl;
			resetColor();

			if (!logFile) return;
			fprintf(logFile, "%s [%s] %s\n", time.c_str(), level.c_str(), message.c_str());
			fflush(logFile);
		}

		void error(const std::string& message) { log("ERROR", message); }
		void success(const std::string& message) { log("SUCCESS", message); }
		void info(const std::string& message) { log("INFO", message); }

		// Pops a Win32 MessageBox with the GetLastError() text for the named function.
		// https://learn.microsoft.com/en-us/windows/win32/debug/retrieving-the-last-error-code
		void errorBox(LPCTSTR lpszFunction)
		{
			LPVOID lpMsgBuf;
			LPVOID lpDisplayBuf;
			DWORD dw = GetLastError();

			FormatMessage(
				FORMAT_MESSAGE_ALLOCATE_BUFFER |
				FORMAT_MESSAGE_FROM_SYSTEM |
				FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL,
				dw,
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
				(LPTSTR)&lpMsgBuf,
				0, NULL);

			lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT,
				(lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR));
			StringCchPrintf((LPTSTR)lpDisplayBuf,
				LocalSize(lpDisplayBuf) / sizeof(TCHAR),
				TEXT("%s failed with error %d: %s"),
				lpszFunction, dw, lpMsgBuf);
			MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK);

			LocalFree(lpMsgBuf);
			LocalFree(lpDisplayBuf);
		}

		int stop(int code)
		{
			info("Press any key to exit...");
			std::cin.get();
			return code;
		}

	private:
		HANDLE consoleHandle = nullptr;
		HWND consoleWindow = nullptr;
		FILE* consoleStream = nullptr;
		FILE* logFile = nullptr;

		void openLogFile(const std::string& logPath)
		{
			fopen_s(&logFile, logPath.c_str(), "w");
		}

		std::string timestamp() const
		{
			const auto now = std::chrono::zoned_time{ std::chrono::current_zone(), std::chrono::floor<std::chrono::seconds>(std::chrono::system_clock::now()) };
			return std::format("[{:%H:%M:%S}]", now);
		}

		BOOL resetColor()
		{
			if (!consoleHandle) return FALSE;
			return SetConsoleTextAttribute(consoleHandle, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
		}

		BOOL setColor(const std::string& level)
		{
			if (!consoleHandle) return FALSE;
			if (level == "INFO") return SetConsoleTextAttribute(consoleHandle, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
			if (level == "ERROR") return SetConsoleTextAttribute(consoleHandle, FOREGROUND_RED | FOREGROUND_INTENSITY);
			if (level == "SUCCESS") return SetConsoleTextAttribute(consoleHandle, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
			if (level == "RPC") return SetConsoleTextAttribute(consoleHandle, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
			return SetConsoleTextAttribute(consoleHandle, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
		}
	};
};
