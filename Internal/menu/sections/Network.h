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

			ImGui::SeparatorText("Backend redirect");
			ImGui::TextUnformatted("Proxy mode");
			ImGui::Tooltip("Internal: the DLL redirects in-process. Mitmproxy: the launcher spawns\nmitmproxy at startup (takes effect next launch). Manual: do nothing.");
			int mode = static_cast<int>(Settings.NETWORK.Proxy);
			changed |= ImGui::RadioButton("Manual", &mode, static_cast<int>(ProxyMode::Manual));
			ImGui::SameLine();
			changed |= ImGui::RadioButton("Internal", &mode, static_cast<int>(ProxyMode::Internal));
			ImGui::SameLine();
			changed |= ImGui::RadioButton("Mitmproxy", &mode, static_cast<int>(ProxyMode::Mitmproxy));
			Settings.NETWORK.Proxy = static_cast<ProxyMode>(mode);

			// Existing redirects, each with a remove button.
			std::string toRemove;
			for (const auto& [original, target] : Settings.NETWORK.Redirects)
			{
				ImGui::BulletText("%s -> %s", original.c_str(), target.c_str());
				ImGui::SameLine();
				if (ImGui::SmallButton(("X##" + original).c_str())) toRemove = original;
			}
			if (!toRemove.empty())
			{
				Settings.NETWORK.Redirects.erase(toRemove);
				changed = true;
			}

			// Add a new original -> target ("host" or "host:port") mapping.
			static char originalBuffer[128] = "";
			static char targetBuffer[128] = "";
			ImGui::InputText("Original host", originalBuffer, sizeof(originalBuffer));
			ImGui::InputText("Target host[:port]", targetBuffer, sizeof(targetBuffer));
			if (ImGui::Button("Add redirect") && originalBuffer[0] && targetBuffer[0])
			{
				Settings.NETWORK.Redirects[originalBuffer] = targetBuffer;
				originalBuffer[0] = '\0';
				targetBuffer[0] = '\0';
				changed = true;
			}

				changed |= ImGui::ToggleButton("Bypass SSL verification", &Settings.NETWORK.BypassSslVerify);
				ImGui::Tooltip("Force curl's cert/host verification off so a redirected host can serve a self-signed cert.\nDisables TLS verification for ALL curl traffic while on.");

				// Mitmproxy script — a launcher-only setting (launcher.settings), so it lives outside
				// the DLL's SETTINGS. Only relevant when the launcher will spawn mitmproxy.
			if (Settings.NETWORK.Proxy == ProxyMode::Mitmproxy)
			{
				ImGui::SeparatorText("Mitmproxy script");
				ImGui::Tooltip("How the launcher starts mitmdump (saved to launcher.settings, applied next launch).\n"
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
				launcherChanged |= ImGui::RadioButton("Default##mitm", &scriptMode, static_cast<int>(Shared::MitmScriptMode::Default));
				ImGui::SameLine();
				launcherChanged |= ImGui::RadioButton("Path##mitm", &scriptMode, static_cast<int>(Shared::MitmScriptMode::Path));
				ImGui::SameLine();
				launcherChanged |= ImGui::RadioButton("Inline##mitm", &scriptMode, static_cast<int>(Shared::MitmScriptMode::Inline));
				mitm.ScriptMode = static_cast<Shared::MitmScriptMode>(scriptMode);

				if (mitm.ScriptMode == Shared::MitmScriptMode::Path)
				{
					if (ImGui::InputText("Script path", pathBuffer, sizeof(pathBuffer)))
					{
						mitm.ScriptPath = pathBuffer;
						launcherChanged = true;
					}
				}
				else if (mitm.ScriptMode == Shared::MitmScriptMode::Inline)
				{
					if (ImGui::InputTextMultiline("Inline python", inlineBuffer, sizeof(inlineBuffer), ImVec2(0, 160)))
					{
						mitm.InlineScript = inlineBuffer;
						launcherChanged = true;
					}
				}
				else
				{
					ImGui::TextDisabled("Runs the bundled scripts/default_proxy.py (redirects above + TLS passthrough).");
				}

				launcherChanged |= ImGui::ToggleButton("Show mitmproxy window", &mitm.ShowConsole);

				if (launcherChanged) LauncherConfigFile().Save();
			}

			ImGui::SeparatorText("HTTP logging");
			changed |= ImGui::ToggleButton("Log HTTP calls", &Settings.NETWORK.HttpLogging);
			changed |= ImGui::ToggleButton("Also log to http.log", &Settings.NETWORK.HttpLogToFile);
			changed |= ImGui::ToggleButton("Redirected hosts only", &Settings.NETWORK.HttpLogRedirectedOnly);

			// Live request flow — populated while HTTP logging is on.
			if (ImGui::CollapsingHeader("Request flow"))
			{
				if (ImGui::SmallButton("Clear")) Network::Http::Clear();

				const auto requests = Network::Http::Recent();
				ImGui::BeginChild("RequestFlow", ImVec2(0, 200), true, ImGuiWindowFlags_HorizontalScrollbar);
				if (requests.empty() && !Settings.NETWORK.HttpLogging)
					ImGui::TextDisabled("Enable \"Log HTTP calls\" to capture requests.");
				for (const auto& request : requests)
					ImGui::TextUnformatted(request.c_str());
				ImGui::EndChild();
			}

			if (changed) Events::Dispatch(Events::Type::SettingsChanged);
		}
	} // namespace Sections
} // namespace Menu
