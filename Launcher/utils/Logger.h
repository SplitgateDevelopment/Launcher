#pragma once

/// @file
/// @brief Launcher-specific Logger: binds Shared::Logger to the existing console and launcher.log.

#include "../../shared/Logger.h"
#include "../../shared/Settings.h"			  // Shared::AppDataPath
#include "../../Internal/settings/Settings.h" // SettingsHelper::AppFolder

// The launcher runs in its own console window, so it just binds Shared::Logger to that existing
// console and mirrors output to launcher.log. That log now lives alongside the DLL's internal.log
// under Documents\<AppFolder>\logs\ (rather than next to the exe), so both logs are in one place.
/// Thin Shared::Logger subclass that attaches to the launcher's console and mirrors to launcher.log.
class Logger : public Shared::Logger
{
  public:
	Logger() { attachConsole(Shared::AppDataPath(SettingsHelper::AppFolder, "logs/launcher.log").string()); }
};
