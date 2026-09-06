#pragma once

/// @file
/// @brief Discord tab: enable/disable Rich Presence, show the live presence state, force a refresh,
/// and display the configured app/image info. The live state is kept current by the DiscordPresence
/// feature (see features/DiscordPresence.h); this tab is the manual control surface.

#include "../../settings/Settings.h"
#include "../../scripting/Events.h"
#include "../../discord/rpc.h"
#include "../ui/UI.h"

namespace Menu
{
	namespace Sections
	{
		/// @brief Renders the Discord tab.
		void DiscordTab()
		{
			UI::SeparatorText("Rich Presence");

			if (UI::Toggle("Enable", &Settings.MISC.DiscordRPCEnabled))
			{
				Events::Dispatch(Events::Type::SettingsChanged); // features (incl. DiscordPresence) refresh
				if (Settings.MISC.DiscordRPCEnabled)
					DiscordRPC::InitPresence();
				else
					Discord_ClearPresence(); // hide it from your profile while off
			}
			UI::Tooltip("Show a Rich Presence on your Discord profile. Updates with the live game state\n(map + K/D) every few seconds.");

			// BeginDisabled/EndDisabled greys out the block in ImGui; the Canvas backend has no equivalent,
			// so it's only applied under the ImGui backend.
			const bool disabled = !Settings.MISC.DiscordRPCEnabled;
			if (UI::IsImGui() && disabled) ImGui::BeginDisabled();

			UI::SeparatorText("Live");
			const char* state = DiscordRPC::GetState();
			UI::Text("State:   %s", (state && state[0]) ? state : "(none)");
			UI::Text("Details: %s", Settings.MENU.Watermark.c_str());
			if (UI::Button("Refresh now")) DiscordRPC::UpdateGameState();
			UI::Tooltip("Push the current map + K/D to Discord immediately (also happens automatically ~every 5s).");

			UI::SeparatorText("Info");
			UI::Text("App ID:  %s", Settings.MISC.DiscordAppID.c_str());
			UI::Text("Image:   %s", "icon");
			UI::TextDisabled("App ID / Steam app id are runtime-only and set at startup.");

			if (UI::IsImGui() && disabled) ImGui::EndDisabled();
		}
	} // namespace Sections
} // namespace Menu
