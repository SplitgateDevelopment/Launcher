#pragma once

#include "../../shared/Logger.h"
#include "../settings/Settings.h" // SettingsHelper::GetAppPath

/**
 * @file
 * @brief Thin facade preserving the `Logger::` namespace API used throughout Internal
 * (Logger::Log, Logger::CreateConsole, ...).
 *
 * It forwards to a single Shared::Logger that owns the in-game console and internal.log. The
 * console is spawned (not attached) because the DLL has no console of its own; the log path
 * comes from SettingsHelper, which lives in Internal so the shared logger stays
 * dependency-free.
 */
namespace Logger
{
	/// @return the process-wide Shared::Logger instance the facade forwards to.
	inline Shared::Logger& Instance()
	{
		static Shared::Logger logger;
		return logger;
	}

	/// Logs a line at the given level.
	/// @param title level tag ("INFO"/"SUCCESS"/"ERROR"/"RPC"/...).
	/// @param message the text to log.
	inline void Log(const std::string& title, const std::string& message)
	{
		Instance().log(title, message);
	}

	/// Spawns the in-game console and opens internal.log (in the app folder).
	inline void CreateConsole()
	{
		const auto logPath = SettingsHelper::GetAppPath("internal.log");
		Instance().createConsole("Splitgate Internal", logPath.string());

		Log("SUCCESS", "Created console");
		Log("INFO", std::format("Logging to {}", logPath.string()));
	}

	/// Closes the console and the log file.
	inline void DestroyConsole()
	{
		Instance().destroyConsole();
	}

	/// Shows or hides the console window.
	/// @param show true to show, false to hide.
	inline void SetConsoleVisibility(bool show)
	{
		Instance().setConsoleVisibility(show);
	}
}; // namespace Logger
