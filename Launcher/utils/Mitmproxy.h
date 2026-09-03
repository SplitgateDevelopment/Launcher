#pragma once

#include <Windows.h>

#include <filesystem>
#include <fstream>
#include <map>
#include <string>

#include <nlohmann/json.hpp>

#include "../../shared/Logger.h" // Shared::Logger (optional progress/failure sink)
#include "ProxyConfig.h"		 // Launcher::ReadLauncherSettings, Shared::MitmproxySettings

/**
 * @file
 * @brief Spawns mitmproxy (mitmdump) from the launcher for ProxyMode::Mitmproxy.
 *
 * The launcher always runs mitmdump with a `-s <addon>` script; how that script is chosen comes
 * from the launcher's own settings (Shared::MitmproxySettings, in launcher.settings, editable
 * from the DLL's Network tab):
 * - Default: runs the bundled `scripts/default_proxy.py` (next to Launcher.exe), fed the DLL's
 *   redirect map through the SPLITGATE_REDIRECTS env var. It rewrites those hosts to the private
 *   server and passes other TLS connections through (parity with the backend's scripts/proxy.py).
 * - Path: an addon file already on disk is run as-is (`mitmdump -s <path>`).
 * - Inline: the inline python is written to a temp file and run as the addon.
 *
 * The default addon and the watchdog live as real .py files under `scripts/` (copied next to the
 * exe at build time) rather than inlined here, so they can be edited/linted as Python.
 *
 * mitmdump runs **hidden** by default (CREATE_NO_WINDOW; MitmproxySettings::ShowConsole shows the
 * window instead), and its lifetime is tied to the game: the `scripts/watchdog.py` addon (always
 * appended, given the game PID) waits on the game process and exits mitmdump when the game closes.
 *
 * NOTE: like the manual setup this replaces, the game still has to route through the proxy —
 * mitmproxy's default mode needs the Windows system proxy pointed at it (127.0.0.1:8080), or a
 * transparent/WinDivert mode. This helper only launches mitmdump; wiring the system proxy (or a
 * transparent mode) is out of scope and left to configuration.
 */
namespace Launcher::Mitmproxy
{
	/// The folder holding the bundled addon scripts: `scripts/` next to Launcher.exe. Empty path
	/// if the module path can't be resolved.
	inline std::filesystem::path ScriptsDir()
	{
		wchar_t buffer[MAX_PATH];
		const DWORD length = GetModuleFileNameW(nullptr, buffer, MAX_PATH);
		if (length == 0 || length == MAX_PATH) return {};
		return std::filesystem::path(buffer).parent_path() / "scripts";
	}

	/// Writes inline python to a temp .py file and returns its path (empty on failure). A stable
	/// name is reused so repeated launches overwrite rather than pile up temp files.
	inline std::string WriteInlineScript(const std::string& content)
	{
		std::error_code ec;
		const auto path = std::filesystem::temp_directory_path(ec) / "splitgate_mitm_inline.py";
		if (ec) return {};

		std::ofstream file(path, std::ios::out | std::ios::binary | std::ios::trunc);
		if (!file.is_open()) return {};

		file << content;
		file.close();
		return path.string();
	}

	/// Resolves the main addon script path for the configured mode. Default points at the bundled
	/// scripts/default_proxy.py (passing the redirect map through SPLITGATE_REDIRECTS); Path uses
	/// the file as-is; Inline is written to a temp file. The path is not existence-checked here —
	/// Spawn validates it so it can log a clear reason.
	inline std::string ResolveScript(const Shared::MitmproxySettings& config, const std::map<std::string, std::string>& redirects)
	{
		switch (config.ScriptMode)
		{
		case Shared::MitmScriptMode::Path:
			return config.ScriptPath;
		case Shared::MitmScriptMode::Inline:
			return WriteInlineScript(config.InlineScript);
		case Shared::MitmScriptMode::Default:
		default:
			SetEnvironmentVariableA("SPLITGATE_REDIRECTS", nlohmann::json(redirects).dump().c_str());
			return (ScriptsDir() / "default_proxy.py").string();
		}
	}

	/**
	 * Starts mitmdump running the addon chosen by the launcher's mitmproxy settings
	 * (Default/Path/Inline), plus the watchdog addon. Reads those settings itself, so callers only
	 * pass the redirect map and the game PID.
	 *
	 * A missing addon (e.g. the bundled scripts/ folder was deleted) is not a hard error: it's
	 * logged through @p logger and Spawn returns false, so the caller can carry on without the
	 * proxy rather than aborting.
	 *
	 * @param redirects original host -> "host[:port]" target (from the DLL's NETWORK settings).
	 * @param gamePid   the game's process id; when non-zero, the watchdog addon exits mitmdump when
	 *                  that process ends. Pass 0 to leave mitmdump running until closed manually.
	 * @param logger    optional sink for progress/failure lines (the launcher's logger); may be null.
	 * @return true if the process was started (mitmdump must be on PATH), false otherwise.
	 */
	inline bool Spawn(const std::map<std::string, std::string>& redirects, DWORD gamePid = 0, Shared::Logger* logger = nullptr)
	{
		const auto log = [&](const std::string& level, const std::string& message)
		{
			if (logger) logger->log(level, message);
		};

		const auto config = ReadLauncherSettings().MITMPROXY;

		const std::string script = ResolveScript(config, redirects);
		if (script.empty() || !std::filesystem::exists(script))
		{
			log("ERROR", "mitmproxy: addon script not found (" + (script.empty() ? std::string("<none>") : script) + "); proxy not started");
			return false;
		}

		std::string command = "mitmdump -s \"" + script + "\"";

		// Watchdog: exit mitmdump when the game process ends (PID passed via the environment). Its
		// absence is non-fatal — mitmdump just won't auto-close with the game.
		if (gamePid)
		{
			const auto watchdog = ScriptsDir() / "watchdog.py";
			if (std::filesystem::exists(watchdog))
			{
				command += " -s \"" + watchdog.string() + "\"";
				SetEnvironmentVariableA("SPLITGATE_GAME_PID", std::to_string(gamePid).c_str());
			}
			else
			{
				log("WARN", "mitmproxy: watchdog.py not found; mitmdump won't auto-close with the game");
			}
		}

		STARTUPINFOA startup{sizeof(startup)};
		PROCESS_INFORMATION process{};

		// Hidden by default (CREATE_NO_WINDOW); a visible console when ShowConsole is set.
		const DWORD creationFlags = config.ShowConsole ? CREATE_NEW_CONSOLE : CREATE_NO_WINDOW;

		if (!CreateProcessA(nullptr, command.data(), nullptr, nullptr, FALSE, creationFlags, nullptr, nullptr, &startup, &process))
		{
			log("ERROR", "mitmproxy: failed to start mitmdump (is it on PATH?)");
			return false;
		}

		CloseHandle(process.hThread);
		CloseHandle(process.hProcess);
		log("SUCCESS", "mitmproxy started (" + script + ")");
		return true;
	}
} // namespace Launcher::Mitmproxy
