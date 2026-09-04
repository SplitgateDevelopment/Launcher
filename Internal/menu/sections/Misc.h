#pragma once

/// @file
/// @brief Misc tab: player FOV/speed sliders, load-into-map action, and user-scripting controls.

#include "../../settings/Settings.h"
#include "../../scripting/Scripts.h"
#include "../../scripting/Events.h"
#include "../gui/Window.h" // Window::SetStreamproof

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

			ImGui::SetNextItemWidth(180.f);
			ImGui::SliderFloat("##fov", &Settings.EXPLOITS.FOV, 80.0f, 160.0f, "FOV %.0f");
			ImGui::SameLine();
			if (ImGui::SmallButton("Reset##fov"))
			{
				Settings.EXPLOITS.FOV = ExploitsSettings{}.FOV;
				Events::Dispatch(Events::Type::SettingsChanged);
			}

			if (!isInGame) ImGui::BeginDisabled();
			ImGui::SetNextItemWidth(180.f);
			ImGui::SliderFloat("##speed", &Settings.EXPLOITS.PlayerSpeed, 0.2f, 4.f, "Speed %.2f");
			ImGui::SameLine();
			if (ImGui::SmallButton("Reset##speed"))
			{
				Settings.EXPLOITS.PlayerSpeed = ExploitsSettings{}.PlayerSpeed;
				Events::Dispatch(Events::Type::SettingsChanged);
			}
			if (!isInGame) ImGui::EndDisabled();

			ImGui::SeparatorText("Game");

			// "Load into map" is only usable out of a game, and only once per session — it disables
			// itself after the first use.
			static bool loadUsed = false;
			const bool loadDisabled = isInGame || loadUsed;
			if (loadDisabled) ImGui::BeginDisabled();
			if (ImGui::Button("Load into map"))
			{
				Events::Dispatch(Events::Type::LoadIntoMap);
				loadUsed = true;
			};
			if (loadDisabled) ImGui::EndDisabled();

			ImGui::SeparatorText("Overlay");
			ImGui::Tooltip("Hide the overlay from screen capture (OBS, Discord, Game Bar). Needs Windows 10 2004+.");
			if (ImGui::ToggleButton("Streamproof", &Settings.MENU.Streamproof))
			{
				Window::SetStreamproof(Settings.MENU.Streamproof);
				Events::Dispatch(Events::Type::SettingsChanged);
			}

			ImGui::SeparatorText("Config");
			if (ImGui::ToggleButton("Autosave on change", &Settings.MISC.AutoSave))
				Events::Dispatch(Events::Type::SettingsChanged);

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