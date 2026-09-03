#pragma once

#include <Windows.h>

#include <map>
#include <string>

/**
 * @file
 * @brief Spawns mitmproxy (mitmdump) from the launcher for ProxyMode::Mitmproxy.
 *
 * Builds one `--map-remote` rule per redirect (https://<from> -> http://<to>) and starts
 * mitmdump in the background before the game launches. mitmdump must be on PATH.
 *
 * NOTE: like the manual setup this replaces, the game still has to route through the proxy —
 * mitmproxy's default mode needs the Windows system proxy pointed at it (127.0.0.1:8080), or
 * a transparent/WinDivert mode. This helper only launches mitmdump; wiring the system proxy
 * (or choosing a transparent mode) is out of scope and left to configuration.
 */
namespace Launcher::Mitmproxy
{
	/// Escapes regex metacharacters in a host so it matches literally in --map-remote.
	inline std::string EscapeHost(const std::string& host)
	{
		std::string out;
		for (char c : host)
		{
			if (c == '.') out += '\\';
			out += c;
		}
		return out;
	}

	/**
	 * Starts mitmdump in a new console with a map-remote rule per redirect.
	 * @param redirects original host -> "host[:port]" target (from the DLL's NETWORK settings).
	 * @return true if the process was started (mitmdump must be on PATH), false otherwise.
	 */
	inline bool Spawn(const std::map<std::string, std::string>& redirects)
	{
		std::string command = "mitmdump";
		for (const auto& [from, to] : redirects)
			command += " --map-remote \"|https://" + EscapeHost(from) + "|http://" + to + "|\"";

		STARTUPINFOA startup{sizeof(startup)};
		PROCESS_INFORMATION process{};

		// CreateProcessA needs a writable command buffer.
		if (!CreateProcessA(nullptr, command.data(), nullptr, nullptr, FALSE, CREATE_NEW_CONSOLE, nullptr, nullptr, &startup, &process))
			return false;

		CloseHandle(process.hThread);
		CloseHandle(process.hProcess);
		return true;
	}
} // namespace Launcher::Mitmproxy
