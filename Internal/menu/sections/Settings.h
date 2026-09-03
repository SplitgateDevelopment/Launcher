#pragma once

#include "../../settings/Settings.h"
#include "../../scripting/Events.h"
#include "../../hook/Hook.h"

namespace Menu {
	namespace Sections {
		void SettingsTab() {
			ImGui::SeparatorText("Config");
			if (ImGui::Button("Save")) {
				SettingsHelper::Save();
			};

			ImGui::SameLine();

			if (ImGui::Button("Reload")) {
				SettingsHelper::Load();
				Events::Dispatch(Events::Type::SettingsChanged);  // features re-read Enabled
			};

			ImGui::SameLine();

			if (ImGui::Button("Reset defaults")) {
				SettingsHelper::Reset();
				Events::Dispatch(Events::Type::SettingsChanged);
			};

			ImGui::SeparatorText("Menu");
			ImGui::HotKey("Open Menu", &Settings.MENU.ShowHotkey);
			ImGui::ToggleButton("Watermark", &Settings.MENU.ShowWatermark);

			ImGui::SeparatorText("Program");
			if (ImGui::Button("Toggle Console")) {
				Settings.MISC.ShowConsole = !Settings.MISC.ShowConsole;
				Logger::SetConsoleVisibility(Settings.MISC.ShowConsole);
			};

			ImGui::SameLine();

			ImGui::BeginDisabled();
			if (ImGui::Button("Unload")) {
				//Hook::UnHook(); TODO add unhooking feature
			}
			ImGui::EndDisabled();
		}
	}
}