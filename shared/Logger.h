#pragma once

/// @file
/// @brief Console-aware, file-mirroring logger shared by the launcher and the injected DLL.

#include <Windows.h>
#include <strsafe.h>
#include <conio.h>
#include <iostream>
#include <fstream>
#include <format>
#include <chrono>
#include <string>
#include <deque>
#include <mutex>
#include <vector>

// Console-aware logger shared by the launcher and the injected DLL. It supports both
// consoles: the launcher attaches to the one it already has, while the DLL spawns a fresh
// one (AllocConsole + stdio redirect + show/hide). Output is colored per level and mirrored
// to a log file with a local-time [HH:MM:SS] prefix. Levels are free strings
// ("INFO"/"SUCCESS"/"ERROR"/"RPC", ...), so callers can add their own.
/// Namespace for code shared between the launcher and the injected DLL.
namespace Shared
{
	/// Colored console logger that mirrors every line to a log file; owns its stdio/file handles.
	class Logger
	{
	  public:
		Logger() = default;
		/// Closes the log file if one was opened.
		~Logger()
		{
			if (logFile) fclose(logFile);
		}

		// Owns C stdio handles — not copyable.
		Logger(const Logger&) = delete;
		Logger& operator=(const Logger&) = delete;

		/// Launcher path: use the process's existing console and mirror to logPath.
		void attachConsole(const std::string& logPath)
		{
			consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
			openLogFile(logPath);
		}

		/// DLL path: allocate a fresh console (redirecting stdio) and mirror to logPath.
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

		/// DLL path: hide and tear down the console spawned by createConsole and close the log file.
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

		/// Shows or hides the spawned console window; no-op when there is no owned window.
		void setConsoleVisibility(bool show)
		{
			if (!consoleWindow) return;
			ShowWindow(consoleWindow, show ? SW_SHOW : SW_HIDE);
		}

		/// Writes one colored, timestamped `[level] message` line to the console and log file, and
		/// keeps it in a capped in-memory ring buffer (see recentLog).
		void log(const std::string& level, const std::string& message)
		{
			const std::string time = timestamp();
			const std::string line = time + " [" + level + "] " + message;

			{
				std::lock_guard<std::mutex> lock(recentMutex);
				recentLines.push_back(line);
				if (recentLines.size() > RecentMax) recentLines.pop_front();
			}

			setColor(level);
			std::cout << line << std::endl;
			resetColor();

			if (!logFile) return;
			fprintf(logFile, "%s\n", line.c_str());
			fflush(logFile);
		}

		/// A snapshot of the most recent log lines (oldest first), for showing in the GUI.
		std::vector<std::string> recentLog()
		{
			std::lock_guard<std::mutex> lock(recentMutex);
			return {recentLines.begin(), recentLines.end()};
		}

		/// Logs `message` at the ERROR level.
		void error(const std::string& message) { log("ERROR", message); }
		/// Logs `message` at the SUCCESS level.
		void success(const std::string& message) { log("SUCCESS", message); }
		/// Logs `message` at the INFO level.
		void info(const std::string& message) { log("INFO", message); }

		/// Pops a Win32 MessageBox with the GetLastError() text for the named function.
		/// https://learn.microsoft.com/en-us/windows/win32/debug/retrieving-the-last-error-code
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

		/// Blocks for a single keypress ("press any key to exit") and returns `code` unchanged.
		/// Uses _getch so any key returns immediately — std::cin.get() is line-buffered and would
		/// wait for Enter, which makes the window feel stuck until then.
		int stop(int code)
		{
			info("Press any key to exit...");
			_getch();
			return code;
		}

	  private:
		HANDLE consoleHandle = nullptr; ///< STD_OUTPUT_HANDLE used for coloring; not owned.
		HWND consoleWindow = nullptr;	///< Only set in the DLL (spawned) path; owned there.
		FILE* consoleStream = nullptr;	///< Redirected stdio stream from the spawned console.
		FILE* logFile = nullptr;		///< Mirror file; null when no console was attached/created.

		static constexpr size_t RecentMax = 200; ///< cap on the in-memory ring buffer
		std::deque<std::string> recentLines;	 ///< recent formatted lines, for the GUI logs panel
		std::mutex recentMutex;					 ///< guards recentLines (log is called off many threads)

		/// Opens (truncating) the mirror log file at logPath.
		void openLogFile(const std::string& logPath)
		{
			fopen_s(&logFile, logPath.c_str(), "w");
		}

		/// Returns the current local time as a "[HH:MM:SS]" prefix.
		std::string timestamp() const
		{
			const auto now = std::chrono::zoned_time{std::chrono::current_zone(), std::chrono::floor<std::chrono::seconds>(std::chrono::system_clock::now())};
			return std::format("[{:%H:%M:%S}]", now);
		}

		/// Restores the default (white) console color; no-op without a console handle.
		BOOL resetColor()
		{
			if (!consoleHandle) return FALSE;
			return SetConsoleTextAttribute(consoleHandle, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
		}

		/// Selects the console color for the given level; no-op without a console handle.
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
}; // namespace Shared
