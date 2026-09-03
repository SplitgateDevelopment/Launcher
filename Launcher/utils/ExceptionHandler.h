#pragma once

#include "../../shared/ExceptionHandler.h"
#include "../../shared/Settings.h"			  // Shared::AppDataPath
#include "../../Internal/settings/Settings.h" // SettingsHelper::AppFolder
#include "Logger.h"							  // Logger

/**
 * @file
 * @brief Launcher-side wiring of the shared crash handler.
 *
 * Installs the same symbolized stack-trace handler the DLL uses, pointed at the shared app
 * folder's Crashes directory and logging progress through the launcher's console/launcher.log.
 * The launcher has no settings to wipe, so no recovery hook is wired. This covers the injection
 * phase — before the DLL's own handler is live inside the game.
 */
namespace Launcher::ExceptionHandler
{
	using ExitMode = Shared::ExceptionHandler::ExitMode;

	/// Installs the crash filter, writing reports under Documents\SplitgateInternal\Crashes and
	/// logging progress through @p logger.
	/// @param logger the launcher logger the report progress is written through; it is captured by
	/// reference and must outlive the handler (i.e. the process).
	/// @param exitMode swallow the exception after reporting (Silent) or let it propagate (Crash).
	inline void Init(Logger& logger, ExitMode exitMode = ExitMode::Silent)
	{
		Shared::ExceptionHandler::Config config;
		config.crashDir = Shared::AppDataPath(SettingsHelper::AppFolder) / "Crashes";
		config.exitMode = exitMode;
		config.log = [&logger](const std::string& level, const std::string& message)
		{ logger.log(level, message); };

		Shared::ExceptionHandler::Install(config);
	}

	/// Removes the crash filter (restores the OS default).
	inline void Disable()
	{
		Shared::ExceptionHandler::Uninstall();
	}
} // namespace Launcher::ExceptionHandler
