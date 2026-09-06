#pragma once

/// @file
/// @brief Network tab: backend-redirect toggle + editable redirect map, and HTTP-logging
/// controls. Backed by Settings.NETWORK and read live by the hooks in the network/ module.

#include <cstring>
#include <string>

#include "../../network/HttpLogger.h"
#include "../../settings/Settings.h"
#include "../../scripting/Events.h"
#include "../../../shared/LauncherSettings.h" // Shared::LauncherSettings (launcher.settings)
#include "../ui/UI.h"

namespace Menu
{
	namespace Sections
	{
		/// The launcher's own settings (launcher.settings), edited here so the launcher picks the
		/// mitmproxy config up on its next start. Kept separate from the DLL's SETTINGS on purpose.
		inline Shared::LauncherSettings& LauncherConfig()
		{
			static Shared::LauncherSettings settings;
			return settings;
		}

		/// The file bound to @ref LauncherConfig (same app folder as splitgate.settings).
		inline Shared::SettingsFile<Shared::LauncherSettings>& LauncherConfigFile()
		{
			static Shared::SettingsFile<Shared::LauncherSettings> file(
				LauncherConfig(),
				Shared::AppDataPath(SettingsHelper::AppFolder, Shared::LauncherSettingsFileName));
			return file;
		}

