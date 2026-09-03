#pragma once

#include <Windows.h>

#include <filesystem>
#include <fstream>
#include <map>
#include <string>

#include "ProxyConfig.h" // Launcher::ReadLauncherSettings, Shared::MitmproxySettings

/**
 * @file
 * @brief Spawns mitmproxy (mitmdump) from the launcher for ProxyMode::Mitmproxy.
 *
 * The launcher always runs mitmdump with a `-s <addon>` script; how that script is chosen comes
 * from the launcher's own settings (Shared::MitmproxySettings, in launcher.settings, editable
 * from the DLL's Network tab):
 * - Default: a script is generated from the DLL's redirect map — it rewrites those hosts to the
 *   private server and passes other TLS connections through (parity with the backend's
 *   scripts/proxy.py, which plain `--map-remote` never had).
 * - Path: an addon file already on disk is run as-is (`mitmdump -s <path>`).
 * - Inline: the inline python is written to a temp file and run as the addon.
 *
 * NOTE: like the manual setup this replaces, the game still has to route through the proxy —
 * mitmproxy's default mode needs the Windows system proxy pointed at it (127.0.0.1:8080), or a
 * transparent/WinDivert mode. This helper only launches mitmdump; wiring the system proxy (or a
 * transparent mode) is out of scope and left to configuration.
 */
namespace Launcher::Mitmproxy
{
	/// Builds a default mitmproxy addon from the redirect map: an in-request host/scheme rewrite
	/// for each redirected host, plus a conservative TLS passthrough (don't intercept a host
	/// again once its handshake has failed) so cert-pinned/unrelated hosts keep working.
	/// @param redirects original host -> "host[:port]" target (from the DLL's NETWORK settings).
	inline std::string GenerateDefaultScript(const std::map<std::string, std::string>& redirects)
	{
		std::string entries;
		for (const auto& [from, to] : redirects)
		{
			std::string host = to;
			std::string port = "80";
			const auto colon = to.rfind(':');
			if (colon != std::string::npos)
			{
				host = to.substr(0, colon);
				port = to.substr(colon + 1);
			}
			entries += "    \"" + from + "\": (\"" + host + "\", " + port + "),\n";
		}

		return "from mitmproxy import http\n"
			   "import collections\n"
			   "\n"
			   "REDIRECTS = {\n" +
			   entries +
			   "}\n"
			   "\n"
			   "class Proxy:\n"
			   "    def request(self, flow: http.HTTPFlow) -> None:\n"
			   "        target = REDIRECTS.get(flow.request.host)\n"
			   "        if target:\n"
			   "            flow.request.scheme = \"http\"\n"
			   "            flow.request.host, flow.request.port = target\n"
			   "\n"
			   "class MaybeTls:\n"
			   "    def __init__(self):\n"
			   "        self.history = collections.defaultdict(lambda: collections.deque(maxlen=200))\n"
			   "    def tls_clienthello(self, data):\n"
			   "        address = data.context.server.peername\n"
			   "        if False in self.history[address]:\n"
			   "            data.ignore_connection = True\n"
			   "    def tls_established_client(self, data):\n"
			   "        self.history[data.context.server.peername].append(True)\n"
			   "    def tls_failed_client(self, data):\n"
			   "        self.history[data.context.server.peername].append(False)\n"
			   "\n"
			   "addons = [Proxy(), MaybeTls()]\n";
	}

	/// Writes @p content to a temp .py file and returns its path (empty on failure). A single
	/// stable name is reused so repeated launches overwrite rather than pile up temp files.
	inline std::string WriteTempScript(const std::string& content)
	{
		std::error_code ec;
		const auto path = std::filesystem::temp_directory_path(ec) / "splitgate_mitm.py";
		if (ec) return {};

		std::ofstream file(path, std::ios::out | std::ios::binary | std::ios::trunc);
		if (!file.is_open()) return {};

		file << content;
		file.close();
		return path.string();
	}

	/// Resolves the addon script path for the configured mode (writing a temp file for Default and
	/// Inline). Empty string if it couldn't be produced.
	inline std::string ResolveScript(const Shared::MitmproxySettings& config, const std::map<std::string, std::string>& redirects)
	{
		switch (config.ScriptMode)
		{
		case Shared::MitmScriptMode::Path:
			return config.ScriptPath;
		case Shared::MitmScriptMode::Inline:
			return WriteTempScript(config.InlineScript);
		case Shared::MitmScriptMode::Default:
		default:
			return WriteTempScript(GenerateDefaultScript(redirects));
		}
	}

	/**
	 * Starts mitmdump in a new console, running the addon chosen by the launcher's mitmproxy
	 * settings (Default/Path/Inline). Reads those settings itself, so callers only pass the
	 * redirect map used to generate the Default addon.
	 * @param redirects original host -> "host[:port]" target (from the DLL's NETWORK settings).
	 * @return true if the process was started (mitmdump must be on PATH), false otherwise.
	 */
	inline bool Spawn(const std::map<std::string, std::string>& redirects)
	{
		const auto config = ReadLauncherSettings().MITMPROXY;

		const std::string script = ResolveScript(config, redirects);
		if (script.empty()) return false;

		std::string command = "mitmdump -s \"" + script + "\"";

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
