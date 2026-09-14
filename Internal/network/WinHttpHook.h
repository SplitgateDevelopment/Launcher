#pragma once

#include <Windows.h>
#include <winhttp.h>
#include <MinHook.h>

#include <format>
#include <mutex>
#include <string>
#include <unordered_map>
#include <utility>

#include "../utils/Logger.h"
#include "HttpLogger.h"
#include "Redirect.h"

#pragma comment(lib, "winhttp.lib")

/**
 * @file
 * @brief WinHTTP redirect + logging hooks. Its exports resolve via GetProcAddress (no game
 * offsets), so this is the offset-free fallback if the game's HTTP goes through WinHTTP.
 * WinHttpConnect reroutes the host/port; WinHttpOpenRequest drops TLS on rerouted connections
 * (the private server is plain HTTP) and logs the call.
 */
namespace Network::WinHttp
{
	/// ASCII string widen/narrow (AccelByte hosts are ASCII).
	inline std::wstring Widen(const std::string& s)
	{
		return std::wstring(s.begin(), s.end());
	}
	inline std::string Narrow(const std::wstring& s)
	{
		std::string out;
		out.reserve(s.size());
		for (wchar_t c : s)
			out.push_back(static_cast<char>(c)); // explicit ASCII narrowing (hosts are ASCII)
		return out;
	}

	using Connect_t = HINTERNET(WINAPI*)(HINTERNET, LPCWSTR, INTERNET_PORT, DWORD);
	using OpenRequest_t = HINTERNET(WINAPI*)(HINTERNET, LPCWSTR, LPCWSTR, LPCWSTR, LPCWSTR, LPCWSTR*, DWORD);

	inline Connect_t OriginalConnect = nullptr;			///< trampoline to the real WinHttpConnect
	inline OpenRequest_t OriginalOpenRequest = nullptr; ///< trampoline to the real WinHttpOpenRequest
	inline bool installed = false;						///< guard so the hooks install once

	inline std::mutex mutex; ///< guards @ref connections
	/// connect handle -> {original host (for logging), was it rerouted?}
	inline std::unordered_map<HINTERNET, std::pair<std::string, bool>> connections;

	/// Reroutes a connection to the configured target when its host is a redirect key, and
	/// records the handle (with the original host) for OpenRequest to find.
	inline HINTERNET WINAPI HookedConnect(HINTERNET session, LPCWSTR serverName, INTERNET_PORT port, DWORD reserved)
	{
		const std::string host = serverName ? Narrow(serverName) : "";
		const auto target = Redirect::Target(host);

		HINTERNET connection;
		if (target)
		{
			const auto [redirectHost, redirectPort] = Redirect::SplitTarget(*target);
			Logger::Log("INFO", std::format("[Network] Redirecting {} -> {}", host, *target));
			connection = OriginalConnect(session, Widen(redirectHost).c_str(), static_cast<INTERNET_PORT>(redirectPort), reserved);
		}
		else
		{
			connection = OriginalConnect(session, serverName, port, reserved);
		}

		if (connection)
		{
			std::lock_guard lock(mutex);
			connections[connection] = {host, target.has_value()};
		}
		return connection;
	}

	/// Logs the request and, on a rerouted connection, clears WINHTTP_FLAG_SECURE so it goes
	/// out as plain HTTP (the private server isn't TLS).
	inline HINTERNET WINAPI HookedOpenRequest(HINTERNET connection, LPCWSTR verb, LPCWSTR object, LPCWSTR version, LPCWSTR referrer, LPCWSTR* acceptTypes, DWORD flags)
	{
		std::string host;
		bool redirected = false;
		{
			std::lock_guard lock(mutex);
			if (const auto it = connections.find(connection); it != connections.end())
			{
				host = it->second.first;
				redirected = it->second.second;
			}
		}

		const std::string method = verb ? Narrow(verb) : "GET";
		const std::string path = object ? Narrow(object) : "/";
		Http::Log(method, "https://" + host + path); // log the original URL

		if (redirected) flags &= ~WINHTTP_FLAG_SECURE;
		return OriginalOpenRequest(connection, verb, object, version, referrer, acceptTypes, flags);
	}

	/// Installs the WinHTTP hooks once (MinHook must already be initialized).
	inline void Install()
	{
		if (installed) return;

		HMODULE dll = LoadLibraryA("winhttp.dll");
		if (!dll) return;

		void* connectAddr = reinterpret_cast<void*>(GetProcAddress(dll, "WinHttpConnect"));
		void* openAddr = reinterpret_cast<void*>(GetProcAddress(dll, "WinHttpOpenRequest"));
		if (!connectAddr || !openAddr) return;

		if (MH_CreateHook(connectAddr, &HookedConnect, reinterpret_cast<void**>(&OriginalConnect)) != MH_OK) return;
		if (MH_CreateHook(openAddr, &HookedOpenRequest, reinterpret_cast<void**>(&OriginalOpenRequest)) != MH_OK) return;

		MH_EnableHook(connectAddr);
		MH_EnableHook(openAddr);

		installed = true;
		Logger::Log("SUCCESS", "[Network] WinHTTP hooks installed");
	}
} // namespace Network::WinHttp
