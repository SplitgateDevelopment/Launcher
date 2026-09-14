#pragma once

#include "CurlHook.h"
#include "HttpLogger.h"
#include "Redirect.h"
#include "WinHttpHook.h"

/**
 * @file
 * @brief Network subsystem entry point. Installs the HTTP hooks (libcurl — the game's real
 * stack — plus WinHTTP as an offset-free fallback). Redirection and logging self-gate on
 * Settings.NETWORK, so this is called once at startup (from Features::Init) and the Network
 * menu tab toggles behavior live.
 */
namespace Network
{
	/// Installs the libcurl and WinHTTP hooks. Requires MinHook to be initialized already.
	inline void Init()
	{
		Curl::Install();
		WinHttp::Install();
	}
} // namespace Network
