#pragma once

#include <Windows.h>
#include <winhttp.h>
#include <MinHook.h>

#include <format>
#include <mutex>
#include <string>
#include <unordered_set>

#include "../settings/Settings.h"
#include "../utils/Logger.h"

#pragma comment(lib, "winhttp.lib")

/**
 * @file
 * @brief In-process backend redirection — rewrites the game's AccelByte host to a private
 * server, so no external proxy (Fiddler / mitmproxy) is needed. See docs/backend-redirect.md.
 *
 * First cut targets **WinHTTP** (its exports resolve via GetProcAddress, so no game-specific
 * offsets): WinHttpConnect swaps the host/port, and WinHttpOpenRequest drops TLS on the
 * redirected connection (the private server speaks plain HTTP). If the in-game log shows no
 * redirect firing, the game uses libcurl/WinInet instead — reuse @ref RewriteUrl in a hook on
 * that stack (curl's `curl_easy_setopt` CURLOPT_URL) rather than WinHTTP.
 *
 * All logic is gated on Settings.NETWORK.RedirectEnabled, so the hooks are installed once and
 * the menu toggle enables/disables redirection live.
 */
namespace Backend
{
	/// Narrow → wide for ASCII host names (AccelByte hosts are ASCII).
	inline std::wstring Widen(const std::string& s)
	{
		return std::wstring(s.begin(), s.end());
	}

	/// @return true if redirection is on and @p host is the configured official host.
	inline bool ShouldRedirect(const std::wstring& host)
	{
		return Settings.NETWORK.RedirectEnabled && !host.empty() && host == Widen(Settings.NETWORK.OfficialHost);
	}

	/**
	 * Reusable URL rewrite for URL-based HTTP stacks (e.g. libcurl's CURLOPT_URL). Downgrades
	 * `https://<OfficialHost>` to `http://<PrivateHost>:<PrivatePort>`, leaving the path
	 * untouched. Returns @p url unchanged if redirection is off or the host doesn't match.
	 */
	inline std::string RewriteUrl(const std::string& url)
	{
		if (!Settings.NETWORK.RedirectEnabled) return url;

		const std::string& official = Settings.NETWORK.OfficialHost;
		const std::string target = std::format("http://{}:{}", Settings.NETWORK.PrivateHost, Settings.NETWORK.PrivatePort);

		for (const std::string scheme : {"https://", "http://"})
		{
			const std::string prefix = scheme + official;
			if (url.rfind(prefix, 0) == 0) // starts with scheme+official host
				return target + url.substr(prefix.size());
		}
		return url;
	}

	// --- WinHTTP hook state ---

	using WinHttpConnect_t = HINTERNET(WINAPI*)(HINTERNET, LPCWSTR, INTERNET_PORT, DWORD);
	using WinHttpOpenRequest_t = HINTERNET(WINAPI*)(HINTERNET, LPCWSTR, LPCWSTR, LPCWSTR, LPCWSTR, LPCWSTR*, DWORD);

	inline WinHttpConnect_t OriginalConnect = nullptr;		   ///< trampoline to WinHttpConnect
	inline WinHttpOpenRequest_t OriginalOpenRequest = nullptr; ///< trampoline to WinHttpOpenRequest
	inline bool installed = false;							   ///< guard so hooks install once

	inline std::mutex redirectedMutex;							///< guards redirectedConnections
	inline std::unordered_set<HINTERNET> redirectedConnections; ///< connect handles we rerouted

	/// Reroutes a connection to the configured host/port when it targets the official host.
	inline HINTERNET WINAPI HookedConnect(HINTERNET session, LPCWSTR serverName, INTERNET_PORT port, DWORD reserved)
	{
		if (serverName && ShouldRedirect(serverName))
		{
			Logger::Log("INFO", std::format("[Backend] Redirecting {} -> {}:{}", Settings.NETWORK.OfficialHost, Settings.NETWORK.PrivateHost, Settings.NETWORK.PrivatePort));

			HINTERNET connection = OriginalConnect(session, Widen(Settings.NETWORK.PrivateHost).c_str(), static_cast<INTERNET_PORT>(Settings.NETWORK.PrivatePort), reserved);
			if (connection)
			{
				std::lock_guard lock(redirectedMutex);
				redirectedConnections.insert(connection);
			}
			return connection;
		}

		return OriginalConnect(session, serverName, port, reserved);
	}

	/// On a rerouted connection, clears WINHTTP_FLAG_SECURE so the request is plain HTTP.
	inline HINTERNET WINAPI HookedOpenRequest(HINTERNET connection, LPCWSTR verb, LPCWSTR object, LPCWSTR version, LPCWSTR referrer, LPCWSTR* acceptTypes, DWORD flags)
	{
		{
			std::lock_guard lock(redirectedMutex);
			if (redirectedConnections.count(connection)) flags &= ~WINHTTP_FLAG_SECURE;
		}

		return OriginalOpenRequest(connection, verb, object, version, referrer, acceptTypes, flags);
	}

	/// Installs the WinHTTP hooks (once). Requires MinHook to already be initialized (it is,
	/// by Hook::Init before Features::Init). The hooks self-gate on RedirectEnabled per call,
	/// so this is safe to call unconditionally and toggle later from the menu.
	inline void Install()
	{
		if (installed) return;

		HMODULE winhttp = LoadLibraryA("winhttp.dll");
		if (!winhttp) return;

		void* connectAddr = reinterpret_cast<void*>(GetProcAddress(winhttp, "WinHttpConnect"));
		void* openAddr = reinterpret_cast<void*>(GetProcAddress(winhttp, "WinHttpOpenRequest"));
		if (!connectAddr || !openAddr) return;

		if (MH_CreateHook(connectAddr, &HookedConnect, reinterpret_cast<void**>(&OriginalConnect)) != MH_OK) return;
		if (MH_CreateHook(openAddr, &HookedOpenRequest, reinterpret_cast<void**>(&OriginalOpenRequest)) != MH_OK) return;

		MH_EnableHook(connectAddr);
		MH_EnableHook(openAddr);

		installed = true;
		Logger::Log("SUCCESS", "[Backend] WinHTTP redirect hooks installed");
	}
} // namespace Backend
