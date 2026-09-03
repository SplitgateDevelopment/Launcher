#pragma once

/// @file
/// @brief Misc tab: player FOV/speed sliders, load-into-map action, and user-scripting controls.

#include "../../settings/Settings.h"
#include "../../scripting/Scripts.h"
#include "../../scripting/Events.h"

namespace Menu
{
	namespace Sections
	{
		/// @brief Renders the Misc tab.
		/// Adjusts player FOV and (only while in-game) player speed; offers a "Load into map" button that
		/// dispatches LoadIntoMap when not already in-game; toggles user scripts (dispatching SettingsChanged)
		/// and lists the currently loaded scripts.
		void MiscTab()
		{
			bool isInGame = Globals::PlayerController->IsInGame();

			ImGui::SeparatorText("Player");
			ImGui::SliderFloat("Player FOV", &Settings.EXPLOITS.FOV, 80.0f, 160.0f);

			if (!isInGame) ImGui::BeginDisabled();
			ImGui::SliderFloat("Player Speed", &Settings.EXPLOITS.PlayerSpeed, 0.2f, 4.f);
			if (!isInGame) ImGui::EndDisabled();

			ImGui::SeparatorText("Game");

			if (isInGame) ImGui::BeginDisabled();
			if (ImGui::Button("Load into map"))
			{
				Events::Dispatch(Events::Type::LoadIntoMap);
			};
			if (isInGame) ImGui::EndDisabled();

			ImGui::SeparatorText("User Scripts");
			if (ImGui::ToggleButton("Enable", &Settings.MISC.UserScriptsEnabled))
				Events::Dispatch(Events::Type::SettingsChanged);

			if (ImGui::TreeNode("Loaded Scripts"))
			{
				for (const auto& script : Scripts::scriptList)
				{
					ImGui::BulletText(script.c_str());
				}

				ImGui::TreePop();
			}
		}
	} // namespace Sections
} // namespace Menu