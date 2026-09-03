#pragma once

/// @file
/// @brief Network tab: backend-redirect toggle + editable redirect map, and HTTP-logging
/// controls. Backed by Settings.NETWORK and read live by the hooks in the network/ module.

#include <string>

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
			changed |= ImGui::ToggleButton("Enable redirect", &Settings.NETWORK.RedirectEnabled);

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

			if (changed) Events::Dispatch(Events::Type::SettingsChanged);
		}
	} // namespace Sections
} // namespace Menu
