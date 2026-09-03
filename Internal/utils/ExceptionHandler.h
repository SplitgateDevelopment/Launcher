#pragma once

#include "../../shared/ExceptionHandler.h"
#include "Logger.h"				  // Logger::Log
#include "../settings/Settings.h" // SettingsHelper

/**
 * @file
 * @brief Internal-side wiring of the shared crash handler.
 *
 * Reports go under the game's app folder, progress is logged through the in-game logger, and
 * on a crash the settings file is deleted (when DEBUG.DeleteSettingsOnCrash is on) so the next
 * launch starts clean. The Init/Disable surface is unchanged for existing callers.
 */
namespace ExceptionHandler
{
	using ExitMode = Shared::ExceptionHandler::ExitMode;

	/// Installs the crash filter wired with the game's crash folder, logger, and settings-wipe
	/// recovery hook.
	/// @param exitMode whether to swallow the exception (Silent) or let it propagate (Crash).
	inline void Init(ExitMode exitMode = ExitMode::Silent)
	{
		Shared::ExceptionHandler::Config config;
		config.crashDir = Shared::AppDataPath(SettingsHelper::AppFolder) / "Crashes";
		config.exitMode = exitMode;
		config.log = [](const std::string& level, const std::string& message)
		{ Logger::Log(level, message); };
		config.onCrash = []()
		{
			if (!Settings.DEBUG.DeleteSettingsOnCrash) return;

			Logger::Log("INFO", "Deleted settings to prevent further errors");
			SettingsHelper::File().Remove();
		};

		Shared::ExceptionHandler::Install(config);
	}

	/// Removes the crash filter.
	inline void Disable()
	{
		Shared::ExceptionHandler::Uninstall();
	}
}; // namespace ExceptionHandler
