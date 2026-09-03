#pragma once

/// @file
/// @brief Launcher-specific Logger: binds Shared::Logger to the existing console and launcher.log.

#include "../../shared/Logger.h"

// The launcher runs in its own console window, so it just binds Shared::Logger to that
// existing console and mirrors output to launcher.log (kept separate from the DLL's
// internal.log). All the actual logging lives in shared/Logger.h.
/// Thin Shared::Logger subclass that attaches to the launcher's console and mirrors to launcher.log.
class Logger : public Shared::Logger
{
  public:
	Logger() { attachConsole("launcher.log"); }
};
