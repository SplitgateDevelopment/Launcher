#pragma once

#include "../../shared/Logger.h"

// The launcher runs in its own console window, so it just binds Shared::Logger to that
// existing console and mirrors output to launcher.log (kept separate from the DLL's
// internal.log). All the actual logging lives in shared/Logger.h.
class Logger : public Shared::Logger
{
  public:
	Logger() { attachConsole("launcher.log"); }
};
