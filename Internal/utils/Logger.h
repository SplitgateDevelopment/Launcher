#pragma once

#include "../../shared/Logger.h"
#include "../settings/Settings.h" // SettingsHelper::GetAppPath

// Thin facade preserving the Logger:: namespace API used throughout Internal (Logger::Log,
// Logger::CreateConsole, ...). It forwards to a single Shared::Logger that owns the in-game
// console and internal.log. The console is spawned (not attached) because the DLL has no
// console of its own; the log path comes from SettingsHelper, which lives in Internal so the
// shared logger stays dependency-free.
namespace Logger
{
	inline Shared::Logger& Instance()
	{
		static Shared::Logger logger;
		return logger;
	}

	inline void Log(const std::string& title, const std::string& message)
	{
		Instance().log(title, message);
	}

	inline void CreateConsole()
	{
		const auto logPath = SettingsHelper::GetAppPath("internal.log");
		Instance().createConsole("Splitgate Internal", logPath.string());

		Log("SUCCESS", "Created console");
		Log("INFO", std::format("Logging to {}", logPath.string()));
	}

	inline void DestroyConsole()
	{
		Instance().destroyConsole();
	}

	inline void SetConsoleVisibility(bool show)
	{
		Instance().setConsoleVisibility(show);
	}
};
