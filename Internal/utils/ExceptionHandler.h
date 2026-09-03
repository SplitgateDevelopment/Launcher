#pragma once

#include "../../shared/ExceptionHandler.h"
#include "Logger.h"                 // Logger::Log
#include "../settings/Settings.h"   // SettingsHelper

// Internal-side wiring of the shared crash handler: reports go under the game's app folder,
// progress is logged through the in-game logger, and on a crash the settings file is deleted
// so the next launch starts clean. The Init/Disable surface is unchanged for existing callers.
namespace ExceptionHandler
{
	using ExitMode = Shared::ExceptionHandler::ExitMode;

	inline void Init(ExitMode exitMode = ExitMode::Silent)
	{
		Shared::ExceptionHandler::Config config;
		config.crashDir = SettingsHelper::GetAppPath() / "Crashes";
		config.exitMode = exitMode;
		config.log = [](const std::string& level, const std::string& message) { Logger::Log(level, message); };
		config.onCrash = []()
		{
			Logger::Log("INFO", "Deleted settings to prevent further errors");
			SettingsHelper::Delete();
		};

		Shared::ExceptionHandler::Install(config);
	}

	inline void Disable()
	{
		Shared::ExceptionHandler::Uninstall();
	}
};