		/// @brief Renders the Network tab: the redirect enable toggle, an add/remove editor for
		/// the original->target redirect map, and the HTTP-logging switches. Dispatches
		/// SettingsChanged on any change.
		void NetworkTab()
		{

			bool changed = false;

			UI::SeparatorText("Backend redirect");
			UI::Text("Proxy mode");
			UI::Tooltip("Internal: the DLL redirects in-process. Mitmproxy: the launcher spawns\nmitmproxy at startup (takes effect next launch). Manual: do nothing.");
			int mode = static_cast<int>(Settings.NETWORK.Proxy);
			changed |= UI::RadioButton("Manual", &mode, static_cast<int>(ProxyMode::Manual));
			UI::SameLine();
			changed |= UI::RadioButton("Internal", &mode, static_cast<int>(ProxyMode::Internal));
			UI::SameLine();
			changed |= UI::RadioButton("Mitmproxy", &mode, static_cast<int>(ProxyMode::Mitmproxy));
			Settings.NETWORK.Proxy = static_cast<ProxyMode>(mode);

			// Existing redirects, each with a remove button.
			std::string toRemove;
			for (const auto& [original, target] : Settings.NETWORK.Redirects)
			{
				UI::BulletText("%s -> %s", original.c_str(), target.c_str());
				UI::SameLine();
				if (UI::SmallButton(("X##" + original).c_str())) toRemove = original;
			}
			if (!toRemove.empty())
			{
				Settings.NETWORK.Redirects.erase(toRemove);
				changed = true;
			}

			// Add a new original -> target ("host" or "host:port") mapping.
			static char originalBuffer[128] = "";
			static char targetBuffer[128] = "";
			UI::InputText("Original host", originalBuffer, sizeof(originalBuffer));
			UI::InputText("Target host[:port]", targetBuffer, sizeof(targetBuffer));
			if (UI::Button("Add redirect") && originalBuffer[0] && targetBuffer[0])
			{
				Settings.NETWORK.Redirects[originalBuffer] = targetBuffer;
				originalBuffer[0] = '\0';
				targetBuffer[0] = '\0';
				changed = true;
			}

			changed |= UI::Toggle("Bypass SSL verification", &Settings.NETWORK.BypassSslVerify);
			UI::Tooltip("Force curl's cert/host verification off so a redirected host can serve a self-signed cert.\nDisables TLS verification for ALL curl traffic while on.");

			// Mitmproxy script — a launcher-only setting (launcher.settings), so it lives outside
			// the DLL's SETTINGS. Only relevant when the launcher will spawn mitmproxy.
			if (Settings.NETWORK.Proxy == ProxyMode::Mitmproxy)
			{
				UI::SeparatorText("Mitmproxy script");
				UI::Tooltip("How the launcher starts mitmdump (saved to launcher.settings, applied next launch).\n"
							   "Default: a generated addon (redirects above + TLS passthrough).\n"
							   "Path: mitmdump -s <file>. Inline: your python, run as the addon.");

				// Load launcher.settings once, then mirror its strings into edit buffers.
				static bool launcherLoaded = LauncherConfigFile().Load();
				auto& mitm = LauncherConfig().MITMPROXY;

				static char pathBuffer[512] = "";
				static char inlineBuffer[8192] = "";
				static bool buffersInit = [&]
				{
					const auto copyInto = [](char* dst, std::size_t size, const std::string& src)
					{
						const std::size_t count = src.size() < size - 1 ? src.size() : size - 1;
						std::memcpy(dst, src.data(), count);
						dst[count] = '\0';
					};
					copyInto(pathBuffer, sizeof(pathBuffer), mitm.ScriptPath);
					copyInto(inlineBuffer, sizeof(inlineBuffer), mitm.InlineScript);
					return true;
				}();
				(void)launcherLoaded;
				(void)buffersInit;

				bool launcherChanged = false;
				int scriptMode = static_cast<int>(mitm.ScriptMode);
				launcherChanged |= UI::RadioButton("Default##mitm", &scriptMode, static_cast<int>(Shared::MitmScriptMode::Default));
				UI::SameLine();
				launcherChanged |= UI::RadioButton("Path##mitm", &scriptMode, static_cast<int>(Shared::MitmScriptMode::Path));
				UI::SameLine();
				launcherChanged |= UI::RadioButton("Inline##mitm", &scriptMode, static_cast<int>(Shared::MitmScriptMode::Inline));
				mitm.ScriptMode = static_cast<Shared::MitmScriptMode>(scriptMode);

				if (mitm.ScriptMode == Shared::MitmScriptMode::Path)
				{
					if (UI::InputText("Script path", pathBuffer, sizeof(pathBuffer)))
					{
						mitm.ScriptPath = pathBuffer;
						launcherChanged = true;
					}
				}
				else if (mitm.ScriptMode == Shared::MitmScriptMode::Inline)
				{
					if (UI::InputTextMultiline("Inline python", inlineBuffer, sizeof(inlineBuffer), 160))
					{
						mitm.InlineScript = inlineBuffer;
						launcherChanged = true;
					}
				}
				else
				{
					UI::TextDisabled("Runs the bundled scripts/default_proxy.py (redirects above + TLS passthrough).");
				}

				launcherChanged |= UI::Toggle("Show mitmproxy window", &mitm.ShowConsole);

				if (launcherChanged) LauncherConfigFile().Save();
			}

			UI::SeparatorText("HTTP logging");
			changed |= UI::Toggle("Log HTTP calls", &Settings.NETWORK.HttpLogging);
			changed |= UI::Toggle("Also log to http.log", &Settings.NETWORK.HttpLogToFile);
			changed |= UI::Toggle("Redirected hosts only", &Settings.NETWORK.HttpLogRedirectedOnly);

			// Live request flow — populated while HTTP logging is on.
			if (UI::CollapsingHeader("Request flow"))
			{
				if (UI::SmallButton("Clear")) Network::Http::Clear();

				const auto requests = Network::Http::Recent();
				UI::BeginChild("RequestFlow", 0, 200);
				if (requests.empty() && !Settings.NETWORK.HttpLogging)
					UI::TextDisabled("Enable \"Log HTTP calls\" to capture requests.");
				for (const auto& request : requests)
					UI::Text("%s", request.c_str());
				UI::EndChild();
			}

			if (changed) Events::Dispatch(Events::Type::SettingsChanged);
		}
	} // namespace Sections
} // namespace Menu
