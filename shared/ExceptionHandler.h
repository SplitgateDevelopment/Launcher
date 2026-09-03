#pragma once

#include <Windows.h>
#include <DbgHelp.h>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <format>
#include <functional>
#include <string>
#include <chrono>
#include <ctime>

#pragma comment(lib, "DbgHelp.lib")

// Reusable last-chance crash handler shared by the launcher and the DLL. It installs a
// SetUnhandledExceptionFilter that writes a symbolized stack trace to a per-crash folder.
// Everything project-specific is injected through Config callbacks: `log` receives progress
// lines, and `onCrash` runs an app-specific recovery action (the DLL deletes its settings;
// the launcher passes nothing). x64 only.
namespace Shared::ExceptionHandler
{
	namespace fs = std::filesystem;

	enum class ExitMode
	{
		Silent = EXCEPTION_EXECUTE_HANDLER, // swallow the exception and continue
		Crash = EXCEPTION_CONTINUE_SEARCH,	// let the crash propagate
	};

	using LogFn = std::function<void(const std::string& level, const std::string& message)>;
	using CrashFn = std::function<void()>;

	struct Config
	{
		fs::path crashDir; // reports go under crashDir / <timestamp> /
		ExitMode exitMode = ExitMode::Silent;
		LogFn log;		 // optional progress sink
		CrashFn onCrash; // optional recovery action, run after the report
	};

	// Local time formatted for a folder name, e.g. 2026-09-03-16-42-05.
	inline std::string CrashTimestamp()
	{
		const std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		tm local;
		localtime_s(&local, &now);

		std::ostringstream ss;
		ss << std::put_time(&local, "%Y-%m-%d-%H-%M-%S");
		return ss.str();
	}

	// Walks the stack for `context` and writes one line per frame to `out`
	// (module!symbol [file:line], or a raw offset when symbols are unavailable).
	inline void WriteStackTrace(CONTEXT* context, std::ostream& out)
	{
		if (!context) return;

		HANDLE process = GetCurrentProcess();
		SymInitialize(process, NULL, TRUE);

		STACKFRAME64 frame{};
#ifdef _M_X64
		const DWORD machineType = IMAGE_FILE_MACHINE_AMD64;
		frame.AddrPC.Offset = context->Rip;
		frame.AddrPC.Mode = AddrModeFlat;
		frame.AddrFrame.Offset = context->Rsp;
		frame.AddrFrame.Mode = AddrModeFlat;
		frame.AddrStack.Offset = context->Rsp;
		frame.AddrStack.Mode = AddrModeFlat;
#else
#error "Platform not supported (x64 only)!"
#endif

		while (StackWalk64(machineType, process, GetCurrentThread(), &frame, context, NULL, SymFunctionTableAccess64, SymGetModuleBase64, NULL))
		{
			const DWORD64 moduleBase = SymGetModuleBase64(process, frame.AddrPC.Offset);
			char moduleName[MAX_PATH];
			if (GetModuleFileNameA((HMODULE)moduleBase, moduleName, MAX_PATH) == 0)
			{
				out << "  " << std::hex << frame.AddrPC.Offset << std::endl;
				continue;
			}

			const std::string moduleFileName = fs::path(moduleName).filename().string();

			char symbolBuffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR)];
			PSYMBOL_INFO symbol = (PSYMBOL_INFO)symbolBuffer;
			symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
			symbol->MaxNameLen = MAX_SYM_NAME;

			DWORD64 displacement = 0;
			if (!SymFromAddr(process, frame.AddrPC.Offset, &displacement, symbol))
			{
				out << "  " << moduleFileName << " + " << frame.AddrPC.Offset - moduleBase << std::endl;
				continue;
			}

			out << "  " << moduleFileName << "!" << symbol->Name;

			IMAGEHLP_LINE64 line;
			line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);
			DWORD lineDisplacement;
			if (SymGetLineFromAddr64(process, frame.AddrPC.Offset, &lineDisplacement, &line))
				out << "    [" << line.FileName << ":" << line.LineNumber << "]";

			out << std::endl;
		}

		SymCleanup(process);
	}

	// Writes a crash report for `context` under `config.crashDir/<timestamp>/StackTrace.log`
	// and then runs `config.onCrash`. Returns the filter code for `config.exitMode`. Pure
	// enough to call directly (e.g. from a test with an RtlCaptureContext context).
	inline LONG WriteCrashLog(const Config& config, DWORD exceptionCode, CONTEXT* context)
	{
		const auto logLine = [&](const std::string& level, const std::string& message)
		{
			if (config.log) config.log(level, message);
		};

		logLine("ERROR", "Exception thrown");
		logLine("ERROR", std::format("Exception Code: 0x{:x}", exceptionCode));

		const fs::path folder = config.crashDir / CrashTimestamp();
		std::error_code ec;
		fs::create_directories(folder, ec);

		const fs::path file = folder / "StackTrace.log";
		logLine("ERROR", std::format("Crash stack trace located at: {}", file.string()));

		std::ofstream out(file, std::ios::out);
		if (out.is_open())
		{
			out << std::format("Exception: (0x{:x})\n", exceptionCode);
			out << "== Stack Trace ==\n";
			WriteStackTrace(context, out);
			out.close();
		}
		else
		{
			logLine("ERROR", "Failed to open crash log file for writing");
		}

		// Recovery runs regardless of whether the report could be written.
		if (config.onCrash) config.onCrash();

		return static_cast<LONG>(config.exitMode);
	}

	inline Config g_config;

	inline LONG WINAPI Filter(EXCEPTION_POINTERS* info)
	{
		return WriteCrashLog(g_config, info->ExceptionRecord->ExceptionCode, info->ContextRecord);
	}

	inline void Install(const Config& config)
	{
		g_config = config;
		SetUnhandledExceptionFilter(Filter);
	}

	inline void Uninstall()
	{
		SetUnhandledExceptionFilter(NULL);
	}
}; // namespace Shared::ExceptionHandler
