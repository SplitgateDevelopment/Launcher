#pragma once

/// @file
/// @brief Settings tab: config persistence (save/reload/reset), menu hotkey and watermark, and program controls.

#include "../../settings/Settings.h"
#include "../../scripting/Events.h"
#include "../../hook/Hook.h"

namespace Menu
{
	namespace Sections
	{
		/// @brief Renders the Settings tab.
		/// Save/Reload/Reset persist, reload, or restore the config (Reload and Reset dispatch SettingsChanged so
		/// features re-read their state); binds the open-menu hotkey and watermark toggle; and offers program
		/// controls to toggle the console and an (currently disabled) Unload button.
		void SettingsTab()
		{
			ImGui::SeparatorText("Config");
			if (ImGui::Button("Save"))
			{
				SettingsHelper::File().Save();
			};

			ImGui::SameLine();

			if (ImGui::Button("Reload"))
			{
				SettingsHelper::File().Load();
				Events::Dispatch(Events::Type::SettingsChanged); // features re-read Enabled
			};

			ImGui::SameLine();

			if (ImGui::Button("Reset defaults"))
			{
				SettingsHelper::File().Reset();
				Events::Dispatch(Events::Type::SettingsChanged);
			};

			ImGui::SeparatorText("Menu");
			ImGui::HotKey("Open Menu", &Settings.MENU.ShowHotkey);
			ImGui::ToggleButton("Watermark", &Settings.MENU.ShowWatermark);

			ImGui::SeparatorText("Program");
			if (ImGui::Button("Toggle Console"))
			{
				Settings.MISC.ShowConsole = !Settings.MISC.ShowConsole;
				Logger::SetConsoleVisibility(Settings.MISC.ShowConsole);
			};

			ImGui::SameLine();

			ImGui::BeginDisabled();
			if (ImGui::Button("Unload"))
			{
				// Hook::UnHook(); TODO add unhooking feature
			}
			ImGui::EndDisabled();
		}
	} // namespace Sections
} // namespace Menu