#pragma once

/// @file
/// @brief Network tab: backend-redirect toggle + editable redirect map, and HTTP-logging
/// controls. Backed by Settings.NETWORK and read live by the hooks in the network/ module.

#include <string>

#include "../../network/HttpLogger.h"
#include "../../settings/Settings.h"
#include "../../scripting/Events.h"

namespace Menu
{
	namespace Sections
	{
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
