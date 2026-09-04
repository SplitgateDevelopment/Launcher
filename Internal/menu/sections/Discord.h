#pragma once

/// @file
/// @brief Discord tab: enable/disable Rich Presence, show the live presence state, force a refresh,
/// and display the configured app/image info. The live state is kept current by the DiscordPresence
/// feature (see features/DiscordPresence.h); this tab is the manual control surface.

#include "../../settings/Settings.h"
#include "../../scripting/Events.h"
#include "../../discord/rpc.h"

namespace Menu
{
	namespace Sections
	{
		/// @brief Renders the Discord tab.
		void DiscordTab()
		{
			ImGui::SeparatorText("Rich Presence");

			if (ImGui::ToggleButton("Enable", &Settings.MISC.DiscordRPCEnabled))
			{
				Events::Dispatch(Events::Type::SettingsChanged); // features (incl. DiscordPresence) refresh
				if (Settings.MISC.DiscordRPCEnabled)
					DiscordRPC::InitPresence();
				else
					Discord_ClearPresence(); // hide it from your profile while off
			}
			ImGui::Tooltip("Show a Rich Presence on your Discord profile. Updates with the live game state\n(map + K/D) every few seconds.");

			if (!Settings.MISC.DiscordRPCEnabled) ImGui::BeginDisabled();

			ImGui::SeparatorText("Live");
			const char* state = DiscordRPC::GetState();
			ImGui::Text("State:   %s", (state && state[0]) ? state : "(none)");
			ImGui::Text("Details: %s", Settings.MENU.Watermark.c_str());
			if (ImGui::Button("Refresh now")) DiscordRPC::UpdateGameState();
			ImGui::Tooltip("Push the current map + K/D to Discord immediately (also happens automatically ~every 5s).");

			ImGui::SeparatorText("Info");
			ImGui::Text("App ID:  %s", Settings.MISC.DiscordAppID.c_str());
			ImGui::Text("Image:   %s", "icon");
			ImGui::TextDisabled("App ID / Steam app id are runtime-only and set at startup.");

			if (!Settings.MISC.DiscordRPCEnabled) ImGui::EndDisabled();
		}
	} // namespace Sections
} // namespace Menu
